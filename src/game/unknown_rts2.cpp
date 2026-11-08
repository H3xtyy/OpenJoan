// game/unknown_rts2.cpp: functions whose original source file is unknown (formerly game/_unplaced/rts2.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../runtime/hitch.hpp"

#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::import_call;
using joan::img::obj;

using i32 = std::int32_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;
using i16 = std::int16_t;

using u32 = std::uint32_t;

using joan::orig::helpers::addr;

// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return import_call<float>(slot, args...);
#else
    return static_cast<float>(import_call<double>(slot, args...));
#endif
}

struct Vec3 {
    float x, y, z;
};

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using joan::img::str;

constexpr fn::Code kListCount = fn::List_count_417ad0;
  // 0x00417ad0


struct alignas(4) Text {  // 0x005a11a0 / 0x005a1170 path string (0x194 bytes on the stack)
    u8 b[0x194];
};

struct Obj {};

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
}  // namespace

// ---------------------------------------- from modules/game_b104_01.cpp (game/b104_01)
namespace {
namespace game_b104_01 {

using joan::img::str;

void* const kItemTypes = obj(d::g_item_res);
void* const kCams = obj(d::g_camera_man);

using joan::orig::helpers::fbits;

struct FVec {
    float x, y, z;
};
static_assert(sizeof(FVec) == 12, "by-value 12-byte argument");
struct Blob24 {
    u32 d[6];
};
static_assert(sizeof(Blob24) == 24, "by-value 24-byte argument");
struct Blob20 {
    u32 d[5];
};
static_assert(sizeof(Blob20) == 20, "by-value 20-byte argument");
struct alignas(4) Path {  // 0x194 bytes in the frame (0x005a11a0 copies 0x190 + NUL)
    u8 bytes[0x194];
};
static_assert(sizeof(Path) == 0x194, "Path");

JOAN_COUNTER(c_pick_448340, "Pick::object_448340");
JOAN_COUNTER(c_select_449b30, "Group::select_449b30");

constexpr img::Addr kUnits = d::g_units;

i32 unit_dead(u32 id) { return joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_rec(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8 unit_busy(void* rec) { return joan::orig::arg::as<u8>(joan::orig::as_unit(rec)->dead()); }                 // 0x0042d0e0
u8* item_type(u32 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(w))); }  // 0x00425ce0
void* cam_entry(u32 i) { return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCams)->motion(joan::orig::arg::i32(i))); }        // 0x00563c80
using joan::orig::helpers::cf;

FVec vsub(const void* a, const void* b) {  // 0x0060cc90 (out, a, b) -> *out
    FVec t;
    return *joan::orig::arg::as<FVec*>(joan::orig::vec_sub(&t, joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
}

// 0x005a1440 / 0x005a1550 path assign / append
void path_assign(Path& p, const char* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }
void path_append(Path& p, const char* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }

struct Hit {
    float d;
    u8 hit;
};

struct H {
    // ------------------------------------------------------------ 0x00448340
    void* pick_448340(u32 x, u32 y, void* out) {
        c_pick_448340.hit();
        u32 cam[10];  // [ebp-0x2c]
        {
            const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(kCams)->target());  // 0x00563dc0
            joan::orig::as_camera_motion(cam_entry(idx))->end(joan::orig::arg::mptr(cam));    // 0x00423fe0
        }
        float box[6];
        box[0] = fbits(cam[1]) - cf(d::flt_1000);
        box[1] = fbits(cam[2]) - cf(d::flt_1000);
        box[2] = fbits(cam[3]) - cf(d::flt_5000);
        box[3] = fbits(cam[1]) + cf(d::flt_1000);
        box[4] = fbits(cam[2]) + cf(d::flt_1000);
        std::memcpy(&box[5], &cam[3], 4);
        if (global<void*>(d::g_world) == nullptr) return nullptr;
        u32 cnt;
        u8** arr = joan::orig::arg::as<u8**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(joan::orig::arg::mptr(box), &cnt));  // 0x00429090
        i32 v50, v54;
        if (joan::orig::arg::as<i32>(joan::orig::as_grid(global<void*>(d::g_camera))->to_cell(joan::orig::arg::i32(x), joan::orig::arg::i32(y), &v50, &v54)) == 0) return nullptr;  // 0x006152b0
        float ray[6];
        joan::orig::as_camera(global<void*>(d::g_camera))->screen_ray(joan::orig::arg::i32(v50), joan::orig::arg::i32(v54), joan::orig::arg::mptr(ray));  // 0x006154d0
        jo::Actor* best = nullptr;
        float bestd = fbits(0x40000000);
        for (i32 i = static_cast<i32>(cnt) - 1; i >= 0; --i) {
            u8* q = arr[i];
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(joan::orig::arg::ptr(ray), joan::orig::arg::ptr(jo::as_cell(q)->box))) == 0) continue;  // 0x0060dfd0
            for (u8* u = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(q)->get_98()); u != nullptr; u = joan::orig::arg::as<u8*>(joan::orig::get_b2(joan::orig::arg::mptr(u)))) {  // 0x0054f510 / 0x0054f530
                if (jo::as_map_object(u)->actor == nullptr || jo::as_map_object(u)->actor->plane == nullptr) continue;
                const i16 kind = jo::as_map_object(u)->class_id;
                if (kind != 1) {
                    if (kind != 0xa) continue;
                    u8* t = item_type(static_cast<u32>(jo::view<u16>(jo::as_map_object(u)->type_id)));  // x64: the call takes the low word only (arg::i16)
                    if (jo::view<i8>(jo::as_item_def(t)->open_with_interact) == 0) continue;
                }
                u32 sph[4];
                joan::orig::as_actor(jo::view<void*>(jo::as_map_object(u)->actor))->bounding_sphere(joan::orig::arg::mptr(sph), 1u);  // 0x005acc80
                if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(joan::orig::arg::mptr(ray), joan::orig::arg::mptr(sph))) == 0) continue;  // 0x0060e3f0
                Hit h{fbits(0x40000000), 0};
                if (jo::as_map_object(u)->actor == nullptr) continue;
                joan::orig::as_bone(jo::view<void*>(jo::as_map_object(u)->actor->bone))->update();  // 0x0060ee60
                u8* mat = jo::as_map_object(u)->actor->bone->matrix_;
                float inv[16];
                import_call<void*>(d::iat_v3d_87_matrix_inverse_2, inv, mat);  // matrix_inverse(out, in)
                FVec p0, p1;
                import_call<void*>(d::iat_v3d_87_point_transform_3, &p0, &ray[0], inv);  // point_transform(out, in, m)
                import_call<void*>(d::iat_v3d_87_point_transform_3, &p1, &ray[3], inv);
                u8* m = jo::view<u8*>(jo::as_map_object(u)->actor);
                switch (jo::as_actor(m)->aim_mode) {
                    case 0:
                        continue;
                    case 1: {
                        const FVec dd = vsub(&p1, &p0);
                        h.hit = joan::orig::arg::as<u8>(joan::orig::shape::sphere_hit(&h.d, p0, dd, joan::orig::arg::mptr(jo::as_map_object(u)->actor->aim_sphere)));  // 0x006a0f90
                        break;
                    }
                    case 2: {
                        const FVec dd = vsub(&p1, &p0);
                        const Blob24 bx = jo::view<Blob24>(jo::as_map_object(u)->actor->aim_box);
                        h.hit = joan::orig::arg::as<u8>(joan::orig::shape::box_hit(&h.d, p0, dd, bx));  // 0x00698970
                        break;
                    }
                    case 3: {
                        const FVec dd = vsub(&p1, &p0);
                        Blob20 cy;  // collide_kind .. collide_height (5 dwords)
                        std::memcpy(&cy, &jo::as_map_object(u)->actor->collide_kind, sizeof cy);
                        h.hit = joan::orig::arg::as<u8>(joan::orig::shape::cylinder_hit(&h.d, p0, dd, cy));  // 0x00699ec0
                        break;
                    }
                    default:
                        break;
                }
                if (h.hit == 0) continue;
                if (best == nullptr || bestd > h.d) {
                    bestd = h.d;
                    best = jo::as_map_object(u)->actor;
                }
            }
        }
        if (best == nullptr) return nullptr;
        joan::orig::segment_point_at(joan::orig::arg::mptr(out), joan::orig::arg::ptr(ray), joan::orig::arg::f32(bestd));  // 0x0060de40
        return best;
    }

    // ------------------------------------------------------------ 0x00449b30
    i32 grp_cnt() { return jo::view<i8>(jo::self<jo::Player>(this)->group_counts[jo::self<jo::Player>(this)->group]); }
    u32& grp_id(i32 i) { return jo::self<jo::Player>(this)->group_units[jo::self<jo::Player>(this)->group * 100 + i]; }
    bool grp_has(i32 id) {
        for (i32 j = 0; j < jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0]); ++j) {
            if (jo::view<i32>(jo::self<jo::Player>(this)->group_units[j]) == id) return true;
        }
        return false;
    }
    void grp_add(i32 id) {
        jo::view<i32>(jo::self<jo::Player>(this)->group_units[jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0])]) = id;
        jo::self<jo::Player>(this)->group_counts[0] = static_cast<u8>(jo::self<jo::Player>(this)->group_counts[0] + 1);
    }

    std::uintptr_t select_449b30(u32 arg) {
        c_select_449b30.hit();
        u32 v10[3];
        const u32 r1 = joan::orig::arg::as<u32>(joan::orig::as_table(obj(d::g_mouse))->at_676(0));  // 0x0043c3f0
        const u32 r2 = joan::orig::arg::as<u32>(joan::orig::as_table(obj(d::g_mouse))->at_672(0));  // 0x0043c3c0
        u8* v14 = joan::orig::arg::as<u8*>(joan::orig::as_world(this)->pick_object_447ab0(joan::orig::arg::i32(r2), joan::orig::arg::i32(r1), joan::orig::arg::mptr(v10)));         // 0x00447ab0
        i32 v18 = (jo::self<jo::Player>(this)->group == 0 && jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0]) != 0) ? 1 : 0;
        if (arg != 0) {
            joan::orig::as_group(this)->release_all();  // 0x00451430
            jo::self<jo::Player>(this)->group_counts[0] = 0;
            jo::view<u8>(jo::self<jo::Player>(this)->group) = 0;
            jo::self<jo::Player>(this)->selected_commander_id = 0;
            v18 = 0;
        } else if (jo::self<jo::Player>(this)->group != 0) {
            jo::self<jo::Player>(this)->group_counts[0] = 0;
            for (i32 i = 0; i < grp_cnt(); ++i) {
                if (unit_dead(grp_id(i))) continue;
                if (unit_busy(unit_rec(grp_id(i))) != 0) continue;
                jo::self<jo::Player>(this)->group_units[jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0])] = grp_id(i);
                jo::self<jo::Player>(this)->group_counts[0] = static_cast<u8>(jo::self<jo::Player>(this)->group_counts[0] + 1);
            }
            jo::view<u8>(jo::self<jo::Player>(this)->group) = 0;
            jo::self<jo::Player>(this)->selected_commander_id = 0;
        }
        {
            const u8 b = joan::orig::arg::as<u8>(joan::orig::as_marker(obj(d::g_mini_map))->visible());  // 0x00717650
            if (b != 0) return static_cast<u32>(static_cast<i32>(static_cast<i8>(b))) & 0xffffff00u;
        }
        if (v14 == nullptr) return 0;
        if (jo::view<u32>(jo::as_actor(v14)->kind) != 0xb) return 0;
        u8* v20 = jo::view<u8*>(jo::as_actor(v14)->owner);
        if (v20 == nullptr) return addr(v14) & 0xffffff00u;
        if (unit_dead(jo::view<u32>(jo::as_unit(v20)->rider_id)) == 0) v20 = unit_rec(jo::view<u32>(jo::as_unit(v20)->rider_id));
        {
            const u32 r = thiscall<u32>(fn::Unit_can_target_453620, this, static_cast<i32>(jo::as_unit(v20)->id));  // 0x00453620
            if ((r & 0xffu) == 0) return r & 0xffffff00u;
        }
        if (joan::orig::arg::as<u8>(joan::orig::as_unit(v20)->is_allied_with(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(jo::self<jo::Player>(this)->unit_id_))))) == 0) {  // 0x005095e0 (word)
            if (static_cast<i32>(jo::as_unit(v20)->id) != jo::self<jo::Player>(this)->unit_id_) return addr(v20);
        }
        if (!grp_has(jo::as_unit(v20)->id)) grp_add(jo::as_unit(v20)->id);
        jo::view<u8>(jo::self<jo::Player>(this)->group) = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(v20)->leads()) != 0) {  // 0x0044a660
            for (i32 k = 0; k < jo::as_group(jo::as_unit(v20)->group)->count; ++k) {
                if (jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0]) >= 0x64) break;
                const i32 id = jo::as_group(jo::as_unit(v20)->group)->members[k];
                if ((joan::orig::arg::as<u32>(joan::orig::as_unit(this)->can_target_453620(id)) & 0xffu) == 0) continue;  // 0x00453620
                if (!grp_has(jo::as_group(jo::as_unit(v20)->group)->members[k])) grp_add(jo::as_group(jo::as_unit(v20)->group)->members[k]);
            }
        }
        if (v18 == 0 && jo::as_unit(v20)->rank >= 4) {
            jo::view<i32>(jo::self<jo::Player>(this)->selected_commander_id) = jo::as_unit(v20)->id;
        } else {
            jo::self<jo::Player>(this)->selected_commander_id = 0;
        }
        Path p;
        joan::orig::as_string(&p)->construct(str(d::s_RTS));      // 0x005a11a0 'RTS'
        u8 heroes = 0;
        for (i32 i = 0; i < grp_cnt(); ++i) {
            if (unit_dead(grp_id(i))) continue;
            if (unit_busy(unit_rec(grp_id(i))) != 0) continue;
            joan::orig::as_unit(unit_rec(grp_id(i)))->attach_weapon_model(1);  // 0x00519250
            if (jo::as_unit(unit_rec(grp_id(i)))->rank >= 4) {
                if (heroes == 0) {
                    path_append(p, joan::orig::arg::as<const char*>(joan::orig::as_player(this)->name_tag(joan::orig::arg::u32(grp_id(i)))));  // 0x00453da0
                    path_append(p, str(d::s_008));                                     // '008'
                }
                heroes = static_cast<u8>(heroes + 1);
            }
        }
        if (static_cast<i8>(heroes) != 1) {
            path_assign(p, str(d::s_RTS_LTINF_00));  // 'RTS_LTINF_00'
            const u32 n = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->random_range(2, 4));  // 0x00590690
            joan::orig::as_string(&p)->append(joan::orig::arg::i32(n));                   // 0x005a15d0
        }
        if (grp_cnt() == 1) {
            path_assign(p, str(d::s_RTS));  // 'RTS'
            path_append(p, joan::orig::arg::as<const char*>(joan::orig::as_player(this)->name_tag(joan::orig::arg::u32(grp_id(0)))));  // 0x00453da0
            if (jo::as_unit(unit_rec(grp_id(0)))->rank >= 4) {
                path_append(p, str(d::s_008));  // '008'
            } else {
                u8* rec = unit_rec(grp_id(0));
                u8* ty = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(rec)->type_id)))));  // 0x0042d0a0
                if (jo::as_unit_type(ty)->unit_class == 8) {
                    path_assign(p, str(d::s_RTS_LTINF_001));  // 'RTS_LTINF_001'
                } else {
                    path_append(p, str(d::s_001));  // '001'
                }
            }
        }
        const char* s = joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str());  // 0x00417d90
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_sound(obj(d::g_sfx_player))->play_kind(s));    // 0x00492c40
        return (r & 0xffffff00u) | 1u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_01", name, addr, fn)
B("Pick::object_448340", 0x00448340, &H::pick_448340);
B("Group::select_449b30", 0x00449b30, &H::select_449b30);
#undef B

}  // namespace game_b104_01
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_target, "Unit::target_point");

struct H {
    // ------------------------------------------------------------ 0x00448870 (ret 8)
    float* target_point(float* out, void* p) {
        c_target.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->is_busy()) == 0) {  // 0x004b62b0
            std::memcpy(out, jo::self<jo::Unit>(this)->position_, 12);
            return out;
        }
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->leads()) != 0) {  // 0x0044a660
            std::memcpy(out, jo::self<jo::Unit>(this)->position_, 12);
            return out;
        }
        std::uint8_t* const o = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_unit(this)->leader());  // 0x004216d0
        if (o == nullptr) {
            std::memcpy(out, jo::self<jo::Unit>(this)->position_, 12);
            return out;
        }
        Vec3 tmp;
        if (p != nullptr) {
            const Vec3 r = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&tmp, joan::orig::arg::ptr(p), joan::orig::arg::ptr(jo::self<jo::Unit>(this)->formation_offset)));  // 0x0060cc20
            std::memcpy(out, &r, 12);
            return out;
        }
        const Vec3 r = *joan::orig::arg::as<Vec3*>(joan::orig::vec_add(&tmp, joan::orig::arg::ptr(&jo::as_unit(o)->group->x), joan::orig::arg::ptr(jo::self<jo::Unit>(this)->formation_offset)));  // 0x0060cc20
        std::memcpy(out, &r, 12);
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Unit::target_point", 0x00448870, &H::target_point);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b115_12.cpp (fix/uninit)
namespace {
namespace game_b115_12 {

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;

u8* gptr(img::Addr a) { return global<u8*>(a); }
i32 w16(i32 v) { return static_cast<i32>(static_cast<i16>(static_cast<u16>(static_cast<u32>(v) & 0xffffu))); }
using joan::orig::helpers::wrap_mul;
using joan::orig::helpers::fbits;
u32 bits(float f) {
    u32 b;
    std::memcpy(&b, &f, sizeof b);
    return b;
}

constexpr img::Addr kUnits(d::g_units);

JOAN_COUNTER(c_cull, "Viewer::cull_visible_4489c0");

void* unit_of(i32 id) { return joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 unit_dead(i32 id) { return joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70

struct H {
    // -------------------------------------------------------------- 0x004489c0
    // Emulated frame [ebp-0x2e4, ebp-0x4): fr[k] = [ebp-0x2e4 + 4k].
    enum : i32 { kBound = 100, kOld = 101, kPos = 102, kBox = 105, kPlanes = 136, kCorners = 160, kFrame = 184 };
    static float frf(const u32* fr, i32 k) { return fbits(fr[k]); }
    // True when the point is on the inner side of all six planes (d >= [0x9cf5c4] for each).
    static bool inside_planes(const u32* fr, float px, float py, float pz) {
        const float k0 = global<float>(d::k_9cf5c4);
        for (i32 j = 0; j < 6; ++j) {
            const i32 p = kPlanes + 4 * j;
            const float d = ((px * frf(fr, p) + py * frf(fr, p + 1)) + pz * frf(fr, p + 2)) + frf(fr, p + 3);
            if (!(d >= k0)) return false;
        }
        return true;
    }
    static float corner_coord(const Fpu& fpu, float z, float f, i32 a, i32 size) {
        const float p = z * f;
        double t = fpu.add(static_cast<double>(a), static_cast<double>(a));
        t = fpu.div(t, static_cast<double>(size));
        t = fpu.sub(t, static_cast<double>(global<float>(d::flt_1)));
        return static_cast<float>(fpu.mul(static_cast<double>(p), t));
    }
    static i32 list_at(u8* s, i32 k) { return jo::view<i32>(jo::as_player(s)->group_units[k]); }
    static void list_push(u8* s, i32 id) {
        jo::view<i32>(jo::as_player(s)->group_units[static_cast<i32>(jo::view<i8>(jo::as_player(s)->group_counts[0]))]) = id;
        jo::as_player(s)->group_counts[0] = static_cast<u8>(jo::as_player(s)->group_counts[0] + 1);
    }
    u8 cull_visible(i32 force) {
        c_cull.hit();
        u8* const s = reinterpret_cast<u8*>(this);
        u32 fr[kFrame];
        std::memset(fr, 0, sizeof fr);
        reinterpret_cast<void(JOAN_STDCALL*)(void*, u32, i32, void*)>(fn::code_value(fn::vector_constructor_iterator))(&fr[kPlanes], 0x10, 6,
                                                                              fn::code_ptr(fn::Plane4_Plane4_425700));  // 0x00449af0
        joan::orig::as_camera(gptr(d::g_camera))->update_matrices();                           // 0x006141a0
        const u8* const mp = joan::orig::arg::as<const u8*>(joan::orig::as_dialog(gptr(d::g_camera))->reset_name());  // 0x00616f80
        float m[16];
        std::memcpy(m, mp, sizeof m);
        const i32 width = jo::as_camera(gptr(d::g_camera))->view_width;
        const i32 height = jo::as_camera(gptr(d::g_camera))->view_height;
        i32 x0 = 0, x1 = 0, y0 = 0, y1 = 0;
        {
            const i32 ia = global<i32>(d::g_mouse_x);
            if (static_cast<double>(ia) > static_cast<double>(jo::as_player(s)->drag_x)) {
                x0 = ftol(static_cast<double>(jo::as_player(s)->drag_x));
                x1 = global<i32>(d::g_mouse_x);
            } else {
                x0 = global<i32>(d::g_mouse_x);
                x1 = ftol(static_cast<double>(jo::as_player(s)->drag_x));
            }
            const i32 ib = global<i32>(d::g_mouse_y);
            if (static_cast<double>(ib) > static_cast<double>(jo::as_player(s)->drag_y)) {
                y0 = ftol(static_cast<double>(jo::as_player(s)->drag_y));
                y1 = global<i32>(d::g_mouse_y);
            } else {
                y0 = global<i32>(d::g_mouse_y);
                y1 = ftol(static_cast<double>(jo::as_player(s)->drag_y));
            }
        }
        const float f120 = m[11] / m[0];
        const float f124 = m[11] / m[5];
        const Fpu fpu{true};
        // corner k at fr[kCorners + 3k]: (near / far, x0 / x1, y0 / y1)
        const u32 zn_bits = jo::view<u32>(jo::as_camera(gptr(d::g_camera))->near_plane);
        const u32 zf_bits = jo::view<u32>(jo::as_camera(gptr(d::g_camera))->far_plane);
        struct CornerSpec {
            bool far_plane;
            bool right;
            bool top;
        };
        const CornerSpec spec[8] = {{false, false, false}, {false, true, false}, {false, true, true},
                                    {false, false, true},  {true, false, false}, {true, true, false},
                                    {true, true, true},    {true, false, true}};
        for (i32 k = 0; k < 8; ++k) {
            const u32 zb = spec[k].far_plane ? zf_bits : zn_bits;
            const float z = fbits(zb);
            fr[kCorners + 3 * k] = bits(corner_coord(fpu, z, f120, spec[k].right ? x1 : x0, width));
            fr[kCorners + 3 * k + 1] = bits(corner_coord(fpu, z, f124, spec[k].top ? y1 : y0, height));
            fr[kCorners + 3 * k + 2] = zb;
        }
        for (i32 k = 0; k < 8; ++k) {
            void* const mat = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_camera(gptr(d::g_camera))->bone_))->matrix());  // 0x0060ef80
            import_call<void*>(d::iat_v3d_87_point_transform_2, &fr[kCorners + 3 * k], mat);                       // point_transform
        }
        joan::orig::as_aabb(&fr[kBox])->set_point(&fr[kCorners]);  // 0x0060d430
        for (i32 k = 1; k < 8; ++k) joan::orig::as_aabb(&fr[kBox])->extend(&fr[kCorners + 3 * k]);  // 0x0060d490
        auto corner = [&fr](i32 k) { return static_cast<void*>(&fr[kCorners + 3 * k]); };
        auto plane = [&fr](i32 j) { return static_cast<void*>(&fr[kPlanes + 4 * j]); };
        joan::orig::plane_from_points(joan::orig::arg::mptr(plane(0)), joan::orig::arg::ptr(corner(0)), joan::orig::arg::ptr(corner(1)), joan::orig::arg::ptr(corner(2)));  // 0x0060d920
        joan::orig::plane_from_points(joan::orig::arg::mptr(plane(1)), joan::orig::arg::ptr(corner(4)), joan::orig::arg::ptr(corner(7)), joan::orig::arg::ptr(corner(6)));
        joan::orig::plane_from_points(joan::orig::arg::mptr(plane(2)), joan::orig::arg::ptr(corner(1)), joan::orig::arg::ptr(corner(0)), joan::orig::arg::ptr(corner(4)));
        joan::orig::plane_from_points(joan::orig::arg::mptr(plane(3)), joan::orig::arg::ptr(corner(2)), joan::orig::arg::ptr(corner(1)), joan::orig::arg::ptr(corner(5)));
        joan::orig::plane_from_points(joan::orig::arg::mptr(plane(4)), joan::orig::arg::ptr(corner(3)), joan::orig::arg::ptr(corner(2)), joan::orig::arg::ptr(corner(6)));
        joan::orig::plane_from_points(joan::orig::arg::mptr(plane(5)), joan::orig::arg::ptr(corner(0)), joan::orig::arg::ptr(corner(3)), joan::orig::arg::ptr(corner(7)));

        if (force != 0) {
            joan::orig::as_group(s)->release_all();  // 0x00451430
            jo::as_player(s)->group_counts[11] = 0;
        } else if (jo::as_player(s)->group != 0) {
            const i32 n = wrap_mul(static_cast<i32>(jo::view<i8>(jo::as_player(s)->group_counts[0])), 4);
            std::memcpy(joan::orig::arg::mptr(jo::as_player(s)->group_units), joan::orig::arg::ptr(jo::as_player(s)->group_units + wrap_mul(static_cast<i32>(jo::as_player(s)->group), 100)), joan::orig::arg::u32(static_cast<u32>(n)));  // memmove
            jo::view<u8>(jo::as_player(s)->group) = 0;
            jo::as_player(s)->group_counts[0] = 0;
            jo::as_player(s)->selected_commander_id = 0;
            jo::as_player(s)->group_counts[11] = jo::as_player(s)->group_counts[0];
        } else {
            jo::as_player(s)->selected_commander_id = 0;
        }
        if (jo::as_player(s)->group != 0) {
            jo::as_player(s)->group_counts[0] = 0;
            jo::view<u8>(jo::as_player(s)->group) = 0;
            jo::as_player(s)->selected_commander_id = 0;
        } else {
            fr[kOld] = static_cast<u32>(static_cast<i32>(jo::view<i8>(jo::as_player(s)->group_counts[11])));
            fr[kBound] = static_cast<u32>(static_cast<i32>(jo::view<i8>(jo::as_player(s)->group_counts[0])));
            std::memset(&fr[0], i32{0}, u32{0x190});  // memset
            for (i32 i = 0; i < static_cast<i32>(fr[kBound]); ++i) {
                if (i >= kFrame) break;  // fix/uninit: would overwrite the `this` local (header)
                if (unit_dead(list_at(s, i)) != 0 ||
                    joan::orig::arg::as<u8>(joan::orig::as_unit(unit_of(list_at(s, i)))->dead()) != 0) {  // 0x0042d0e0
                    fr[i] = 0;
                } else {
                    fr[i] = static_cast<u32>(list_at(s, i));
                }
            }
            for (i32 i = static_cast<i32>(fr[kOld]); i < static_cast<i32>(fr[kBound]); ++i) {
                if (i < 0 || i >= kFrame) continue;  // fix/uninit: outside the emulated frame (header)
                if (unit_dead(static_cast<i32>(fr[i])) != 0) continue;
                float pos[3] = {0.0f, 0.0f, 0.0f};
                joan::orig::as_unit(unit_of(static_cast<i32>(fr[i])))->position(joan::orig::arg::mptr(pos));  // 0x004fedb0
                if (!inside_planes(fr, pos[0], pos[1], pos[2])) {
                    joan::orig::as_unit(unit_of(static_cast<i32>(fr[i])))->attach_weapon_model(i32{0});  // 0x00519250
                    fr[i] = 0;
                }
            }
            jo::as_player(s)->group_counts[0] = 0;
            jo::as_player(s)->group_counts[11] = 0;
            jo::view<u8>(jo::as_player(s)->group) = 0;
            jo::as_player(s)->selected_commander_id = 0;
            if (fr[kBound] != 0) {
                for (i32 i = 0; i < 100; ++i) {
                    if (unit_dead(static_cast<i32>(fr[i])) != 0) continue;
                    if (jo::view<i8>(jo::as_player(s)->group_counts[0]) >= 100) break;
                    list_push(s, static_cast<i32>(fr[i]));
                    if (i < static_cast<i32>(fr[kOld])) jo::as_player(s)->group_counts[11] = static_cast<u8>(jo::as_player(s)->group_counts[11] + 1);
                }
            }
        }
        // 0x004492ab: objects of the map cells inside the view box.
        i32 n = 0;
        if (global<void*>(d::g_world) == nullptr) return 0;
        void* const* const cells = joan::orig::arg::as<void**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(&fr[kBox], &n));  // 0x00429090
        for (i32 c = n - 1; c >= 0; --c) {
            u8* const cell = static_cast<u8*>(cells[c]);
            if (jo::view<i8>(jo::as_player(s)->group_counts[0]) >= 100) break;
            for (u8* o = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(cell)->get_90()); o != nullptr; o = joan::orig::arg::as<u8*>(joan::orig::get_341(joan::orig::arg::mptr(o)))) {  // 0x0054eec0 / 0x0054ef40
                if (unit_dead(jo::as_player(s)->unit_id_) != 0) continue;
                if (joan::orig::arg::as<u8>(joan::orig::as_unit(s)->can_target_453620(static_cast<i32>(jo::as_unit(o)->id))) == 0) continue;  // 0x00453620
                if (joan::orig::arg::as<u8>(joan::orig::as_unit(o)->is_allied_with(joan::orig::arg::i16(w16(static_cast<u16>(jo::as_player(s)->unit_id_))))) == 0 &&                    // 0x005095e0
                    static_cast<i32>(jo::as_unit(o)->id) != jo::as_player(s)->unit_id_) {
                    continue;
                }
                joan::orig::as_unit(o)->position(&fr[kPos]);
                const bool inside = inside_planes(fr, frf(fr, kPos), frf(fr, kPos + 1), frf(fr, kPos + 2));
                if (jo::view<i8>(jo::as_player(s)->group_counts[0]) >= 100) break;  // next cell
                if (!inside) continue;
                bool found = false;
                for (i32 k = 0; k < jo::view<i8>(jo::as_player(s)->group_counts[0]); ++k) {
                    if (list_at(s, k) == static_cast<i32>(jo::as_unit(o)->id)) found = true;
                }
                if (!found) list_push(s, static_cast<i32>(jo::as_unit(o)->id));
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(o)->leads()) == 0) continue;  // 0x0044a660
                for (i32 mbr = 0; mbr < static_cast<i32>(jo::as_group(jo::as_unit(o)->group)->count); ++mbr) {
                    const i32 id2 = jo::as_group(jo::as_unit(o)->group)->members[mbr];
                    if (joan::orig::arg::as<u8>(joan::orig::as_unit(s)->can_target_453620(joan::orig::arg::i32(id2))) == 0) continue;
                    bool found2 = false;
                    for (i32 k = 0; k < jo::view<i8>(jo::as_player(s)->group_counts[0]); ++k) {
                        if (list_at(s, k) == static_cast<i32>(jo::as_group(jo::as_unit(o)->group)->members[mbr])) found2 = true;
                    }
                    if (found2) continue;
                    if (jo::view<i8>(jo::as_player(s)->group_counts[0]) >= 100) break;
                    list_push(s, static_cast<i32>(jo::as_group(jo::as_unit(o)->group)->members[mbr]));
                }
            }
        }
        jo::view<u8>(jo::as_player(s)->group) = 0;
        jo::as_player(s)->selected_commander_id = 0;
        for (i32 i = 0; i < static_cast<i32>(jo::view<i8>(jo::as_player(s)->group_counts[jo::as_player(s)->group])); ++i) {
            if (unit_dead(jo::view<i32>(jo::as_player(s)->group_units[jo::as_player(s)->group * 100 + i])) != 0) continue;
            if (joan::orig::arg::as<u8>(joan::orig::as_unit(unit_of(jo::view<i32>(jo::as_player(s)->group_units[jo::as_player(s)->group * 100 + i])))->dead()) != 0) continue;
            joan::orig::as_unit(unit_of(jo::view<i32>(jo::as_player(s)->group_units[jo::as_player(s)->group * 100 + i])))->attach_weapon_model(i32{1});
        }
        return 1;
    }

    // -------------------------------------------------------------- 0x006a9570
    struct FV {
        float x, y, z;
    };
    static_assert(sizeof(FV) == 12, "sizes");
};

