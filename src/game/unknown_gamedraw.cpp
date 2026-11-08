// game/unknown_gamedraw.cpp: functions whose original source file is unknown (formerly game/_unplaced/gamedraw.cpp;
// re/attribution.csv: between ww/speech_s.cpp and ww/t_scrres.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/records.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

namespace {
// v3d "M" results (length, length_sq, dot_product): the replacements (src/lib/v3d/v3d87.cpp) return double. On x86
// the caller reads st0 as the float it always used (same code as before); x64 returns the double in xmm0, so it is
// read as a double there.
template <class... A>
float v3d_float(joan::img::Addr slot, A... args) {
#if UINTPTR_MAX == 0xffffffffu
    return joan::call::import_call<float>(slot, args...);
#else
    return static_cast<float>(joan::call::import_call<double>(slot, args...));
#endif
}
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::global;

using joan::hook::x87::Fpu;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i32 = std::int32_t;


using joan::call::import_call;

using u8 = std::uint8_t;

using i8 = std::int8_t;

using i16 = std::int16_t;

struct Obj {};

}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

// ---- static initialisers / atexit destructors (cdecl, no args) ----
u32 JOAN_CDECL init_4d2aa0() {
    joan::orig::init_pi_a3a294();
    return 0;  // eax: only reached from the _initterm table, which ignores it
}
u32 JOAN_CDECL init_4d2af0() {
    joan::orig::init_a3a298();
    return 0;  // eax: only reached from the _initterm table, which ignores it
}
u32 JOAN_CDECL init_4d42f0() {
    joan::orig::init_a3a2a0();
    return 0;  // eax: only reached from the _initterm table, which ignores it
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("init_4d2aa0", 0x004d2aa0, &init_4d2aa0);
B("init_4d2af0", 0x004d2af0, &init_4d2af0);
B("init_4d42f0", 0x004d42f0, &init_4d42f0);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
namespace {
namespace game_b98_03 {

// ------------------------------------------------------------------------------- cdecl
float pi_from_atan() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(Fpu{true}.mul(a, 4.0));       // qword [0x9cf788] = 4.0
}
void JOAN_CDECL init_pi_a3a294() { global<float>(d::g_pi_a3a294) = pi_from_atan(); }  // 0x004d2ac0

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("init_pi_a3a294", 0x004d2ac0, &init_pi_a3a294);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b83_06.cpp (game/b83_06)
namespace {
namespace game_b83_06 {

// Path string (0x190 chars + NUL, 0x005a11a0 / 0x005a1550 / 0x00417d90), no destructor.
struct alignas(4) Path {
    char b[0x194];
};

JOAN_COUNTER(c_anim, "AnimRes::update");

using joan::orig::helpers::game_new;  // operator new
std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }

struct H {
    // ------------------------------------------------------------------ 0x004d2b10
    void anim_update(std::int16_t id) {
        std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(this)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x0041a180
        if (jo::as_model_entry(rec)->load_state == 0) return;
        c_anim.hit();
        const std::int8_t b18 = jo::as_model_entry(rec)->param_a;
        const std::int8_t b1c = jo::as_model_entry(rec)->param_b;
        const std::int16_t w20 = jo::as_model_entry(rec)->param_anim;
        void* const res_mgr = obj(d::g_anim_res_mgr);
        auto res = [&]() { return jo::as_model_entry(rec)->actor_def; };
        if (jo::as_model_entry(rec)->load_state == 1) {
            if (jo::as_model_entry(rec)->actor_def == nullptr) {
                void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::ActorDef)));  // 0x2d8 on x86
                void* const o = mem != nullptr ? joan::orig::as_actor_def(mem)->construct() : nullptr;  // 0x005af390
                jo::as_model_entry(rec)->actor_def = static_cast<jo::ActorDef*>(o);
                if (b18 != 0 || b1c == 0) {
                    if (jo::view<std::int16_t>(jo::as_model_entry(rec)->anim) != 0) {
                        void* const m = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(res_mgr)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_model_entry(rec)->anim)))));  // 0x004b2730
                        joan::orig::as_anim_set(m)->add_ref();  // 0x004152b0
                        jo::as_model_entry(rec)->load_state = 2;
                    } else {
                        jo::as_model_entry(rec)->load_state = 2;
                    }
                } else {
                    joan::orig::as_anim_res_mgr(res_mgr)->add_ref(joan::orig::arg::i16(w20));  // 0x00417af0
                    jo::as_model_entry(rec)->load_state = 2;
                }
            } else {
                jo::as_model_entry(rec)->load_state = 0;
            }
        } else if (jo::as_model_entry(rec)->load_state == 2) {
            if (b18 != 0 || b1c == 0) {
                if (jo::view<std::int16_t>(jo::as_model_entry(rec)->anim) != 0) {
                    void* m = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(res_mgr)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_model_entry(rec)->anim)))));  // 0x004b2730
                    joan::orig::as_anim_set(m)->stream();  // 0x00415300
                    m = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(res_mgr)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_model_entry(rec)->anim)))));
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set(m)->idle()) != 0) {  // 0x004156a0
                        joan::orig::as_actor_def(res())->load_kind(joan::orig::arg::str(jo::as_model_entry(rec)->dir), joan::orig::arg::str(jo::as_model_entry(rec)->file), joan::orig::arg::u32(jo::view<std::int16_t>(jo::as_model_entry(rec)->anim)), 1u);  // 0x005b0000
                        jo::as_model_entry(rec)->load_state = 3;
                    }
                } else {
                    joan::orig::as_actor_def(res())->load_kind(joan::orig::arg::str(jo::as_model_entry(rec)->dir), joan::orig::arg::str(jo::as_model_entry(rec)->file), joan::orig::arg::u32(jo::view<std::int16_t>(jo::as_model_entry(rec)->anim)), 1u);  // 0x005b0000
                    jo::as_model_entry(rec)->load_state = 3;
                }
            } else {
                joan::orig::as_anim_res_mgr(res_mgr)->stream(joan::orig::arg::i16(w20));  // 0x00417b60
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_res_mgr(res_mgr)->idle(joan::orig::arg::i16(w20))) != 0) {  // 0x00417db0
                    const std::uint8_t zero = 0;
                    void* const* const h =
                        static_cast<void* const*>(joan::orig::as_anim_res_mgr(res_mgr)->slot_4174f0(joan::orig::arg::u32(static_cast<std::int32_t>(w20))));  // 0x004174f0
                    void* const hv = *h;
                    joan::orig::as_actor_def(res())->load_anim(joan::orig::arg::str(jo::as_model_entry(rec)->dir), joan::orig::arg::str(jo::as_model_entry(rec)->file), joan::orig::arg::u32(zero), jo::as_model_entry(rec)->file, hv);  // 0x005af5d0
                    jo::as_model_entry(rec)->load_state = 3;
                }
            }
        } else if (jo::as_model_entry(rec)->load_state == 3) {
            joan::orig::as_actor_def(res())->poll_load();                             // 0x005af890
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_actor_def(res())->loaded()) != 0) {  // 0x005afda0
                if (jo::view<std::uint32_t>(jo::as_model_entry(rec)->anim_db_id) != 0) {
                    jo::TexScriptRec* const tbl =
                        joan::orig::arg::as<jo::TexScriptRec*>(joan::orig::as_anim_db(obj(d::g_texture_effect_res))->entry(joan::orig::arg::u32(jo::view<std::int16_t[2]>(jo::as_model_entry(rec)->anim_db_id)[0])));  // 0x004b27b0
                    auto model = [&]() { return jo::as_actor_def(res())->model->mesh; };
                    for (std::int32_t n = model()->material_count; n > 0; --n) {
                        jo::Material* const mat = &model()->materials[n - 1];
                        std::int32_t i = 0;
                        for (; i < 6; ++i)
                            if (n == static_cast<std::int32_t>(jo::view<std::int16_t>(tbl->material_indices[i]))) break;
                        if (i == 6) continue;
                        for (std::int32_t k = jo::view<std::int16_t>(tbl->stage_count[i]); k > 0; --k) {
                            void* const t = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(&tbl->stage_names[i * 0x84]), 1, 0u));  // 0x006099c0
                            joan::orig::as_material(mat)->set_texture(joan::orig::arg::i32(k - 1), joan::orig::arg::mptr(t));         // 0x005d5650
                            joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(t));    // 0x0060a550
                            mat->raster |= 0x100u;
                            mat->texture_mode = jo::view<std::uint32_t>(jo::as_model_entry(rec)->anim_db_id) | 0x1000000u |
                                                           (static_cast<std::uint32_t>(i) << 16);
                        }
                    }
                }
                jo::as_model_entry(rec)->load_state = 4;
            }
        } else if (jo::as_model_entry(rec)->load_state == 4) {
            Path path;
            joan::orig::as_string(&path)->construct(joan::orig::arg::str(jo::as_model_entry(rec)->dir));  // 0x005a11a0
            joan::orig::as_string(&path)->append(str(d::s_ANIM));                         // 0x005a1550 "ANIM\"
            for (std::int32_t i = 0; i < jo::as_model_entry(rec)->morph_count; ++i) {
                jo::ModelMorphRec* const e = &jo::as_model_entry(rec)->morphs[i];
                void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Morph)));  // 0x84 on x86
                void* const o = mem != nullptr ? joan::orig::as_morph(mem)->construct() : nullptr;  // 0x005c67b0
                jo::view<void*>(e->morph) = o;
                joan::orig::as_morph(e->morph)->load(joan::orig::arg::str(e->name), joan::orig::arg::str(jo::as_model_entry(rec)->dir), joan::orig::arg::str(e->file), joan::orig::arg::mptr(0));  // 0x005c6820
                if (strlen_(e->anim) != 0) {
                    void* const mem2 = game_new(static_cast<std::uint32_t>(sizeof(jo::MorphAnim)));  // 0x10 on x86
                    void* const a = mem2 != nullptr ? joan::orig::as_morph_anim(mem2)->construct() : nullptr;  // 0x005cae20
                    jo::view<void*>(e->morph->anim) = a;
                    void* const anim_obj = e->morph;
                    char* const anim_name = e->anim;
                    const char* const dir = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
                    joan::orig::as_morph_anim(e->morph->anim)->load(joan::orig::arg::str(dir), joan::orig::arg::str(anim_name), joan::orig::arg::mptr(anim_obj));  // 0x005cae80
                }
            }
            jo::as_model_entry(rec)->load_state = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_06", name, addr, fn)