// 0x004489c0: emulated frame; indices outside [ebp-0x2e4, ebp-0x4) are skipped / stop the copy.
JOAN_REPLACE("fix/uninit", "Viewer::cull_visible_4489c0", 0x004489c0, &H::cull_visible);

}  // namespace game_b115_12
}  // namespace

// ---------------------------------------- from modules/game_world5.cpp (game/world5)
namespace {
namespace game_world5 {

struct Obj {};

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

constexpr img::Addr kUnits(d::g_items);
constexpr img::Addr kSounds(d::g_physic_sys);

JOAN_COUNTER(c_unload_files, "Cell::unload_files");

struct H {
    // 0x00449780 Cell::unload_files.
    void unload_files() {
        c_unload_files.hit();
        std::uint8_t* next = nullptr;
        for (std::uint8_t* p = jo::view<std::uint8_t*>(jo::self<jo::Cell>(this)->objects_head); p != nullptr; p = next) {
            next = jo::view<std::uint8_t*>(jo::as_map_object(p)->cell_next);
            void* const def = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(img::obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::as_map_object(p)->type_id))));  // 0x00425ce0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_item_def(def)->is_soldier()) == 0)  // 0x004a4050
                joan::orig::as_units(img::obj(kUnits))->remove_49f370(static_cast<std::int32_t>(jo::as_map_object(p)->index));  // 0x0049f370
            else
                virtual_call(p, 0x20);
        }
        if (jo::view<void*>(jo::self<jo::Cell>(this)->world->scene) != nullptr) {
            for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Cell>(this)->world->scene))->count_5c()) - 1; i >= 0; --i) {  // 0x005eed70
                std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Cell>(this)->world->scene))->occluder(joan::orig::arg::i32(i)));  // 0x005eed90
                // The anti-portal Mesh that World::read_anti_portal (0x00559470) tagged with set_30_34(cell, 1).
                jo::Mesh* const m = jo::as_mesh(e);
                if (static_cast<void*>(m->vertices_f6) == static_cast<void*>(this) && reinterpret_cast<std::uintptr_t>(m->vertices_f7) == 1u) {
                    joan::orig::as_scene(jo::view<void*>(jo::self<jo::Cell>(this)->world->scene))->remove_5c(joan::orig::arg::i32(i));  // 0x005eedc0
                    break;
                }
            }
        }
        void* const slots = img::obj(d::g_particle_effect_array);
        for (std::int32_t n = jo::self<jo::Cell>(this)->sector_count; n > 0; --n) {
            for (std::int32_t j = joan::orig::as_dynarray(slots)->size(); j >= 1; --j) {  // 0x00417ad0
                if (joan::orig::as_dynarray(slots)->empty_at(joan::orig::arg::i32(j)) != 0) continue;  // 0x0041bd90
                jo::Actor* const owner = jo::as_model_obj(joan::orig::as_dynarray(slots)->ptr_at(joan::orig::arg::i32(j)))->fx.object_;  // 0x0041bd60
                void* const sector = joan::orig::arg::as<void*>(joan::orig::as_world(this)->sector(joan::orig::arg::i32(n)));  // 0x0054e1b0
                if (static_cast<void*>(owner->scene) == sector) joan::orig::as_effect_registry(slots)->remove(joan::orig::arg::i32(j));  // 0x0041bbf0
            }
            joan::orig::as_plane(this)->remove_material(joan::orig::arg::i32(n));  // 0x0042b640
        }
        if (jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b) != nullptr || jo::view<void*>(jo::self<jo::Cell>(this)->path_space) != nullptr) joan::orig::as_players(img::obj(kSounds))->remove_cell(jo::self<jo::Cell>(this)->path_space, jo::self<jo::Cell>(this)->path_space_b);  // 0x006ad450
        if (jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b) != nullptr) {
            if (jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b) != jo::view<void*>(jo::self<jo::Cell>(this)->path_space)) {
                void* const p = jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b);
                if (p != nullptr) joan::orig::as_pfd_data(p)->destroy(1u);  // 0x004201a0 delete
            }
            jo::view<void*>(jo::self<jo::Cell>(this)->path_space_b) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Cell>(this)->path_space) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Cell>(this)->path_space);
            if (p != nullptr) joan::orig::as_pfd_data(p)->destroy(1u);
            jo::view<void*>(jo::self<jo::Cell>(this)->path_space) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Cell>(this)->tracks) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Cell>(this)->tracks);
            if (p != nullptr) joan::orig::as_tracks(p)->destroy(1u);  // 0x0041dfa0 delete
            jo::view<void*>(jo::self<jo::Cell>(this)->tracks) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Cell>(this)->camera_zones) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Cell>(this)->camera_zones);
            if (p != nullptr) joan::orig::as_camera_zones(p)->destroy(1u);  // 0x00420150 delete
            jo::view<void*>(jo::self<jo::Cell>(this)->camera_zones) = nullptr;
        }
    }
};

#define W5(name, addr, fn) JOAN_REPLACE("game/world5", name, addr, fn)
W5("Cell::unload_files", 0x00449780, &H::unload_files);
#undef W5

}  // namespace game_world5
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

void* const kTypes = obj(d::g_item_res);

void* type_record(i16 type) { return joan::orig::arg::as<void*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(type))); }  // 0x00425ce0

struct H {
    // ---------------------------------------------------------------- 0x00449a30
    std::uintptr_t set_type_449a30(u32 type) {
        joan::orig::as_map_object(this)->set_kind(joan::orig::arg::i16(type));  // 0x004178e0
        u8* const rec = static_cast<u8*>(type_record(static_cast<i16>(type)));
        jo::self<jo::Wall>(this)->max_hp = jo::view<u32>(jo::as_item_def(rec)->i32_5e);
        jo::self<jo::Wall>(this)->u32_c6 = jo::self<jo::Wall>(this)->max_hp;
        jo::self<jo::Wall>(this)->u32_ce = jo::view<u32>(jo::as_item_def(rec)->model_values[0]);
        jo::self<jo::Wall>(this)->invincible = 0;
        jo::self<jo::Wall>(this)->busy = jo::view<u32>(jo::as_item_def(rec)->model_count);
        return addr(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Unit::set_type_449a30", 0x00449a30, &H::set_type_449a30);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {

JOAN_COUNTER(c_vector_ctor, "vector constructor iterator");

// 0x00449af0 `vector constructor iterator'(array, size, count, ctor), stdcall (ret 0x10): calls
// the argument-less thiscall constructor on each element while --count >= 0.
void JOAN_STDCALL vector_ctor(void* array, std::int32_t size, std::int32_t count, void* ctor) {
    c_vector_ctor.hit();
    using Ctor = void(JOAN_THISCALL*)(void*);
    std::uint8_t* p = static_cast<std::uint8_t*>(array);
    for (;;) {
        count = static_cast<std::int32_t>(static_cast<std::uint32_t>(count) - 1u);
        if (count < 0) break;
        reinterpret_cast<Ctor>(ctor)(p);
        p += size;
    }
}

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("vector constructor iterator", 0x00449af0, &vector_ctor);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b111_19.cpp (fix/uninit)
namespace {
namespace game_b111_19 {

using joan::call::thiscall;


static_assert(sizeof(Vec3) == 12, "vec3");

JOAN_COUNTER(c_pick, "PickCursor::use_44a430 (fix/uninit)");

// 0x004eef00 (thunk 0x0040a164, ret 0x40) on the event queue 0xa5ccd0.
void post_event(u32 type, i32 w10, i32 w18, Vec3 v, u32 t) {
    joan::orig::as_triggers(obj(d::g_a5ccd0))->add(joan::orig::arg::u32(type), joan::orig::arg::u32(u32{2}), joan::orig::arg::i16(w10), joan::orig::arg::u32(u32{1}), joan::orig::arg::i16(w18), static_cast<std::int8_t>(joan::orig::arg::i32(u32{1})), v, joan::orig::arg::u32(u32{0x3f800000}), static_cast<std::int8_t>(joan::orig::arg::i32(u32{0})), joan::orig::arg::u32(t), joan::orig::arg::u32(t), static_cast<std::int8_t>(joan::orig::arg::i32(u32{1})), nullptr, joan::orig::arg::i16(u32{0}));
}

// ---- fix/uninit: 0x00433af0 ------------------------------------------------------------------
struct H {
    // ---- fix/uninit: 0x0044a430 --------------------------------------------------------------
    u8 pick_use() {
        c_pick.hit();
        Vec3 hit{0.0f, 0.0f, 0.0f};  // [ebp-0x10]: 12 bytes written by 0x0060de40 via 0x00448340
        const u32 r1 = joan::orig::arg::as<u32>(joan::orig::as_table(obj(d::g_mouse))->at_676(i32{0}));  // 0x0043c3f0
        const u32 r2 = joan::orig::arg::as<u32>(joan::orig::as_table(obj(d::g_mouse))->at_672(i32{0}));  // 0x0043c3c0
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_player(this)->rts_pick_object(joan::orig::arg::u32(r2), joan::orig::arg::u32(r1), &hit));           // 0x00448340
        if (p == nullptr) return 0;
        if (jo::as_actor(p)->kind != 1) return 0;
        u8* const e = jo::view<u8*>(jo::as_actor(p)->owner);
        const i32 kind = jo::as_map_object(e)->class_id;
        // fix/uninit: the original passes never-written stack vectors ([ebp-0x24..-0x1c] /
        // [ebp-0x30..-0x28]); a zero vector is used.
        const Vec3 zero{0.0f, 0.0f, 0.0f};
        if (kind == 1) {
            if (jo::as_map_object(e)->side != 0) {
                u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))));  // 0x0041b3c0
                if (static_cast<i32>(jo::as_unit(u)->side) != static_cast<i32>(jo::as_map_object(e)->side)) return 0;
            }
            u8* const v = thiscall<u8*>(static_cast<const std::uintptr_t*>(jo::as_map_object(e)->vtable)[0x4c / 4], e);  // virtual +0x4c
            if (jo::view<i8>(jo::as_gate(v)->need_key) != 0) return 0;  // virtual +0x4c = the Gate (0x0048e900 returns this)
            const u32 t = global<u32>(d::g_now);
            post_event(0x20, jo::as_map_object(e)->portrait_unit, static_cast<i16>(jo::self<jo::Player>(this)->unit_id_), zero, t);
            return 1;
        }
        if (kind == 0xa) {
            u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(e)->type_id))));  // 0x00425ce0
            if (jo::view<i8>(jo::as_item_def(g)->open_with_interact) == 0) return 0;
            u8* const g2 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(e)->type_id))));
            if (jo::view<i8>(jo::as_item_def(g2)->need_key) != 0) return 0;
            const u32 t = global<u32>(d::g_now);
            post_event(0x200, jo::as_map_object(e)->portrait_unit, static_cast<i16>(jo::self<jo::Player>(this)->unit_id_), zero, t);
            return 1;
        }
        return 0;
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: 0x0044a430 - never-written event vector -> zero vector.
F("PickCursor::use_44a430", 0x0044a430, &H::pick_use);
#undef F

}  // namespace game_b111_19
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

using joan::call::thiscall;

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_drain, "Obj::drain");

void* unit(i32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0

struct H {
    // 0x0044a5f0
    float drain(i32 id, i32) {
        c_drain.hit();
        void* const u = unit(id);
        const float t = joan::orig::arg::as<float>(joan::orig::as_unit(u)->attack_value_5104f0()) * global<float>(d::flt_0_5);  // 0x005104f0
        jo::self<jo::Order0c_a>(this)->amount = joan::hook::x87::ftol(Fpu{true}.sub(static_cast<double>(jo::self<jo::Order0c_a>(this)->amount), t));
        if (jo::self<jo::Order0c_a>(this)->amount < 0) jo::self<jo::Order0c_a>(this)->amount = 0;
        return t;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::drain", 0x0044a5f0, &H::drain);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units {

struct H {
    std::int32_t leads() { return jo::self<jo::Unit>(this)->group != nullptr ? 1 : 0; }  // 0x0044a660
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("Unit::leads", 0x0044a660, &H::leads);
#undef U1

}  // namespace game_units
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
namespace {
namespace game_b104_02 {

using joan::call::thiscall;

using u16 = std::uint16_t;

void* const kKinds = obj(d::g_types);

using joan::orig::helpers::addr;

static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kUnits = obj(d::g_units);
void* const kSpeech = obj(d::g_sfx_player);

i32 word_arg(u16 w) { return static_cast<i32>(static_cast<i16>(w)); }

// common thunks
i32 unit_dead(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_ptr(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* kind_of(u16 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(word_arg(w)))); }  // 0x0042d0a0
u32 speech(const char* s) { return joan::orig::arg::as<u32>(joan::orig::as_sound(kSpeech)->play_kind(s)); }    // 0x00492c40

// 0x004bb660 + 0x00584580 on the unit's +0x2a1 helper
void detach_helper(u8* u) {
    joan::orig::as_action(jo::as_unit(u)->orders)->dispatch(joan::orig::arg::mptr(static_cast<void*>(u)));  // 0x004bb660
    joan::orig::dynarray(jo::as_unit(u)->orders)->zap(1);                   // 0x00584580
}

JOAN_COUNTER(c_siege_release, "World::siege_release_44a690");

struct H {
    // ------------------------------------------------------------------ 0x0044a690
    void release_list(u8* engine, jo::ArrI32* jo::Unit::* list, bool full) {
        for (i32 i = 0;; ++i) {
            const u8* kind = kind_of(jo::view<u16>(jo::as_unit(engine)->type_id));
            if (!(i < jo::as_unit_type(kind)->slot_count)) break;
            if (unit_dead(jo::view<i32*>(jo::as_unit(engine)->*list)[i]) != 0) continue;
            u8* const u = unit_ptr(jo::view<i32*>(jo::as_unit(engine)->*list)[i]);
            joan::orig::as_unit(u)->release_slot();  // 0x00528300
            if (jo::as_unit(u)->orders != nullptr && jo::view<u32>(jo::as_unit(u)->orders->current.kind) != 0) detach_helper(u);
            jo::view<i32*>(jo::as_unit(engine)->mask_slots_b)[i] = 0;
            jo::view<i32*>(jo::as_unit(engine)->mask_slots)[i] = 0;
            jo::view<u32>(jo::as_unit(u)->weapon_slot) = 0;
            jo::view<u32>(jo::as_unit(u)->weapon) = 0;
            if (full) {
                jo::view<u16>(jo::as_unit(u)->anim_special) = 0;
                joan::orig::as_cell_object(u)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
                joan::orig::as_unit(u)->play_idle(0, 1);        // 0x00508b90
                joan::orig::unit_refresh_items(joan::orig::arg::i16(word_arg(jo::view<u16>(jo::as_unit(u)->id))));  // 0x0065b9b0
            }
        }
    }
    std::uintptr_t siege_release_44a690() {
        c_siege_release.hit();
        Vec3 pos;  // [ebp-0x10]
        const i32 y = joan::orig::arg::as<i32>(joan::orig::as_table(obj(d::g_mouse))->at_676(0));  // 0x0043c3f0
        const i32 x = joan::orig::arg::as<i32>(joan::orig::as_table(obj(d::g_mouse))->at_672(0));  // 0x0043c3c0
        u8* const picked = joan::orig::arg::as<u8*>(joan::orig::as_world(this)->pick_object_447ab0(joan::orig::arg::i32(x), joan::orig::arg::i32(y), &pos));  // 0x00447ab0
        if (picked == nullptr) return 0;
        if (jo::as_actor(picked)->kind != 0xb) return addr(picked);
        u8* const engine = jo::view<u8*>(jo::as_actor(picked)->owner);
        if (jo::as_unit(engine)->crew_slots_ready == 0) return 0;
        release_list(engine, &jo::Unit::mask_slots_b, false);
        release_list(engine, &jo::Unit::mask_slots, true);
        const u32 r = speech(str(d::s_RTS_SIEGE_007));  // "RTS_SIEGE_007"
        return (r & 0xffffff00u) | 1u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("World::siege_release_44a690", 0x0044a690, &H::siege_release_44a690);
#undef B

}  // namespace game_b104_02
}  // namespace

// ---------------------------------------- from modules/game_b89_04.cpp (game/b89_04)
namespace {
namespace game_b89_04 {

using joan::call::thiscall;

constexpr Fpu kFpu{true};

JOAN_COUNTER(c_front2, "Group::front_point2");

// Shared body of 0x00451dc0 / 0x0044a950.
template <class H>  // the heading member is read as its raw dword
void front_point(void* self, float* out, H jo::Group::* heading, float jo::Group::* pos_x, float jo::Group::* pos_y) {
    std::uint32_t angle = 0;                                                // ebp-0x8 (1-byte object)
    joan::orig::as_angle(&angle)->construct();                                    // 0x00576120 (ctor, no-op)
    joan::orig::as_angle(&angle)->set(joan::orig::arg::f32(jo::view<std::uint32_t>(jo::as_group(self)->*heading)));     // 0x00576240 (float raw)
    if (jo::as_group(self)->formation != 1) return;
    const std::int32_t spacing = 100;   // ebp-0x10
    const std::int32_t front = -100;    // ebp-0x14
    std::int32_t width = 400;           // ebp-0x18 (uninitialised in the original when count > 25)
    const std::int32_t count = static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::as_group(self)->count));
    if (count <= 1)
        width = (1 - 1) * spacing;
    else if (count <= 4)
        width = (2 - 1) * spacing;
    else if (count <= 9)
        width = (3 - 1) * spacing;
    else if (count <= 0x10)
        width = (4 - 1) * spacing;
    else if (count <= 0x19)
        width = (5 - 1) * spacing;
    const std::int32_t gx = front - width / 2;  // ebp-0x1c
    const std::int32_t gy = 0;                  // ebp-0x20
    const float c1 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());  // 0x00576ac0
    const float t1 = static_cast<float>(kFpu.add(kFpu.mul(c1, static_cast<double>(gx)),
                                                  jo::as_group(self)->*pos_x));  // ebp-0x28
    const float s1 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());  // 0x005768d0
    out[0] = static_cast<float>(kFpu.sub(t1, kFpu.mul(s1, static_cast<double>(gy))));
    const float s2 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());
    const float t2 = static_cast<float>(kFpu.add(kFpu.mul(s2, static_cast<double>(gx)),
                                                  jo::as_group(self)->*pos_y));  // ebp-0x2c
    const float c2 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());
    out[1] = static_cast<float>(kFpu.add(kFpu.mul(c2, static_cast<double>(gy)), t2));
}

struct H {
    // ------------------------------------------------------------------------ 0x0044a950
    void group_front2(float* out) {
        c_front2.hit();
        front_point(this, out, &jo::Group::angle, &jo::Group::x, &jo::Group::y);
    }
};

#define B89(name, addr, fn) JOAN_REPLACE("game/b89_04", name, addr, fn)
B89("Group::front_point2", 0x0044a950, &H::group_front2);
#undef B89

}  // namespace game_b89_04
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using joan::call::thiscall;

void* const kUnitTable = obj(d::g_units);
void* const kRandom = obj(d::g_misc);

struct alignas(4) Path {  // 0x005a11a0 path string (frame slot 0x194 bytes)
    u8 bytes[0x194];
};

JOAN_COUNTER(c_select_slot, "Group::select_slot");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct H {
    // ------------------------------------------------------------------ 0x0044aad0
    u8 select_slot(i32 idx_arg) {
        c_select_slot.hit();
        const i32 idx = sx16(idx_arg);
        if (idx == 0) return 0;
        if (idx >= 10) return 0;
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[idx])) == 0) return 0;
        joan::orig::as_group(this)->release_all();  // 0x00451430
        jo::view<u8>(jo::self<jo::Player>(this)->group) = static_cast<u8>(idx_arg);
        for (i32 i = 0;; ++i) {
            const i32 s = static_cast<i32>(jo::self<jo::Player>(this)->group);
            if (!(i < static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[s])))) break;
            const auto id_at = [this, i]() { return jo::view<i32>(jo::self<jo::Player>(this)->group_units[static_cast<i32>(jo::self<jo::Player>(this)->group) * 100 + i]); };
            if (unit_missing(id_at()) != 0) continue;
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(unit_get(id_at()))->dead())) != 0) continue;  // 0x0042d0e0
            joan::orig::as_unit(unit_get(id_at()))->attach_weapon_model(1);  // 0x00519250
        }
        Path path;                                                           // ebp-0x19c
        joan::orig::as_string(&path)->construct(str(d::s_RTS_LTINF_00));                     // 0x005a11a0 "RTS_LTINF_00"
        const i32 r = joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->random_range(2, 4));              // 0x00590690
        joan::orig::as_string(&path)->append(joan::orig::arg::i32(r));                  // 0x005a15d0
        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
        joan::orig::as_sound(obj(d::g_sfx_player))->play_kind(s);                       // 0x00492c40
        return 1;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Group::select_slot", 0x0044aad0, &H::select_slot);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b110_16.cpp (game/b110_16)
namespace {
namespace game_b110_16 {

using joan::call::thiscall;
using joan::call::import_call;

using joan::orig::helpers::units;

// Virtual call without arguments through vtable slot `slot` (byte offset).
void* vcall(const void* self, std::int32_t slot) {  // slot: the entry's byte offset in the (x86) vtable
    const std::uintptr_t* const vtable = *static_cast<const std::uintptr_t* const*>(self);  // every vtable object: the table at +0
    return thiscall<void*>(vtable[slot / 4], self);
}

struct V3 {
    float v[3];
};
struct Ray {  // two points: 0x006154d0 writes both, 0x0060de40 reads both
    float p0[3];
    float p1[3];
};
struct Blk24 {
    u32 d[6];
};
struct Blk20 {
    u32 d[5];
};
struct State40 {
    u32 d[10];
};
static_assert(sizeof(V3) == 12 && sizeof(Ray) == 24 && sizeof(Blk24) == 24 && sizeof(Blk20) == 20 &&
                  sizeof(State40) == 40,
              "by-value sizes");

u32 bits(float f) {
    u32 b = 0;
    std::memcpy(&b, &f, 4);
    return b;
}

JOAN_COUNTER(c_pick, "Cursor::pick_44ac60");

struct H {
    // =================================================================== 0x0044ac60 helpers
    // Number of members of the current group: (char)this[0xf9 + (char)this[0x1236]].
    i32 group_count() {
        return jo::view<i8>(jo::self<jo::Player>(this)->group_counts[static_cast<i32>(jo::self<jo::Player>(this)->group)]);
    }
    u32 group_member(i32 k) {
        return jo::self<jo::Player>(this)->group_units[static_cast<i32>(jo::self<jo::Player>(this)->group) * 100 + k];
    }

    // Tests the object at [h + off] (h = list entry) against the ray; updates the nearest hit.
    // Corresponds to 0x0044af16..0x0044b35c (owner +0x2f5) and 0x0044b4cb..0x0044b911 (+0x86).
    // C::*m: the owner's actor member (Unit +0x2f5 / MapObject +0x86), read again at every use as the original does.
    template <class C>
    static void consider(void* h, jo::Actor* C::* m, Ray& ray, void*& best, float& best_d) {
        const auto act = [&]() -> jo::Actor* { return jo::self<C>(h)->*m; };
        void* const t = act()->plane;
        if (t != nullptr && static_cast<i32>(jo::as_plane(t)->vertex_format) == 7) {
            alignas(4) u8 sphere[16];
            joan::orig::as_actor(act())->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), joan::orig::arg::u32(i32{1}));  // 0x005acc80
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&ray, joan::orig::arg::mptr(static_cast<void*>(sphere)))) == 0) return;   // 0x0060e3f0
            float d = 2.0f;
            u8 hit = 0;
            if (act() == nullptr) return;
            joan::orig::as_bone(act()->bone)->update();  // 0x0060ee60
            void* const mat = act()->bone->matrix_;
            alignas(4) u8 inv[64];
            import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), mat);  // matrix_inverse(out, in)
            V3 a, b;
            import_call<void*>(d::iat_v3d_87_point_transform_3, &a, static_cast<void*>(ray.p0), static_cast<void*>(inv));  // point_transform
            import_call<void*>(d::iat_v3d_87_point_transform_3, &b, static_cast<void*>(ray.p1), static_cast<void*>(inv));
            const u32 shape = jo::as_actor(act())->aim_mode;
            if (shape == 0) return;
            if (shape == 1) {
                V3 tmp;
                const V3 dir = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &b, &a));  // 0x0060cc90: b - a
                void* const p = act()->aim_sphere;
                hit = joan::orig::arg::as<u8>(joan::orig::shape::sphere_hit(&d, a, dir, joan::orig::arg::mptr(p)));  // 0x006a0f90
            } else if (shape == 2) {
                V3 tmp;
                const V3 dir = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &b, &a));
                Blk24 blk;
                std::memcpy(&blk, act()->aim_box, sizeof blk);
                hit = joan::orig::arg::as<u8>(joan::orig::shape::box_hit(&d, a, dir, blk));  // 0x00698970
            } else if (shape == 3) {
                V3 tmp;
                const V3 dir = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &b, &a));
                Blk20 blk;
                std::memcpy(&blk, &act()->collide_kind, sizeof blk);
                hit = joan::orig::arg::as<u8>(joan::orig::shape::cylinder_hit(&d, a, dir, blk));  // 0x00699ec0
            }
            if (hit == 0) return;
            if (best == nullptr || best_d > d) {  // fcomp + test ah,0x41 / jne: ordered >
                best_d = d;
                best = act();
            }
            return;
        }
        if (t == nullptr) return;
        alignas(4) u8 sphere[16];
        joan::orig::as_actor(act())->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), joan::orig::arg::u32(i32{1}));  // 0x005acc80
        if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&ray, joan::orig::arg::mptr(static_cast<void*>(sphere)))) == 0) return;   // 0x0060e3f0
        void* mesh = nullptr;
        void* mesh_aux = nullptr;  // select_mesh writes a pointer (the bone) here: pointer-sized, or x64 clobbers the neighbouring local
        joan::orig::as_actor(act())->select_mesh(joan::orig::arg::mptr(global<void*>(d::g_camera)), i32{1}, &mesh, &mesh_aux);  // 0x005ad060
        float d = 0.0f;  // written by 0x005d4a80 whenever it returns non-zero
        if (joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->segment_hit(&d, &ray)) == 0) return;  // 0x005d4a80
        if (best == nullptr || best_d > d) {
            best_d = d;
            best = act();
        }
    }

    // Copies "name" (8 chars, strncpy) into +0x1258 unless the cursor id +0x1254 is already `id`.
    void set_cursor(u32 id, img::Addr name) {
        if (jo::self<jo::Player>(this)->point_kind == id) return;
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Player>(this)->point_name)), joan::orig::arg::str(str(name)), u32{8});  // strncpy
        jo::self<jo::Player>(this)->point_kind = id;
    }

    // Distance check of 0x0044c475 / 0x0044c5a9 / 0x0044c66b: length(p + 0xe - ([w + 0x6d] + 0x2a)).
    static double dist_to_site(void* site, void* unit_base) {
        V3 tmp;
        const V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::as_unit(unit_base)->position_), joan::orig::arg::ptr(site)));  // 0x0060cc90
        return import_call<double>(d::iat_v3d_87_length, &d);                                   // length
    }

    // 0x0044c25f: ground pick / resource sites.
    u8 pick_ground_44c25f(const V3& eye, i32* out_kind, i32* out_id) {
        V3 hit_pos{};
        const u8 ok = joan::orig::as_player(this)->ray_query(joan::orig::arg::u32(global<i32>(d::g_mouse_x)), joan::orig::arg::u32(global<i32>(d::g_mouse_y)), eye.v[0], eye.v[1], eye.v[2],
                                   &hit_pos);  // 0x00452830 (writes hit_pos on every true path)
        if (static_cast<i8>(ok) == 0) return 1;
        const i32 cell = joan::orig::arg::as<i32>(joan::orig::as_player(this)->find_place_at(hit_pos));  // 0x00452610
        if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(joan::orig::arg::i32(cell))) != 0 ||  // 0x0044cf80
            joan::orig::arg::as<i32>(joan::orig::as_group(this)->count_active()) > 1) {                     // 0x004524c0
            return group_count() != 0 ? 1 : 0;
        }
        void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(obj(d::g_places))->get_e(joan::orig::arg::i32(cell)));  // 0x0043c450
        const i32 type = jo::as_facility(o)->kind;
        if (type == 3) {
            void* const w = vcall(o, 0x24);  // the FlagFacility (0x00469310 returns this)
            if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_places))->empty_at_e(static_cast<i32>(jo::as_flag_facility(w)->i16_91))) != 0) return 1;
            if (jo::as_flag_facility(w)->list_79 == nullptr) return 1;
            const u32 leader = joan::orig::arg::as<u32>(joan::orig::as_group(this)->first_ready_member());  // 0x00452570
            if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(leader)) != 0) {  // 0x00428b70
                void* const u = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
                if (static_cast<i32>(jo::as_unit(u)->side) != jo::as_flag_facility(w)->owner->nation) return 1;
                if (static_cast<i32>(jo::view<i8>(jo::as_flag_facility(w)->flag_97)) != 1) return 1;
                *out_kind = 0xa;
                *out_id = cell;
                set_cursor(0xf, d::s_BARR01);  // "BARR01"
                return 1;
            }
            {
                void* const u = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(leader));
                if (static_cast<i32>(jo::as_unit(u)->side) != jo::as_flag_facility(w)->owner->nation) return 1;
            }
            if (static_cast<i32>(jo::view<i8>(jo::as_flag_facility(w)->flag_97)) != 1) return 1;
            void* const site = jo::as_flag_facility(w)->owner->position_;  // read before the unit lookup
            void* const u = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(leader));
            if (dist_to_site(site, u) > 512.0) return 1;  // je when len > 512 (ordered)
            *out_kind = 0xa;
            *out_id = cell;
            set_cursor(0xf, d::s_BARR01);  // "BARR01"
            return 1;
        }
        if (type != 0x10) return 1;
        void* const w = vcall(o, 0x1c);  // the Siege (0x0046cb90 returns this)
        const u32 leader = joan::orig::arg::as<u32>(joan::orig::as_group(this)->first_ready_member());  // 0x00452570
        if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(leader)) != 0) return 0;
        {
            void* const u = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(leader));
            if (static_cast<i32>(jo::as_unit(u)->side) != jo::as_siege(w)->owner->nation) return 1;
        }
        {
            void* const site = jo::as_siege(w)->owner->position_;
            void* const u = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(leader));
            if (dist_to_site(site, u) > 512.0) return 1;
        }
        if (jo::view<u32>(jo::as_siege(w)->i32_79) == 0) {
            *out_kind = 0xa;
            set_cursor(0x10, d::s_WEAPON00);  // "WEAPON00"
            *out_id = cell;
            return 1;
        }
        const i32 type_id = joan::orig::arg::as<i32>(joan::orig::as_unit_types(obj(d::g_types))->find(str(d::s_ALENCON_9fa6e0)));  // 0x005270b0 "ALENCON"
        const i32 index = joan::orig::arg::as<i32>(joan::orig::as_units(units())->find_by_3b(joan::orig::arg::i32(type_id)));                     // 0x0051fd50
        void* const a = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(index));  // 0x0041b3f0
        if (a == nullptr) return 1;
        if (static_cast<i32>(jo::as_unit(a)->id) != static_cast<i32>(leader)) return 1;
        {
            void* const site = jo::as_siege(w)->owner->position_;
            if (dist_to_site(site, a) >= 1024.0) return 1;  // test ah,1 / je: proceed when < or unordered
        }
        *out_kind = 0xa;
        *out_id = cell;
        if (jo::view<u32>(jo::as_siege(w)->i32_79) == 1) {
            set_cursor(0x11, d::s_IRON01);  // "IRON01"
        } else {
            set_cursor(0x12, d::s_WOOD01);  // "WOOD01"
        }
        return 1;
    }

    // 0x0044b963: the nearest hit is a unit (kind 0xb). Returns 0 / 1, or -1 to fall through to
    // the ground pick.
    i32 pick_unit_44b963(void* best, void* sel, i32* out_kind, i32* out_id) {
        void* u = jo::view<void*>(jo::as_actor(best)->owner);
        {
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->type_id))));  // 0x0042d0a0
            if (static_cast<i32>(jo::as_unit_type(t)->unit_class) == 8 && jo::as_unit(u)->crew_slots_ready != 0)
                global<i32>(d::g_a5704d) = jo::as_unit(u)->id;
        }
        if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(u)->rider_id))) == 0)  // 0x00428b70
            u = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_unit(u)->rider_id)));  // 0x0041b3c0
        if (group_count() == 0) return -1;
        u32 leader = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_group(this)->count_active()) == 1) {  // 0x004524c0
            const u32 id = joan::orig::arg::as<u32>(joan::orig::as_group(this)->first_ready_member());  // 0x00452570
            if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(id)) == 0) {
                void* const lu = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(id));
                if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(lu)->dead())) == 0) leader = id;  // 0x0042d0e0
            }
        }
        if (jo::as_unit(u)->can_interact != 0 && leader != 0 && leader == jo::view<u32>(jo::self<jo::Player>(this)->unit_id_) &&
            static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead())) == 0) {
            V3 pa, pb, tmp;
            joan::orig::as_unit(joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(leader)))->position(&pa);  // 0x004fedb0
            joan::orig::as_unit(u)->position(&pb);
            const V3 d = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &pa, &pb));  // 0x0060cc90
            if (!(import_call<double>(d::iat_v3d_87_length, &d) > 100.0)) {     // je when len > 100 (ordered)
                *out_kind = 0xb;
                *out_id = jo::as_unit(u)->id;
                return 1;
            }
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(sel)->hostile_to(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->id)))) != 0) {  // 0x005094d0
            *out_kind = 1;
            *out_id = jo::as_unit(u)->id;
            return 1;
        }
        if (jo::as_unit(u)->crew_slots_ready != 0) {
            const bool a = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})))) == 0;  // 0x0050d550
            const bool b = joan::orig::arg::as<i32>(joan::orig::as_unit(u)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(i32{2})))) == 0;
            if (a || b) {
                i32 k = 0;
                for (; k < group_count(); ++k) {
                    if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(group_member(k))) != 0) continue;
                    void* const v = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(group_member(k)));
                    if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(v)->dead())) != 0) continue;
                    if (joan::orig::arg::as<i32>(joan::orig::as_unit(v)->leads()) != 0) continue;  // 0x0044a660
                    {
                        const i32 ty = jo::as_unit(v)->type_id;
                        if (ty == joan::orig::arg::as<i32>(joan::orig::as_unit_types(obj(d::g_types))->find(str(d::s_ENGLISH_LIGHT_INFANTRY)))) break;  // ENGLISH_LIGHT_INFANTRY
                    }
                    {
                        const i32 ty = jo::as_unit(v)->type_id;
                        if (ty == joan::orig::arg::as<i32>(joan::orig::as_unit_types(obj(d::g_types))->find(str(d::s_FRENCH_LIGHT_INFANTRY)))) break;  // FRENCH_LIGHT_INFANTRY
                    }
                    {
                        const i32 ty = jo::as_unit(v)->type_id;
                        if (ty == joan::orig::arg::as<i32>(joan::orig::as_unit_types(obj(d::g_types))->find(str(d::s_MER_LIGHT_INFANTRY)))) break;  // MER_LIGHT_INFANTRY
                    }
                }
                if (k != group_count()) {
                    *out_kind = 6;
                    *out_id = jo::as_unit(u)->id;
                    return 1;
                }
            }
        }
        {
            void* const side_u = joan::orig::arg::as<void*>(joan::orig::as_unit(u)->nation());  // a Nation*  // 0x00509400
            void* const side_s = joan::orig::arg::as<void*>(joan::orig::as_unit(sel)->nation());
            if (side_u == side_s && jo::as_unit(u)->rank >= 4) {
                i32 k = 0;
                for (; k < group_count(); ++k) {
                    if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(group_member(k))) != 0) continue;
                    void* const v = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(group_member(k)));
                    if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(v)->dead())) != 0) continue;
                    if (jo::as_unit(v)->rank < 3) continue;
                    if (v == u) continue;
                    break;
                }
                if (k != group_count()) {
                    *out_kind = 8;
                    *out_id = jo::as_unit(u)->id;
                    return 1;
                }
            }
        }
        {
            void* const side_u = joan::orig::arg::as<void*>(joan::orig::as_unit(u)->nation());  // a Nation*
            void* const side_s = joan::orig::arg::as<void*>(joan::orig::as_unit(sel)->nation());
            if (side_u != side_s || joan::orig::arg::as<i32>(joan::orig::as_unit(u)->leads()) == 0) {
                *out_kind = 9;
                return 1;
            }
        }
        bool all = true;  // [ebp-0x1fc]
        i32 k = 0;
        for (; k < group_count(); ++k) {
            if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(group_member(k))) != 0) continue;
            void* const v = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(group_member(k)));
            if (jo::as_unit(v)->rank >= 3) {
                all = false;
                break;
            }
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(v)->dead())) != 0) continue;
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(v)->leads()) != 0) continue;
            if (v == u) continue;
            {
                void* const t = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(v)->type_id))));
                if (static_cast<i32>(jo::as_unit_type(t)->unit_class) == 8) continue;
            }
            {
                void* const t = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(v)->type_id))));
                if (static_cast<i32>(jo::as_unit_type(t)->unit_class) == 9) continue;
            }
            if (jo::as_unit(v)->group_id == jo::as_unit(u)->group_id) continue;
            break;
        }
        if (all && k != group_count()) {
            *out_kind = 7;
            *out_id = jo::as_unit(u)->id;
            return 1;
        }
        return -1;
    }

    // 0x0044c06f: the nearest hit is a building / item (kind 1). Returns 1, or -1 to fall through.
    i32 pick_object_44c06f(void* best, void* sel, i32* out_kind, i32* out_id, V3* out_pos) {
        void* const o = jo::view<void*>(jo::as_actor(best)->owner);
        if (group_count() == 0) return -1;
        const i32 type = jo::as_map_object(o)->class_id;
        if (type == 10) {  // byte table index 1: 0x0044c0d9
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(o)->type_id))));  // 0x00425ce0
            if (jo::view<i8>(jo::as_item_def(t)->open_with_interact) == 0) {
                *out_kind = 2;
                *out_id = jo::as_map_object(o)->index;
            }
            return 1;
        }
        if (type == 1 || type == 4 || type == 0x12) {  // byte table index 0: 0x0044c118
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(sel)->relation_to_side(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(o)->side)))) != 0) {  // 0x00509590
                bool set = false;
                if (jo::as_map_object(o)->class_id == 1 && jo::as_gate(vcall(o, 0x4c))->hp > 0 &&
                    static_cast<i32>(jo::as_gate(vcall(o, 0x4c))->side) == 1) {
                    const i32 d1 = jo::as_gate(vcall(o, 0x4c))->state;
                    const i32 d2 = jo::as_gate(vcall(o, 0x4c))->side;
                    set = d1 != d2;
                }
                if (!set) {
                    if (jo::as_map_object(o)->class_id == 4 && jo::view<i32>(jo::as_wall(vcall(o, 0x58))->u32_c6) > 0) {
                        set = true;
                    } else if (jo::as_map_object(o)->class_id == 0x12) {
                        set = true;
                    }
                }
                if (set) {
                    *out_kind = 2;
                    *out_id = jo::as_map_object(o)->index;
                    return 1;
                }
            }
        }
        // 0x0044c204 (byte table index 2 and out-of-range types)
        if (jo::as_map_object(o)->world_item == nullptr) return -1;
        void* const t = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(o)->type_id))));
        if (jo::view<i8>(jo::as_item_def(t)->pickable) == 0) return -1;
        *out_kind = 4;
        std::memcpy(out_pos, jo::as_map_object(o)->pos, 12);
        return 1;
    }

    // ------------------------------------------------------------------ 0x0044ac60
    bool pick_44ac60(i32* out_kind, i32* out_id, V3* out_pos) {
        c_pick.hit();
        *out_kind = 0;
        std::memcpy(out_pos, obj(d::g_pos), 12);
        global<i32>(d::g_a5704d) = 0;
        if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) != 0) return false;  // 0x00428b70
        void* const sel = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_marker(obj(d::g_mini_map))->visible())) != 0) return false;  // 0x00717650

        State40 state;  // [ebp-0x30]: +4 eye position, +0x1c distance
        {
            const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(obj(d::g_camera_man))->target());               // 0x00563dc0
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(joan::orig::arg::i32(idx)));     // 0x00563c80
            joan::orig::as_camera_motion(slot)->end(&state);                                // 0x00423fe0
        }
        V3 fwd;
        {
            jo::Bone* const cam = jo::as_camera(global<void*>(d::g_camera))->bone_;  // eax = [0xa48ccc]; ecx = [eax]; [ecx+0x78]
            fwd.v[0] = jo::view<float[16]>(cam->matrix_)[8];  // bone matrix +0x20
            fwd.v[1] = jo::view<float[16]>(cam->matrix_)[9];
            fwd.v[2] = jo::view<float[16]>(cam->matrix_)[10];
        }
        V3 eye;
        std::memcpy(&eye, &state.d[1], 12);
        {
            V3 t1;
            const V3 scaled = *joan::orig::arg::as<V3*>(joan::orig::vec_scale(&t1, &fwd, joan::orig::arg::f32(state.d[7])));  // 0x0060cd00
            joan::orig::vec3_sub_assign(joan::orig::arg::mptr(&eye), joan::orig::arg::mptr(&scaled));                             // 0x0060ce20: eye -= scaled
        }
        float box[6];  // [ebp-0x54]: lo, hi
        box[0] = eye.v[0] - 1000.0f;
        box[1] = eye.v[1] - 1000.0f;
        box[2] = eye.v[2] - 5000.0f;
        box[3] = eye.v[0] + 1000.0f;
        box[4] = eye.v[1] + 1000.0f;
        box[5] = eye.v[2] + 0.0f;  // fadd of the zeroed [ebp-0x14]
        if (global<void*>(d::g_world) == nullptr) return false;
        i32 count = 0;
        void* const* const cells = joan::orig::arg::as<void**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x00429090

        int result = -1;
        i32 sx = 0, sy = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_grid(global<void*>(d::g_camera))->to_cell(joan::orig::arg::i32(global<i32>(d::g_mouse_x)), joan::orig::arg::i32(global<i32>(d::g_mouse_y)), &sx, &sy)) != 0) {  // 0x006152b0
            Ray ray;
            joan::orig::as_camera(global<void*>(d::g_camera))->screen_ray(joan::orig::arg::i32(sx), joan::orig::arg::i32(sy), &ray);  // 0x006154d0
            void* best = nullptr;  // [ebp-0x80]
            float best_d = 2.0f;   // [ebp-0x84]
            for (i32 i = count - 1; i >= 0; --i) {
                void* const e = cells[i];
                if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(&ray, joan::orig::arg::ptr(jo::as_cell(e)->box))) == 0) continue;  // 0x0060dfd0
                for (void* o = joan::orig::arg::as<void*>(joan::orig::as_world_cell(e)->get_90()); o != nullptr;
                     o = joan::orig::arg::as<void*>(joan::orig::get_341(joan::orig::arg::mptr(o)))) {  // 0x0054eec0 / 0x0054ef40
                    if (jo::as_unit(o)->actor == nullptr) continue;
                    if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(o)->dead())) != 0) continue;  // 0x0042d0e0
                    if (jo::as_unit(o)->actor->plane == nullptr) continue;
                    if (joan::orig::arg::as<u8>(joan::orig::as_actor(jo::as_unit(o)->actor)->byte_10a()) == 0) continue;  // 0x0044cee0
                    consider(o, &jo::Unit::actor, ray, best, best_d);
                }
                for (void* q = joan::orig::arg::as<void*>(joan::orig::as_world_cell(e)->get_98()); q != nullptr;
                     q = joan::orig::arg::as<void*>(joan::orig::get_b2(joan::orig::arg::mptr(q)))) {  // 0x0054f510 / 0x0054f530
                    if (jo::as_map_object(q)->actor == nullptr) continue;
                    if (jo::as_map_object(q)->actor->plane == nullptr) continue;
                    {
                        const i32 ty = jo::as_map_object(q)->class_id;
                        if (ty != 10 && ty != 1 && ty != 0x27 && ty != 4) {
                            if (jo::as_map_object(q)->world_item == nullptr) continue;
                            void* const t =
                                joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(q)->type_id))));  // 0x00425ce0
                            if (jo::view<i8>(jo::as_item_def(t)->pickable) == 0) continue;
                        }
                    }
                    if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) == 0) {
                        void* const s = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));
                        if (jo::as_unit(s)->side == jo::as_map_object(q)->side) continue;
                    }
                    {
                        const i32 ty = jo::as_map_object(q)->class_id;
                        if (ty == 1) {
                            if (jo::as_gate(vcall(q, 0x4c))->hp < 0) continue;
                        } else if (ty == 4) {
                            if (jo::view<i32>(jo::as_wall(vcall(q, 0x58))->u32_c6) < 0) continue;
                        }
                    }
                    consider(q, &jo::MapObject::actor, ray, best, best_d);
                }
            }
            if (best != nullptr) {
                joan::orig::segment_point_at(joan::orig::arg::mptr(out_pos), &ray, joan::orig::arg::f32(bits(best_d)));  // 0x0060de40
                const u32 kind = jo::view<u32>(jo::as_actor(best)->kind);
                if (kind == 1) {
                    result = pick_object_44c06f(best, sel, out_kind, out_id, out_pos);
                } else if (kind == 0xb) {
                    result = pick_unit_44b963(best, sel, out_kind, out_id);
                }
            }
        }
        if (result >= 0) return result != 0;
        return pick_ground_44c25f(eye, out_kind, out_id) != 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_16", name, addr, fn)
B("Cursor::pick_44ac60", 0x0044ac60, &H::pick_44ac60);
#undef B

}  // namespace game_b110_16
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

using joan::call::thiscall;

struct Obj {};

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

constexpr fn::Code kListGet = fn::List_get_41b3f0;    // 0x0041b3f0
std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }
void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

constexpr img::Addr kGameUnits(d::g_units);
constexpr img::Addr kFiles(d::g_misc);
std::uint8_t* world() { return global<std::uint8_t*>(d::g_world); }

JOAN_COUNTER(c_world_draw, "World::draw");

struct H {
    std::int32_t list_empty_at(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }  // 0x0044cf80 / 0x00485760 / 0x0063e3f0
    std::uint8_t byte_10a() { return jo::self<jo::Actor>(this)->visible; }  // 0x0044cee0

    // 0x0044c800 World::draw (this = CameraFollow at 0xa45ef8-ish: +0 enabled, +4 motions,
    // +8 followed unit, +0xc camera, +0x20 / +0x21 / +0x22 streaming state, +0x2f motion).
    void world_draw() {
        joan::hitch::Span hitch_span("world_draw");  // [Debug] HitchLog
        if (jo::self<jo::CameraFollow>(this)->active == 0) return;
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_menu_frame_state)) == 2) return;
        c_world_draw.hit();
        for (std::int32_t i = 0; i < jo::as_world(world())->cell_count; ++i) {
            std::uint8_t* const c = reinterpret_cast<std::uint8_t*>(jo::as_world(world())->cells[i].item);
            if (static_cast<std::int8_t>(jo::as_cell(c)->loaded) != 0 && jo::view<void*>(jo::as_cell(c)->camera_zones) != nullptr) joan::orig::as_camera_zones(jo::view<void*>(jo::as_cell(c)->camera_zones))->update();  // 0x00425fc0
        }
        auto current = [this]() { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_camera_follow(this)->motion(joan::orig::arg::i32(joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera_follow(this)->target())))); };  // 0x00563c80(0x00563dc0)
        if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_camera_follow(this)->target()) == 0) joan::orig::as_camera_follow(this)->start(joan::orig::arg::i32(jo::self<jo::CameraFollow>(this)->i32_8));  // 0x00563a20
        const std::int32_t moving = joan::orig::arg::as<std::int32_t>(joan::orig::as_camera_motion(current())->update());  // 0x004237a0
        std::uint32_t state[10];
        std::memcpy(state, &jo::as_camera_motion(current())->current_, sizeof state);  // the CamPose
        float near_box[6], far_box[6];
        joan::orig::as_aabb(near_box)->set_point(static_cast<void*>(&state[1]));  // 0x0060d430
        for (std::int32_t k = 0; k < 3; ++k) near_box[k] = near_box[k] - 8000.0f;
        for (std::int32_t k = 3; k < 6; ++k) near_box[k] = near_box[k] + 8000.0f;
        joan::orig::as_aabb(far_box)->set_point(static_cast<void*>(&state[1]));
        for (std::int32_t k = 0; k < 3; ++k) far_box[k] = far_box[k] - 16000.0f;
        for (std::int32_t k = 3; k < 6; ++k) far_box[k] = far_box[k] + 16000.0f;
        std::int32_t waiting = 0;
        if (global<std::int32_t>(d::g_a0bbf8) == -1) {
            global<std::int32_t>(d::g_a0bbf8) = 0;
            const char* const name = &img::global<const char>(d::s_free_world_video_memory_sys);  // "free_world_video_memory.sys"
            if (joan::orig::as_misc(img::obj(kFiles))->is_file_exist(joan::orig::arg::str(name)) != 0) {  // 0x00590b80 exists
                alignas(8) std::uint8_t reader[0x200];
                joan::orig::as_file_txt(reader)->construct(joan::orig::arg::str(name));  // 0x00589550
                global<std::int32_t>(d::g_a0bbf8) = joan::hook::x87::ftol(joan::orig::arg::as<double>(joan::orig::as_file_txt(reader)->get_num_589ee0()));  // 0x00589ee0 number
                joan::orig::as_file_txt(reader)->destruct();                            // 0x00589890
            }
        }
        void* const strings = img::obj(d::g_strings);
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_a5d384)) != 0) {
            const std::int32_t pending = joan::orig::arg::as<std::int32_t>(joan::orig::as_world(world())->cells_pending(joan::orig::arg::mptr(static_cast<void*>(near_box)), joan::orig::arg::mptr(static_cast<void*>(far_box)), 1));  // 0x005479a0
            if (pending != 0) joan::orig::as_streams(img::obj(d::g_dialogue))->pause_all(static_cast<char>(joan::orig::arg::i32(static_cast<char>(0))));  // 0x0063c6d0
            joan::orig::as_world(world())->stream_cells(joan::orig::arg::mptr(static_cast<void*>(near_box)), joan::orig::arg::mptr(static_cast<void*>(far_box)), 1);           // 0x00547630
            if (pending != 0) joan::orig::as_streams(img::obj(d::g_dialogue))->pause_all(static_cast<char>(joan::orig::arg::i32(static_cast<char>(1))));
            jo::self<jo::CameraFollow>(this)->u8_20 = 0;
            jo::self<jo::CameraFollow>(this)->u8_21 = 1;
            jo::self<jo::CameraFollow>(this)->u8_22 = 0;
        } else if (static_cast<std::int8_t>(jo::self<jo::CameraFollow>(this)->u8_21) != 0) {
            joan::orig::as_world(world())->stream_cells(joan::orig::arg::mptr(static_cast<void*>(near_box)), joan::orig::arg::mptr(static_cast<void*>(far_box)), 1);
            jo::self<jo::CameraFollow>(this)->u8_20 = 0;
            jo::self<jo::CameraFollow>(this)->u8_21 = 0;
            jo::self<jo::CameraFollow>(this)->u8_22 = 0;
        } else if (static_cast<std::int8_t>(jo::self<jo::CameraFollow>(this)->u8_20) == 0) {
            if (world() != nullptr) {
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_world(world())->cells_pending(joan::orig::arg::mptr(static_cast<void*>(near_box)), joan::orig::arg::mptr(static_cast<void*>(far_box)), 1)) != 0) {
                    jo::self<jo::CameraFollow>(this)->u8_20 = 1;
                    void* const text = joan::orig::arg::as<void*>(joan::orig::as_txt_res(strings)->sms_text_13());  // 0x004dd920
                    joan::orig::as_sms(img::obj(d::g_sms))->add(joan::orig::arg::str(text), 1, 0);  // 0x006e5700 message
                    waiting = 1;
                } else {
                    jo::self<jo::CameraFollow>(this)->u8_22 = 0;
                }
                if (global<std::int32_t>(d::g_a0bbf8) > 0 && global<std::uint32_t>(d::g_a52438) != 0) {
                    void* const device = global<void*>(d::g_game_device);
                    if (com_call<std::uint32_t>(device, 4) < global<std::uint32_t>(d::g_a0bbf8)) {  // GetAvailableTextureMem
                        joan::orig::as_world(world())->free_far_cells(joan::orig::arg::mptr(static_cast<void*>(far_box)), 1);  // 0x004457e0
                        if (com_call<std::uint32_t>(device, 4) < global<std::uint32_t>(d::g_a0bbf8))
                            joan::orig::as_world(world())->free_far_cells(joan::orig::arg::mptr(static_cast<void*>(near_box)), 1);
                    }
                }
            }
        } else if (static_cast<std::int32_t>(global<std::int16_t>(d::g_a48c5e)) == 3) {
            joan::orig::as_world(world())->stream_cells(joan::orig::arg::mptr(static_cast<void*>(near_box)), joan::orig::arg::mptr(static_cast<void*>(far_box)), 1);
            jo::self<jo::CameraFollow>(this)->u8_20 = 0;
            joan::orig::as_image2d(img::obj(d::g_a48af0))->set_name(joan::orig::arg::str(joan::orig::arg::as<void*>(joan::orig::as_txt_res(strings)->sms_text_14())));  // 0x006298c0(0x004dd950)
            jo::self<jo::CameraFollow>(this)->u8_22 = 0;
        } else {
            waiting = 1;
        }
        if (moving != 0 && (waiting == 0 || static_cast<std::int8_t>(jo::self<jo::CameraFollow>(this)->u8_22) == 0))
            joan::orig::as_camera(jo::view<void*>(jo::self<jo::CameraFollow>(this)->camera))->set_frame(joan::orig::arg::mptr(static_cast<void*>(&jo::as_camera_motion(current())->current_)));  // 0x00614040
        if (jo::view<void*>(jo::as_world(world())->scene) != nullptr && jo::view<void*>(jo::as_world(world())->scene) != jo::view<void*>(jo::self<jo::CameraFollow>(this)->camera->scene))
            joan::orig::as_camera(jo::view<void*>(jo::self<jo::CameraFollow>(this)->camera))->set_scene(jo::as_world(world())->scene);  // 0x00613db0
        void* const sky = img::obj(d::g_light_sys);
        joan::orig::as_light_sys(sky)->update_moon();  // 0x004a7980
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(jo::view<void*>(jo::self<jo::CameraFollow>(this)->camera))->begin_frame()) == 0) return;  // 0x00614630 begin
        for (std::int32_t i = list_count(img::obj(kGameUnits)); i > 0; --i) {
            std::uint8_t* const unit = static_cast<std::uint8_t*>(list_get(img::obj(kGameUnits), i));
            if (unit == nullptr) continue;
            if (jo::as_unit(unit)->draw == nullptr) continue;
            if (jo::as_unit(unit)->move_mode >= 3)
                joan::orig::as_unit_draw(jo::as_unit(unit)->draw)->hide();  // 0x00522850
            else
                joan::orig::as_unit_draw(jo::as_unit(unit)->draw)->draw();  // 0x0043b890
        }
        void* const objects = img::obj(d::g_places);
        for (std::int32_t i = list_count(objects); i > 0; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(objects)->empty_at_e(joan::orig::arg::i32(i))) != 0) continue;  // 0x0044cf80
            std::uint8_t* const o = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(objects)->get_e(joan::orig::arg::i32(i)));  // 0x0043c450
            if (jo::as_facility(o)->kind != 0x10) continue;
            using Virtual = void* (Obj::*)();
            JOAN_MEMBER_POINTER_CHECK(Virtual);
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(o)[0x1c / 4], sizeof(void*));
            void* const target = (reinterpret_cast<Obj*>(o)->*method)();
            joan::orig::as_workshop_facility(target)->progress_bar();  // 0x0046c540
        }
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CameraFollow>(this)->camera))->render();  // 0x00614760 Camera::render
        joan::orig::as_light_sys(sky)->after_frame();            // 0x004a7ba0
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
#define W3(name, addr, fn) JOAN_REPLACE("game/world3", name, addr, fn)
W1("List::empty_at_e", 0x0044cf80, &H::list_empty_at);
W1("get_10a", 0x0044cee0, &H::byte_10a);
W3("World::draw", 0x0044c800, &H::world_draw);
#undef W1
#undef W3

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_world_2.cpp (game/world3)
namespace {
namespace game_world_2 {

using joan::call::thiscall;

using joan::call::thiscall;

std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }

constexpr img::Addr kUnits(d::g_items);

JOAN_COUNTER(c_effects_cleanup, "Effects::cleanup");

struct H {
    // 0x0044cdd0 Effects::cleanup(cell): effects (+8 unit id) whose unit is gone or outside
    // the cell (unit +0xaa cell, +0x56 position vs cell box +8) are deleted.
    void effects_cleanup(std::uint8_t* c) {
        c_effects_cleanup.hit();
        for (std::int32_t i = list_count(this); i > 0; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->empty_at_f(joan::orig::arg::i32(i))) != 0) continue;  // 0x00485760
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(this)->get_f(joan::orig::arg::i32(i)));  // 0x00485730
            if (joan::orig::as_units(img::obj(kUnits))->list.empty_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_missile(e)->object_id))) != 0) continue;  // 0x00425d50
            std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(img::obj(kUnits))->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_missile(e)->object_id))));  // 0x00425d20
            if (jo::view<std::uint8_t*>(jo::as_map_object(unit)->cell) != c && (joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(jo::as_cell(c)->box)->contains(static_cast<void*>(jo::as_map_object(unit)->pos))) & 0xff) == 0) continue;  // 0x0060d5f0
            if (e != nullptr) joan::orig::as_missile(e)->destroy(1u);  // 0x004ea370
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));  // 0x005858c0
        }
    }
};