B("AnimRes::update", 0x004d2b10, &H::anim_update);
#undef B

}  // namespace game_b83_06
}  // namespace

// ---------------------------------------- from modules/game_objects1.cpp (game/objects1)
namespace {
namespace game_objects1 {


struct V3 {
    float x, y, z;
};
struct B20 {
    std::uint32_t d[5];
};
struct B24 {
    std::uint32_t d[6];
};
static_assert(sizeof(V3) == 12 && sizeof(B20) == 20 && sizeof(B24) == 24, "by-value sizes");

JOAN_COUNTER(c_ray, "Sky::ray_clear");

struct H {
    // 0x004d30f0 Sky::ray_clear(camera, point): 0 when a scene node / actor between the camera
    // eye (pulled back along its view axis) and point blocks it.
    std::uint8_t ray_clear(void* camera, V3 point) {
        c_ray.hit();
        std::uint8_t frame[40] = {};
        joan::orig::as_camera(camera)->frame(joan::orig::arg::mptr(frame));  // 0x006140a0
        void* const root = jo::as_world(global<void*>(d::g_world))->scene;
        float rot[16];
        joan::orig::orbit_matrix(joan::orig::arg::mptr(rot), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::CamPose>(frame)->target[0])), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::CamPose>(frame)->target[1])), joan::orig::arg::f32(jo::view<std::uint32_t>(jo::self<jo::CamPose>(frame)->target[2])), joan::orig::arg::f32(0));  // 0x0060bd00
        float axis[3];
        joan::orig::matrix_axis_z(joan::orig::arg::mptr(axis), joan::orig::arg::ptr(rot));  // 0x0060c220
        // The segment (point, pulled-back eye): the segment tests below take one pointer to both points (the
        // original keeps them adjacent at ebp-0x94 / ebp-0x88); two separate locals left q to the frame layout.
        struct Line {
            V3 p, q;
        };
        static_assert(sizeof(Line) == 24, "Line: two adjacent points");
        Line line{point, {}};
        V3& p = line.p;  // ebp-0x94
        V3& q = line.q;  // ebp-0x88
        const float back = jo::view<float>(jo::self<jo::CamPose>(frame)->u32_1c);
        float t = axis[0] * back;
        q.x = jo::self<jo::CamPose>(frame)->eye[0] - t;
        t = axis[1] * back;
        q.y = jo::self<jo::CamPose>(frame)->eye[1] - t;
        t = axis[2] * back;
        q.z = jo::self<jo::CamPose>(frame)->eye[2] - t;
        float box[6];
        joan::orig::as_aabb(box)->set_point(&p);  // 0x0060d430
        joan::orig::as_aabb(box)->extend(&q);  // 0x0060d490
        std::int32_t count = 0;
        void** const nodes = joan::orig::arg::as<void**>(joan::orig::as_scene(root)->nodes_in_box(joan::orig::arg::mptr(box), &count));  // 0x005ef610
        float dist = 0.0f;  // out only (every reader writes it first)
        for (std::int32_t i = 0; i < count; ++i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::segment_hits_box(&line, joan::orig::arg::ptr(jo::as_scene_s(nodes[i])->desc))) != 0 && jo::as_scene_s(nodes[i])->plane != nullptr) {  // 0x0060dfd0
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::as_scene_s(nodes[i])->plane)->segment_hit(&dist, &line)) != 0) return 0;  // 0x005d4a80
            }
            for (std::uint8_t* a = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene_s(nodes[i])->first_actor()); a != nullptr; a = jo::view<std::uint8_t*>(jo::as_actor(a)->scene_prev)) {  // 0x005effa0
                if (jo::as_actor(a)->kind == 0xb) continue;
                if (jo::as_actor(a)->faded != 0) continue;
                void* const plane = jo::as_actor(a)->plane;
                if (plane != nullptr && static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::as_plane(plane)->vertex_format)) == 7) {
                    float sphere[4];
                    joan::orig::as_actor(a)->bounding_sphere(joan::orig::arg::mptr(sphere), 1u);  // 0x005acc80
                    if (!(sphere[3] > global<float>(d::flt_200))) continue;
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::segment_hits_sphere(&line, joan::orig::arg::mptr(sphere))) == 0) continue;  // 0x0060e3f0
                    std::uint8_t hit = 0;
                    joan::orig::as_bone(jo::as_actor(a)->bone)->update();  // 0x0060ee60
                    void* const m = jo::as_actor(a)->bone->matrix_;
                    float inv[16];
                    import_call<void*>(d::iat_v3d_87_matrix_inverse_2, inv, m);  // matrix_inverse
                    V3 lp, lq;
                    import_call<void*>(d::iat_v3d_87_point_transform_3, &lp, &p, inv);  // point_transform
                    import_call<void*>(d::iat_v3d_87_point_transform_3, &lq, &q, inv);
                    const std::uint32_t shape = jo::as_actor(a)->aim_mode;
                    if (shape <= 3) {
                        if (shape == 0) return 0;
                        V3 tmp, dir;
                        std::memcpy(&dir, joan::orig::arg::as<void*>(joan::orig::vec_sub(&tmp, &lq, &lp)), 12);  // 0x0060cc90
                        if (shape == 1) {
                            hit = joan::orig::arg::as<std::uint8_t>(joan::orig::shape::sphere_hit(&dist, lp, dir, joan::orig::arg::mptr(jo::as_actor(a)->aim_sphere)));  // 0x006a0f90
                        } else if (shape == 2) {
                            B24 b;
                            std::memcpy(&b, jo::as_actor(a)->aim_box, 24);
                            hit = joan::orig::arg::as<std::uint8_t>(joan::orig::shape::box_hit(&dist, lp, dir, b));  // 0x00698970
                        } else {
                            B20 b;
                            std::memcpy(&b, &jo::as_actor(a)->collide_kind, 20);
                            hit = joan::orig::arg::as<std::uint8_t>(joan::orig::shape::cylinder_hit(&dist, lp, dir, b));  // 0x00699ec0
                        }
                    }
                    if (hit != 0) return 0;
                    continue;
                }
                if (plane == nullptr) continue;
                float sphere[4];
                joan::orig::as_actor(a)->bounding_sphere(joan::orig::arg::mptr(sphere), 1u);
                if (!(sphere[3] > global<float>(d::flt_200))) continue;
                if (joan::orig::arg::as<std::int32_t>(joan::orig::segment_hits_sphere(&line, joan::orig::arg::mptr(sphere))) == 0) continue;
                const std::int32_t face = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::as_actor(a)->plane)->segment_hit(&dist, &line));
                if (face == 0) continue;
                const jo::ArrU16* tbl;
                if (jo::as_actor(a)->plane->data_pending != 0) {
                    tbl = jo::as_actor(a)->plane->source->triangle_materials;
                } else {
                    tbl = jo::as_actor(a)->plane->triangle_materials;
                }
                const std::int32_t material = tbl[face - 1].value;
                const jo::Material* const mats = jo::as_actor(a)->def->model->mesh->materials;
                if (jo::view<std::int32_t>(mats[material].blend) != 1) return 0;
            }
        }
        return 1;
    }
};