#define W3(name, addr, fn) JOAN_REPLACE("game/world3", name, addr, fn)
W3("Effects::cleanup", 0x0044cdd0, &H::effects_cleanup);
#undef W3

}  // namespace game_world_2
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01_2 {

struct H {
    // Callers (rts.cpp, esfuncs.cpp) pass the HUD's MiniMap (0x00a4f381): its fields, which on x64 lie after the
    // 8-byte icons / view pointers.
    std::uint16_t get_8() { return jo::view<std::uint16_t>(jo::self<jo::MiniMap>(this)->x); }    // 0x0044cf10
    std::uint8_t get_e() { return jo::self<jo::MiniMap>(this)->active; }    // 0x0044cf30
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Obj::get_8", 0x0044cf10, &H::get_8);
B("Obj::get_e", 0x0044cf30, &H::get_e);
#undef B

}  // namespace game_b92_01_2
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

struct H {
    // Callers (rts.cpp, minires.cpp) pass the HUD's MiniMap (0x00a4f381): +0xf is MiniMap::visible (on x64 after the
    // 8-byte view pointer; View::word_f there was the view pointer's top bytes).
    void* set_word_f(std::uint16_t v) {  // 0x0044cf50
        jo::view<std::uint16_t>(jo::self<jo::MiniMap>(this)->visible) = v;
        return this;
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("View::set_word_f", 0x0044cf50, &H::set_word_f);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b110_08.cpp (game/b110_08)
namespace {
namespace game_b110_08 {

using joan::call::thiscall;

// v3d_87.dll exports are cdecl, called through their IAT slot.
using joan::orig::helpers::v3d_call;

using joan::orig::helpers::imul;

struct V3f {  // by-value float vector argument
    float x, y, z;
};
struct V3 {  // 12 bytes copied as raw dwords
    u32 x, y, z;
};
struct Str {  // 0x005a11a0 string object (0x190 chars + NUL slot)
    char b[0x194];
};
struct Order {  // 0x004e4520 order record (writes 0x19 bytes; 0x1c-byte slot in the frame)
    u8 b[0x1c];
};
static_assert(sizeof(V3f) == 12 && sizeof(V3) == 12 && sizeof(Str) == 0x194 && sizeof(Order) == 0x1c,
              "sizes");

void* const kUnits = obj(d::g_units);
const Fpu kFpu{true};

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
bool unit_busy(const void* u) { return joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead()) != 0; }  // 0x0042d0e0 (al)
u8* unit_class(const void* u) {                                              // 0x0042d0a0 (ret 4, word)
    return joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(u)->type_id)));
}
bool over_target() { return joan::orig::arg::as<u8>(joan::orig::as_marker(obj(d::g_mini_map))->visible()) != 0; }  // 0x00717650 (al)
i32 rnd(i32 lo, i32 hi) { return joan::orig::arg::as<i32>(joan::orig::as_misc(obj(d::g_misc))->random_range(joan::orig::arg::i32(lo), joan::orig::arg::i32(hi))); }  // 0x00590690

void str_ctor(Str& s, img::Addr t) { joan::orig::as_string(&s)->construct(joan::orig::arg::str(str(t))); }  // 0x005a11a0
void str_assign(Str& s, const char* t) { joan::orig::as_string(&s)->assign(joan::orig::arg::str(t)); }  // 0x005a1440
void str_assign(Str& s, img::Addr t) { str_assign(s, str(t)); }
void str_append(Str& s, const char* t) { joan::orig::as_string(&s)->append(joan::orig::arg::str(t)); }  // 0x005a1550
void str_append_int(Str& s, i32 v) { joan::orig::as_string(&s)->append(joan::orig::arg::i32(v)); }  // 0x005a15d0
const char* c_str(Str& s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str()); }  // 0x00417d90
void play_voice(Str& s) {                                                               // 0x00492c40
    const char* p = c_str(s);
    joan::orig::as_sound(obj(d::g_sfx_player))->play_kind(p);
}

// fsin / fcos of the heading object (64-bit st0 results).
double ang_cos(void* a) { return joan::orig::arg::as<double>(joan::orig::as_turns(a)->cos_577b50()); }  // 0x00577b50
double ang_sin(void* a) { return joan::orig::arg::as<double>(joan::orig::as_turns(a)->sin_577b10()); }  // 0x00577b10

/// [Fixes] ImmovableSiege: whether any crew position of the unit type `type` moves it (AttachPointUsage M or B in
/// unit\unit.res, bit 1 of the type's position flags, the bits Unit::has_mask_ready 0x0050d550 tests with mask 1).
/// The trebuchet and the arbalest have only F positions: no crew moves them.
bool has_move_position(const jo::UnitType* type) {
    for (i32 i = 0; i < type->slot_count; ++i) {
        if ((static_cast<const i8*>(type->buffers[2])[i] & 1) != 0) return true;
    }
    return false;
}

JOAN_COUNTER(c_move, "RtsGroup::move_formation_44cfb0");

struct H {
    i32 grp() { return jo::self<jo::Player>(this)->group; }
    i32 count() { return jo::view<i8>(jo::self<jo::Player>(this)->group_counts[grp()]); }
    i32 id(i32 i) { return jo::view<i32>(jo::self<jo::Player>(this)->group_units[grp() * 100 + i]); }
    const char* unit_name(i32 unit_id) { return joan::orig::arg::as<const char*>(joan::orig::as_player(this)->name_tag(joan::orig::arg::u32(unit_id))); }  // 0x00453da0
    void set_cursor(img::Addr name, i32 mode) {
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Player>(this)->point_name)), joan::orig::arg::str(str(name)), 8u);  // strncpy
        jo::view<i32>(jo::self<jo::Player>(this)->point_kind) = mode;
    }

    // Voice line prefix: "RTS" + name of the first ready unit + suffix, or the generic line.
    // Returns the number of ready units (byte counter).
    i8 voice_prefix(Str& s, img::Addr suffix) {
        str_ctor(s, d::s_RTS);  // "RTS"
        i8 n = 0;
        for (i32 i = 0; i < count(); ++i) {
            if (unit_missing(id(i)) != 0 || unit_busy(unit_get(id(i)))) continue;
            if (jo::as_unit(unit_get(id(i)))->rank < 4) continue;
            if (n == 0) {
                str_append(s, unit_name(id(i)));
                str_append(s, str(suffix));
            }
            n = static_cast<i8>(n + 1);
        }
        return n;
    }

    // Grid helper: rnd-rounded sqrt of n as column count (0x00590740 mode 2), half width.
    static i32 columns(i32 n) {
        const double sq = joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(n))));  // sqrt
        const float sqf = static_cast<float>(sq);
        return ftol(joan::orig::arg::as<double>(joan::orig::as_misc(obj(d::g_misc))->round_decimals(joan::orig::arg::f32(sqf), i32{0}, i32{2})));  // 0x00590740
    }
    static float half_width(i32 cols, i32 spacing) {
        return static_cast<float>(
            kFpu.div(static_cast<double>(imul(cols - 1, spacing)), static_cast<double>(global<float>(d::flt_2))));
    }

    u8 move_formation(V3f pos, i32 attack) {
        c_move.hit();
        if (!over_target()) {
            const u8 ok = joan::orig::arg::as<u8>(joan::orig::as_collision_grid(obj(d::g_collision_grid))->region_hit(pos.x, pos.y, pos.z, joan::orig::arg::u32(i32{1})));  // 0x00693580
            if (ok == 0) {
                // ---- target refused
                jo::self<jo::Player>(this)->cursor_mode = 5;
                set_cursor(d::s_CMOVTO00, 7);  // "CMOVTO00"
                Str s;
                const i8 n = voice_prefix(s, d::s_015);  // "015"
                if (n != 1) {
                    str_assign(s, d::s_RTS_LTINF_0);  // "RTS_LTINF_0"
                    str_append_int(s, rnd(0xb, 0xd));
                }
                if (count() == 1 && unit_missing(id(0)) == 0) {
                    str_assign(s, d::s_RTS);  // "RTS"
                    str_append(s, unit_name(id(0)));
                    if (jo::as_unit(unit_get(id(0)))->rank >= 4) {
                        str_append(s, str(d::s_015));  // "015"
                    } else {
                        str_append(s, str(d::s_0));  // "0"
                        str_append_int(s, rnd(0xb, 0xd));
                    }
                    if (jo::as_unit_type(unit_class(unit_get(id(0))))->unit_class == 8) str_assign(s, d::s_RTS_SIEGE_004);  // "RTS_SIEGE_004"
                }
                play_voice(s);
                return 0;
            }
        }

        // ---- 0x0044d32c: issue the move
        i32 spacing = 0x50;
        i32 cols = columns(count());
        float half = half_width(cols, spacing);

        for (i32 i = 0; i < count(); ++i) {
            if (unit_missing(id(i)) != 0) continue;
            u8* u = unit_get(id(i));
            const i32 keep = jo::as_unit(u)->task.priority;
            joan::orig::as_unit(u)->release_slot();  // 0x00528300
            jo::as_unit(u)->task.priority = static_cast<i8>(keep);
            if (jo::as_unit(u)->orders != nullptr && jo::as_unit(u)->orders->current.kind != 0) {
                joan::orig::as_action(jo::as_unit(u)->orders)->dispatch(joan::orig::arg::mptr(u));       // 0x004bb660
                joan::orig::dynarray(jo::as_unit(u)->orders)->zap(i32{1});  // 0x00584580
            }
        }

        // Corner distances (result only feeds the dead word flag [ebp-0x1e0]).
        {
            V3f a{pos.x - half, pos.y - half, pos.z};
            const double k = static_cast<double>(imul(cols - 1, spacing));
            V3f b{static_cast<float>(kFpu.sub(kFpu.add(k, pos.x), half)),
                  static_cast<float>(kFpu.sub(kFpu.add(k, pos.y), half)), pos.z};
            V3 tmp1{0, 0, 0};
            V3 tmp2{0, 0, 0};
            V3 d1 = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(static_cast<void*>(&tmp1), static_cast<void*>(&pos), static_cast<void*>(&a)));  // 0x0060cc90
            V3 d2 = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(static_cast<void*>(&tmp2), static_cast<void*>(&pos), static_cast<void*>(&b)));  // 0x0060cc90
            const float l1 = static_cast<float>(v3d_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&d1)));
            const double l2 = v3d_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&d2));
            const i16 flag = (l2 > static_cast<double>(l1)) ? i16{1} : i16{0};
            (void)flag;
        }

        // Sorted unit types with counts.
        i32 ntypes = 0;
        i32 cnts[100];
        i32 types[100];
        std::memset(cnts, 0, sizeof cnts);
        std::memset(types, 0, sizeof types);  // only entries < ntypes are ever read
        V3 centre = global<V3>(d::g_pos);
        for (i32 i = 0; i < count(); ++i) {
            if (unit_missing(id(i)) != 0) continue;
            u8* u = unit_get(id(i));
            if (unit_busy(u)) continue;
            bool found = false;
            i32 ins = -1;
            for (i32 j = 0; j < ntypes; ++j) {
                if (types[j] == jo::as_unit(u)->group_id) {
                    cnts[j] = cnts[j] + 1;
                    found = true;
                    break;
                }
                if (types[j] < jo::as_unit(u)->group_id) ins = j;
            }
            if (!found) {
                if (ins == -1) {
                    cnts[ntypes] = cnts[ntypes] + 1;
                    types[ntypes] = jo::as_unit(u)->group_id;
                    ++ntypes;
                } else {
                    const u32 bytes = static_cast<u32>(ntypes - ins) << 2;
                    std::memmove(&types[ins + 1], &types[ins], bytes);
                    std::memmove(&cnts[ins + 1], &cnts[ins], bytes);
                    cnts[ins] = cnts[ins] + 1;
                    types[ins] = jo::as_unit(u)->group_id;
                    ++ntypes;
                }
            }
            joan::orig::vec_add_in_place(static_cast<void*>(&centre), static_cast<void*>(jo::as_unit(u)->position_));  // 0x0060cdc0
        }
        if (count() != 0) {
            const float inv = static_cast<float>(
                kFpu.div(static_cast<double>(global<float>(d::flt_1)), static_cast<double>(count())));
            joan::orig::vec_scale_in_place(static_cast<void*>(&centre), joan::orig::arg::f32(inv));  // 0x0060ce80
        }
        u32 heading = 0;  // 0x00577190 ctor leaves it untouched; 0x00577b90 writes it
        joan::orig::as_angle(&heading)->construct_577190();  // 0x00577190
        {
            float cx, cy;
            std::memcpy(&cx, &centre.x, 4);
            std::memcpy(&cy, &centre.y, 4);
            const double dx = static_cast<float>(pos.x - cx);
            const double dy = static_cast<float>(pos.y - cy);
            joan::orig::as_angle(&heading)->from_xy(joan::orig::arg::f64(dx), joan::orig::arg::f64(dy));  // 0x00577b90
        }

        spacing = 0x190;
        cols = columns(ntypes);
        half = half_width(cols, spacing);
        i32 placed[100];
        std::memset(placed, 0, sizeof placed);
        const i32 ordtype = attack != 0 ? 0x13 : 0xc;

        for (i32 i = 0; i < ntypes; ++i) {
            const float ox = static_cast<float>(imul((-i) / cols, spacing));
            const float oy = static_cast<float>(
                kFpu.sub(static_cast<double>(half), static_cast<double>(imul(i % cols, spacing))));
            const double c1 = ang_cos(&heading);
            const float t900 = static_cast<float>(kFpu.add(kFpu.mul(c1, ox), pos.x));
            const double s1 = ang_sin(&heading);
            const float px = static_cast<float>(kFpu.sub(t900, kFpu.mul(s1, oy)));
            const double s2 = ang_sin(&heading);
            const float t904 = static_cast<float>(kFpu.add(kFpu.mul(s2, ox), pos.y));
            const double c2 = ang_cos(&heading);
            const float py = static_cast<float>(kFpu.add(kFpu.mul(c2, oy), t904));
            const float pz = pos.z;

            for (i32 j = 0; j < count(); ++j) {
                if (unit_missing(id(j)) != 0) continue;
                u8* u = unit_get(id(j));
                if (unit_busy(u)) continue;
                if (jo::as_unit(u)->task.kind == 0xc) continue;
                if (jo::as_unit(u)->group_id != types[i]) continue;

                if (joan::orig::as_unit(u)->leader() != nullptr && cnts[i] > 1) {  // 0x004216d0
                    // Formation unit: its own offset (+0x41a z, +0x422 / +0x426 x / y).
                    if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->leads()) != 0) {  // 0x0044a660
                        u8* p = jo::view<u8*>(jo::as_unit(u)->group);
                        jo::as_group(p)->x = px;
                        jo::as_group(p)->y = py;
                        jo::as_group(p)->z = pz;
                        const double a = joan::orig::arg::as<double>(joan::orig::as_angle(&heading)->to_radians());  // 0x005773b0
                        jo::as_group(jo::as_unit(u)->group)->angle = static_cast<float>(a);
                        joan::orig::as_group(jo::as_unit(u)->group)->arrange(i32{1}, i32{1});  // 0x004fa0f0
                    }
                    const i32 a6 = global<i32>(d::g_now) + 0x2bf20;
                    const i32 a5 = ftol(kFpu.add(static_cast<double>(ftol(pz)), jo::as_unit(u)->formation_offset[2]));
                    const float t910 = static_cast<float>(ftol(py));
                    const double sa = ang_sin(&heading);
                    const float t914 = static_cast<float>(kFpu.add(kFpu.mul(sa, jo::as_unit(u)->formation_grid_x), t910));
                    const double ca = ang_cos(&heading);
                    const i32 a4 = ftol(kFpu.add(kFpu.mul(ca, jo::view<float>(jo::as_unit(u)->form_slot_y)), t914));
                    const float t91c = static_cast<float>(ftol(px));
                    const double cb = ang_cos(&heading);
                    const float t920 = static_cast<float>(kFpu.add(kFpu.mul(cb, jo::as_unit(u)->formation_grid_x), t91c));
                    const double sb = ang_sin(&heading);
                    const i32 a3 = ftol(kFpu.sub(t920, kFpu.mul(sb, jo::view<float>(jo::as_unit(u)->form_slot_y))));
                    const i32 a2 = jo::as_unit(u)->task.priority + 1;
                    Order ord;
                    joan::orig::as_unit_task(&ord)->set(joan::orig::arg::u32(ordtype), static_cast<std::uint8_t>(joan::orig::arg::u32(a2)), joan::orig::arg::u32(a3), joan::orig::arg::u32(a4), joan::orig::arg::u32(a5), joan::orig::arg::u32(a6));  // 0x004e4520
                    joan::orig::as_unit(u)->release_slot();                                          // 0x00528300
                    joan::orig::as_unit(u)->set_task(joan::orig::arg::mptr(static_cast<void*>(&ord)));                 // 0x00527fb0
                } else {
                    spacing = jo::as_unit_type(unit_class(u))->unit_class == 8 ? 200 : 0x50;
                    cols = columns(cnts[i]);
                    half = half_width(cols, spacing);
                    const float ox2 = static_cast<float>(imul((-placed[i]) / cols, spacing));
                    const float oy2 = static_cast<float>(
                        kFpu.sub(static_cast<double>(half), static_cast<double>(imul(placed[i] % cols, spacing))));
                    const i32 a6 = global<i32>(d::g_now) + 0x2bf20;
                    const i32 a5 = ftol(pz);
                    const float t954 = static_cast<float>(ftol(py));
                    const double sa = ang_sin(&heading);
                    const float t958 = static_cast<float>(kFpu.add(kFpu.mul(sa, ox2), t954));
                    const double ca = ang_cos(&heading);
                    const i32 a4 = ftol(kFpu.add(kFpu.mul(ca, oy2), t958));
                    const float t960 = static_cast<float>(ftol(px));
                    const double cb = ang_cos(&heading);
                    const float t964 = static_cast<float>(kFpu.add(kFpu.mul(cb, ox2), t960));
                    const double sb = ang_sin(&heading);
                    const i32 a3 = ftol(kFpu.sub(t964, kFpu.mul(sb, oy2)));
                    const i32 a2 = jo::as_unit(u)->task.priority + 1;
                    Order ord;
                    joan::orig::as_unit_task(&ord)->set(joan::orig::arg::u32(ordtype), static_cast<std::uint8_t>(joan::orig::arg::u32(a2)), joan::orig::arg::u32(a3), joan::orig::arg::u32(a4), joan::orig::arg::u32(a5), joan::orig::arg::u32(a6));  // 0x004e4520
                    joan::orig::as_unit(u)->release_slot();                                          // 0x00528300
                    joan::orig::as_unit(u)->set_task(joan::orig::arg::mptr(static_cast<void*>(&ord)));                 // 0x00527fb0
                }
                placed[i] = placed[i] + 1;
            }
        }

        jo::self<jo::Player>(this)->cursor_mode = attack != 0 ? 4 : 0xc;
        if (over_target()) {
            set_cursor(d::s_POINT00, 1);  // "POINT00"
        } else if (attack != 0) {
            set_cursor(d::s_MOVTO00, 0xe);  // "MOVTO00"
        } else {
            set_cursor(d::s_MOVTO00, 6);  // "MOVTO00"
        }

        Str s;
        const i8 n = voice_prefix(s, d::s_003);  // "003"
        if (n != 1) {
            str_assign(s, d::s_RTS_LTINF_00);  // "RTS_LTINF_00"
            str_append_int(s, rnd(5, 7));
        }
        if (count() == 1 && unit_missing(id(0)) == 0) {
            str_assign(s, d::s_RTS);  // "RTS"
            str_append(s, unit_name(id(0)));
            if (jo::as_unit(unit_get(id(0)))->rank >= 4) {
                str_append(s, str(d::s_003));  // "003"
            } else {
                str_assign(s, d::s_RTS_LTINF_00);  // "RTS_LTINF_00"
                str_append_int(s, rnd(5, 7));
            }
            if (jo::as_unit_type(unit_class(unit_get(id(0))))->unit_class == 8) {
                u8* u = unit_get(id(0));
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})))) != 0) {  // 0x0050d550
                    str_assign(s, d::s_RTS_SIEGE_003);                    // "RTS_SIEGE_003"
                } else {
                    if (joan::hook::settings().fix_immovable_siege && !has_move_position(jo::as_unit_type(unit_class(u)))) {
                        // [Fixes] ImmovableSiege: no crew position moves this weapon, so more men would not help.
                        str_assign(s, "RTS_LTINF_011");  // "unable to comply"
                    } else {
                        str_assign(s, d::s_RTS_SIEGE_005);  // "RTS_SIEGE_005": more men needed to move
                    }
                    jo::self<jo::Player>(this)->cursor_mode = 5;
                    set_cursor(d::s_CMOVTO00, 7);  // "CMOVTO00"
                }
            }
        }
        play_voice(s);
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_08", name, addr, fn)
B("RtsGroup::move_formation_44cfb0", 0x0044cfb0, &H::move_formation);
#undef B

}  // namespace game_b110_08
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

JOAN_COUNTER(c_world_init, "World::init_44e8c0");

void* const kRes = obj(d::g_misc);