#define OB1(name, addr, fn) JOAN_REPLACE("game/objects1", name, addr, fn)
OB1("Sky::ray_clear", 0x004d30f0, &H::ray_clear);
#undef OB1

}  // namespace game_objects1
}  // namespace

// ---------------------------------------- from modules/game_b109_19.cpp (game/b109_19)
namespace {
namespace game_b109_19 {

using joan::hook::x87::ftol;

// v3d_87.dll exports are cdecl, called through their IAT slot.
using joan::orig::helpers::v3d_call;
using joan::orig::helpers::fbits;
i32 ftolf(float f) { return ftol(static_cast<double>(f)); }

struct V3 {  // 12 bytes copied as raw dwords
    u32 x, y, z;
};
struct V3f {  // the same bytes as the three floats of a by-value argument (System V: XMM registers)
    float x, y, z;
};
template <class To, class From>
To rebits(const From& from) {
    static_assert(sizeof(To) == sizeof(From));
    To to;
    std::memcpy(&to, &from, sizeof to);
    return to;
}

V3 load_v3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

constexpr img::Addr kLength = d::iat_v3d_87_length;  // v3d_87.dll length(const D3DVECTOR&): returns a double
#if UINTPTR_MAX == 0xffffffffu
float vec_length(const void* v) { return v3d_call<float>(kLength, v); }  // st0, rounded once as the original's fstp dword
#else
// x64: the double comes back in xmm0; read as a float the low half came back.
float vec_length(const void* v) { return static_cast<float>(v3d_call<double>(kLength, v)); }
#endif
// 0x0060cc90 vector subtract (cdecl out, a, b): out = a - b.
V3 vsub(const void* a, const void* b) {
    V3 out{0, 0, 0};
    V3 copy = *joan::orig::arg::as<V3*>(joan::orig::vec_sub(static_cast<void*>(&out), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)));
    return copy;
}

JOAN_COUNTER(c_path, "PathGrid::find_path_4d3790");

struct H {
    // -------------------------------------------------------------- 0x004d3790
    u8* node(i32 n) { return reinterpret_cast<u8*>(&jo::self<jo::PathGraph>(this)->nodes[n - 1]); }
    i32 next_stamp() {
        if (jo::view<i32>(jo::self<jo::PathGraph>(this)->u32_4e) == 0) {
            for (i32 i = 0; i < jo::view<i32>(jo::self<jo::PathGraph>(this)->node_count); ++i) jo::self<jo::PathGraph>(this)->nodes[i].search_stamp = 0;
            jo::view<i32>(jo::self<jo::PathGraph>(this)->u32_4e) = 1;
        }
        const i32 stamp = jo::view<i32>(jo::self<jo::PathGraph>(this)->u32_4e);
        jo::view<i32>(jo::self<jo::PathGraph>(this)->u32_4e) = jo::view<i32>(jo::self<jo::PathGraph>(this)->u32_4e) + 1;
        return stamp;
    }
    static u8* path_point(void* path, i32 i) { return joan::orig::arg::as<u8*>(joan::orig::as_list(path)->from_end(joan::orig::arg::i32(i))); }  // 0x0066e8d0
    // Sum of the segment lengths from point first-1 .. count-1 (fild + fadd at PC24, _ftol).
    static i32 path_tail(void* path, i32 first) {
        i32 total = 0;
        for (i32 i = first; i < joan::orig::arg::as<i32>(joan::orig::as_list(path)->count_66e8a0()); ++i) {  // 0x0066e8a0
            const V3 a = load_v3(path_point(path, i - 1));
            const V3 b = load_v3(path_point(path, i));
            const V3 d = vsub(&a, &b);
            const float acc = static_cast<float>(total);  // fild + fstp float
            const float len = vec_length(&d);
            total = ftolf(len + acc);
        }
        return total;
    }
    u8 find_path(V3f from_f, V3f to_f, void* path, u32 extra) {
        const V3 from = rebits<V3>(from_f), to = rebits<V3>(to_f);
        c_path.hit();
        joan::orig::as_path(path)->clear();  // 0x004eeca0
        i32 i8_ = 0;  // [ebp-0x8..-0x10]: from (overwritten below before use)
        i32 ic = 0;
        i32 i10 = 0;
        const i32 i14 = ftolf(fbits(to.x));
        const i32 i18 = ftolf(fbits(to.y));
        const i32 i1c = ftolf(fbits(to.z));
        if (jo::self<jo::PathGraph>(this)->node_count == 0 || jo::self<jo::PathGraph>(this)->nodes == nullptr) return 0;
        i32 n20 = 0;
        i32 n24 = 0;
        joan::orig::as_path_graph(this)->nearest_two(&n20, &n24, rebits<V3f>(from), rebits<V3f>(to));  // 0x004ec480
        if (n20 == 0 || n24 == 0) return 0;
        u8* p28 = node(n20);
        u8* const p2c = node(n24);
        i8_ = ftolf(jo::self<jo::TrackNode>(p28)->pos[0]);
        ic = ftolf(jo::self<jo::TrackNode>(p28)->pos[1]);
        i10 = ftolf(jo::self<jo::TrackNode>(p28)->pos[2]);
        i32 stamp = next_stamp();
        const V3 start = from;
        {
            const V3 d = vsub(jo::self<jo::TrackNode>(p28)->pos, &start);
            (void)vec_length(&d);  // stored to [ebp-0x48], overwritten before any read
        }
        joan::orig::as_path_box(path)->set(joan::orig::arg::i32(i8_), joan::orig::arg::i32(ic), joan::orig::arg::i32(i10), joan::orig::arg::i32(i14), joan::orig::arg::i32(i18), joan::orig::arg::i32(i1c));  // 0x0066e640
        if (static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_path(this)->find_4ed040(joan::orig::arg::mptr(p28), joan::orig::arg::mptr(p2c), joan::orig::arg::u32(stamp), joan::orig::arg::mptr(path), joan::orig::arg::i32(extra)))) == 0) {  // 0x004ed040
            joan::orig::as_path(path)->clear();  // 0x004eeca0
            return 0;
        }
        u8* const p4c = path_point(path, 1);
        u8* p50 = path_point(path, 2);
        if (p50 == nullptr) {
            joan::orig::as_path(path)->clear();  // 0x004eeca0
            return 0;
        }
        i32 index = 2;
        const V3 v60 = load_v3(p4c);
        V3 v6c = load_v3(p50);
        i32 seg;
        {
            const V3 d = vsub(&v60, &v6c);
            seg = ftolf(vec_length(&d));
        }
        float dist;
        {
            const V3 d = vsub(&v6c, &start);
            dist = vec_length(&d);
        }
        i32 rest = path_tail(path, 3);
        const Fpu fpu{true};
        while (p4c != nullptr && p50 != nullptr && seg > 0) {
            if (seg >= 10 && dist >= global<float>(d::flt_10)) break;  // continue when < 10 or unordered
            const double sum = fpu.add(static_cast<double>(rest), static_cast<double>(dist));
            const i32 limit = static_cast<i32>(static_cast<u32>(rest) + static_cast<u32>(seg));
            if (!(sum > static_cast<double>(limit))) break;
            ++index;
            p50 = path_point(path, index);
            if (p50 != nullptr) {
                v6c = load_v3(p50);
                {
                    const V3 d = vsub(&v60, &v6c);
                    seg = ftolf(vec_length(&d));
                }
                {
                    const V3 d = vsub(&v6c, &start);
                    dist = vec_length(&d);
                }
                rest = path_tail(path, index + 1);
            }
        }
        if (p50 == nullptr) {
            joan::orig::as_path(path)->clear();  // 0x004eeca0
            return 0;
        }
        const V3 corner = load_v3(p50);
        joan::orig::as_path_graph(this)->nearest_two(&n20, &n24, rebits<V3f>(corner), rebits<V3f>(to));  // 0x004ec480
        p28 = node(n20);  // (the original re-converts its +8/+0xc/+0x10 to dead int locals here)
        stamp = next_stamp();
        joan::orig::as_path(path)->clear();  // 0x004eeca0
        joan::orig::as_path_box(path)->set(joan::orig::arg::i32(ftolf(jo::self<jo::TrackNode>(p28)->pos[0])), joan::orig::arg::i32(ftolf(jo::self<jo::TrackNode>(p28)->pos[1])), joan::orig::arg::i32(ftolf(jo::self<jo::TrackNode>(p28)->pos[2])), joan::orig::arg::i32(ftolf(jo::self<jo::TrackNode>(p2c)->pos[0])), joan::orig::arg::i32(ftolf(jo::self<jo::TrackNode>(p2c)->pos[1])), joan::orig::arg::i32(ftolf(jo::self<jo::TrackNode>(p2c)->pos[2])));  // 0x0066e640
        return static_cast<i8>(joan::orig::arg::as<u32>(joan::orig::as_path(this)->find_4ed040(joan::orig::arg::mptr(p28), joan::orig::arg::mptr(p2c), joan::orig::arg::u32(stamp), joan::orig::arg::mptr(path), joan::orig::arg::i32(extra)))) != 0 ? 1 : 0;  // 0x004ed040
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_19", name, addr, fn)
B("PathGrid::find_path_4d3790", 0x004d3790, &H::find_path);
#undef B

}  // namespace game_b109_19
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
namespace {
namespace game_b103_04 {

u32 bits(float v) {
    u32 b;
    std::memcpy(&b, &v, 4);
    return b;
}

void* const kUnits = obj(d::g_map_object_list);   // unit list
void* const kTypes = obj(d::g_item_res);   // unit type table

JOAN_COUNTER(c_find_target, "Unit::find_target");

void JOAN_CDECL init_4d42b0() { joan::orig::init_422a00(); }

// 3-float vector a - b (0x0060cc90, cdecl, returns out).
void vsub(float* out, const float* a, const float* b) { joan::orig::vec_sub(joan::orig::arg::mptr(out), joan::orig::arg::ptr(a), joan::orig::arg::ptr(b)); }
float vlength(float* v) { return v3d_float(d::iat_v3d_87_length, static_cast<void*>(v)); }
double vdot(float* a, float* b) { return import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(a), static_cast<void*>(b)); }

struct H {
    // ------------------------------------------------------------------------ 0x004d6320
    static u8* vcall_4c(u8* o) {
        using Virtual = u8* (Obj::*)();
        JOAN_MEMBER_POINTER_CHECK(Virtual);
        Virtual method{};
        std::memcpy(&method, &joan::call::vtable_of(o)[0x4c / 4], sizeof(void*));
        return (reinterpret_cast<Obj*>(o)->*method)();
    }
    static u8* type_of(u8* o) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(jo::as_map_object(o)->type_id))); }  // 0x00425ce0
    i32 find_target(i32 range) {
        c_find_target.hit();
        float pos[3];
        joan::orig::as_unit(this)->position(joan::orig::arg::mptr(pos));  // 0x004fedb0
        float best = static_cast<float>(range);
        i32 result = 0;
        for (i32 i = joan::orig::as_dynarray(kUnits)->size(); i > 0; --i) {  // 0x00417ad0
            u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (o == nullptr) continue;
            if (jo::view<i32>(jo::as_map_object(o)->interact_trigger) != 0x200 && jo::as_map_object(o)->class_id != 0xb) continue;
            if (jo::as_map_object(o)->side != 0 && jo::as_map_object(o)->side != jo::self<jo::Unit>(this)->side) {
                if (jo::as_map_object(o)->class_id == 1) {
                    const float a = joan::orig::arg::as<float>(joan::orig::as_map_object(o)->heading_to(joan::orig::arg::mptr(pos)));  // 0x0041af80
                    if (a > -90.0f && a < 90.0f) continue;
                    if (!(jo::as_gate(vcall_4c(o))->max_hp > 0)) continue;  // slot 0x4c returns the Gate
                    if (jo::view<i32>(jo::as_item_def(type_of(o))->struction_type) == 5) continue;
                } else if (jo::as_map_object(o)->class_id != 0xb) {
                    continue;
                }
            }
            if (jo::as_map_object(o)->class_id == 1) {
                if (static_cast<i32>(jo::as_gate(vcall_4c(o))->state) > 1) continue;
                if (jo::view<i8>(jo::as_item_def(type_of(o))->need_key) != 0) {
                    if (static_cast<i32>(jo::as_gate(vcall_4c(o))->state) > 0) continue;
                }
            } else if (jo::as_map_object(o)->class_id == 0xb) {
                if (joan::orig::arg::as<i32>(joan::orig::as_world(this)->flag_1fd()) != 0) continue;  // 0x0042d690
            }
            u8* const type = type_of(o);
            float pos2[3];
            joan::orig::as_map_object(o)->position(joan::orig::arg::mptr(pos2));  // 0x00499930
            if (jo::as_map_object(o)->class_id == 0xb) {
                float d[3];
                vsub(d, pos2, pos);
                float len = vlength(d);
                float v[3] = {global<float>(d::g_a18a40), global<float>(d::g_a18a44), global<float>(d::g_a18a48)};
                float m1[16];
                joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(m1), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->heading)), 0u, 0u);  // 0x0060bf40
                float row[3] = {m1[0], m1[1], m1[2]};
                const float r1 = joan::orig::arg::as<float>(joan::orig::as_map_object(o)->yaw());  // 0x00499e20
                const float r2 = joan::orig::arg::as<float>(joan::orig::as_unit(o)->get_66());  // 0x0049a8c0
                const float r3 = joan::orig::arg::as<float>(joan::orig::as_map_object(o)->roll());  // 0x0049ad10
                float m2[16];
                joan::orig::matrix_rotation_ypr(joan::orig::arg::mptr(m2), joan::orig::arg::u32(bits(r1)), joan::orig::arg::u32(bits(r2)), joan::orig::arg::u32(bits(r3)));
                import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(v), static_cast<void*>(m2));  // vector_transform
                if (!(len > 90.0f)) {
                    float e[3];
                    vsub(e, pos, pos2);
                    if (vdot(v, e) >= 0.0 && !(vdot(v, row) > 0.0)) return jo::as_map_object(o)->index;
                }
                pos2[2] = static_cast<float>(static_cast<i32>(jo::view<i16>(jo::as_item_def(type)->stats[1]))) + pos2[2];
                vsub(d, pos2, pos);
                len = vlength(d);
                if (!(len > 90.0f)) {
                    float e[3];
                    vsub(e, pos, pos2);
                    if (!(vdot(v, e) > 0.0) && vdot(v, row) > 0.0) return jo::as_map_object(o)->index;
                }
            } else {
                float d[3];
                vsub(d, pos2, pos);
                const float len = vlength(d);
                if (len > best) continue;
                const float dz = pos2[2] - pos[2];
                const i32 dist = joan::orig::arg::as<i32>(joan::orig::libc::abs(joan::orig::arg::i32(joan::hook::x87::ftol(dz))));  // abs
                if (dist > range / 3) continue;
                best = len;
                result = jo::as_map_object(o)->index;
            }
        }
        return result;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("init_4d42b0", 0x004d42b0, &init_4d42b0);