struct H {
    // ------------------------------------------------------------------ 0x0044e8c0
    void world_init(u16 id, const char* name, const char* title, const u32* block) {
        c_world_init.hit();
        joan::orig::as_world(this)->cleanup();  // 0x00440320
        jo::self<jo::World>(this)->id = id;
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::World>(this)->name)), joan::orig::arg::str(name), 0x20u);  // strncpy
        jo::self<jo::World>(this)->name[32] = 0;
        if (title != nullptr) {
            std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::World>(this)->title)), joan::orig::arg::str(title), 0x20u);  // strncpy
            jo::self<jo::World>(this)->title[32] = 0;
        } else {
            jo::self<jo::World>(this)->title[0] = 0;
        }
        jo::view<u32>(jo::self<jo::World>(this)->cell_count) = 0;
        if (block != nullptr) {
            for (i32 k = 0; k < 6; ++k) jo::view<u32>(jo::self<jo::World>(this)->bounds[k]) = block[k];
        } else {
            alignas(8) u8 path[0x194];   // [ebp-0x2a8]
            alignas(8) u8 file[((sizeof(jo::FileTxt) > 0x19c ? sizeof(jo::FileTxt) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];   // [ebp-0x444] (x64: the larger FileTxt)
            alignas(4) char line[0x100];  // [ebp-0x114]
            joan::orig::as_string(path)->construct();                                    // 0x005a1170
            joan::orig::as_string(path)->assign(str(d::s_MAP_9cf818));                   // 0x005a1440 "MAP\"
            joan::orig::as_string(path)->append(joan::orig::arg::str(jo::self<jo::World>(this)->name));  // 0x005a1550
            joan::orig::as_string(path)->append(str(d::s_WOR));                   // 0x005a1550 ".WOR"
            void* const n = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());                    // 0x00417d90
            joan::orig::as_file_txt(file)->construct(joan::orig::arg::str(n));          // 0x00589550
            joan::orig::as_file_txt(file)->get_token(1);                             // 0x00589c70
            joan::orig::as_file_txt(file)->get_token(1);
            joan::orig::as_file_txt(file)->get_token(1);
            joan::orig::as_file_txt(file)->read_line(static_cast<char*>(static_cast<void*>(line)), 0xff);                         // 0x00589f10
            const i32 w = joan::crt::atoi(joan::orig::arg::str(static_cast<const char*>(line)));  // atoi
            joan::orig::as_file_txt(file)->get_token(1);
            joan::orig::as_file_txt(file)->read_line(static_cast<char*>(static_cast<void*>(line)), 0xff);                         // 0x00589f10
            const i32 h = joan::crt::atoi(joan::orig::arg::str(static_cast<const char*>(line)));  // atoi
            jo::view<u32>(jo::self<jo::World>(this)->bounds[0]) = 0;
            jo::view<u32>(jo::self<jo::World>(this)->bounds[1]) = 0;
            jo::view<u32>(jo::self<jo::World>(this)->bounds[2]) = 0xc59c4000u;  // -5000.0f
            jo::self<jo::World>(this)->bounds[3] = static_cast<float>(static_cast<i32>(static_cast<u32>(w) << 11));
            jo::self<jo::World>(this)->bounds[4] = static_cast<float>(static_cast<i32>(static_cast<u32>(h) << 11));
            jo::view<u32>(jo::self<jo::World>(this)->bounds[5]) = 0x459c4000u;  // 5000.0f
            joan::orig::as_file_txt(file)->destruct();  // 0x00589890
        }
        alignas(8) u8 path2[0x19c];  // [ebp-0x5e0]
        joan::orig::as_string(path2)->construct();                                    // 0x005a1170
        joan::orig::as_string(path2)->assign(str(d::s_MAP_9cf818));                   // 0x005a1440 "MAP\"
        joan::orig::as_string(path2)->append(joan::orig::arg::str(jo::self<jo::World>(this)->name));  // 0x005a1550
        joan::orig::as_string(path2)->append(str(d::s_TER));                   // 0x005a1550 ".TER"
        void* const n2 = joan::orig::arg::as<void*>(joan::orig::as_string(path2)->c_str());                   // 0x00417d90
        if (joan::orig::as_misc(kRes)->is_file_exist(joan::orig::arg::str(n2)) != 0) {                         // 0x00590b80
            alignas(8) u8 file2[((sizeof(jo::FileTxt) > 0x19c ? sizeof(jo::FileTxt) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x77c] (x64: the larger FileTxt)
            void* const n3 = joan::orig::arg::as<void*>(joan::orig::as_string(path2)->c_str());  // 0x00417d90
            joan::orig::as_file_txt(file2)->construct(joan::orig::arg::str(n3));  // 0x00589550
            joan::orig::as_map(this)->create_territories(joan::orig::arg::mptr(static_cast<void*>(file2)));  // 0x00547590
            joan::orig::as_file_txt(file2)->destruct();                  // 0x00589890
        }
        jo::self<jo::World>(this)->active = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("World::init", 0x0044e8c0, &H::world_init);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
namespace {
namespace game_b104_02_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

void* const kCamMan = obj(d::g_camera_man);
void* const kKinds = obj(d::g_types);

void* const kRandom = obj(d::g_misc);

using joan::call::stdcall_call;

using u32 = std::uint32_t;

using joan::orig::helpers::addr;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");
struct alignas(4) Vec4 {
    float x, y, z, w;
};
struct alignas(4) Plane {
    float a, b, c, d;
};
static_assert(sizeof(Plane) == 16, "Plane");
struct alignas(4) Box {
    u8 b[0x18];
};
struct alignas(4) Order {  // 0x004e4520 record, [ebp-0x28] .. [ebp-0xc]
    u8 b[0x1c];
};

void* const kUnits = obj(d::g_units);
void* const kSpeech = obj(d::g_sfx_player);

i32 word_arg(u16 w) { return static_cast<i32>(static_cast<i16>(w)); }

// common thunks
i32 unit_dead(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_ptr(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* kind_of(u16 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(word_arg(w)))); }  // 0x0042d0a0
u32 unit_busy(void* u) { return joan::orig::arg::as<u32>(joan::orig::as_unit(u)->dead()); }               // 0x0042d0e0
u32 speech(const char* s) { return joan::orig::arg::as<u32>(joan::orig::as_sound(kSpeech)->play_kind(s)); }    // 0x00492c40
void text_ctor(Text& t, const char* s) { joan::orig::as_string(&t)->construct(joan::orig::arg::str(s)); }  // 0x005a11a0
void text_append(Text& t, const char* s) { joan::orig::as_string(&t)->append(joan::orig::arg::str(s)); }  // 0x005a1550
void text_assign(Text& t, const char* s) { joan::orig::as_string(&t)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void text_append_int(Text& t, i32 v) { joan::orig::as_string(&t)->append(joan::orig::arg::i32(v)); }  // 0x005a15d0
const char* text_cstr(Text& t) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&t)->c_str()); }  // 0x00417d90
i32 random_range(i32 lo, i32 hi) { return joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->random_range(joan::orig::arg::i32(lo), joan::orig::arg::i32(hi))); }  // 0x00590690

// 0x004bb660 + 0x00584580 on the unit's +0x2a1 helper
void detach_helper(u8* u) {
    joan::orig::as_action(jo::as_unit(u)->orders)->dispatch(joan::orig::arg::mptr(static_cast<void*>(u)));  // 0x004bb660
    joan::orig::dynarray(jo::as_unit(u)->orders)->zap(1);                   // 0x00584580
}

JOAN_COUNTER(c_siege_board, "World::siege_board_44eb80");
JOAN_COUNTER(c_siege_attack, "World::siege_attack_44f1f0");
JOAN_COUNTER(c_cull_lights, "World::cull_lights_452d20");

struct H {
    // selected group of the current player: count byte and id list
    i32 sel_index() { return static_cast<i8>(jo::view<u8>(jo::self<jo::Player>(this)->group)); }
    i32 sel_count() { return static_cast<i8>(jo::self<jo::Player>(this)->group_counts[sel_index()]); }
    i32 sel_id(i32 i) { return jo::view<i32>(jo::self<jo::Player>(this)->group_units[sel_index() * 100 + i]); }
    i32 sel_first() { return jo::view<i32>(jo::self<jo::Player>(this)->group_units[sel_index() * 100]); }

    // ------------------------------------------------------------------ 0x0044eb80 / 0x0044f1f0
    void order_selection(i32 id, i32 order) {
        for (i32 i = 0; i < sel_count(); ++i) {
            if (unit_dead(sel_id(i)) != 0) continue;
            u8* const u = unit_ptr(sel_id(i));
            if (static_cast<i8>(unit_busy(u)) != 0) continue;
            Order rec;
            joan::orig::as_unit_task(&rec)->set(joan::orig::arg::u32(order), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<i32>(jo::as_unit(u)->task.priority) + 1)), joan::orig::arg::u32(id), 0u, 0u, 0u);         // 0x004e4520
            joan::orig::as_unit(u)->release_slot();  // 0x00528300
            if (jo::as_unit(u)->orders != nullptr && jo::view<u32>(jo::as_order(jo::view<u8*>(jo::as_unit(u)->orders))->current.kind) != 0) detach_helper(u);
            joan::orig::as_unit(u)->set_task(&rec);  // 0x00527fb0
        }
    }
    // jump tables 0x0044f085 / 0x0044f59c: kind type 1 -> 3, 2..4 -> 2, 5..6 -> 4, 7 -> keep, else 3
    void set_mode_from_type(i32 t) {
        switch (t) {
            case 1: jo::self<jo::Player>(this)->point_kind = 3; break;
            case 2:
            case 3:
            case 4: jo::self<jo::Player>(this)->point_kind = 2; break;
            case 5:
            case 6: jo::self<jo::Player>(this)->point_kind = 4; break;
            case 7: break;
            default: jo::self<jo::Player>(this)->point_kind = 3; break;
        }
    }
    void append_padded_random(Text& t, i32 r) {
        text_append(t, str(r >= 10 ? img::Addr{d::s_0} : img::Addr{d::s_00}));
        text_append_int(t, r);
    }
    u32 siege_board_44eb80(i32 id) {
        c_siege_board.hit();
        const u32 dead = static_cast<u32>(unit_dead(id));
        if (dead != 0) return dead & 0xffffff00u;
        const u32 busy = unit_busy(unit_ptr(id));
        if (static_cast<i8>(busy) != 0) return busy & 0xffffff00u;
        order_selection(id, 0xd);
        if (sel_count() == 1 && unit_dead(sel_first()) == 0) {
            u8* const u = unit_ptr(sel_first());
            set_mode_from_type(jo::as_unit_type(kind_of(jo::view<u16>(jo::as_unit(u)->type_id)))->unit_class);
        } else {
            jo::self<jo::Player>(this)->point_kind = 3;
        }
        jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 1;
        Text t;  // [ebp-0x1c0]
        text_ctor(t, str(d::s_RTS));  // "RTS"
        if (sel_count() == 1 && unit_dead(sel_first()) == 0) {
            const i32 w = jo::as_unit(unit_ptr(sel_first()))->id;
            text_append(t, joan::orig::arg::as<const char*>(joan::orig::as_player(this)->name_tag(joan::orig::arg::u32(w))));  // 0x00453da0
            if (jo::as_unit(unit_ptr(sel_first()))->rank >= 4) {
                text_append(t, str(d::s_003));  // "003"
            } else {
                append_padded_random(t, random_range(8, 10));
            }
            const u8* const kind = kind_of(jo::view<u16>(jo::as_unit(unit_ptr(sel_first()))->type_id));
            if (jo::as_unit_type(kind)->unit_class == 8) {
                u8* const u = unit_ptr(sel_first());
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(2)))) == 0) {  // 0x0050d550
                    text_assign(t, str(d::s_RTS_SIEGE_005));         // "RTS_SIEGE_005"
                } else {
                    const i32 r = random_range(8, 10);
                    text_assign(t, str(d::s_RTS_LTINF));  // "RTS_LTINF_"
                    append_padded_random(t, r);
                }
            }
        } else {
            const i32 r = random_range(8, 10);
            text_assign(t, str(d::s_RTS_LTINF));  // "RTS_LTINF_"
            append_padded_random(t, r);
        }
        const u32 r = speech(text_cstr(t));
        return (r & 0xffffff00u) | 1u;
    }
    std::uintptr_t siege_attack_44f1f0(i32 id) {
        c_siege_attack.hit();
        const u32 hit = joan::orig::arg::as<u32>(joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)));  // 0x00425d50
        if (hit != 0) return hit & 0xffffff00u;
        order_selection(id, 0x11);
        if (sel_count() == 1 && unit_dead(sel_first()) == 0) {
            u8* const u = unit_ptr(sel_first());  // [ebp-0x2c]
            if (jo::as_unit(u)->rank >= 4) {
                Text t;  // [ebp-0x1c0]
                text_ctor(t, str(d::s_RTS));  // "RTS"
                text_append(t, joan::orig::arg::as<const char*>(joan::orig::as_player(obj(d::g_player_unit_recno))->name_tag(joan::orig::arg::u32(static_cast<i32>(jo::as_unit(u)->id)))));  // 0x00453da0
                text_append(t, str(d::s_005));  // "005"
                speech(text_cstr(t));
            } else if (jo::as_unit_type(kind_of(jo::view<u16>(jo::as_unit(u)->type_id)))->unit_class == 8) {
                if (joan::orig::arg::as<i32>(joan::orig::as_unit(u)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(2)))) != 0) {  // 0x0050d550
                    Text t;                                  // [ebp-0x354]
                    text_ctor(t, str(d::s_RTS_LTINF_00));           // "RTS_LTINF_00"
                    text_append_int(t, random_range(5, 7));
                    speech(text_cstr(t));
                } else {
                    Text t;                         // [ebp-0x4e8]
                    text_ctor(t, str(d::s_RTS_SIEGE_006));  // "RTS_SIEGE_006"
                    speech(text_cstr(t));
                }
            }
            set_mode_from_type(jo::as_unit_type(kind_of(jo::view<u16>(jo::as_unit(u)->type_id)))->unit_class);
        } else {
            jo::self<jo::Player>(this)->point_kind = 3;
            Text t;                         // [ebp-0x67c]
            text_ctor(t, str(d::s_RTS));  // "RTS"
            text_append(t, str(d::s_LTINF));  // "_LTINF_"
            text_append(t, str(d::s_00));
            text_append_int(t, random_range(8, 9));
            speech(text_cstr(t));
        }
        jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 2;
        return (addr(this) & 0xffffff00u) | 1u;
    }

    // ------------------------------------------------------------------ 0x00452d20
    u32 cull_lights_452d20() {
        c_cull_lights.hit();
        const u32 mode = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(kCamMan)->target());  // 0x00563dc0
        if (mode != 4) return mode;
        void* const cam = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamMan)->motion(4));  // 0x00563c80
        joan::orig::as_camera_man(cam)->unmark_tracked();                             // 0x004258a0
        Plane planes[6];                                            // [ebp-0xc8]
        joan::orig::vector(joan::orig::arg::mptr(static_cast<void*>(planes)), 0x10, 6, joan::orig::arg::mptr(reinterpret_cast<void*>(fn::code_value(fn::Plane4_Plane4_425700))));  // 0x00449af0
        void* const view = global<void*>(d::g_camera);
        joan::orig::as_camera(view)->update_matrices();  // 0x006141a0
        float m[16];                      // [ebp-0x108]
        std::memcpy(m, joan::orig::arg::as<const float*>(joan::orig::as_dialog(global<void*>(d::g_camera))->reset_name()), sizeof m);  // 0x00616f80
        const float k = global<float>(d::flt_0_5);
        const float a = m[11] / m[0] * k;  // [ebp-0x10c]
        const float b = m[11] / m[5] * k;  // [ebp-0x110]
        Vec3 c[8];                         // [ebp-0x68]
        const float zn = jo::as_camera(global<void*>(d::g_camera))->near_plane;
        const float zf = jo::as_camera(global<void*>(d::g_camera))->far_plane;
        c[0] = {-zn * a, -zn * b, zn};
        c[1] = {zn * a, -zn * b, zn};
        c[2] = {zn * a, zn * b, zn};
        c[3] = {-zn * a, zn * b, zn};
        c[4] = {-zf * a, -zf * b, zf};
        c[5] = {zf * a, -zf * b, zf};
        c[6] = {zf * a, zf * b, zf};
        c[7] = {-zf * a, zf * b, zf};
        for (i32 i = 0; i < 8; ++i) {
            void* const mat = joan::orig::arg::as<void*>(joan::orig::as_bone(*global<void**>(d::g_camera))->matrix());  // 0x0060ef80
            import_call<void*>(d::iat_v3d_87_point_transform_2, &c[i], mat);                                   // point_transform
        }
        Box box;  // [ebp-0x12c]
        joan::orig::as_aabb(&box)->set_point(&c[0]);  // 0x0060d430
        for (i32 i = 1; i < 8; ++i) joan::orig::as_aabb(&box)->extend(&c[i]);  // 0x0060d490
        joan::orig::plane_from_points(&planes[0], &c[0], &c[1], &c[2]);  // 0x0060d920
        joan::orig::plane_from_points(&planes[1], &c[4], &c[7], &c[6]);
        joan::orig::plane_from_points(&planes[2], &c[1], &c[0], &c[4]);
        joan::orig::plane_from_points(&planes[3], &c[2], &c[1], &c[5]);
        joan::orig::plane_from_points(&planes[4], &c[3], &c[2], &c[6]);
        joan::orig::plane_from_points(&planes[5], &c[0], &c[3], &c[7]);
        i32 count = 0;  // [ebp-0x130] (0x005ef610 writes it first)
        u8** const list = joan::orig::arg::as<u8**>(joan::orig::as_scene(jo::view<void*>(jo::as_world(global<u8*>(d::g_world))->scene))->nodes_in_box(&box, &count));  // 0x005ef610
        const float zero = global<float>(d::k_9cf5c4);
        for (i32 i = 0; i < count; ++i) {
            i32 state = 2;
            for (i32 j = 0; j < 6; ++j) {
                float lo, hi;  // [ebp-0x13c], [ebp-0x140]
                joan::orig::box_plane_extent(&planes[j], joan::orig::arg::mptr(jo::as_scene_s(list[i])->desc), &lo, &hi);  // 0x0060dc80
                if (!(hi >= zero)) {
                    state = 0;
                    break;
                }
                if (!(lo >= zero)) state = 1;
            }
            if (state == 0) continue;
            for (u8* node = joan::orig::arg::as<u8*>(joan::orig::as_scene_s(list[i])->first_actor()); node != nullptr; node = jo::view<u8*>(jo::as_actor(node)->scene_prev)) {  // 0x005effa0
                if (jo::as_actor(node)->kind != 1) continue;
                const u8* const rec = jo::view<u8*>(jo::as_actor(node)->owner);
                const u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_map_object(rec)->type_id))));  // 0x00425ce0
                if (jo::as_item_def(type)->can_transparent == 0) continue;
                Vec4 s = *reinterpret_cast<const Vec4*>(jo::as_actor_def(jo::view<u8*>(jo::as_actor(node)->def))->model->sphere);
                s.w = s.w * jo::as_actor(node)->scale;
                Vec3 off;                                  // [ebp-0x16c]
                joan::orig::as_actor(node)->position(&off);  // 0x005ac270
                s.x = s.x + off.x;
                s.y = s.y + off.y;
                s.z = s.z + off.z;
                const i32 r = ftol(static_cast<double>(s.w));
                const float neg_r = static_cast<float>(static_cast<i32>(0u - static_cast<u32>(r)));
                i32 inside = 2;
                for (i32 j = 0; j < 6; ++j) {
                    const Plane& p = planes[j];
                    const float d = s.x * p.a + s.y * p.b + s.z * p.c + p.d;
                    if (!(d >= neg_r)) {
                        inside = 0;
                        break;
                    }
                    if (static_cast<double>(r) > static_cast<double>(d)) inside = 1;
                }
                if (inside == 0) continue;
                if (jo::view<u32>(jo::as_actor(node)->faded) == 0) {
                    joan::orig::as_actor(node)->set_faded(static_cast<char>(joan::orig::arg::i32(1)));  // 0x005ae590
                    u32 tmp188 = 0, tmp184 = 0;
                    void* const col = joan::orig::as_colour(&tmp188)->construct(0xff, 0xff, 0xff, 0x60);  // 0x0042a260
                    joan::orig::as_colour(&tmp184)->from_rgba(joan::orig::arg::mptr(col));                                         // 0x005e95b0
                    joan::orig::as_plane(jo::view<void*>(jo::as_actor(node)->plane))->set_fade(joan::orig::arg::u32(tmp184));                          // 0x005d5e80
                    joan::orig::as_camera_man(cam)->track(joan::orig::arg::mptr(node));                                              // 0x00425720
                } else {
                    joan::orig::as_camera_man(cam)->mark_tracked(joan::orig::arg::mptr(node));  // 0x00425840
                }
            }
        }
        joan::orig::as_camera_man(cam)->sweep_tracked();  // 0x004258f0
        return 0;  // 0x004258f0 returns nothing; the only caller discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("World::siege_board_44eb80", 0x0044eb80, &H::siege_board_44eb80);
B("World::siege_attack_44f1f0", 0x0044f1f0, &H::siege_attack_44f1f0);
B("World::cull_lights_452d20", 0x00452d20, &H::cull_lights_452d20);
#undef B

}  // namespace game_b104_02_2
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01_3 {

using joan::call::thiscall;
using joan::img::obj;

using joan::orig::helpers::s16;
using joan::orig::helpers::s8;

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kUnits = obj(d::g_units);   // 0x00428b70 / 0x0041b3c0 / 0x0041b3f0 / 0x0051fdd0 / 0x00417ad0
void* const kTypes = obj(d::g_types);   // 0x0042d0a0

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_nearest, "Unit::nearest_enemy_of");
JOAN_COUNTER(c_army, "Army::apply");

struct H {
    // ------------------------------------------------------------ 0x0044f0b0 (ret 4)
    std::int16_t nearest_enemy_of(std::int32_t kind) {
        c_nearest.hit();
        float range;
        if (kind == 2) {
            range = 70.0f;  // 0x428c0000
        } else if (kind == 3) {
            range = 150.0f;  // 0x43160000
        } else {
            return 0;
        }
        Vec3 self_pos;                                         // ebp-0x20
        joan::orig::as_unit(this)->position(&self_pos);  // 0x004fedb0
        for (std::int32_t i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
            std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (u == nullptr) continue;
            if (s16(jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->id)) == i) continue;
            if (s8(jo::as_unit(u)->can_be_ridden) == 0) continue;
            Vec3 pos;                                         // ebp-0x14
            joan::orig::as_unit(u)->position(&pos);     // 0x004fedb0
            std::uint8_t* const type =
                joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::as_unit(u)->type_id)))));  // 0x0042d0a0
            Vec3 tmp;
            Vec3 d = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &pos, &self_pos));  // 0x0060cc90
            // A component beyond the range by more than the length's rounding (1e-4 relative; ranges <= 150): the
            // length is beyond it too - the far units skip the x87 length. (NaN compares false: the length decides.)
            const float far = range * 1.0001f;
            if (std::fabs(d.x) > far || std::fabs(d.y) > far || std::fabs(d.z) > far) continue;
            const float len = v3d_float(d::iat_v3d_87_length, &d);                   // length
            if (len > range) continue;                                       // test ah,0x41 je
            if (s16(jo::view<std::uint16_t>(jo::as_unit_type(type)->kind)) == kind) return static_cast<std::int16_t>(i);
        }
        return 0;
    }

    // ------------------------------------------------------------ 0x00452be0 (ret 4)
    std::int32_t army_id(std::int32_t i) {
        return jo::view<std::int32_t>(jo::self<jo::Player>(this)->group_units[s8(jo::view<std::uint8_t>(jo::self<jo::Player>(this)->group)) * 100 + i]);
    }
    void army_apply(std::uint32_t flag) {
        c_army.hit();
        if (jo::self<jo::Player>(this)->mode != 4) return;
        for (std::int32_t i = 0; i < s8(jo::self<jo::Player>(this)->group_counts[s8(jo::view<std::uint8_t>(jo::self<jo::Player>(this)->group))]); ++i) {
            if (unit_missing(army_id(i)) != 0) continue;
            if (s8(joan::orig::arg::as<std::uint8_t>(joan::orig::as_unit(unit_get(army_id(i)))->dead())) != 0) continue;  // 0x0042d0e0
            joan::orig::as_unit(unit_get(army_id(i)))->attach_weapon_model(joan::orig::arg::i32(static_cast<std::int8_t>(flag)));  // 0x00519250
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Unit::nearest_enemy_of", 0x0044f0b0, &H::nearest_enemy_of);
B("Army::apply", 0x00452be0, &H::army_apply);
#undef B

}  // namespace game_b92_01_3
}  // namespace

// ---------------------------------------- from modules/game_b87_05.cpp (game/b87_05)
namespace {
namespace game_b87_05 {

using joan::call::thiscall;
using joan::img::obj;

void* const kStations = obj(d::g_nations);

using joan::orig::helpers::addr_of;

std::uint32_t station_missing(std::int32_t id) { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_list(kStations)->is_null(joan::orig::arg::i32(id))); }  // 0x00484870
void* station_get(std::int32_t id) { return joan::orig::as_dynarray(kStations)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0047b600

JOAN_COUNTER(c_set_nation, "Place::set_nation");

struct H {
    // ------------------------------------------------------------ 0x0044f5c0 (ret 4)
    std::uintptr_t set_nation(std::int32_t nation) {
        c_set_nation.hit();
        if (jo::self<jo::Place>(this)->nation == nation) return addr_of(this);
        std::uintptr_t eax;
        if (station_missing(jo::self<jo::Place>(this)->nation) == 0)
            joan::orig::as_nation(station_get(jo::self<jo::Place>(this)->nation))->remove_member(joan::orig::arg::mptr(this));  // 0x0045c460
        jo::self<jo::Place>(this)->nation = nation;
        eax = station_missing(jo::self<jo::Place>(this)->nation);
        if (eax == 0) joan::orig::as_nation(station_get(jo::self<jo::Place>(this)->nation))->toggle_station(this);  // 0x00498b70 (returns nothing; callers discard the result)
        for (void* node = jo::view<void*>(jo::self<jo::Place>(this)->facility_tail); node != nullptr;) {
            virtual_call(node, 0x14, jo::self<jo::Place>(this)->nation, nation);
            node = jo::as_facility(node)->prev;
            eax = addr_of(node);
        }
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_05", name, addr, fn)
B("Place::set_nation", 0x0044f5c0, &H::set_nation);
#undef B

}  // namespace game_b87_05
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01_2 {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::orig::helpers::addr;

// 0x0044f6b0 qsort comparator: float +4 ascending (NaN -> -1), then int +0 (the second int
// test compares b with b and never fires).
struct IdDist {  // the sorted {unit id, distance} entries (8 bytes)
    i32 id;
    float dist;
};
i32 JOAN_CDECL by_float_then_int(const void* pa, const void* pb) {
    const IdDist* const a = static_cast<const IdDist*>(pa);
    const IdDist* const b = static_cast<const IdDist*>(pb);
    if (!(a->dist >= b->dist)) return -1;  // test ah,1: less or unordered
    if (a->dist > b->dist) return 1;       // test ah,0x41 == 0
    if (a->id < b->id) return -1;
    return 0;  // the original's `b[0] > b[0]` test can never be true
}

struct H {
    // ---------------------------------------------------------------- 0x00451390
    std::uintptr_t detach_451390() {
        std::uintptr_t eax = addr(this);
        if (jo::self<jo::Wall>(this)->actor_a != nullptr && jo::self<jo::Wall>(this)->actor_a->scene != nullptr)
            eax = joan::orig::arg::as<u32>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Wall>(this)->actor_a->scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Wall>(this)->actor_a))));  // 0x005efdb0
        if (jo::self<jo::Wall>(this)->actor_b != nullptr) {
            eax = joan::orig::arg::as<u32>(jo::self<jo::Wall>(this)->actor_b);
            if (jo::self<jo::Wall>(this)->actor_b->scene != nullptr)
                eax = joan::orig::arg::as<u32>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b->scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Wall>(this)->actor_b))));
        }
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Sort::by_float_then_int", 0x0044f6b0, &by_float_then_int);
B("Obj::detach_451390", 0x00451390, &H::detach_451390);
#undef B

}  // namespace game_b102_01_2
}  // namespace