B("Unit::find_target", 0x004d6320, &H::find_target);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b99_04.cpp (game/b99_04)
namespace {
namespace game_b99_04 {

// ------------------------------------------------------------------------------- cdecl
// 0x004d4310
void JOAN_CDECL init_a3a2a0() { global<float>(d::g_a3a2a0) = global<float>(d::g_pi_a3a294) / global<float>(d::flt_4); }

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_04", name, addr, fn)
B99("init_a3a2a0", 0x004d4310, &init_a3a2a0);
#undef B99

}  // namespace game_b99_04
}  // namespace

// ---------------------------------------- from modules/game_b104_05.cpp (game/b104_05)
namespace {
namespace game_b104_05 {

using u16 = std::uint16_t;

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_outline, "Outline::build_4d4340");

// ------------------------------------------------------------------------ 0x004d4340
using joan::orig::helpers::game_new;  // operator new

void JOAN_CDECL outline_build() {
    c_outline.hit();
    if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) != 0) return;  // 0x00428b70
    void* const unit = joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3c0
    void* const shape = jo::as_unit(unit)->cell;
    if (shape == nullptr) return;
    if (static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_world_cell(shape)->path_data_flags())) == 0) return;  // 0x00560060
    if (global<void*>(d::g_a39a0c) != nullptr) {
        void* const scene = jo::as_world_cell(shape)->world->scene;
        joan::orig::as_scene(scene)->remove_group(joan::orig::arg::mptr(global<void*>(d::g_a39a0c)));  // 0x005ea5e0
        global<void*>(d::g_a39a0c) = nullptr;
    }
    if (global<u32>(d::g_a39a10) == 0) return;

    void* const raw = game_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
    void* const mesh = raw != nullptr ? joan::orig::as_plane(raw)->construct(4u, 0u) : nullptr;  // 0x005cbaf0
    joan::orig::as_plane(mesh)->init(0, 0, 0, 1u);                 // 0x005cc7a0
    const i32 mat = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_materials(1));          // 0x005cd470
    jo::Material* const m = &jo::as_plane(mesh)->materials[static_cast<u32>(mat)];
    joan::orig::as_material(m)->construct();  // 0x005d52a0
    m->blend = 5;
    m->texture_mode = 0;

    void* const list = joan::orig::arg::as<void*>(joan::orig::as_world_cell(shape)->grid(joan::orig::arg::i16(1)));  // 0x005600c0
    const i32 n = joan::orig::arg::as<i32>(joan::orig::as_path_space(list)->zone_count());             // 0x0066b9f0
    for (i32 i = 1; i <= n; ++i) {
        void* const poly = joan::orig::arg::as<void*>(joan::orig::as_billboard(jo::as_path_space(list)->zones)->item(joan::orig::arg::i32(i)));  // 0x00637da0
        const i32 vbase = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_vertices(joan::orig::arg::i32(jo::view<u32>(jo::as_pfd_zone(poly)->point_count) << 1)));  // 0x005cd7f0
        const i32 lbase = joan::orig::arg::as<i32>(joan::orig::as_plane(mesh)->add_triangles(joan::orig::arg::i32(jo::as_pfd_zone(poly)->point_count)));       // 0x005ce6f0
        for (i32 j = 1; j <= jo::as_pfd_zone(poly)->point_count; ++j) {
            const u32 k0 = static_cast<u32>(vbase) + static_cast<u32>(j) * 2u - 2u;
            const u32 k1 = static_cast<u32>(vbase) + static_cast<u32>(j) * 2u - 1u;
            const i32* pt = joan::orig::arg::as<const i32*>(joan::orig::as_pfd_zone(poly)->point(joan::orig::arg::i32(j)));  // 0x00671890
            float v = static_cast<float>(pt[0]);
            jo::as_plane(mesh)->vertices_f4[k0].pos[0] = v;
            pt = joan::orig::arg::as<const i32*>(joan::orig::as_pfd_zone(poly)->point(joan::orig::arg::i32(j)));
            v = static_cast<float>(pt[1]);
            jo::as_plane(mesh)->vertices_f4[k0].pos[1] = v;
            pt = joan::orig::arg::as<const i32*>(joan::orig::as_pfd_zone(poly)->point(joan::orig::arg::i32(j)));
            v = static_cast<float>(static_cast<i32>(static_cast<u32>(pt[2]) + 5u));
            jo::as_plane(mesh)->vertices_f4[k0].pos[2] = v;
            jo::view<u8[4]>(jo::as_plane(mesh)->vertices_f4[k0].attr[0])[2] = 0xff;
            jo::view<u8[4]>(jo::as_plane(mesh)->vertices_f4[k0].attr[0])[0] = 0;
            jo::view<u8[4]>(jo::as_plane(mesh)->vertices_f4[k0].attr[0])[1] = 0;
            const i32 next = j == jo::as_pfd_zone(poly)->point_count ? 1 : j + 1;
            pt = joan::orig::arg::as<const i32*>(joan::orig::as_pfd_zone(poly)->point(joan::orig::arg::i32(next)));
            v = static_cast<float>(pt[0]);
            jo::as_plane(mesh)->vertices_f4[k1].pos[0] = v;
            pt = joan::orig::arg::as<const i32*>(joan::orig::as_pfd_zone(poly)->point(joan::orig::arg::i32(next)));
            v = static_cast<float>(pt[1]);
            jo::as_plane(mesh)->vertices_f4[k1].pos[1] = v;
            pt = joan::orig::arg::as<const i32*>(joan::orig::as_pfd_zone(poly)->point(joan::orig::arg::i32(next)));
            v = static_cast<float>(static_cast<i32>(static_cast<u32>(pt[2]) + 5u));
            jo::as_plane(mesh)->vertices_f4[k1].pos[2] = v;
            jo::view<u8[4]>(jo::as_plane(mesh)->vertices_f4[k1].attr[0])[2] = 0;
            jo::view<u8[4]>(jo::as_plane(mesh)->vertices_f4[k1].attr[0])[0] = 0xff;
            jo::view<u8[4]>(jo::as_plane(mesh)->vertices_f4[k1].attr[0])[1] = 0;
            joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(static_cast<u32>(lbase) + static_cast<u32>(j) - 1u), joan::orig::arg::u32(// 0x005ce800
                          mat), joan::orig::arg::u32(k0), joan::orig::arg::u32(k1), joan::orig::arg::u32(k1));
        }
        joan::orig::as_plane(mesh)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(mesh)->sort_by_material(1);   // 0x005cf1d0
    }
    if (global<void*>(d::g_a39a0c) == nullptr) {
        void* const scene = jo::as_world_cell(shape)->world->scene;
        global<void*>(d::g_a39a0c) = joan::orig::arg::as<void*>(joan::orig::as_scene(scene)->add_node());  // 0x005ea460
        joan::orig::as_scene_source(global<void*>(d::g_a39a0c))->reset(joan::orig::arg::mptr(jo::as_world_cell(shape)->box));  // 0x005ef880
    }
    joan::orig::as_scene_source(global<void*>(d::g_a39a0c))->set_38(joan::orig::arg::mptr(mesh));  // 0x005efbe0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b104_05", name, addr, fn)
B("Outline::build_4d4340", 0x004d4340, &outline_build);
#undef B

}  // namespace game_b104_05
}  // namespace

// ---------------------------------------- from modules/game_draw1.cpp (game/draw1)
namespace {
namespace game_draw1 {

// `mov ecx, object; push object; call [vtable + index * 4]` with the callee popping 4 bytes:
// emulated exactly (ecx = object and one stack argument = object), which is right for both a
// COM stdcall method without arguments and a thiscall with one argument.
template <class R = void>
R self_virtual(void* object, std::int32_t index) {
    using Fn = R (Obj::*)(void*);
    const std::uintptr_t fn = reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(object)[index]);
    return (static_cast<Obj*>(object)->*joan::hook::original<Fn>(fn))(object);
}

constexpr img::Addr kText = d::g_text_a5d140;    // text renderer
constexpr img::Addr kTimer = d::g_misc;   // clock / number formatting
constexpr img::Addr kSysEn = d::g_sys3d;   // D3D wrapper
constexpr img::Addr kSprites = d::g_sprites_a46128; // 2D sprite queue
constexpr img::Addr kVideo = d::g_video_file;   // video player

// 0x0042a260 Color(r, g, b, a) on a 4-byte local, returned dword copied out.
std::uint32_t color(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    std::uint32_t* const p = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
    return *p;
}
std::uint32_t white() { return color(0xff, 0xff, 0xff, 0xff); }
// 0x006255d0 Text::draw_line(x, y, size, text, color, -1, 0, 1, 2, 0, 1.0f, 1.0f), ret 0x30.
void text(std::int32_t x, std::int32_t y, std::int32_t size, const char* s, std::uint32_t c) {
    joan::orig::as_text(obj(kText))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(size), s, joan::orig::arg::u32(c), joan::orig::arg::i32(-1), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x006255d0
}
// 0x0058fe20 number -> string (value, 1), ret 0xc.
const char* number(double v) { return joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(kTimer))->format_double(joan::orig::arg::f64(v), 1)); }  // 0x0058fe20
// 0x004d69a0 integer -> string (value, 1), ret 8.
const char* integer(std::uint32_t v) { return joan::orig::as_misc(obj(kTimer))->format_int(joan::orig::arg::u32(v), 1u); }  // 0x004d69a0
std::uint32_t clock_ms() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(obj(kTimer))->get_time_591050()); }  // 0x00591050