// ---------------------------------------- from modules/game_b115_12.cpp (fix/uninit)
namespace {
namespace game_b115_12_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

using joan::orig::helpers::at;
i32 w16(i32 v) { return static_cast<i32>(static_cast<i16>(static_cast<u16>(static_cast<u32>(v) & 0xffffu))); }
using joan::orig::helpers::wrap_sub;

constexpr img::Addr kUnits(d::g_units);
constexpr img::Addr kTypes(d::g_types);
constexpr img::Addr kUtil(d::g_misc);
constexpr img::Addr kSpeech(d::g_sfx_player);

JOAN_COUNTER(c_assign, "Leader::assign_troops_44f750");

void text_ctor(Text& t, const char* s) { joan::orig::as_string(&t)->construct(joan::orig::arg::str(s)); }  // 0x005a11a0
void text_set(Text& t, const char* s) { joan::orig::as_string(&t)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void text_add(Text& t, const char* s) { joan::orig::as_string(&t)->append(joan::orig::arg::str(s)); }  // 0x005a1550
void text_add_num(Text& t, i32 v) { joan::orig::as_string(&t)->append(joan::orig::arg::i32(v)); }  // 0x005a15d0
const char* c_str(Text& t) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&t)->c_str()); }  // 0x00417d90
void* unit_of(i32 id) { return joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 unit_dead(i32 id) { return joan::orig::as_dynarray(img::obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void delete_virtual(void* p) {
    if (p != nullptr) virtual_call<void*>(p, 0, i32{1});
}

struct H {
    // -------------------------------------------------------------- 0x006a9570
    struct FV {
        float x, y, z;
    };
    static_assert(sizeof(FV) == 12, "sizes");
    static FV copy_ret(const void* p) {  // 3 dwords copied from a returned vector pointer
        FV v;
        std::memcpy(&v, p, sizeof v);
        return v;
    }
    static float vlen(const void* v) { return v3d_float(d::iat_v3d_87_length, v); }

    // -------------------------------------------------------------- 0x0044f750
    static i32 vis_count(u8* s) { return jo::view<i8>(jo::as_player(s)->group_counts[jo::as_player(s)->group]); }
    static i32 vis_id(u8* s, i32 i) {
        return jo::view<i32>(jo::as_player(s)->group_units[static_cast<i32>(jo::as_player(s)->group) * 100 + i]);
    }
    static void vis_clear(u8* s, i32 i) {
        jo::view<i32>(jo::as_player(s)->group_units[static_cast<i32>(jo::as_player(s)->group) * 100 + i]) = 0;
    }
    static u8* type_of(u8* unit) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(img::obj(kTypes))->get(joan::orig::arg::i16(w16(jo::view<u16>(jo::as_unit(unit)->type_id))))); }  // 0x0042d0a0
    static i32 type_id(const char* name) { return joan::orig::arg::as<i32>(joan::orig::as_unit_types(img::obj(kTypes))->find(joan::orig::arg::str(name))); }              // 0x005270b0
    static bool light_infantry(u8* u) {
        return static_cast<i32>(jo::as_unit(u)->type_id) == type_id(str(d::s_ENGLISH_LIGHT_INFANTRY)) ||
               static_cast<i32>(jo::as_unit(u)->type_id) == type_id(str(d::s_FRENCH_LIGHT_INFANTRY)) ||
               static_cast<i32>(jo::as_unit(u)->type_id) == type_id(str(d::s_MER_LIGHT_INFANTRY));
    }
    static u8 in_castle(void* u) { return joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead()); }  // 0x0042d0e0
    static i32 formation(u8* u) { return joan::orig::arg::as<i32>(joan::orig::as_unit(u)->leads()); }  // 0x0044a660
    static i32 member_count(u8* leader) { return jo::as_group(jo::as_unit(leader)->group)->count; }
    static i32 member_id(u8* leader, i32 k) { return jo::as_group(jo::as_unit(leader)->group)->members[k]; }
    static void detach(u8* u) {  // +0x31d controller: stop and delete its +0x4a task
        if (jo::as_unit(u)->draw != nullptr && jo::as_unit(u)->draw->label_ != nullptr) {
            joan::orig::as_unit_draw(jo::as_unit(u)->draw)->detach_b();  // 0x005228e0
            delete_virtual(jo::view<void*>(jo::as_unit(u)->draw->label_));
            jo::as_unit(u)->draw->label_ = nullptr;
        }
    }
    static void speak(const char* name) { joan::orig::as_sound(img::obj(kSpeech))->play_kind(name); }  // 0x00492c40
    static void speak_rts(u8* s, i32 id, bool veteran) {
        Text t;
        text_ctor(t, str(d::s_RTS));  // "RTS"
        text_add(t, joan::orig::arg::as<const char*>(joan::orig::as_player(s)->name_tag(joan::orig::arg::u32(id))));  // 0x00453da0
        text_add(t, veteran ? str(d::s_012) : str(d::s_005));  // "012" / "005"
        speak(c_str(t));
    }
    struct alignas(4) Order {  // 0x004e4520 order, 0x19 bytes
        u8 b[0x1c];
    };
    static void order_ctor(Order& o, i32 kind, i32 slot, i32 leader_id, i32 a, i32 b, i32 c) {
        joan::orig::as_unit_task(&o)->set(joan::orig::arg::u32(kind), static_cast<std::uint8_t>(joan::orig::arg::u32(slot)), joan::orig::arg::u32(leader_id), joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c));  // 0x004e4520
    }
    static void unlink_slot(u8* u, void* by) {  // 0x00528300 and the +0x2a1 group removal
        joan::orig::as_unit(u)->release_slot();  // 0x00528300
        if (jo::as_unit(u)->orders != nullptr && jo::view<u32>(jo::as_unit(u)->orders->current.kind) != 0) {
            joan::orig::as_action(jo::as_unit(u)->orders)->dispatch(joan::orig::arg::mptr(by == nullptr ? static_cast<void*>(u) : by));  // 0x004bb660
            joan::orig::dynarray(jo::as_unit(u)->orders)->zap(i32{1});                                   // 0x00584580
        }
    }
    u8 assign_troops(i32 unit_id, i32 flag) {
        c_assign.hit();
        u8* const s = reinterpret_cast<u8*>(this);
        if (unit_dead(unit_id) != 0 || in_castle(unit_of(unit_id)) != 0) return 0;
        u8* const leader = static_cast<u8*>(unit_of(unit_id));  // [ebp-0x8]
        const u32 now = global<u32>(d::g_now);                // [ebp-0xc]
        u8 spoke = 0;                                           // [ebp-0x10]
        if (jo::as_unit(leader)->crew_slots_ready != 0) {
            u16 c14 = 0, c18 = 0, c1c = 0;
            for (i32 i = 0; i < jo::as_unit_type(type_of(leader))->slot_count; ++i) {
                if (unit_dead(jo::view<i32*>(jo::as_unit(leader)->mask_slots_b)[i]) == 0) c1c = static_cast<u16>(c1c + 1);
            }
            for (i32 i = 0; i < jo::as_unit_type(type_of(leader))->slot_count; ++i) {
                if (unit_dead(jo::view<i32*>(jo::as_unit(leader)->mask_slots)[i]) != 0) c14 = static_cast<u16>(c14 + 1);
            }
            for (i32 i = 0; i < vis_count(s); ++i) {
                if (unit_dead(vis_id(s, i)) != 0) continue;
                u8* const u = static_cast<u8*>(unit_of(vis_id(s, i)));  // [ebp-0x24]
                if (in_castle(u) != 0) continue;
                if (!light_infantry(u)) continue;
                if (!(static_cast<i32>(static_cast<i16>(c18)) <
                      static_cast<i32>(static_cast<i16>(c14)) - static_cast<i32>(static_cast<i16>(c1c)))) {
                    bool released = false;
                    for (i32 k = 0; k < jo::as_unit_type(type_of(leader))->slot_count; ++k) {
                        if (unit_dead(jo::view<i32*>(jo::as_unit(leader)->mask_slots_b)[k]) != 0) continue;
                        u8* const m = static_cast<u8*>(unit_of(jo::view<i32*>(jo::as_unit(leader)->mask_slots_b)[k]));  // [ebp-0x30]
                        bool keep = false;
                        for (i32 j = 0; j < vis_count(s); ++j) {
                            if (unit_dead(vis_id(s, j)) != 0) continue;
                            u8* const v = static_cast<u8*>(unit_of(vis_id(s, j)));
                            if (in_castle(v) != 0) continue;
                            if (!light_infantry(v)) continue;
                            if (v == m) {
                                keep = true;
                                break;
                            }
                        }
                        if (!(jo::view<u32>(jo::as_unit(m)->task_kind) < now)) keep = true;
                        if (keep) continue;
                        unlink_slot(m, nullptr);
                        jo::view<i32*>(jo::as_unit(leader)->mask_slots_b)[k] = 0;
                        jo::view<i32*>(jo::as_unit(leader)->mask_slots)[k] = 0;
                        jo::view<u32>(jo::as_unit(m)->weapon_slot) = 0;
                        jo::view<u32>(jo::as_unit(m)->weapon) = 0;
                        released = true;
                        break;
                    }
                    if (!released) break;
                }
                const i32 i40 = joan::orig::arg::as<i32>(joan::orig::as_unit(leader)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})))) == 0 ? 1 : 0;  // 0x0050d550
                const i32 i44 = joan::orig::arg::as<i32>(joan::orig::as_unit(leader)->has_mask_ready(static_cast<std::int8_t>(joan::orig::arg::i32(i32{2})))) == 0 ? 1 : 0;
                if (i40 == 0 && i44 == 0) break;
                if (jo::view<u32>(jo::as_unit(u)->weapon) != 0 && jo::view<u32>(jo::as_unit(u)->weapon_slot) != 0) {
                    if (unit_dead(jo::as_unit(u)->weapon) == 0) {
                        jo::view<i32*>(jo::as_unit(unit_of(jo::as_unit(u)->weapon))->mask_slots)[jo::as_unit(u)->weapon_slot] = 0;
                        jo::view<i32*>(jo::as_unit(unit_of(jo::as_unit(u)->weapon))->mask_slots_b)[jo::as_unit(u)->weapon_slot] = 0;
                        const float sp = joan::orig::arg::as<float>(joan::orig::as_ground_object(unit_of(jo::as_unit(u)->weapon))->heading());  // 0x004ff850
                        joan::orig::as_unit(u)->set_heading(joan::orig::arg::f32(sp / global<float>(d::flt_0_017453292)));             // 0x0050abd0
                        joan::orig::as_unit(unit_of(jo::as_unit(u)->weapon))->play_idle(i32{0}, i32{1});    // 0x00508b90
                    }
                    jo::view<u32>(jo::as_unit(u)->weapon) = 0;
                    jo::view<u32>(jo::as_unit(u)->slot_index) = 0;
                    jo::view<u32>(jo::as_unit(u)->weapon_slot) = 0;
                    jo::view<u16>(jo::as_unit(u)->anim_special) = 0;
                }
                joan::orig::as_member(u)->release();  // 0x0042fcb0
                joan::orig::as_unit(u)->release_slot();  // 0x00528300
                if (jo::as_unit(u)->orders != nullptr && jo::view<u32>(jo::as_unit(u)->orders->current.kind) != 0) {
                    joan::orig::as_action(jo::as_unit(u)->orders)->dispatch(joan::orig::arg::mptr(unit_of(vis_id(s, i))));
                    joan::orig::dynarray(jo::as_unit(u)->orders)->zap(i32{1});
                }
                Order o60;
                order_ctor(o60, 0xf, static_cast<i32>(jo::as_unit(u)->task.priority) + 1, static_cast<i32>(jo::as_unit(leader)->id),
                           i40 != 0 ? 0 : 1, 0, 0);
                joan::orig::as_unit(u)->set_task(&o60);         // 0x00527fb0
                joan::orig::as_unit(u)->attach_weapon_model(i32{0});       // 0x00519250
                for (i32 j = 0; j < vis_count(s); ++j) {
                    if (vis_id(s, j) == static_cast<i32>(jo::as_unit(u)->id)) vis_clear(s, j);
                }
                c18 = static_cast<u16>(c18 + 1);
                jo::as_player(s)->cursor_mode = 6;
                spoke = 1;
                jo::view<i32>(jo::as_player(s)->point_kind) = 8;
                std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_player(s)->point_name)), str(d::s_ASSTO_00), u32{8});  // "ASSTO_00"
            }
            if (spoke != 0) {
                if (joan::orig::arg::as<i32>(joan::orig::as_misc(img::obj(kUtil))->random_range(i32{0}, i32{1})) != 0) {  // 0x00590690
                    speak(str(d::s_RTS_SIEGE_001));  // "RTS_SIEGE_001"
                } else {
                    speak(str(d::s_RTS_SIEGE_002));  // "RTS_SIEGE_002"
                }
            }
            return finish(s, spoke);
        }
        // 0x0044ff10
        i8 f68 = 0;   // [ebp-0x68]
        i32 c6c = 0;  // [ebp-0x6c]
        for (i32 i = 0; i < vis_count(s); ++i) {
            if (unit_dead(vis_id(s, i)) != 0) continue;
            u8* const u = static_cast<u8*>(unit_of(vis_id(s, i)));
            if (in_castle(u) != 0) continue;
            if (formation(u) != 0 || !(jo::as_unit(u)->rank < 3)) {
                f68 = 1;
                continue;
            }
            if (static_cast<i32>(jo::as_unit(u)->group_id) != static_cast<i32>(jo::as_unit(leader)->group_id)) ++c6c;
        }
        if (f68 == 0 && formation(leader) != 0) {
            for (i32 k = member_count(leader) - 1; k >= 0; --k) {
                u8* const m = static_cast<u8*>(unit_of(member_id(leader, k)));
                if (in_castle(m) == 0) continue;
                joan::orig::as_member(m)->release();  // 0x0042fcb0
                detach(m);
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(leader)->morale_level()) < c6c + member_count(leader)) {  // 0x00448080
                if (flag == 0 && member_count(leader) != 0) {
                    jo::as_player(s)->castle_popup_open = 1;
                    if (global<u8>(d::g_game_running) != 0) {
                        global<u8>(d::g_game_running) = 0;
                        global<u16>(d::g_saved_game_speed) = static_cast<u16>(ftol(static_cast<double>(global<float>(d::g_game_speed))));
                        joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::u32(i32{0}));  // 0x0042f930
                    }
                    const i32 have = joan::orig::arg::as<i32>(joan::orig::as_unit(leader)->morale_level());
                    const i32 spare = wrap_sub(have, c6c) < 0 ? 0 : wrap_sub(have, c6c);
                    const i32 need = wrap_sub(member_count(leader), spare);
                    const i32 joan = static_cast<i32>(jo::as_unit(leader)->type_id) == type_id(str(d::s_JOAN_OF_ARC)) ? 1 : 0;
                    const char* const msg = joan::orig::as_menu(obj(d::g_strings))->charmove_1(jo::as_unit(leader)->name_, joan::orig::arg::u32(have), joan::orig::arg::u32(c6c), joan::orig::arg::u32(need), joan::orig::arg::u32(joan));  // 0x004de530
                    joan::orig::as_popup(obj(d::g_popup_a52df8))->set_title(nullptr, nullptr);  // 0x006d52c0
                    joan::orig::as_popup(obj(d::g_popup_a52df8))->set_text_6d5230(msg, nullptr);    // 0x006d5230
                    joan::orig::as_popup(obj(d::g_popup_a52df8))->set_buttons(joan::orig::arg::u32(i32{3}));         // 0x006d5340
                    joan::orig::as_popup(obj(d::g_popup_a52df8))->load_gpopup();                      // 0x006d3be0
                    return 0;
                }
                assign_sorted(s, leader, c6c, f68, spoke);
            } else {
                for (i32 i = 0; i < vis_count(s); ++i) {
                    if (unit_dead(vis_id(s, i)) != 0) continue;
                    u8* const u = static_cast<u8*>(unit_of(vis_id(s, i)));  // [ebp-0x590]
                    if (in_castle(u) != 0 || formation(u) != 0 || !(jo::as_unit(u)->rank < 3) ||
                        static_cast<i32>(jo::as_unit(u)->group_id) == static_cast<i32>(jo::as_unit(leader)->group_id)) {
                        continue;
                    }
                    if (jo::as_unit_type(type_of(u))->unit_class == 8 || jo::as_unit_type(type_of(u))->unit_class == 9) continue;
                    if (!(member_count(leader) < joan::orig::arg::as<i32>(joan::orig::as_unit(leader)->morale_level()))) break;
                    joan::orig::as_member(u)->release();
                    joan::orig::as_unit(u)->link_to(static_cast<i32>(jo::as_unit(leader)->id));  // 0x00438f80
                    if (jo::as_unit(u)->draw != nullptr) {
                        detach(u);
                        joan::orig::as_unit_draw(jo::as_unit(u)->draw)->create_label();  // 0x00520e80
                    }
                    jo::as_player(s)->cursor_mode = 7;
                    spoke = 1;
                    jo::view<i32>(jo::as_player(s)->point_kind) = 9;
                    std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_player(s)->point_name)), str(d::s_ASSIGN01), u32{8});  // "ASSIGN01"
                }
                if (spoke != 0) speak_rts(s, static_cast<i32>(jo::as_unit(leader)->id), jo::as_unit(leader)->rank >= 4);
            }
        }
        // 0x00450b63
        if (f68 != 0 && jo::as_unit(leader)->rank >= 4) {
            for (i32 i = 0; i < vis_count(s); ++i) {
                if (unit_dead(vis_id(s, i)) != 0) continue;
                u8* const u = static_cast<u8*>(unit_of(vis_id(s, i)));  // [ebp-0x72c]
                if (in_castle(u) != 0) continue;
                if (jo::as_unit(u)->rank < 3) continue;
                joan::orig::as_unit(u)->release_slot();
                if (jo::as_unit(u)->orders != nullptr && jo::view<u32>(jo::as_unit(u)->orders->current.kind) != 0) {
                    joan::orig::as_action(jo::as_unit(u)->orders)->dispatch(joan::orig::arg::mptr(unit_of(vis_id(s, i))));
                    joan::orig::dynarray(jo::as_unit(u)->orders)->zap(i32{1});
                }
                Order o748;
                order_ctor(o748, 0xa, static_cast<i32>(jo::as_unit(u)->task.priority) + 1, static_cast<i32>(jo::as_unit(leader)->id), 0, 0, 0);
                joan::orig::as_unit(u)->set_task(&o748);
                jo::as_player(s)->cursor_mode = 8;
                spoke = 1;
                jo::view<i32>(jo::as_player(s)->point_kind) = 0xa;
                std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_player(s)->point_name)), str(d::s_FOLLOW01), u32{8});  // "FOLLOW01"
                if (u == leader) continue;
                Text t;
                text_ctor(t, str(d::s_RTS));  // "RTS"
                text_add(t, joan::orig::arg::as<const char*>(joan::orig::as_player(s)->name_tag(joan::orig::arg::u32(static_cast<i32>(jo::as_unit(u)->id)))));
                if (jo::as_unit(u)->rank >= 4) {
                    const char* const tn = jo::as_unit_type(type_of(leader))->name;
                    if (joan::crt::stricmp(joan::orig::arg::str(tn), str(d::s_CHARLIE7)) != 0) {  // "CHARLIE7"
                        text_add(t, str(d::s_013));                              // "013"
                    } else {
                        text_add(t, str(d::s_014));  // "014"
                    }
                } else {
                    text_set(t, str(d::s_RTS_LTINF_00));  // "RTS_LTINF_00"
                    text_add_num(t, joan::orig::arg::as<i32>(joan::orig::as_misc(img::obj(kUtil))->random_range(i32{5}, i32{7})));
                }
                speak(c_str(t));
            }
        }
        return finish(s, spoke);
    }
    // 0x00450e1a
    static u8 finish(u8* s, u8 spoke) {
        if (spoke == 0) {
            jo::view<i32>(jo::as_player(s)->point_kind) = 0xb;
            std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_player(s)->point_name)), str(d::s_CASSTO00), u32{8});  // "CASSTO00"
            Text t;
            text_ctor(t, str(d::s_RTS_LTINF_0));  // "RTS_LTINF_0"
            text_add_num(t, joan::orig::arg::as<i32>(joan::orig::as_misc(img::obj(kUtil))->random_range(i32{0xb}, i32{0xd})));
            speak(c_str(t));
        }
        return 1;
    }
    // 0x0045023d: nearest-first assignment through the {id, dist} array [ebp-0x3ac]. R emulates
    // the frame [ebp-0x3ac, ebp): entry k at R + 8k; live locals inside it: [ebp-0x6c] count
    // (R + 0x340), [ebp-0x68] flag byte (R + 0x344), [ebp-0x10] spoke flag byte (R + 0x39c).
    // fix/uninit (header): entries >= 116 are not written (116 would overwrite the leader pointer
    // [ebp-0x8]) and the assign loop stops at index 0. x64: R holds only i32 / float / byte values (no pointers),
    // so its x86 offsets are the same bytes on x64.
    static constexpr i32 kMaxEntries = 116;
    static void assign_sorted(u8* s, u8* leader, i32& c6c_io, i8& f68_io, u8& spoke_io) {
        alignas(4) u8 R[0x3ac];
        std::memset(R, 0, sizeof R);
        at<i32>(R, 0x340) = c6c_io;
        at<i8>(R, 0x344) = f68_io;
        at<u8>(R, 0x39c) = spoke_io;
        i32 n = 0;  // [ebp-0x3b0]
        float lpos[3] = {0.0f, 0.0f, 0.0f};
        joan::orig::as_unit(leader)->position(joan::orig::arg::mptr(lpos));  // 0x004fedb0
        for (i32 k = member_count(leader) - 1; k >= 0; --k) {
            if (k >= kMaxEntries) continue;
            u8* const u = static_cast<u8*>(unit_of(member_id(leader, k)));
            float upos[3] = {0.0f, 0.0f, 0.0f};
            joan::orig::as_unit(u)->position(joan::orig::arg::mptr(upos));
            at<i32>(R, 8 * k) = member_id(leader, k);
            FV tmp;
            FV d = copy_ret(joan::orig::arg::as<void*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(lpos), joan::orig::arg::ptr(upos))));  // 0x0060cc90
            at<float>(R, 8 * k + 4) = vlen(&d);
        }
        const i32 lim = joan::orig::arg::as<i32>(joan::orig::as_unit(leader)->morale_level());  // [ebp-0x3d4]
        {
            const i32 cnt = member_count(leader);
            const u32 num = static_cast<u32>(cnt < 0 ? 0 : (cnt > kMaxEntries ? kMaxEntries : cnt));
            joan::orig::libc::qsort(joan::orig::arg::mptr(static_cast<void*>(R)), joan::orig::arg::u32(num), joan::orig::arg::u32(u32{8}), fn::code_ptr(fn::Sort_by_float_then_int));  // qsort
        }
        const i32 d8 = wrap_sub(lim, at<i32>(R, 0x340)) < 0 ? 0 : wrap_sub(lim, at<i32>(R, 0x340));
        for (i32 k = member_count(leader) - 1; k >= d8; --k) {
            if (k >= kMaxEntries) continue;
            u8* const u = static_cast<u8*>(unit_of(at<i32>(R, 8 * k)));
            joan::orig::as_member(u)->release();  // 0x0042fcb0
            detach(u);
        }
        const i32 e0 = wrap_sub(lim, d8) < at<i32>(R, 0x340) ? wrap_sub(lim, d8) : at<i32>(R, 0x340);
        for (i32 i = 0; i < vis_count(s); ++i) {
            if (unit_dead(vis_id(s, i)) != 0) continue;
            u8* const u = static_cast<u8*>(unit_of(vis_id(s, i)));
            if (in_castle(u) != 0) continue;
            if (jo::as_unit_type(type_of(u))->unit_class == 8 || jo::as_unit_type(type_of(u))->unit_class == 9) continue;
            if (formation(u) != 0) continue;
            if (!(jo::as_unit(u)->rank < 3)) continue;
            if (static_cast<i32>(jo::as_unit(u)->group_id) == static_cast<i32>(jo::as_unit(leader)->group_id)) continue;
            float upos[3] = {0.0f, 0.0f, 0.0f};
            joan::orig::as_unit(u)->position(joan::orig::arg::mptr(upos));
            if (n >= kMaxEntries) continue;
            at<i32>(R, 8 * n) = vis_id(s, i);
            FV tmp;
            FV d = copy_ret(joan::orig::arg::as<void*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(lpos), joan::orig::arg::ptr(upos))));
            at<float>(R, 8 * n + 4) = vlen(&d);
            ++n;
        }
        joan::orig::libc::qsort(joan::orig::arg::mptr(static_cast<void*>(R)), joan::orig::arg::u32(static_cast<u32>(n)), joan::orig::arg::u32(u32{8}), fn::code_ptr(fn::Sort_by_float_then_int));
        for (i32 k = n - 1; k >= wrap_sub(n, e0); --k) {
            if (k < 0) break;
            u8* const u = static_cast<u8*>(unit_of(at<i32>(R, 8 * k)));
            joan::orig::as_member(u)->release();
            joan::orig::as_unit(u)->link_to(static_cast<i32>(jo::as_unit(leader)->id));  // 0x00438f80
            if (jo::as_unit(u)->draw != nullptr) {
                detach(u);
                joan::orig::as_unit_draw(jo::as_unit(u)->draw)->create_label();  // 0x00520e80
            }
            jo::as_player(s)->cursor_mode = 7;
            at<u8>(R, 0x39c) = 1;
            jo::view<i32>(jo::as_player(s)->point_kind) = 9;
            std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_player(s)->point_name)), str(d::s_ASSIGN01), u32{8});  // "ASSIGN01"
        }
        if (at<u8>(R, 0x39c) != 0) speak_rts(s, static_cast<i32>(jo::as_unit(leader)->id), jo::as_unit(leader)->rank >= 4);
        c6c_io = at<i32>(R, 0x340);
        f68_io = at<i8>(R, 0x344);
        spoke_io = at<u8>(R, 0x39c);
    }
};

// 0x0044f750: emulated sort array frame; fills capped at 116 entries, assign loop stops at 0.
JOAN_REPLACE("fix/uninit", "Leader::assign_troops_44f750", 0x0044f750, &H::assign_troops);

}  // namespace game_b115_12_2
}  // namespace

// ---------------------------------------- from modules/game_b83_06.cpp (game/b83_06)
namespace {
namespace game_b83_06 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");
// Line buffers with a pad in front (buf[-1] write on an empty line) and behind (read_line
// 0x00589f10 stores the NUL at buf[n], and n == N for the 0x40 / 0x240 reads; in the original
// that byte lands in a dead / later-overwritten local, strip_last then terminates at N - 1).
template <std::uint32_t N>
struct alignas(4) Line {
    char pad[4];
    char b[N];
    char tail[4];
};

JOAN_COUNTER(c_trigger, "Scene::create_trigger");

std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
std::int32_t atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

char* token(void* r) { return joan::orig::as_file_txt(r)->get_token(1); }                     // 0x00589c70
void read_line(void* r, char* buf, std::int32_t n) { joan::orig::as_file_txt(r)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(n)); }  // 0x00589f10
void strip_last(char* buf) { buf[static_cast<std::int32_t>(strlen_(buf)) - 1] = 0; }

// TriggerList::add 0x004eef00 (16 dword args, ret 0x40).
std::int32_t add_trigger(std::int32_t type, std::int32_t a, std::int32_t w10, std::int32_t d14, std::int32_t w18,
                         std::int32_t b1c, V3 pos, float radius, std::int32_t b30, std::int32_t d34,
                         std::int32_t d38, std::int32_t b3c, void* d40, std::int32_t w44) {
    return joan::orig::arg::as<std::int32_t>(joan::orig::as_triggers(obj(d::g_a5ccd0))->add(joan::orig::arg::u32(type), joan::orig::arg::u32(a), joan::orig::arg::i16(w10), joan::orig::arg::u32(d14), joan::orig::arg::i16(w18), static_cast<std::int8_t>(joan::orig::arg::i32(b1c)), pos, joan::orig::arg::bits(radius), static_cast<std::int8_t>(joan::orig::arg::i32(b30)), joan::orig::arg::u32(d34), joan::orig::arg::u32(d38), static_cast<std::int8_t>(joan::orig::arg::i32(b3c)), d40, joan::orig::arg::i16(w44)));
}

struct H {
    // ------------------------------------------------------------------ 0x00450ea0
    std::int32_t create_trigger(void* reader, std::int32_t flag) {
        c_trigger.hit();
        Line<0x40> name;
        token(reader);
        token(reader);
        read_line(reader, name.b, 0x40);
        strip_last(name.b);
        const std::int32_t type = joan::orig::arg::as<std::int32_t>(joan::orig::trigger_type_from_name(joan::orig::arg::str(static_cast<char*>(name.b))));  // 0x00494dc0
        V3 pos;
        token(reader);
        pos.x = static_cast<float>(static_cast<std::int32_t>(static_cast<std::uint32_t>(atoi_(token(reader))) +
                                                             jo::view<std::uint32_t>(jo::self<jo::Cell>(this)->origin_x)));
        token(reader);
        pos.y = static_cast<float>(static_cast<std::int32_t>(static_cast<std::uint32_t>(atoi_(token(reader))) +
                                                             jo::view<std::uint32_t>(jo::self<jo::Cell>(this)->origin_y)));
        token(reader);
        pos.z = static_cast<float>(atoi_(token(reader)));
        token(reader);
        const std::int32_t radius = atoi_(token(reader));
        std::int16_t word = 0;  // only set and used for types 0x20 / 0x80
        if (type == 4 || type == 8) {
            token(reader);
            read_line(reader, name.b, 0x40);
            strip_last(name.b);
        }
        if (type == 0x20 || type == 0x80) {
            token(reader);
            word = static_cast<std::int16_t>(atoi_(token(reader)));
        }
        if (type == 0x100) {
            token(reader);
            read_line(reader, name.b, 0x40);
            strip_last(name.b);
        }
        if (type == 0x800 || type == 0x1000) {
            token(reader);
            read_line(reader, name.b, 0x40);
            strip_last(name.b);
        }
        if (flag == 0) return 1;
        const float r = static_cast<float>(radius);
        if (type == 4 || type == 8) {
            void* const node = joan::orig::arg::as<void*>(joan::orig::as_world(this)->find_sector(joan::orig::arg::str(static_cast<char*>(name.b))));  // 0x0054e6a0
            add_trigger(type, 1, 0, 1, 0, 1, pos, r, 0, 1, 0, 0, node, 0);
        } else if (type == 0x80) {
            void* const node = joan::orig::arg::as<void*>(joan::orig::as_world(this)->find_sector(joan::orig::arg::str(static_cast<char*>(name.b))));  // 0x0054e6a0
            const std::int32_t id = add_trigger(0x80, 1, 0, 1, 0, 1, pos, r, 0, 1, 0, 0, node, 0);
            std::uint8_t* const t = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(id)));  // 0x00485f20
            jo::as_trigger(t)->value24 = word;
        } else if (type == 0x20) {
            const std::int32_t id = add_trigger(0x20, 1, 0, 1, 0, 1, pos, r, 0, 1, 0, 0, 0, 0);
            std::uint8_t* const t = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(id)));  // 0x00485f20
            jo::as_trigger(t)->value24 = word;
        } else if (type == 0x100) {
            const std::int32_t item = joan::orig::arg::as<std::int32_t>(joan::orig::as_item_list(obj(d::g_items))->find_by_name(joan::orig::arg::str(static_cast<char*>(name.b))));  // 0x0049f420
            if (item != 0 &&
                jo::as_map_object(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(item))))->portrait_unit != 0) {  // 0x00425d20
                const std::int16_t kind = jo::as_map_object(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(item))))->portrait_unit;
                add_trigger(0x100, 2, kind, 1, 0, 1, pos, r, 0, 1, 0, 0, 0, 0);
            }
        } else if (type == 0x800 || type == 0x1000) {
            const std::int32_t snd = joan::orig::arg::as<std::int32_t>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(static_cast<char*>(name.b)));  // 0x004d12f0 (add_trigger reads 16 bits)
            add_trigger(type, 4, snd, 3, 0, 1, pos, r, 0, 0, 0, 0, 0, 0);
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_06", name, addr, fn)
B("Scene::create_trigger", 0x00450ea0, &H::create_trigger);
#undef B

}  // namespace game_b83_06
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