void label(std::int32_t x, std::int32_t y, const char* s) {
    const std::uint32_t c = white();
    text(x, y, 0xa, s, c);
}
void float_value(std::int32_t x, std::int32_t y, img::Addr global_float) {
    const std::uint32_t c = white();
    const char* const s = number(static_cast<double>(global<float>(global_float)));
    text(x, y, 0xa, s, c);
}
void frame_time(std::int32_t x, std::int32_t y) {
    const std::uint32_t c = white();
    const double v = joan::orig::arg::as<double>(joan::orig::as_timer(obj(d::g_cpu_usage))->elapsed_ms(joan::orig::arg::mptr(obj(d::g_a485c0))));  // 0x0045e7f0 (st0)
    const char* const s = number(v);
    text(x, y, 0xa, s, c);
}
void polygon_count() {
    text(0x12c, 0x64, 0xa, str(d::s_Polygons_Count), white());  // 'Polygons Count:'
    const std::uint32_t c = white();
    const char* const s = integer(global<std::uint32_t>(d::g_a526bf));
    text(0x1c2, 0x64, 0xa, s, c);
}
// fild qword (zero-extended dword); fdiv qword 1024.0; fdiv qword 1024.0 -> qword.
double megabytes(std::uint32_t bytes) {
    const joan::hook::x87::Fpu fpu{true};
    const double k = global<double>(d::dbl_1024);  // 1024.0
    return fpu.div(fpu.div(static_cast<double>(bytes), k), k);
}

void sprites_flush() {
    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_sprite_queue(obj(kSprites))->begin_frame()) != 0) {  // 0x00631d50
        joan::orig::as_sprite_queue(obj(kSprites))->end_frame();                       // 0x00631d80
    } else {
        joan::orig::as_sprite_queue(obj(kSprites))->release_list();  // 0x00631dc0
    }
}
bool flag(std::uintptr_t thunk, img::Addr object) { return thiscall<std::int8_t>(thunk, obj(object)) != 0; }

// The 0x00a39a14 scattering parameter panel: switch object, label, y, float value.
struct ScatterRow {
    img::Addr object, label;
    std::int32_t y;
    img::Addr value;
};
const ScatterRow kScatter[11] = {
    {d::g_a39a18, d::s_Rayleigh, 0x32, d::g_a47f8c},   // 'Rayleigh'
    {d::g_a39a1c, d::s_Mie, 0x46, d::g_a47f90},   // 'Mie'
    {d::g_a39a20, d::s_Inscattering, 0x5a, d::g_a47f94},   // 'Inscattering'
    {d::g_a39a24, d::s_G, 0x6e, d::g_a47f98},   // 'G'
    {d::g_a39a28, d::s_Distance_Modifier, 0x82, d::g_a47f9c},   // 'Distance Modifier'
    {d::g_a39a2c, d::s_Color_R, 0xa0, d::g_a47f7c},   // 'Color R'
    {d::g_a39a30, d::s_Color_G, 0xb4, d::g_a47f80},   // 'Color G'
    {d::g_a39a34, d::s_Color_B, 0xc8, d::g_a47f84},   // 'Color B'
    {d::g_a39a38, d::s_Direction_X, 0xe6, d::g_a47f70},   // 'Direction X'
    {d::g_a39a3c, d::s_Direction_Y, 0xfa, d::g_a47f74},   // 'Direction Y'
    {d::g_a39a40, d::s_Direction_Z, 0x10e, d::g_a47f78},  // 'Direction Z'
};

JOAN_COUNTER(c_draw, "Game::draw");