using joan::call::thiscall;
using joan::img::obj;

void* const kUnitList = obj(d::g_units);

JOAN_COUNTER(c_release_all, "Group::release_all");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    // ---- 0x00451430 Group::release_all() ---------------------------------------------------
    void release_all() {
        c_release_all.hit();
        for (std::int32_t i = 0; i < 10; ++i) {
            for (std::int32_t j = 0; j < static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->group_counts[i])); ++j) {
                std::uint32_t* const list = &jo::self<jo::Player>(this)->group_units[i * 100];
                if (joan::orig::as_dynarray(kUnitList)->empty_at(joan::orig::arg::i32(jo::view<std::int32_t>(list[j]))) != 0) continue;  // 0x00428b70
                void* const unit = joan::orig::as_dynarray(kUnitList)->ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(list[j])));  // 0x0041b3c0
                joan::orig::as_unit(unit)->attach_weapon_model(0);  // 0x00519250
            }
        }
        jo::self<jo::Player>(this)->selected_hero = 0;
        jo::self<jo::Player>(this)->selected_commander_id = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Group::release_all", 0x00451430, &H::release_all);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b109_07.cpp (game/b109_07)
namespace {
namespace game_b109_07 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

JOAN_COUNTER(c_select_hero, "Army::select_hero_group_451530");

void* const kUnits = obj(d::g_units);

struct alignas(4) Vec3 {
    float x, y, z;
};
struct alignas(4) CamBlock {
    u8 b[0x28];
};
static_assert(sizeof(Vec3) == 12, "12-byte by-value argument");
static_assert(sizeof(CamBlock) == 0x28, "0x28-byte by-value argument");

// 0x0051fd50(0x005270b0(name)): unit id of a named hero.
u32 hero_unit(img::Addr name) {
    const u32 cls = joan::orig::arg::as<u32>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(str(name))));  // 0x005270b0
    return joan::orig::arg::as<u32>(joan::orig::as_units(kUnits)->find_by_3b(joan::orig::arg::i32(cls)));                          // 0x0051fd50
}
constexpr img::Addr kHeroNames[8] = {d::s_JOAN_OF_ARC, d::s_JEAN_DE_METZ, d::s_LAHIRE_9cfb3c, d::s_ALENCON_9cfb30,
                                          d::s_ORLEAN, d::s_charlie7, d::s_arthur_de_richemont, d::s_FLAVY};

struct H {
    // ------------------------------------------------------------------ 0x00451530
    u32 list_base(i32 list) { return 0x105u + static_cast<u32>(list * 0x190); }
    u32& list_at(i32 list, i32 k) { return jo::self<jo::Player>(this)->group_units[list * 100 + k]; }
    i32 list_count(i32 list) { return static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[list])); }
    // 0x00428b70(id) != 0, else 0x0042d0e0(0x0041b3c0(id)) != 0; the id is re-read from the list.
    bool unit_gone(i32 i) {
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(list_at(static_cast<i32>(jo::self<jo::Player>(this)->group), i))) != 0) return true;  // 0x00428b70
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(list_at(static_cast<i32>(jo::self<jo::Player>(this)->group), i)));  // 0x0041b3c0
        return joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0;                                                              // 0x0042d0e0
    }
    void list_push(u32 v) {
        jo::self<jo::Player>(this)->group_units[static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0]))] = v;
        jo::self<jo::Player>(this)->group_counts[0] = static_cast<u8>(jo::self<jo::Player>(this)->group_counts[0] + 1);
    }

    u8 select_hero_group(i16 hero, u32 unit_id, i32 force, i32 speak) {
        c_select_hero.hit();
        if (unit_id != 0) {
            for (i32 k = 0; k < 8; ++k) {
                if (unit_id == hero_unit(kHeroNames[k])) {
                    hero = static_cast<i16>(k + 1);
                    break;
                }
            }
        } else {
            if (static_cast<i32>(hero) == 0) return 0;
            const u32 sel = static_cast<u32>(static_cast<i32>(hero) - 1);
            if (sel <= 7) unit_id = hero_unit(kHeroNames[sel]);
        }
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(unit_id)) != 0) return 0;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(unit_id)));  // 0x0041b3c0
        if (joan::orig::arg::as<i8>(joan::orig::as_unit(unit)->dead()) != 0) return 0;              // 0x0042d0e0
        if (jo::as_unit(unit)->speech_off != 0) return 0;
        Vec3 pos{0, 0, 0};
        joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(static_cast<void*>(&pos)));  // 0x004fedb0
        if (joan::orig::arg::as<i32>(joan::orig::unit::is_hero(joan::orig::arg::i32(unit_id))) == 0) {             // 0x0042ff90
            if (joan::orig::arg::as<i8>(joan::orig::as_mini_map(obj(d::g_mini_map))->can_drop_at(pos)) == 0) return 0;  // 0x00717910
        }
        alignas(8) u8 speech[0x194];                                      // [ebp-0x1a8]
        joan::orig::as_string(speech)->construct(str(d::s_RTS));                // 0x005a11a0 "RTS"
        void* const sfx = joan::orig::arg::as<void*>(joan::orig::as_player(this)->name_tag(joan::orig::arg::u32(unit_id)));    // 0x00453da0
        joan::orig::as_string(speech)->append(joan::orig::arg::str(sfx));                // 0x005a1550
        if (speak != 0 && (static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->selected_hero)) == static_cast<i32>(hero) || force != 0)) {
            if (joan::orig::arg::as<i32>(joan::orig::unit::is_hero(joan::orig::arg::i32(unit_id))) != 0) {  // 0x0042ff90
                joan::orig::as_string(speech)->append(str(d::s_001));                              // 0x005a1550 "001"
                joan::orig::as_player(this)->select_hero(static_cast<i32>(hero));             // 0x00432300
            } else {
                joan::orig::as_string(speech)->append(str(d::s_002));                // 0x005a1550 "002"
            }
            joan::orig::as_marker(obj(d::g_mini_map))->update();  // 0x00714040
            CamBlock cam;
            {
                void* const cams = obj(d::g_camera_man);
                const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(cams)->target());              // 0x00563dc0
                void* const c = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(cams)->motion(joan::orig::arg::i32(idx)));      // 0x00563c80
                joan::orig::as_camera_motion(c)->end(joan::orig::arg::mptr(static_cast<void*>(cam.b)));     // 0x00423fe0
            }
            jo::as_cam_params(cam.b)->position[0] = pos.x;
            jo::as_cam_params(cam.b)->position[1] = pos.y;
            jo::as_cam_params(cam.b)->position[2] = pos.z;
            jo::view<u32>(jo::as_cam_params(cam.b)->orbit[1]) = 0x3f490fdbu;  // pi / 4
            for (;;) {
                // fcomp K; test ah,1; jne -> leave when f1c < K or unordered
                if (!(jo::as_cam_params(cam.b)->orbit[3] >= global<float>(d::g_a0bbf0))) break;
                bool apply = joan::orig::arg::as<i8>(joan::orig::as_player(this)->corners_on_map(cam)) != 0;  // 0x00453930
                // test ah,0x40; je -> keep stepping only when f1c != K (ordered)
                if (!apply) {
                    const float v = jo::as_cam_params(cam.b)->orbit[3], k = global<float>(d::g_a0bbf0);
                    apply = !(v < k || v > k);
                }
                if (apply) {
                    void* const cams = obj(d::g_camera_man);
                    const u32 idx = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(cams)->target());          // 0x00563dc0
                    void* const c = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(cams)->motion(joan::orig::arg::i32(idx)));  // 0x00563c80
                    joan::orig::as_camera_motion(c)->start(joan::orig::arg::mptr(static_cast<void*>(cam.b)), joan::orig::arg::mptr(static_cast<void*>(cam.b)));  // 0x00423ec0
                    break;
                }
                jo::as_cam_params(cam.b)->orbit[3] = jo::as_cam_params(cam.b)->orbit[3] - global<float>(d::flt_200);  // 200.0f
                if (!(jo::as_cam_params(cam.b)->orbit[3] >= global<float>(d::g_a0bbf0)))
                    jo::view<u32>(jo::as_cam_params(cam.b)->orbit[3]) = global<u32>(d::g_a0bbf0);
            }
        } else {
            joan::orig::as_string(speech)->append(str(d::s_002));                // 0x005a1550 "002"
        }
        {
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_string(speech)->c_str());  // 0x00417d90
            joan::orig::as_sound(obj(d::g_sfx_player))->play_kind(name);        // 0x00492c40
        }
        if (speak != 0 || (jo::self<jo::Player>(this)->group == 0 && jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0]) == 0)) {
            joan::orig::as_group(this)->release_all();  // 0x00451430
            jo::self<jo::Player>(this)->selected_hero = static_cast<u8>(hero);
            jo::self<jo::Player>(this)->group_counts[0] = 0;
            jo::view<u8>(jo::self<jo::Player>(this)->group) = 0;
            jo::view<u8>(jo::self<jo::Player>(this)->group) = 0;
            jo::view<i32>(jo::self<jo::Player>(this)->selected_commander_id) = static_cast<i32>(jo::as_unit(unit)->id);
        } else if (jo::self<jo::Player>(this)->group != 0) {
            jo::self<jo::Player>(this)->group_counts[0] = 0;
            for (i32 i = 0; i < list_count(static_cast<i32>(jo::self<jo::Player>(this)->group)); ++i) {
                if (unit_gone(i)) continue;
                const u32 v = list_at(static_cast<i32>(jo::self<jo::Player>(this)->group), i);
                list_push(v);
            }
            jo::view<u8>(jo::self<jo::Player>(this)->group) = 0;
            jo::self<jo::Player>(this)->selected_commander_id = 0;
        } else {
            jo::self<jo::Player>(this)->selected_commander_id = 0;
        }
        list_push(static_cast<u32>(static_cast<i32>(jo::as_unit(unit)->id)));
        for (i32 j = 0; j < static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count); ++j) {
            if (!(static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0])) < 100)) break;
            const i32 member = jo::as_group(jo::as_unit(unit)->group)->members[j];
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(this)->can_target_453620(joan::orig::arg::i32(member))) == 0) continue;  // 0x00453620
            list_push(static_cast<u32>(static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->members[j])));
        }
        for (i32 j = 0; j < list_count(static_cast<i32>(jo::self<jo::Player>(this)->group)); ++j) {
            if (unit_gone(j)) continue;
            void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(list_at(static_cast<i32>(jo::self<jo::Player>(this)->group), j)));  // 0x0041b3c0
            joan::orig::as_unit(u)->attach_weapon_model(1);  // 0x00519250
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_07", name, addr, fn)
B("Army::select_hero_group", 0x00451530, &H::select_hero_group);
#undef B

}  // namespace game_b109_07
}  // namespace

// ---------------------------------------- from modules/game_b89_04.cpp (game/b89_04)
namespace {
namespace game_b89_04_2 {

using joan::call::thiscall;

constexpr Fpu kFpu{true};

JOAN_COUNTER(c_front, "Group::front_point");

// Shared body of 0x00451dc0 / 0x0044a950.
template <class H>  // the heading member is read as its raw dword
void front_point(void* self, float* out, H jo::Group::* heading, float jo::Group::* pos_x, float jo::Group::* pos_y) {
    std::uint32_t angle = 0;                                                // ebp-0x8 (1-byte object)
    joan::orig::as_angle(&angle)->construct();                                    // 0x00576120 (ctor, no-op)
    joan::orig::as_angle(&angle)->set(joan::orig::arg::f32(jo::view<std::uint32_t>(jo::as_group(self)->*heading)));     // 0x00576240 (float raw)
    if (jo::as_group(self)->formation != 1) return;
    const std::int32_t spacing = 100;   // ebp-0x10
    const std::int32_t front = -100;    // ebp-0x14
    std::int32_t width = 400;           // ebp-0x18 (uninitialised in the original when count > 25)
    const std::int32_t count = static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::as_group(self)->count));
    if (count <= 1)
        width = (1 - 1) * spacing;
    else if (count <= 4)
        width = (2 - 1) * spacing;
    else if (count <= 9)
        width = (3 - 1) * spacing;
    else if (count <= 0x10)
        width = (4 - 1) * spacing;
    else if (count <= 0x19)
        width = (5 - 1) * spacing;
    const std::int32_t gx = front - width / 2;  // ebp-0x1c
    const std::int32_t gy = 0;                  // ebp-0x20
    const float c1 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());  // 0x00576ac0
    const float t1 = static_cast<float>(kFpu.add(kFpu.mul(c1, static_cast<double>(gx)),
                                                  jo::as_group(self)->*pos_x));  // ebp-0x28
    const float s1 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());  // 0x005768d0
    out[0] = static_cast<float>(kFpu.sub(t1, kFpu.mul(s1, static_cast<double>(gy))));
    const float s2 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin());
    const float t2 = static_cast<float>(kFpu.add(kFpu.mul(s2, static_cast<double>(gx)),
                                                  jo::as_group(self)->*pos_y));  // ebp-0x2c
    const float c2 = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos());
    out[1] = static_cast<float>(kFpu.add(kFpu.mul(c2, static_cast<double>(gy)), t2));
}

struct H {
    // ------------------------------------------------------------------------ 0x00451dc0
    void group_front(float* out) {
        c_front.hit();
        front_point(this, out, &jo::Group::u32_54, &jo::Group::front_x, &jo::Group::front_y);
    }
};

#define B89(name, addr, fn) JOAN_REPLACE("game/b89_04", name, addr, fn)
B89("Group::front_point", 0x00451dc0, &H::group_front);
#undef B89

}  // namespace game_b89_04_2
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_drain_unit, "Obj::drain_unit");
JOAN_COUNTER(c_drain_item, "Obj::drain_item");

u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x00451f20
    float drain_unit(i32 id, i32) {
        c_drain_unit.hit();
        u8* const unit = unit_get(id);
        const float v = joan::orig::arg::as<float>(joan::orig::as_unit(unit)->attack_value_5104f0()) * global<float>(d::flt_0_5);  // 0x005104f0
        const Fpu fpu{true};
        const double d = fpu.sub(static_cast<double>(jo::view<i32>(jo::self<jo::Wall>(this)->u32_c6)), static_cast<double>(v));
        const float r = !(d >= 0.0) ? 0.0f : static_cast<float>(d);  // < 0 or NaN -> 0
        jo::view<i32>(jo::self<jo::Wall>(this)->u32_c6) = joan::hook::x87::ftol(static_cast<double>(r));
        return v;
    }

    // ------------------------------------------------------------------ 0x00452b20
    float drain_item(i32 id, i32) {
        c_drain_item.hit();
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)) != 0) return global<float>(d::k_9cf5c4);  // 0x00425d50
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
        const float v = static_cast<float>(jo::view<i32>(jo::as_map_object(e)->damage));
        const Fpu fpu{true};
        const double d = fpu.sub(static_cast<double>(jo::self<jo::Order0c_a>(this)->amount), static_cast<double>(v));
        const float r = !(d >= 0.0) ? 0.0f : static_cast<float>(d);
        jo::self<jo::Order0c_a>(this)->amount = joan::hook::x87::ftol(static_cast<double>(r));
        return v;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::drain_unit", 0x00451f20, &H::drain_unit);
B96("Obj::drain_item", 0x00452b20, &H::drain_item);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b101_01.cpp (game/b101_01)
namespace {
namespace game_b101_01 {

using joan::hook::global;

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::orig::helpers::as_float;

JOAN_COUNTER(c_pick_corners, "Camera::pick_corners_451fe0");

struct H {
    // ------------------------------------------------------------------ 0x00451fe0
    u8 pick_corners_451fe0(const u8* in, u8* out) {
        c_pick_corners.hit();
        u8 rays[4][0x18];  // [ebp-0x7c + i * 0x18]: two points each
        u32 box[6];        // [ebp-0x1c]
        i32 sx = 0, sy = 0;
        for (i32 i = 0; i < 4; ++i) {
            const i32 y = ftol(static_cast<double>(reinterpret_cast<const float*>(in)[i * 3 + 1]));  // in: 4 points of 3 floats
            const i32 x = ftol(static_cast<double>(reinterpret_cast<const float*>(in)[i * 3]));
            if (joan::orig::arg::as<i32>(joan::orig::as_grid(global<void*>(d::g_camera))->to_cell(joan::orig::arg::i32(x), joan::orig::arg::i32(y), &sx, &sy)) == 0) return 0;  // 0x006152b0
            joan::orig::as_camera(global<void*>(d::g_camera))->screen_ray(joan::orig::arg::i32(sx), joan::orig::arg::i32(sy), joan::orig::arg::mptr(static_cast<void*>(rays[i])));  // 0x006154d0
        }
        joan::orig::as_aabb(static_cast<void*>(box))->set_point(static_cast<void*>(rays[0]));         // 0x0060d430
        joan::orig::as_aabb(static_cast<void*>(box))->extend(static_cast<void*>(rays[0] + 0xc));   // 0x0060d490
        for (i32 i = 1; i < 4; ++i) {
            joan::orig::as_aabb(static_cast<void*>(box))->extend(static_cast<void*>(rays[i]));
            joan::orig::as_aabb(static_cast<void*>(box))->extend(static_cast<void*>(rays[i] + 0xc));
        }
        i32 count = 0;
        void* const* const nodes =
            joan::orig::arg::as<void**>(joan::orig::as_scene(jo::view<void*>(jo::as_world(global<u8*>(d::g_world))->scene))->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x005ef610
        u32 t = 0;  // [ebp-0xa8]
        for (i32 i = 0; i < 4; ++i) {
            u8* best = nullptr;
            u32 best_t = 0;
            for (i32 j = count - 1; j >= 0; --j) {
                void* const o = nodes[j];  // a SceneS
                if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(static_cast<void*>(rays[i]), static_cast<void*>(jo::as_scene_s(o)->desc))) == 0) continue;  // 0x0060dfd0
                u8* const m = jo::view<u8*>(jo::as_scene_s(o)->plane);
                if (m == nullptr) continue;
                if (joan::orig::arg::as<i32>(joan::orig::as_plane(m)->visible()) == 0) continue;  // 0x005d0480
                if (joan::orig::arg::as<i32>(joan::orig::as_plane(m)->segment_hit(&t, joan::orig::arg::mptr(static_cast<void*>(rays[i])))) == 0) continue;  // 0x005d4a80
                if (best != nullptr) {
                    // fld best; fcomp t; test ah,0x41; jne skip: replace only when best > t
                    if (!(as_float(best_t) > as_float(t))) continue;
                }
                best_t = t;
                best = m;
            }
            if (best == nullptr) return 0;
            joan::orig::segment_point_at(static_cast<void*>(out + i * 0xc), static_cast<void*>(rays[i]), joan::orig::arg::f32(best_t));  // 0x0060de40
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_01", name, addr, fn)
B("Camera::pick_corners_451fe0", 0x00451fe0, &H::pick_corners_451fe0);
#undef B

}  // namespace game_b101_01
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;


struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

void* const kUnits = obj(d::g_units);

std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
std::uint8_t box_contains(const void* box, const Vec3* p) { return joan::orig::arg::as<std::uint8_t>(joan::orig::as_aabb(box)->contains(joan::orig::arg::ptr(p))); }  // 0x0060d5f0

JOAN_COUNTER(c_find_zone, "Zones::find_containing");

struct H {
    // 0x00452280
    float drain_452280(std::int32_t id, std::int32_t) {
        std::uint8_t* const u = unit_get(id);
        const float t = joan::orig::arg::as<float>(joan::orig::as_unit(u)->defence_value()) * global<float>(d::flt_0_5);  // 0x00510bf0 (fld dword)
        const Fpu fpu{true};
        const double diff = fpu.sub(static_cast<double>(jo::self<jo::Order0c_a>(this)->amount), t);
        float r;  // ebp-0x10
        if (!(diff >= static_cast<double>(global<float>(d::k_9cf5c4))))  // test ah,1: < or NaN
            r = 0.0f;
        else
            r = static_cast<float>(diff);
        jo::self<jo::Order0c_a>(this)->amount = ftol(r);
        return t;
    }

    // 0x00452610 (this unused)
    std::int32_t find_containing(Vec3 p) {
        c_find_zone.hit();
        void* const zones = obj(d::g_places);
        for (std::int32_t n = joan::orig::as_dynarray(zones)->size(); n >= 1; --n) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(zones)->empty_at_e(joan::orig::arg::i32(n))) != 0) continue;              // 0x0044cf80
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(zones)->get_e(joan::orig::arg::i32(n)));        // 0x0043c450
            if (jo::as_facility(e)->kind == 3) {
                std::uint8_t* const o = virtual_call<std::uint8_t*>(e, 0x24);
                if (box_contains(jo::as_facility(o)->owner->box, &p) != 0) return n;
            } else if (jo::as_facility(e)->kind == 0x10) {
                std::uint8_t* const o = virtual_call<std::uint8_t*>(e, 0x1c);
                if (box_contains(jo::as_facility(o)->owner->box, &p) != 0) return n;
            }
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("Unit::drain_452280", 0x00452280, &H::drain_452280);
B("Zones::find_containing", 0x00452610, &H::find_containing);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;


using u32 = std::uint32_t;

JOAN_COUNTER(c_map_quad, "map_quad");

struct H {
    // ------------------------------------------------------------------ 0x00452320
    u8 map_quad(const float* pts, u8* out) {
        c_map_quad.hit();
        u32 l68 = 0;     // ebp-0x68
        u32 info[10];    // ebp-0x98 (0x006140a0 copies 0x28 bytes)
        u32 quad[4][6];  // ebp-0x64
        i32 l6c = 0, l70 = 0;
        void* const g = global<void*>(d::g_camera);
        joan::orig::as_camera(g)->frame(joan::orig::arg::mptr(static_cast<void*>(info)));  // 0x006140a0
        {  // dead loads (ebp-0xa4 / -0xa0 / -0x9c)
            volatile u32 d0 = jo::view<u32[16]>(jo::as_camera(global<void*>(d::g_camera))->bone_->matrix_)[8];  // bone matrix +0x20
            volatile u32 d1 = jo::view<u32[16]>(jo::as_camera(global<void*>(d::g_camera))->bone_->matrix_)[9];
            volatile u32 d2 = jo::view<u32[16]>(jo::as_camera(global<void*>(d::g_camera))->bone_->matrix_)[10];
            (void)d0;
            (void)d1;
            (void)d2;
        }
        for (i32 i = 0; i < 4; ++i) {
            const i32 y = joan::hook::x87::ftol(static_cast<double>(pts[i * 3 + 1]));
            const i32 x = joan::hook::x87::ftol(static_cast<double>(pts[i * 3]));
            if (joan::orig::arg::as<i32>(joan::orig::as_grid(global<void*>(d::g_camera))->to_cell(joan::orig::arg::i32(x), joan::orig::arg::i32(y), &l6c, &l70)) == 0) return 0;  // 0x006152b0
            joan::orig::as_camera(global<void*>(d::g_camera))->screen_ray(joan::orig::arg::i32(l6c), joan::orig::arg::i32(l70), joan::orig::arg::mptr(static_cast<void*>(quad[i])));  // 0x006154d0
            if (joan::orig::arg::as<i32>(joan::orig::z_plane_hit(&l68, joan::orig::arg::mptr(static_cast<void*>(quad[i])), joan::orig::arg::f32(info[3]))) != 0) {     // 0x0060e6a0
                joan::orig::segment_point_at(static_cast<void*>(out + i * 0xc), static_cast<void*>(quad[i]), joan::orig::arg::f32(l68));  // 0x0060de40
            }
        }
        return 1;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("map_quad", 0x00452320, &H::map_quad);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::call::thiscall;
using joan::img::obj;

void* const kUnitTable = obj(d::g_units);

JOAN_COUNTER(c_count_active, "Group::count_active");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x004524c0
    i32 count_active() {
        c_count_active.hit();
        i32 n = 0;
        for (i32 i = 0; i < static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0])); ++i) {
            const i32 id = jo::view<i32>(jo::self<jo::Player>(this)->group_units[i]);
            if (unit_missing(id) != 0) continue;
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(unit_get(id))->leads()) != 0) ++n;  // 0x0044a660
        }
        return n;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Group::count_active", 0x004524c0, &H::count_active);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

void* const kUnits = obj(d::g_units);
void* const kTypes = obj(d::g_types);

// Unit table helpers (0x00428b70 = "id invalid", 0x0041b3c0 = unit by id).
std::int32_t unit_invalid(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }   // 0x0041b3c0

JOAN_COUNTER(c_paired_anim, "Unit::start_paired_anim");
JOAN_COUNTER(c_can_target, "Unit::can_target");
JOAN_COUNTER(c_first_ready, "Group::first_ready_member");

struct H {
    // 0x00452570
    std::int32_t first_ready_member() {
        c_first_ready.hit();
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->group_counts[0])); ++i) {
            const std::int32_t id = jo::view<std::int32_t>(jo::self<jo::Player>(this)->group_units[i]);
            if (unit_invalid(id) != 0) continue;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit_get(id))->leads()) != 0) return id;  // 0x0044a660
        }
        return 0;
    }

    // 0x004533e0
    void start_paired_anim() {
        c_paired_anim.hit();
        if (unit_invalid(jo::self<jo::Unit>(this)->mount_id) != 0) return;
        std::uint8_t* const t = unit_get(jo::self<jo::Unit>(this)->mount_id);
        const float r = joan::orig::arg::as<float>(joan::orig::as_misc(obj(d::g_misc))->rand_unit_inclusive());  // 0x00590660
        jo::self<jo::Unit>(this)->paired_anim_start_ms = 0;
        const std::int32_t k = jo::as_unit(t)->anim_kind;
        if (k != 0xb && k != 9 && k != 0xa && k != 0x10) return;
        if (k == 0xa || k == 0x10) {
            joan::orig::as_unit(this)->set_action(0x43, 0, 1);  // 0x00501190
            joan::orig::as_unit(t)->set_action(1, 0, 1);
            if (jo::self<jo::Unit>(this)->actor != nullptr) {
                std::uint8_t* const q = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_unit(t)->actor->record))));  // 0x006af310
                jo::as_shot(q)->state.vec0_x = global<std::uint32_t>(d::g_pos);
                jo::view<std::uint32_t>(jo::as_shot(q)->state.vec0_y) = global<std::uint32_t>(d::g_a69cdc);
                jo::view<std::uint32_t>(jo::as_shot(q)->state.vec0_z) = global<std::uint32_t>(d::g_a69ce0);
                jo::as_shot(q)->state.vel_len = 0xbf800000u;  // -1.0f
            }
        } else if (r > global<float>(d::g_a3cce4)) {  // test ah,0x41 / je
            joan::orig::as_unit(this)->set_action(0x47, 0, 1);
            joan::orig::as_unit(t)->set_action(0x3f, 0, 1);
        } else {
            joan::orig::as_unit(this)->set_action(0x48, 0, 1);
            joan::orig::as_unit(t)->set_action(0x3e, 0, 1);
        }
        jo::self<jo::Unit>(this)->mount_busy = 1;
        jo::as_unit(t)->mount_busy = 1;
    }

    // 0x00453620
    std::uint8_t can_target(std::int32_t id) {
        c_can_target.hit();
        if (unit_invalid(id) != 0) return 0;
        std::uint8_t* const u = unit_get(id);
        if (jo::as_unit_type(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id))))->unit_class == 0 ||  // 0x0042d0a0
            jo::as_unit_type(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id))))->kind == 2 ||
            jo::as_unit_type(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id))))->kind == 7)
            return 0;
        if (jo::as_unit(u)->speech_off != 0 || jo::as_unit(u)->task.kind == 0xf || jo::as_unit(u)->weapon != 0 ||
            joan::orig::arg::as<std::int8_t>(joan::orig::as_unit(u)->dead()) != 0)  // 0x0042d0e0
            return 0;
        if (jo::as_unit_type(joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(jo::as_unit(u)->type_id))))->slot_count != 0 &&
            joan::orig::arg::as<std::int32_t>(joan::orig::unit::guard_check(joan::orig::arg::i16(jo::as_unit(u)->id), str(d::s_M_9d006c))) == 0 &&  // 0x00656bb0
            joan::orig::arg::as<std::int32_t>(joan::orig::unit::guard_check(joan::orig::arg::i16(jo::as_unit(u)->id), str(d::s_F_9d0068))) == 0)
            return 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Group::first_ready_member", 0x00452570, &H::first_ready_member);