struct H {
    std::int32_t state() { return static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Wnd>(this)->mode)); }

    // Video overlay (0x004d4ae6..0x004d4d0a).
    static void video() {
        const std::uint32_t now = clock_ms();
        std::int32_t dt = static_cast<std::int32_t>(now - global<std::uint32_t>(d::g_movie_start_ms));
        if (global<std::int32_t>(d::g_movie_start_ms) == 0) {
            dt = 0;
            global<std::uint32_t>(d::g_movie_start_ms) = clock_ms();
        }
        const std::int32_t st = joan::orig::arg::as<std::int32_t>(joan::orig::as_video(obj(kVideo))->update(joan::orig::arg::u32(dt)));  // 0x00685a80
        if (st == 2 || st == 4) {
            const joan::hook::x87::Fpu fpu{true};
            // fild dword; fidiv dword (the low dword of the zero-extended qword: signed).
            const double a = fpu.div(static_cast<double>(global<std::int32_t>(d::g_screen_w)),
                                     static_cast<double>(global<std::int32_t>(d::g_video_width)));
            const double b = fpu.div(static_cast<double>(global<std::int32_t>(d::g_screen_h)),
                                     static_cast<double>(global<std::int32_t>(d::g_video_height)));
            // fcompp st0 = b, st1 = a; test ah,0x41 jne: b <= a or unordered -> b.
            const float scale = static_cast<float>(b > a ? a : b);
            const std::int32_t w = joan::hook::x87::ftol(
                fpu.mul(static_cast<double>(global<std::uint32_t>(d::g_video_width)), static_cast<double>(scale)));
            const std::int32_t h = joan::hook::x87::ftol(
                fpu.mul(static_cast<double>(global<std::uint32_t>(d::g_video_height)), static_cast<double>(scale)));
            const std::int32_t x0 = static_cast<std::int32_t>(global<std::uint32_t>(d::g_screen_w) - static_cast<std::uint32_t>(w)) / 2;
            const std::int32_t y0 = static_cast<std::int32_t>(global<std::uint32_t>(d::g_screen_h) - static_cast<std::uint32_t>(h)) / 2;
            const std::uint32_t bottom = static_cast<std::uint32_t>(y0) + static_cast<std::uint32_t>(h) - 1u;
            const std::uint32_t right = static_cast<std::uint32_t>(x0) + static_cast<std::uint32_t>(w) - 1u;
            const std::uint32_t src_bottom = global<std::uint32_t>(d::g_video_height) - 1u;
            const std::uint32_t src_right = global<std::uint32_t>(d::g_video_width) - 1u;
            const std::uint32_t c = white();
            // The original pushed x / y as `mov cx/dx, word; push ecx/edx` (colour in the upper half); the callee
            // reads the int16 arguments' low words only.
            joan::orig::as_sprite_queue(obj(kSprites))->blit_image(  // 0x006332b0 video blit, ret 0x3c
                joan::orig::arg::i16(x0), joan::orig::arg::i16(y0), 0xau, joan::orig::arg::mptr(global<void*>(d::g_video_texture)), c, 0, 0, 1, 0,
                0, joan::orig::arg::i16(src_right), joan::orig::arg::i16(src_bottom), 0, joan::orig::arg::i16(right), joan::orig::arg::i16(bottom));
            self_virtual<std::int32_t>(jo::self<jo::Texture2D>(global<void*>(d::g_video_texture))->d3d_texture, 9);
        }
        if (st == 4 || st == 3) {
            joan::orig::as_video(obj(kVideo))->close();  // 0x00686160
            global<std::uint32_t>(d::g_movie_start_ms) = 0;
        }
    }

    // Debug overlay for state 9 (0x004d4d31..0x004d5302).
    static void overlay9() {
        label(0x12c, 0x14, str(d::s_Frame_Rate));  // 'Frame Rate'
        float_value(0x190, 0x14, d::g_a51698);
        label(0x1f4, 0x14, str(d::s_Frame_Time));  // 'Frame Time:'
        frame_time(0x258, 0x14);
        label(0x12c, 0x28, str(d::s_Far_clip));  // 'Far clip:'
        float_value(0x190, 0x28, d::g_far_clip);
        label(0x1f4, 0x28, str(d::s_Speed));  // 'Speed:'
        float_value(0x258, 0x28, d::g_game_speed);
        label(0x12c, 0x3c, str(d::s_Init_Video_Memory));  // 'Init Video Memory:'
        {
            const std::uint32_t c = white();
            const char* const s = number(megabytes(global<std::uint32_t>(d::g_a69b28)));
            text(0x1c2, 0x3c, 0xa, s, c);
        }
        label(0x12c, 0x50, str(d::s_Free_Video_Memory));  // 'Free Video Memory:'
        {
            const std::uint32_t free_mem = self_virtual<std::uint32_t>(global<void*>(d::g_game_device), 4);  // GetAvailableTextureMem
            const std::uint32_t c = white();
            const char* const s = number(megabytes(free_mem));
            text(0x1c2, 0x50, 0xa, s, c);
        }
        polygon_count();
    }

    // Debug overlay for state 2 (0x004d531f..0x004d557b).
    static void overlay2() {
        label(0x12c, 0x14, str(d::s_Frame_Rate));  // 'Frame Rate'
        float_value(0x190, 0x14, d::g_a51698);
        label(0x1f4, 0x14, str(d::s_Frame_Time));  // 'Frame Time:'
        frame_time(0x258, 0x14);
        polygon_count();
    }

    static void scatter_panel() {  // 0x004d5595..0x004d5ec5
        for (std::int32_t i = 0; i < 11; ++i) {
            const ScatterRow& r = kScatter[i];
            if (global<std::int32_t>(r.object) != 0) joan::orig::as_widget(global<void*>(r.object))->try_activate(static_cast<char>(joan::orig::arg::i32(7)));  // 0x0062bd30
            text(0xa, r.y, 1, str(r.label), color(0, 0, 0, 0xff));
            const std::uint32_t c = white();
            const char* const s = number(static_cast<double>(global<float>(r.value)));
            text(0xd2, r.y, 1, s, c);
        }
    }

    static bool menu_open() {  // 0x004303f0 / 0x00430370 / 0x006c03e0 / 0x006ed4c0 / 0x006ba890
        return flag(fn::code_value(fn::Byte_get_4303f0), d::g_hud) || flag(fn::code_value(fn::Obj_get_4b05), d::g_save_menu) || flag(fn::code_value(fn::X_byte_30), d::g_barracks_screen) ||
               flag(fn::code_value(fn::X_byte_34), d::g_workshop_screen) || flag(fn::code_value(fn::X_byte_0_6ba890), d::g_armory_screen);
    }

    void draw() {  // 0x004d48f0
        c_draw.hit();
        joan::orig::as_stats(obj(kSysEn))->begin_frame();  // 0x00600df0 begin
        if (static_cast<std::int8_t>(jo::self<jo::Wnd>(this)->u8_151) != 0) {
            joan::orig::as_quit_popup(obj(d::g_quit_popup))->draw();  // 0x006e47b0 QuitPopup::draw
            sprites_flush();
            return;
        }
        joan::orig::as_chat_box(obj(d::g_chat_box))->draw(7u);  // 0x0045b9d0

        // Jump table 0x004d62c3 / 0x004d62cb: states 0, 2, 4..14.
        {
            const std::uint32_t s = static_cast<std::uint32_t>(state());
            if (s <= 14 && s != 1 && s != 3) joan::orig::as_main_menu(obj(d::g_main_menu))->reset_widgets();  // 0x0070d670
        }
        if (jo::view<std::uint8_t>(jo::self<jo::Wnd>(this)->mode) == 9) {
            joan::orig::as_hud(obj(d::g_player_hud))->load_resources();  // 0x006f97d0
        } else {
            joan::orig::as_hud(obj(d::g_player_hud))->free_resources();  // 0x006f98a0
        }
        // Jump table 0x004d62da (state - 1).
        {
            void* const screens = obj(d::g_game_state);
            switch (static_cast<std::uint32_t>(state()) - 1u) {
                case 0: joan::orig::as_screen(screens)->empty_47b7a0(); break;   // 0x0047b7a0
                case 2: joan::orig::as_game(screens)->draw_ending(); break;   // 0x0047b830
                case 3: joan::orig::as_screen(screens)->empty_47b960(); break;   // 0x0047b960
                case 4: joan::orig::as_screen(screens)->empty_47b9e0(); break;   // 0x0047b9e0
                case 5: joan::orig::as_screen(screens)->empty_47ba80(); break;   // 0x0047ba80
                case 6: joan::orig::as_screen(screens)->empty_47bb00(); break;   // 0x0047bb00
                case 7: joan::orig::as_screen(screens)->empty_47bb80(); break;   // 0x0047bb80
                case 8: joan::orig::as_world(obj(d::g_camera_man))->draw(); break;  // 0x0044c800 World::draw
                case 10: joan::orig::as_screen(screens)->update_47bbf0(); break;  // 0x0047bbf0
                case 11: joan::orig::as_game(screens)->draw_loading_screen(); break;  // 0x0047bce0
                case 12: joan::orig::as_screen(screens)->update_47bdd0(); break;  // 0x0047bdd0
                case 13: joan::orig::as_screen(screens)->update_47bef0(); break;  // 0x0047bef0
                case 14: joan::orig::as_game(screens)->draw_credits(); break;  // 0x0047bfd0
                default: break;                                 // 1, 9, > 14
            }
        }
        joan::orig::as_texture_manager(obj(d::g_texture_array))->purge(2);  // 0x0060a870
        joan::orig::as_texture_manager(obj(d::g_texture_array))->trim_loaders(0);  // 0x0060a990
        if (state() == 2 && joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(obj(d::g_view3d))->begin_frame()) != 0) {  // 0x00614630
            joan::orig::as_camera(obj(d::g_view3d))->render();                                         // 0x00614760
        }
        if (state() == 2 || state() == 9) {
            if (global<std::int32_t>(kVideo) != 0) video();
            joan::orig::as_debug_overlay(obj(d::g_debug_overlay))->draw();  // 0x004d69f0
            if (state() == 9 && global<std::int32_t>(d::g_a3a2ac) != 0) overlay9();
            if (state() == 2 && global<std::int32_t>(d::g_a3a2ac) != 0) overlay2();
            if (global<std::int32_t>(d::g_a39a14) != 0 && global<std::int32_t>(d::g_a52623) != 0) scatter_panel();
            if (global<std::int32_t>(d::g_video_playing) == 0) ui();
        }
        joan::orig::as_stats(obj(kSysEn))->end_frame();  // 0x00600e50 end
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_a58f96)) != 0 && state() == 9) {
            global<std::uint8_t>(d::g_a58f96) = 0;
            joan::orig::as_game(obj(d::g_game_state))->play_sound(joan::orig::arg::u32(global<std::uint32_t>(d::g_mission)));  // 0x00472d10
        }
    }

    // 0x004d5ed2..0x004d6283.
    void ui() {
        if (!menu_open()) {
            for (std::int8_t i = 1; i <= 4; i = static_cast<std::int8_t>(i << 1)) {
                joan::orig::as_hud(obj(d::g_player_hud))->draw_corner_icon(joan::orig::arg::u32(i));     // 0x006f9970 Hud::draw_corner_icon
                joan::orig::as_hud(obj(d::g_player_hud))->draw(static_cast<char>(joan::orig::arg::i32(i)));     // 0x006f9b60 Hud::draw
                joan::orig::as_hud(obj(d::g_player_hud))->draw_pause_caption(static_cast<char>(joan::orig::arg::i32(i)));     // 0x006f9a60
                joan::orig::as_panel(obj(d::g_popup_a50808))->draw_frame(1u, static_cast<std::uint8_t>(joan::orig::arg::u32(i)));  // 0x0062a9a0 (reads the byte only)
            }
        }
        const std::int8_t four = 4;
        joan::orig::as_sms(obj(d::g_sms))->draw(1u);  // 0x006e5140 PickupNotice::draw
        joan::orig::as_panel(obj(d::g_quick_save_screen))->refresh_buttons();     // 0x006e39a0
        if (global<std::uint8_t>(d::g_game_running) == 0) joan::orig::as_weather_system(obj(d::g_weather_system))->flush_lightning();  // 0x0068c6c0
        joan::orig::as_inventory(obj(d::g_a56398))->draw();        // 0x006f6280 Inventory::draw
        joan::orig::as_glory_screen(obj(d::g_glory_screen))->draw(static_cast<std::uint8_t>(joan::orig::arg::u32(four)));  // 0x006e6dc0
        joan::orig::as_victory_screen(obj(d::g_victory_screen))->tick();        // 0x006e8c10
        joan::orig::as_letter(obj(d::g_letter))->draw();        // 0x006e05a0
        joan::orig::as_armory_screen(obj(d::g_armory_screen))->draw_buttons();        // 0x006bbac0
        if (!flag(fn::code_value(fn::Obj_get_4b05), d::g_save_menu) && !flag(fn::code_value(fn::Byte_get_4303f0), d::g_hud) && !flag(fn::code_value(fn::X_byte_30), d::g_barracks_screen) &&
            !flag(fn::code_value(fn::X_byte_34), d::g_workshop_screen) && !flag(fn::code_value(fn::X_byte_0_6ba890), d::g_armory_screen)) {
            if (flag(fn::code_value(fn::Obj_byte0_4af5d0), d::g_message_indicator)) joan::orig::as_messages(obj(d::g_message_indicator))->tick();  // 0x004af5d0 / 0x004af530
            if (flag(fn::code_value(fn::X_byte_3f), d::g_names)) joan::orig::as_messages(obj(d::g_names))->update();  // 0x004d6a10 / 0x004ae390 messages
            if (flag(fn::code_value(fn::Obj_flag_8_or_9), d::g_goal_panel)) joan::orig::as_goal_panel(obj(d::g_goal_panel))->draw();  // 0x006e0380 / 0x006df180
            if (flag(fn::code_value(fn::Obj_byte8_6d34b0), d::g_talk_prompt)) joan::orig::as_talk_prompt(obj(d::g_talk_prompt))->draw();  // 0x006d34b0 / 0x006d23b0 TalkPrompt::draw
        }
        joan::orig::as_game(obj(d::g_player_unit_recno))->update_mode();        // 0x0042f620
        joan::orig::as_cutscene(obj(d::g_game_a5d380))->draw_bars();        // 0x006ca230
        joan::orig::as_fade_title(obj(d::g_fade_title))->draw();        // 0x00761340
        joan::orig::as_opt_menu(obj(d::g_opt_menu))->draw(0u);     // 0x0073d2c0 OptionMenu::draw
        joan::orig::as_msg_log(obj(d::g_msg_log))->draw();        // 0x006e1130 MessageLog::draw
        joan::orig::as_obj_menu(obj(d::g_objective_menu))->draw(1, 0);  // 0x0071f690 ObjectiveMenu::draw
        joan::orig::as_ingame_menu(obj(d::g_ingame_menu))->draw(0u);     // 0x006f3af0 IngameMenu::draw
        joan::orig::as_save_menu(obj(d::g_save_menu))->draw_74e170(0u);     // 0x0074e170 SaveMenu::draw
        joan::orig::as_game_over(obj(d::g_game_over_screen))->draw(joan::orig::arg::u32(four));  // 0x006b7160
        joan::orig::as_roster_screen(obj(d::g_roster_screen))->draw();        // 0x006e60f0
        joan::orig::as_shop(obj(d::g_buy))->draw();        // 0x006c4070 Shop::draw
        joan::orig::as_barracks_screen(obj(d::g_barracks_screen))->draw();        // 0x006bdea0 unit labels
        joan::orig::as_workshop_screen(obj(d::g_workshop_screen))->draw();        // 0x006ea310 unit labels
        if (state() == 9) joan::orig::as_dialogue(obj(d::g_dialogue))->draw_63c8a0(static_cast<std::uint8_t>(joan::orig::arg::u32(7)));  // 0x0063c8a0 dialogue
        sprites_flush();
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_a4fca8)) != 0) {
            std::uint8_t camera_state[0x28];  // out parameter only (rep movsd 10 dwords), never read
            joan::orig::as_camera(global<void*>(d::g_a4fdcc))->frame(joan::orig::arg::mptr(static_cast<void*>(camera_state)));  // 0x006140a0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(global<void*>(d::g_a4fdcc))->begin_frame()) != 0) {                  // 0x00614630
                joan::orig::as_camera(global<void*>(d::g_a4fdcc))->render();                                        // 0x00614760
            }
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(global<void*>(d::g_a4feb5))->begin_frame()) != 0) {
                joan::orig::as_camera(global<void*>(d::g_a4feb5))->render();
            }
        }
        if (flag(fn::code_value(fn::Byte_get_4303d0), d::g_objective_menu)) {  // 0x004303d0
            std::uint8_t camera_state[0x28];
            joan::orig::as_camera(global<void*>(d::g_a53e05))->frame(joan::orig::arg::mptr(static_cast<void*>(camera_state)));  // 0x006140a0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(global<void*>(d::g_a53e05))->begin_frame()) != 0) {                  // 0x00614630
                joan::orig::as_camera(global<void*>(d::g_a53e05))->render();                                        // 0x00614760
            }
        }
        joan::orig::as_obj_menu(obj(d::g_objective_menu))->draw_camera();        // 0x00720bc0
        joan::orig::as_obj_menu(obj(d::g_objective_menu))->draw(2, 0);  // 0x0071f690
        joan::orig::as_obj_menu(obj(d::g_objective_menu))->draw_army_title();        // 0x00727740
        joan::orig::as_shop(obj(d::g_buy))->draw_portrait();        // 0x006c3690 Shop::draw_portrait
        joan::orig::as_sms(obj(d::g_sms))->draw(2u);     // 0x006e5140
        joan::orig::as_armory_screen(obj(d::g_armory_screen))->draw_camera();        // 0x006bbc50
        joan::orig::as_quit_popup(obj(d::g_quit_popup))->draw();        // 0x006e47b0 QuitPopup::draw
        joan::orig::as_ending(obj(d::g_ending_screen))->draw();        // 0x006ce450
        sprites_flush();
    }
};