B("Unit::start_paired_anim", 0x004533e0, &H::start_paired_anim);
B("Unit::can_target", 0x00453620, &H::can_target);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using u32 = std::uint32_t;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_spawn_200, "Obj::spawn_effect_200");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x00452720
    bool spawn_effect_200(i32 id) {
        c_spawn_200.hit();
        if (unit_missing(id) != 0) return false;
        if (unit_missing(joan::orig::arg::as<i32>(joan::orig::as_group(this)->first_ready_member())) != 0) return false;  // 0x00452570
        u8* const unit = unit_get(joan::orig::arg::as<i32>(joan::orig::as_group(this)->first_ready_member()));
        const u32 t1 = global<u32>(d::g_now);
        const u32 t2 = global<u32>(d::g_now);
        V3 pos;
#if defined(__GNUC__) && !defined(__clang__)
        // GCC sees DynArray::ptr_at return null on one path (first_ready_member is called twice, the emptiness check
        // above used the first result); the original has no null check either (0x00452720), so it is kept.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overread"
#endif
        std::memcpy(&pos, jo::as_unit(unit)->position_, sizeof(V3));  // +0xe
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        joan::orig::as_triggers(obj(d::g_a5ccd0))->add(joan::orig::arg::u32(// 0x004eef00 (ret 0x40)
                       static_cast<i32>(0x200)), joan::orig::arg::u32(static_cast<i32>(1)), joan::orig::arg::i16(static_cast<i16>(id)), joan::orig::arg::u32(static_cast<i32>(1)), joan::orig::arg::i16(jo::as_unit(unit)->id), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(1))), pos, joan::orig::arg::bits(100.0f), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(0))), joan::orig::arg::u32(t2), joan::orig::arg::u32(t1), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u8>(1))), nullptr, joan::orig::arg::i16(static_cast<i16>(0)));
        jo::view<u32>(jo::self<jo::Player>(this)->jump_hold_ms) = 0;
        jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 0xb;
        return true;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::spawn_effect_200", 0x00452720, &H::spawn_effect_200);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using u32 = std::uint32_t;

float fconst(img::Addr a) { return img::global<const float>(a); }

// 12-byte vector passed by value (copied as raw dwords like the original's mov copies).
struct V3 {
    u32 x, y, z;
};
static_assert(sizeof(V3) == 12, "V3");

JOAN_COUNTER(c_ray_query, "Pick::ray_query_452830");

struct H {
    // ------------------------------------------------------------------ 0x00452830
    u8 ray_query(u32 sx, u32 sy, float x, float y, float z, void* out) {
        c_ray_query.hit();
        float box[6];
        box[0] = x - fconst(d::flt_5000);
        box[1] = y - fconst(d::flt_5000);
        box[2] = z - fconst(d::flt_1000);
        box[3] = x + fconst(d::flt_5000);
        box[4] = y + fconst(d::flt_5000);
        box[5] = z + fconst(d::flt_1000);
        i32 count = 0;
        void* const grid = jo::view<void*>(jo::as_world(global<void*>(d::g_world))->scene);
        void** const list = joan::orig::arg::as<void**>(joan::orig::as_scene(grid)->nodes_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x005ef610
        i32 rx = 0, ry = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_grid(global<void*>(d::g_camera))->to_cell(joan::orig::arg::i32(sx), joan::orig::arg::i32(sy), &rx, &ry)) == 0) return 0;  // 0x006152b0
        alignas(4) u8 ray[0x18];
        joan::orig::as_camera(global<void*>(d::g_camera))->screen_ray(joan::orig::arg::i32(rx), joan::orig::arg::i32(ry), joan::orig::arg::mptr(static_cast<void*>(ray)));  // 0x006154d0
        void* best_obj = nullptr;  // [ebp-0x50]
        void* best_ent = nullptr;  // [ebp-0x54]
        float t_obj = 0.0f;        // [ebp-0x48], read only when best_obj != 0
        float t_ent = 0.0f;        // [ebp-0x4c], read only when best_ent != 0
        for (i32 i = count - 1; i >= 0; --i) {
            void* const node = list[i];  // a SceneS
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(static_cast<void*>(ray), joan::orig::arg::ptr(jo::as_scene_s(node)->desc))) == 0) continue;  // 0x0060dfd0
            for (void* e = joan::orig::arg::as<void*>(joan::orig::as_scene_node(node)->attachments()); e != nullptr;                  // 0x005efff0
                 e = joan::orig::arg::as<void*>(joan::orig::get24(joan::orig::arg::mptr(e)))) {                                      // 0x005f0020
                void* const tgt = jo::as_actor(e)->plane;
                if (jo::as_actor(e)->kind != 1) continue;
                void* const u = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_map_object(jo::as_actor(e)->owner)->type_id)))));  // 0x00425ce0
                if (jo::view<i8>(jo::as_item_def(u)->has_path) == 0) continue;
                if (tgt != nullptr && jo::as_plane(tgt)->vertex_format == 7) continue;
                if (tgt == nullptr) continue;
                alignas(4) u8 sph[0x10];
                joan::orig::as_actor(e)->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sph)), 1u);  // 0x005acc80
                if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(joan::orig::arg::mptr(static_cast<void*>(ray)), joan::orig::arg::mptr(static_cast<void*>(sph)))) == 0) continue;  // 0x0060e3f0
                float t = 0.0f;
                if (joan::orig::arg::as<i32>(joan::orig::as_plane(jo::as_actor(e)->plane)->segment_hit(&t, joan::orig::arg::mptr(static_cast<void*>(ray)))) == 0) continue;  // 0x005d4a80
                if (best_ent != nullptr && !(t_ent > t)) continue;
                t_ent = t;
                best_ent = e;
            }
            void* const p = jo::as_scene_s(node)->plane;
            if (p == nullptr) continue;
            if (joan::orig::arg::as<i32>(joan::orig::as_plane(p)->visible()) == 0) continue;  // 0x005d0480
            float t2 = 0.0f;
            if (joan::orig::arg::as<i32>(joan::orig::as_plane(p)->segment_hit(&t2, joan::orig::arg::mptr(static_cast<void*>(ray)))) == 0) continue;
            if (best_obj != nullptr && !(t_obj > t2)) continue;
            t_obj = t2;
            best_obj = p;
        }
        if (best_ent != nullptr && best_obj != nullptr) {
            if (t_obj > t_ent) {
                joan::orig::segment_point_at(joan::orig::arg::mptr(out), static_cast<void*>(ray), joan::orig::arg::f32(t_ent));  // 0x0060de40
                V3 v;
                std::memcpy(&v, out, sizeof(v));
                if ((joan::orig::arg::as<u32>(joan::orig::as_collision_grid(obj(d::g_collision_grid))->region_hit(joan::orig::arg::f32(v.x), joan::orig::arg::f32(v.y), joan::orig::arg::f32(v.z), 1u)) & 0xffu) != 0) return 1;  // 0x00693580
            }
            joan::orig::segment_point_at(joan::orig::arg::mptr(out), static_cast<void*>(ray), joan::orig::arg::f32(t_obj));
            return 1;
        }
        if (best_ent != nullptr) {
            joan::orig::segment_point_at(joan::orig::arg::mptr(out), static_cast<void*>(ray), joan::orig::arg::f32(t_ent));
            return 1;
        }
        if (best_obj != nullptr) {
            joan::orig::segment_point_at(joan::orig::arg::mptr(out), static_cast<void*>(ray), joan::orig::arg::f32(t_obj));
            return 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Pick::ray_query_452830", 0x00452830, &H::ray_query);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units_2 {

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }
    std::uint8_t in_group(std::int32_t id) {  // 0x00453590: current group (+0x1236) of the 0x190-byte group table
        const std::int32_t g = static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Player>(this)->group));
        for (std::int32_t i = 0; i < static_cast<std::int8_t>(jo::self<jo::Player>(this)->group_counts[g]); ++i)
            if (jo::view<std::int32_t>(jo::self<jo::Player>(this)->group_units[g * 100 + i]) == id) return 1;
        return 0;
    }
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("Groups::contains", 0x00453590, &H::in_group);
#undef U1

}  // namespace game_units_2
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

using joan::call::thiscall;
using joan::img::obj;

using u32 = std::uint32_t;

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_apply_units, "Group::apply_units_453790");

struct H {
    // ------------------------------------------------------------------ 0x00453790
    u32 id_at(i32 i) {
        const i32 g = static_cast<i8>(jo::view<u8>(jo::self<jo::Player>(this)->group));
        return jo::self<jo::Player>(this)->group_units[g * 100 + i];
    }
    std::uintptr_t apply_units_453790(u32 flag) {
        c_apply_units.hit();
        for (i32 i = 0;; ++i) {
            const i32 g = static_cast<i8>(jo::view<u8>(jo::self<jo::Player>(this)->group));
            if (!(i < static_cast<i32>(static_cast<i8>(jo::self<jo::Player>(this)->group_counts[g])))) break;
            if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id_at(i))) != 0) continue;  // 0x00428b70
            void* unit = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id_at(i)));  // 0x0041b3c0
            if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_unit(unit)->dead())) != 0) continue;  // 0x0042d0e0
            unit = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id_at(i)));  // 0x0041b3c0
            if (jo::as_unit(unit)->rank < 4) continue;
            if (static_cast<i8>(static_cast<u8>(flag)) != 0)
                joan::orig::as_order(this)->consume_43b490(joan::orig::arg::u32(id_at(i)));  // 0x0043b490
            else
                joan::orig::as_unit(this)->use_spiritual_item_43b990(joan::orig::arg::i32(id_at(i)));  // 0x0043b990
        }
        // eax = this (loaded for the loop bound) with al = 1
        return (reinterpret_cast<std::uintptr_t>(this) & 0xffffff00u) | 1u;

    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Group::apply_units_453790", 0x00453790, &H::apply_units_453790);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b115_10.cpp (fix/uninit)
namespace {
namespace game_b115_10 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using u32 = std::uint32_t;


struct V3 {
    float d[3];
};
static_assert(sizeof(V3) == 12, "V3");
struct Cam28 {
    u32 d[10];
};
static_assert(sizeof(Cam28) == 0x28, "Cam28");

JOAN_COUNTER(c_corners, "Cam::corners_on_map_453930");

struct H {
    // ===================================================================== 0x00453930
    u8 corners_on_map(Cam28 cam) {
        c_corners.hit();
        float pt[4][2];
        pt[0][0] = 0.0f;
        pt[0][1] = 0.0f;
        pt[1][0] = 0.0f;
        pt[1][1] = static_cast<float>(global<i32>(d::g_screen_h) - 1);
        pt[2][0] = static_cast<float>(global<i32>(d::g_screen_w) - 1);
        pt[2][1] = 0.0f;
        pt[3][0] = static_cast<float>(global<i32>(d::g_screen_w) - 1);
        pt[3][1] = static_cast<float>(global<i32>(d::g_screen_h) - 1);
        u32 t = 0;  // [ebp-0xc8] (0.0f)
        Cam28 saved{};
        auto cams = []() { return global<void*>(d::g_camera); };
        joan::orig::as_camera(cams())->frame(&saved);  // 0x006140a0
        joan::orig::as_camera(cams())->set_frame(&cam);    // 0x00614040
        joan::orig::as_camera(cams())->update_matrices();          // 0x006141a0
        u32 rays[4][6];
        V3 hits[4] = {};  // fix: the zero vector when 0x0060e6a0 returns 0
        std::memset(rays, 0, sizeof rays);
        for (i32 i = 0; i < 4; ++i) {
            u32 rx = 0;
            u32 ry = 0;
            const i32 y = ftol(pt[i][1]);
            const i32 x = ftol(pt[i][0]);
            if (joan::orig::arg::as<i32>(joan::orig::as_grid(cams())->to_cell(joan::orig::arg::i32(x), joan::orig::arg::i32(y), &rx, &ry)) == 0) {  // 0x006152b0
                joan::orig::as_camera(cams())->set_frame(&saved);
                return 0;
            }
            joan::orig::as_camera(cams())->screen_ray(joan::orig::arg::i32(rx), joan::orig::arg::i32(ry), joan::orig::arg::mptr(rays[i]));  // 0x006154d0
            u32 box[6] = {};
            joan::orig::as_aabb(box)->set_point(&rays[i][3]);  // 0x0060d430
            joan::orig::as_aabb(box)->extend(joan::orig::arg::ptr(rays[i]));      // 0x0060d490
            i32 count = 0;
            void* const scene = jo::view<void*>(jo::as_world(global<void*>(d::g_world))->scene);
            void* const* const list = joan::orig::arg::as<void**>(joan::orig::as_scene(scene)->nodes_in_box(joan::orig::arg::mptr(box), &count));  // 0x005ef610
            bool found = false;
            for (i32 j = count - 1; j >= 0; --j) {
                void* const e = list[j];  // a SceneS
                if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(joan::orig::arg::ptr(rays[i]), joan::orig::arg::ptr(jo::as_scene_s(e)->desc))) == 0) continue;  // 0x0060dfd0
                void* const o = jo::as_scene_s(e)->plane;
                if (o == nullptr) continue;
                if (joan::orig::arg::as<i32>(joan::orig::as_plane(o)->visible()) == 0) continue;  // 0x005d0480
                u32 t140 = 0;
                if (joan::orig::arg::as<i32>(joan::orig::as_plane(o)->segment_hit(&t140, joan::orig::arg::mptr(rays[i]))) != 0) {  // 0x005d4a80
                    found = true;
                    break;
                }
            }
            if (!found) {
                joan::orig::as_camera(cams())->set_frame(&saved);
                return 0;
            }
            if (joan::orig::arg::as<i32>(joan::orig::z_plane_hit(&t, joan::orig::arg::mptr(rays[i]), joan::orig::arg::f32(cam.d[3]))) != 0)  // 0x0060e6a0
                joan::orig::segment_point_at(&hits[i], joan::orig::arg::ptr(rays[i]), joan::orig::arg::f32(t));           // 0x0060de40
            if (joan::orig::arg::as<i8>(joan::orig::as_mini_map(obj(d::g_mini_map))->can_drop_at(hits[i])) == 0) {  // 0x00717910
                joan::orig::as_camera(cams())->set_frame(&saved);
                return 0;
            }
        }
        joan::orig::as_camera(cams())->set_frame(&saved);
        return 1;
    }
};

// fix/uninit registrations (each: the garbage read and the value used; see the header)
// 0x00453930: unwritten hit point (ray parallel to the plane) -> zero vector.
JOAN_REPLACE("fix/uninit", "Cam::corners_on_map_453930", 0x00453930, &H::corners_on_map);

}  // namespace game_b115_10
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

std::int32_t stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_name_tag, "Unit::name_tag");

// 0x0042d0a0 through its thunk: record of 0xa48d88 by word index.
void* unit_record(std::int16_t index) {
    return joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<std::int32_t>(index))));  // 0x0042d0a0
}

struct H {
    // 0x00453cc0 (this = Cell::tracks)
    void* get_c() { return jo::self<jo::Tracks>(this)->res; }

    // 0x00453da0
    const char* name_tag(std::uint32_t id) {
        c_name_tag.hit();
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(id)) != 0) return nullptr;  // 0x00428b70
        void* const u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id));  // 0x0041b3c0
        switch (jo::view<std::uint32_t>(jo::as_unit(u)->rank)) {
            case 4:
            case 5: {
                struct Pair {
                    img::Addr name, tag;
                };
                static const Pair kNames[] = {
                    {d::s_JOAN_OF_ARC, d::s_JOAN_9cf9f8},  // JOAN_OF_ARC -> _JOAN_
                    {d::s_JEAN_DE_METZ, d::s_METZ},  // JEAN_DE_METZ -> _METZ_
                    {d::s_LAHIRE_9cfb3c, d::s_LAHIRE_9d0038},  // LAHIRE -> _LAHIRE_
                    {d::s_CHARLIE7, d::s_CHARLES_9d00e0},  // CHARLIE7 -> _CHARLES_
                    {d::s_ALENCON_9cfb30, d::s_ALENCON_9d00d4},  // ALENCON -> _ALENCON_
                    {d::s_ORLEAN, d::s_ORLEANS},  // ORLEAN -> _ORLEANS_
                    {d::s_arthur_de_richemont, d::s_RICHEMONT},  // arthur_de_richemont -> _RICHEMONT_
                };
                for (std::uint32_t k = 0; k < 7; ++k) {
                    const std::uint8_t* const rec = static_cast<std::uint8_t*>(unit_record(jo::as_unit(u)->type_id));
                    if (stricmp_(jo::as_unit_type(rec)->name, str(kNames[k].name)) == 0) return str(kNames[k].tag);
                }
                return nullptr;
            }
            case 0:
            case 1:
            case 2:
            case 3: {
                void* const rec = unit_record(jo::as_unit(u)->type_id);
                switch (static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::as_unit_type(rec)->unit_class) - 1)) {
                    case 0: return str(d::s_LTINF);  // _LTINF_
                    case 1: return str(d::s_HVINF);  // _HVINF_
                    case 2: return str(d::s_POLE);  // _POLE_
                    case 3: return str(d::s_MACE);  // _MACE_
                    case 4: return str(d::s_ARCHER_9d0020);  // _ARCHER_
                    case 5: return str(d::s_CROSS_9d0084);  // _CROSS_
                    case 7: return str(d::s_SIEGE_9d0078);  // _SIEGE_
                    default: return nullptr;
                }
            }
            default: return nullptr;
        }
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::get_c", 0x00453cc0, &H::get_c);
M("Unit::name_tag", 0x00453da0, &H::name_tag);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01_2 {

using joan::img::obj;

void* const kUnitTable = obj(d::g_units);

JOAN_COUNTER(c_check_state, "Obj::check_state_41");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------------ 0x00453ce0
    i32 check_state_41() {  // this = NewsItem (news.cpp)
        c_check_state.hit();
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::NewsItem>(this)->arg_b)) == 0) return 0;
        i32 ok = 0;  // ebp-0x8
        const i32 st = static_cast<i32>(jo::view<i8>(jo::self<jo::NewsItem>(this)->arg_b));
        if (st == 1 || st == 2) {
            // ok stays 0
        } else if (st == 3) {
            if (unit_missing(static_cast<i32>(jo::view<i16>(jo::self<jo::NewsItem>(this)->arg_w1))) == 0) {
                unit_get(static_cast<i32>(jo::view<i16>(jo::self<jo::NewsItem>(this)->arg_w1)));  // ebp-0xc (dead)
                ok = 1;
            }
        } else if (static_cast<i32>(jo::view<i8>(jo::self<jo::NewsItem>(this)->arg_b)) == 0) {
            ok = 1;
        }
        if (ok == 0) jo::self<jo::NewsItem>(this)->arg_b = 0;
        return ok;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::check_state_41", 0x00453ce0, &H::check_state_41);
#undef B99

}  // namespace game_b99_01_2
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x00454000
    void set_distance_model(char mode) {
        const u32 ok = static_cast<u32>(joan::orig::arg::as<u8>(joan::orig::as_sound3dsystem(this)->is_initialized()));  // 0x005a1110
        if (ok == 0) return;
        jo::view<char>(jo::self<jo::Sound3DSystem>(this)->distance_model) = mode;
        switch (mode) {
            case 0: import_call<void>(d::iat_OpenAL32_alDistanceModel, static_cast<i32>(0)); break;       // alDistanceModel
            case 1: import_call<void>(d::iat_OpenAL32_alDistanceModel, static_cast<i32>(0xd001)); break;
            case 2: import_call<void>(d::iat_OpenAL32_alDistanceModel, static_cast<i32>(0xd002)); break;
            default: break;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Sound::set_distance_model", 0x00454000, &H::set_distance_model);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_charmove, "load_charmove_params");

i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

// 0x00454090: CHARMOVE parameters (value strings start 9 characters into the entry).
const char* charmove(img::Addr key) {
    void* const e = joan::orig::arg::as<void*>(joan::orig::as_ini(obj(d::g_steer))->section(joan::orig::arg::str(str(d::s_CHARMOVE))));  // 0x0047cbf0
    return joan::orig::arg::as<const char*>(joan::orig::as_ini(e)->value(joan::orig::arg::str(str(key)))) + 9;                       // 0x0047c180
}
float atof_f(const char* s) { return static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s)))); }
static void JOAN_CDECL load_charmove_params() {
    c_charmove.hit();
    global<float>(d::g_a3373c) = atof_f(charmove(d::s_AIM_R));  // AIM_R
    global<float>(d::g_a33758) = atof_f(charmove(d::s_AIM_MXD));  // AIM_MXD
    global<float>(d::g_aim_ha) = atof_f(charmove(d::s_AIM_HA));  // AIM_HA
    global<float>(d::g_turn_ang) = atof_f(charmove(d::s_TURN_ANG));  // TURN_ANG
    global<i32>(d::g_jumpmaxd) = atoi_(charmove(d::s_JUMPMAXD));     // JUMPMAXD
    global<i32>(d::g_a33748) = atoi_(charmove(d::s_JUMPMIND));     // JUMPMIND
    global<i32>(d::g_a33744) = atoi_(charmove(d::s_CHAINPRO));     // CHAINPRO
    global<i32>(d::g_a33750) = atoi_(charmove(d::s_CRI_TIME));     // CRI_TIME
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("load_charmove_params", 0x00454090, &load_charmove_params);
#undef B94

}  // namespace game_b94_01_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Player::rts_pick_object(std::uint32_t a0, std::uint32_t a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b104_01::H::pick_448340>(this, a0, a1, a2)); }  // 0x448340
auto joan::orig::Unit::target_point(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_01::H::target_point>(this, a0, a1)); }  // 0x448870
auto joan::orig::Player::cull_visible(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b115_12::H::cull_visible>(this, a0)); }  // 0x4489c0
auto joan::orig::Cell::unload_files() -> void { joan::bind::member<&::game_world5::H::unload_files>(this); }  // 0x449780
auto joan::orig::vector(void* a0, std::int32_t a1, std::int32_t a2, void* a3) -> void { joan::bind::free<&::game_leaves80a::vector_ctor>(a0, a1, a2, a3); }  // 0x449af0
auto joan::orig::Group::select_449b30(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_01::H::select_449b30>(this, a0)); }  // 0x449b30
auto joan::orig::Player::rts_pick_use() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b111_19::H::pick_use>(this)); }  // 0x44a430
auto joan::orig::MapObject::drain_hp(std::int32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b94_01::H::drain>(this, a0, a1)); }  // 0x44a5f0
auto joan::orig::Unit::leads() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_units::H::leads>(this)); }  // 0x44a660
auto joan::orig::World::siege_release_44a690() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_02::H::siege_release_44a690>(this)); }  // 0x44a690
auto joan::orig::Group::front_point2(void* a0) -> void { joan::bind::member<&::game_b89_04::H::group_front2>(this, a0); }  // 0x44a950
auto joan::orig::Group::select_slot(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b99_01::H::select_slot>(this, a0)); }  // 0x44aad0
auto joan::orig::Player::rts_pick(void* a0, void* a1, void* a2) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b110_16::H::pick_44ac60>(this, a0, a1, a2)); }  // 0x44ac60
auto joan::orig::World::draw() -> void { joan::bind::member<&::game_world::H::world_draw>(this); }  // 0x44c800
auto joan::orig::ThrowItems::cleanup_in_cell(void* a0) -> void { joan::bind::member<&::game_world_2::H::effects_cleanup>(this, a0); }  // 0x44cdd0
auto joan::orig::Actor::byte_10a() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_world::H::byte_10a>(this)); }  // 0x44cee0
auto joan::orig::Marker::word_8() -> std::uint16_t { return joan::bind::conv<std::uint16_t>(joan::bind::member<&::game_b92_01_2::H::get_8>(this)); }  // 0x44cf10
auto joan::orig::Marker::byte_e() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b92_01_2::H::get_e>(this)); }  // 0x44cf30
auto joan::orig::Marker::set_word_f(std::uint16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::set_word_f>(this, a0)); }  // 0x44cf50
auto joan::orig::List::empty_at_e(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world::H::list_empty_at>(this, a0)); }  // 0x44cf80
auto joan::orig::World::init(std::uint16_t a0, const char* a1, const char* a2, void* a3) -> void { joan::bind::member<&::game_b108_03::H::world_init>(this, a0, a1, a2, a3); }  // 0x44e8c0
auto joan::orig::World::siege_board_44eb80(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_02_2::H::siege_board_44eb80>(this, a0)); }  // 0x44eb80
auto joan::orig::Unit::nearest_enemy_of(std::int32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b92_01_3::H::nearest_enemy_of>(this, a0)); }  // 0x44f0b0
auto joan::orig::World::siege_attack_44f1f0(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_02_2::H::siege_attack_44f1f0>(this, a0)); }  // 0x44f1f0
auto joan::orig::Place::set_nation(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::set_nation>(this, a0)); }  // 0x44f5c0
auto joan::orig::Player::assign_troops(std::int32_t a0, std::int32_t a1) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b115_12_2::H::assign_troops>(this, a0, a1)); }  // 0x44f750
auto joan::orig::Scene::create_trigger(void* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_06::H::create_trigger>(this, a0, a1)); }  // 0x450ea0
auto joan::orig::Group::release_all() -> void { joan::bind::member<&::game_b86_06::H::release_all>(this); }  // 0x451430
auto joan::orig::Army::select_hero_group(std::int16_t a0, std::uint32_t a1, std::int32_t a2, std::int32_t a3) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b109_07::H::select_hero_group>(this, a0, a1, a2, a3)); }  // 0x451530
auto joan::orig::Group::front_point(void* a0) -> void { joan::bind::member<&::game_b89_04_2::H::group_front>(this, a0); }  // 0x451dc0
auto joan::orig::Building::drain_hp(std::int32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b96_01::H::drain_unit>(this, a0, a1)); }  // 0x451f20
auto joan::orig::Unit::drain_452280(std::int32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b100_01::H::drain_452280>(this, a0, a1)); }  // 0x452280
auto joan::orig::Player::map_quad(void* a0, void* a1) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b98_01::H::map_quad>(this, a0, a1)); }  // 0x452320
auto joan::orig::Group::count_active() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_01::H::count_active>(this)); }  // 0x4524c0
auto joan::orig::Group::first_ready_member() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::first_ready_member>(this)); }  // 0x452570
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"  // the inlined H::spawn_effect_200, see its memcpy
#pragma GCC diagnostic ignored "-Wstringop-overread"
#endif
auto joan::orig::Player::spawn_effect_200(std::int32_t a0) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b95_01::H::spawn_effect_200>(this, a0)); }  // 0x452720
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
auto joan::orig::Player::ray_query(std::uint32_t a0, std::uint32_t a1, float a2, float a3, float a4, void* a5) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_15::H::ray_query>(this, a0, a1, a2, a3, a4, a5)); }  // 0x452830
auto joan::orig::MapObject::drain_item(std::int32_t a0, std::int32_t a1) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b96_01::H::drain_item>(this, a0, a1)); }  // 0x452b20
auto joan::orig::Army::apply(std::uint32_t a0) -> void { joan::bind::member<&::game_b92_01_3::H::army_apply>(this, a0); }  // 0x452be0
auto joan::orig::World::cull_lights_452d20() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_02_2::H::cull_lights_452d20>(this)); }  // 0x452d20
auto joan::orig::Unit::start_paired_anim() -> void { joan::bind::member<&::game_b91_02::H::start_paired_anim>(this); }  // 0x4533e0
auto joan::orig::Player::group_contains(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_units_2::H::in_group>(this, a0)); }  // 0x453590
auto joan::orig::Unit::can_target_453620(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b91_02::H::can_target>(this, a0)); }  // 0x453620
auto joan::orig::Group::apply_units_453790(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_02::H::apply_units_453790>(this, a0)); }  // 0x453790
auto joan::orig::NameRecord::get_c() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_03::H::get_c>(this)); }  // 0x453cc0
auto joan::orig::NewsItem::check_state_41() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_01_2::H::check_state_41>(this)); }  // 0x453ce0
auto joan::orig::Player::name_tag(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b90_03::H::name_tag>(this, a0)); }  // 0x453da0
auto joan::orig::Sound::set_distance_model(char a0) -> void { joan::bind::member<&::game_b93_01::H::set_distance_model>(this, a0); }  // 0x454000
auto joan::orig::load_charmove_params() -> void { joan::bind::free<&::game_b94_01_2::load_charmove_params>(); }  // 0x454090