JOAN_REPLACE("game/draw1", "Game::draw", 0x004d48f0, &H::draw);

}  // namespace game_draw1
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

struct H {
    void empty() {}  // 0x004eb550, 0x004d69f0, 0x0047b7a0 ...

    const char* format_int(std::uint32_t a, std::uint32_t b) { return joan::orig::as_misc(this)->format_int_58fca0(joan::orig::arg::i32(a), joan::orig::arg::i32(b)); }  // 0x0058fca0
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Misc::format_int", 0x004d69a0, &H::format_int);
L82("empty_004d69f0", 0x004d69f0, &H::empty);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

struct H {
    // ------------------------------------------------------------------------ 0x004d6a10
    u32 byte_3f() {
        return (joan::call::register_low_bits(this) & 0xffffff00u) | jo::self<jo::MessageList>(this)->flag_3f;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("X::byte_3f", 0x004d6a10, &H::byte_3f);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_pi_a3a294() -> void { joan::bind::free<&::game_b98_03::init_pi_a3a294>(); }  // 0x4d2ac0
auto joan::orig::ModelResMgr::update_anim(std::int16_t a0) -> void { joan::bind::member<&::game_b83_06::H::anim_update>(this, a0); }  // 0x4d2b10
auto joan::orig::init_a3a2a0() -> void { joan::bind::free<&::game_b99_04::init_a3a2a0>(); }  // 0x4d4310
auto joan::orig::Game::draw() -> void { joan::bind::member<&::game_draw1::H::draw>(this); }  // 0x4d48f0
auto joan::orig::Unit::find_target(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_04::H::find_target>(this, a0)); }  // 0x4d6320
auto joan::orig::Misc::format_int(std::uint32_t a0, std::uint32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_leaves82b::H::format_int>(this, a0, a1)); }  // 0x4d69a0
auto joan::orig::DebugOverlay::draw() -> void { joan::bind::member<&::game_leaves82b::H::empty>(this); }  // 0x4d69f0
