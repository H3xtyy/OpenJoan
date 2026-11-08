// game/unknown_vtrail.cpp: functions whose original source file is unknown (formerly game/_unplaced/vtrail.cpp;
// re/attribution.csv: between ww/uti3d/videocap.cpp and ww/enphysic/collimgr.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
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

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::img::obj;

using i16 = std::int16_t;

using joan::img::str;

using joan::hook::global;

using joan::call::import_call;

void* const kObjects = obj(d::g_items);

using joan::hook::x87::Fpu;

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b83_04.cpp (game/b83_04)
namespace {
namespace game_b83_04 {

JOAN_COUNTER(c_task_init, "UnitTask::init");

struct H {

    // 0x006871f0 (ret 4)
    void task_init(std::uint8_t* unit) {
        c_task_init.hit();
        jo::self<jo::Trail>(this)->unit_id = static_cast<std::int32_t>(jo::as_map_object(unit)->index);
        jo::self<jo::Trail>(this)->scene = jo::as_actor(jo::as_map_object(unit)->actor)->scene;
        jo::self<jo::Trail>(this)->point_count = 0;
        jo::self<jo::Trail>(this)->last_add = 0;
        jo::self<jo::Trail>(this)->node_count = 0;
        jo::self<jo::Trail>(this)->hold_ms = 0x514;
        jo::self<jo::Trail>(this)->rate_hz = 0x3c;
        jo::self<jo::Trail>(this)->max_nodes = 0x1f4;
        jo::self<jo::Trail>(this)->attached = 0;
        jo::self<jo::Trail>(this)->enabled = 1;
        std::uint8_t* const type = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_map_object(unit)->type_id)));  // 0x00425ce0
        jo::self<jo::Trail>(this)->point_count = jo::as_item_def(type)->blur_vertex_count;
        if (static_cast<std::int32_t>(jo::self<jo::Trail>(this)->point_count) == 0) return;
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::Trail>(this)->point_count); ++i)
            std::memcpy(&jo::self<jo::Trail>(this)->points[i * 6], &jo::as_item_def(type)->blur_vertices[i * 3], 12);
        if (joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(static_cast<const char*>(jo::as_item_def(type)->blur_name)))) != 0) {  // strlen
            void* const named = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(static_cast<const char*>(jo::as_item_def(type)->blur_name)), 1, 0u));  // 0x006099c0
            joan::orig::as_trail(this)->build_687cc0(joan::orig::arg::mptr(named));  // 0x00687cc0
            joan::orig::texture_ref::release(joan::orig::arg::mptr(named));      // 0x0042cef0
            std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Trail>(this)->unit_id)));  // 0x00425d20
            joan::orig::as_scene(jo::view<void*>(jo::as_actor(jo::as_map_object(u)->actor)->scene))->attach(joan::orig::arg::mptr(jo::self<jo::Trail>(this)->actor));          // 0x005efc50
            jo::self<jo::Trail>(this)->scene = jo::as_actor(jo::as_map_object(u)->actor)->scene;
            return;
        }
        joan::orig::as_trail(this)->build_687cc0(joan::orig::arg::mptr(static_cast<void*>(nullptr)));  // 0x00687cc0
    }

    // 0x00687130
    H* ctor_687130() {
        joan::orig::as_trail_nodes(&jo::self<jo::Trail>(this)->nodes)->construct();  // 0x00689370
        std::memset(static_cast<void*>(this), 0, sizeof(jo::Trail));  // memset (0xaf on x86)
        return this;
    }
};

#define B83_04(name, addr, fn) JOAN_REPLACE("game/b83_04", name, addr, fn)
B83_04("UnitTask::init", 0x006871f0, &H::task_init);
B83_04("Obj687130::constructor", 0x00687130, &H::ctor_687130);
#undef B83_04

}  // namespace game_b83_04
}  // namespace

// ---------------------------------------- from modules/game_leaves82a.cpp (game/leaves82a)
namespace {
namespace game_leaves82a {

struct H {
    // 0x00687180 destructor: body 0x00687410, then the member at +0xa3 (0x006893a0).
    void player_dtor() {
        joan::orig::as_trail(this)->release();                                     // 0x00687410
        joan::orig::as_list(&jo::self<jo::Trail>(this)->nodes)->dtor_6893a0();  // 0x006893a0
    }

    // 0x00689400: when the count +8 is non-zero, deletes the nodes from +0 along +0x80
    // (0x00689330, delete 0x006894a0), decrementing +8 per node.
    void list_clear() {
        if (jo::view<std::uint32_t>(jo::self<jo::TrailNodes>(this)->count_) == 0) return;
        std::uint8_t* node = jo::view<std::uint8_t*>(jo::self<jo::TrailNodes>(this)->newest);
        while (node != nullptr) {
            std::uint8_t* const current = node;
            node = jo::view<std::uint8_t*>(jo::as_trail_node(current)->older);
            joan::orig::as_trail_node(current)->unlink();  // 0x00689330
            if (current != nullptr) joan::orig::as_trail_node(current)->destroy(1u);  // 0x006894a0
            jo::self<jo::TrailNodes>(this)->count_ = jo::self<jo::TrailNodes>(this)->count_ - 1;
        }
    }
};

#define LEAVES82A(name, addr, fn) JOAN_REPLACE("game/leaves82a", name, addr, fn)
LEAVES82A("Player::~Player", 0x00687180, &H::player_dtor);
LEAVES82A("List::clear", 0x00689400, &H::list_clear);
#undef LEAVES82A

}  // namespace game_leaves82a
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
namespace {
namespace game_leaves81 {

JOAN_COUNTER(c_source_release, "SoundSource::release");

using joan::orig::helpers::node_remove;  // 0x005efdb0

struct H {

    // 0x00687410 SoundSource::release
    void source_release() {
        c_source_release.hit();
        if (jo::view<void*>(jo::self<jo::Trail>(this)->actor) != nullptr) {
            node_remove(jo::view<void*>(jo::self<jo::Trail>(this)->scene), jo::view<void*>(jo::self<jo::Trail>(this)->actor));
            void* const p = jo::view<void*>(jo::self<jo::Trail>(this)->actor);
            if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840 delete
            jo::view<void*>(jo::self<jo::Trail>(this)->actor) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Trail>(this)->actor_def) != nullptr) {
            joan::orig::as_actor_def(jo::view<void*>(jo::self<jo::Trail>(this)->actor_def))->release();  // 0x005b03d0
            void* const p = jo::view<void*>(jo::self<jo::Trail>(this)->actor_def);
            if (p != nullptr) joan::orig::as_actor_def(p)->destroy(1u);  // 0x004b0c20 delete
            jo::view<void*>(jo::self<jo::Trail>(this)->actor_def) = nullptr;
        }
        joan::orig::as_list(&jo::self<jo::Trail>(this)->nodes)->clear();  // 0x00689400
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("SoundSource::release", 0x00687410, &H::source_release);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_b89_04.cpp (game/b89_04)
namespace {
namespace game_b89_04 {

std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

JOAN_COUNTER(c_deform, "MeshDeform::update");

struct H {
    // ------------------------------------------------------------------------ 0x00687500
    void deform_update(std::int32_t duration) {
        if (sx16(jo::self<jo::Trail>(this)->point_count) == 0) return;
        c_deform.hit();
        if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->unit_id))) != 0) {  // 0x00425d50
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::Trail>(this)->scene))->detach(joan::orig::arg::mptr(jo::self<jo::Trail>(this)->actor));  // 0x005efdb0
            return;
        }
        joan::orig::as_list(&jo::self<jo::Trail>(this)->nodes)->clear();  // 0x00689400
        jo::self<jo::Trail>(this)->hold_start = global<std::uint32_t>(d::g_now);
        if (duration == 0)
            jo::view<std::int32_t>(jo::self<jo::Trail>(this)->hold_ms) = 0x514;
        else
            jo::view<std::int32_t>(jo::self<jo::Trail>(this)->hold_ms) = duration;
        std::uint8_t* const owner =
            joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->unit_id))));  // 0x00425d20, ebp-0x8
        void* frame = nullptr;  // ebp-0xc (always written by 0x005ad060)
        void* unused = nullptr;  // ebp-0x10
        joan::orig::as_actor(jo::view<void*>(jo::as_map_object(owner)->actor))->select_mesh(joan::orig::arg::mptr(global<void*>(d::g_camera)), std::int32_t{1}, joan::orig::arg::mptr(static_cast<void*>(&frame)), joan::orig::arg::mptr(static_cast<void*>(&unused)));  // 0x005ad060
        void* const node = joan::orig::arg::as<void*>(joan::orig::as_plane(frame)->bone_5cf8d0());  // 0x005cf8d0
        std::uint8_t* matrix = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bone(node)->matrix());  // 0x0060ef80, ebp-0x14
        if (jo::as_map_object(owner)->actor->anim != nullptr)
            matrix = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bone(jo::view<void*>(jo::as_map_object(owner)->actor->bone))->matrix());
        std::uint32_t pos[3];  // ebp-0x20
        pos[0] = jo::view<std::uint32_t>(jo::self<jo::Matrix44>(matrix)->m[12]);
        pos[1] = jo::view<std::uint32_t>(jo::self<jo::Matrix44>(matrix)->m[13]);
        pos[2] = jo::view<std::uint32_t>(jo::self<jo::Matrix44>(matrix)->m[14]);
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::Trail>(this)->actor))->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(pos)));  // 0x005ac310
        jo::self<jo::Trail>(this)->node_count = 0;
        std::uint32_t xf[6 * 6];  // ebp-0x98 (see header): 6 records of 6 dwords, the transformed point first
        std::memset(xf, 0, sizeof xf);
        for (std::int32_t i = 0; i < sx16(jo::self<jo::Trail>(this)->point_count); ++i)
            import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&xf[i * 6]), static_cast<void*>(&jo::self<jo::Trail>(this)->points[i * 6]),
                               static_cast<void*>(matrix));  // point_transform(out, in, m)
        for (std::int32_t k = 0; k < sx16(jo::self<jo::Trail>(this)->max_nodes) + 1; ++k) {
            for (std::int32_t i = 0; i < sx16(jo::self<jo::Trail>(this)->point_count); ++i) {
                const std::uint32_t* const src = &xf[i * 6];
                const std::uint32_t index = jo::self<jo::Trail>(this)->vtx_base +
                                            static_cast<std::uint32_t>(sx16(jo::self<jo::Trail>(this)->point_count)) *
                                                static_cast<std::uint32_t>(k) +
                                            static_cast<std::uint32_t>(i);
                std::uint8_t* const dst = jo::view<std::uint8_t*>(jo::self<jo::Trail>(this)->actor->plane->vertices_f4) +
                                          index * 0x18u;
                jo::view<std::uint32_t>(jo::as_vertex_f4(dst)->pos[0]) = src[0];
                jo::view<std::uint32_t>(jo::as_vertex_f4(dst)->pos[1]) = src[1];
                jo::view<std::uint32_t>(jo::as_vertex_f4(dst)->pos[2]) = src[2];
            }
        }
        joan::orig::as_plane(jo::view<void*>(jo::self<jo::Trail>(this)->actor->plane))->invalidate(joan::orig::arg::u32(std::int32_t{2}));  // 0x005cf920
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::Trail>(this)->scene))->detach(joan::orig::arg::mptr(jo::self<jo::Trail>(this)->actor));       // 0x005efdb0
        joan::orig::as_scene(jo::view<void*>(jo::as_map_object(owner)->actor->scene))->attach(joan::orig::arg::mptr(jo::self<jo::Trail>(this)->actor));  // 0x005efc50
        jo::self<jo::Trail>(this)->scene = jo::as_map_object(owner)->actor->scene;
        jo::self<jo::Trail>(this)->attached = 1;
    }
};

#define B89(name, addr, fn) JOAN_REPLACE("game/b89_04", name, addr, fn)
B89("MeshDeform::update", 0x00687500, &H::deform_update);
#undef B89

}  // namespace game_b89_04
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx3)
namespace {
namespace game_fx {

using joan::hook::x87::ftol;

struct V3 {
    float x, y, z;
};
V3 load3(const void* p) {
    V3 v;
    std::memcpy(&v, p, sizeof v);
    return v;
}
void store3(void* p, const V3& v) { std::memcpy(p, &v, sizeof v); }

std::uint32_t now() { return global<std::uint32_t>(d::g_now); }  // game clock (ms)
void* a48ccc() { return global<void*>(d::g_camera); }

// x87 comparisons (fcomp + fnstsw): C0 = below or unordered, C3 = equal or unordered.
bool below_or_nan(float a, float b) { return !(a >= b); }

constexpr fn::Code kPlaneBone = fn::Plane_bone_5cf8d0;   // 0x005cf8d0
constexpr fn::Code kNodeRemove = fn::SceneSector_remove_actor;  // 0x005efdb0
constexpr fn::Code kNodeAdd = fn::SceneSector_add_actor;     // 0x005efc50

constexpr img::Addr kUnits(d::g_items);

JOAN_COUNTER(c_trail, "Trail::update");
JOAN_COUNTER(c_trail_add, "Trail::add_node");
JOAN_COUNTER(c_trail_age, "Trail::age");
JOAN_COUNTER(c_trail_build, "Trail::build");
JOAN_COUNTER(c_hermite, "Trail::hermite");

struct H {

    void trail_update() {  // 0x00687880
        if (jo::self<jo::Trail>(this)->attached == 0) return;
        c_trail.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_units(img::obj(kUnits))->empty_at(joan::orig::arg::i32(jo::self<jo::Trail>(this)->unit_id))) != 0) {  // 0x00425d50
            thiscall(kNodeRemove, jo::view<void*>(jo::self<jo::Trail>(this)->scene), jo::view<void*>(jo::self<jo::Trail>(this)->actor));
            jo::self<jo::Trail>(this)->attached = 0;
            return;
        }
        const std::uint32_t t = now();
        void* const unit = joan::orig::arg::as<void*>(joan::orig::as_units(img::obj(kUnits))->get_425d20(joan::orig::arg::i32(jo::self<jo::Trail>(this)->unit_id)));  // 0x00425d20
        if (jo::self<jo::Trail>(this)->hold_start + jo::self<jo::Trail>(this)->hold_ms < t) joan::orig::as_trail(this)->release_hold();  // 0x00687850
        joan::orig::as_trail(this)->age(joan::orig::arg::i16(static_cast<std::int16_t>(0x96)));                         // 0x00688550
        if (jo::self<jo::Trail>(this)->last_add + static_cast<std::uint32_t>(1000 / static_cast<std::int32_t>(jo::self<jo::Trail>(this)->rate_hz)) < t) {
            if (jo::self<jo::Trail>(this)->hold_start == 0 && joan::orig::arg::as<std::int32_t>(joan::orig::as_trail_nodes(&jo::self<jo::Trail>(this)->nodes)->count()) > 2) {  // 0x00689af0
                joan::orig::as_trail(this)->age(joan::orig::arg::i16(static_cast<std::int16_t>(1)));
                joan::orig::as_trail(this)->add_node();  // 0x006883d0
                jo::self<jo::Trail>(this)->last_add = t;
            } else if (jo::self<jo::Trail>(this)->hold_start != 0) {
                joan::orig::as_trail(this)->add_node();
                jo::self<jo::Trail>(this)->last_add = t;
            } else {
                joan::orig::as_trail(this)->age(joan::orig::arg::i16(static_cast<std::int16_t>(1)));
            }
        }
        if (jo::self<jo::Trail>(this)->hold_start == 0 && jo::self<jo::Trail>(this)->node_count == 0) {
            thiscall(kNodeRemove, jo::view<void*>(jo::as_actor(jo::as_map_object(unit)->actor)->scene), jo::view<void*>(jo::self<jo::Trail>(this)->actor));
            jo::self<jo::Trail>(this)->attached = 0;
        }
        if (jo::self<jo::Trail>(this)->dirty != 0) joan::orig::as_trail(this)->build_6889c0();  // 0x006889c0
    }
    void trail_release_hold() { jo::self<jo::Trail>(this)->hold_start = 0; }  // 0x00687850
    std::int32_t nodes_count() { return jo::self<jo::TrailNodes>(this)->count_; }  // 0x00689af0

    // 0x006883d0 Trail::add_node: the points in world space from the unit's current pose.
    void trail_add_node() {
        c_trail_add.hit();
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_units(img::obj(kUnits))->empty_at(joan::orig::arg::i32(jo::self<jo::Trail>(this)->unit_id))) != 0) {
            thiscall(kNodeRemove, jo::view<void*>(jo::self<jo::Trail>(this)->scene), jo::view<void*>(jo::self<jo::Trail>(this)->actor));
            return;
        }
        if (jo::self<jo::Trail>(this)->node_count >= jo::self<jo::Trail>(this)->max_nodes) joan::orig::as_trail(this)->age(joan::orig::arg::i16(static_cast<std::int16_t>(1)));
        void* const unit = joan::orig::arg::as<void*>(joan::orig::as_units(img::obj(kUnits))->get_425d20(joan::orig::arg::i32(jo::self<jo::Trail>(this)->unit_id)));
        void* plane = nullptr;
        void* unused = nullptr;
        joan::orig::as_actor(jo::view<void*>(jo::as_map_object(unit)->actor))->select_mesh(joan::orig::arg::mptr(a48ccc()), 1, &plane, &unused);  // 0x005ad060
        void* m = joan::orig::arg::as<void*>(joan::orig::as_bone(thiscall<void*>(kPlaneBone, plane))->matrix());
        if (jo::as_actor(jo::as_map_object(unit)->actor)->anim != nullptr) m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::as_map_object(unit)->actor)->bone))->matrix());
        float points[6 * 3] = {};
        for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i)
            import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&points[i * 3]), static_cast<void*>(&jo::self<jo::Trail>(this)->points[i * 6]), m);  // point_transform
        joan::orig::as_trail_nodes(&jo::self<jo::Trail>(this)->nodes)->push(joan::orig::arg::mptr(static_cast<void*>(points)));  // 0x006895c0
        jo::self<jo::Trail>(this)->dirty = 1;
    }

    // 0x00688550 Trail::age(ms): drop nodes older than now - ms; the oldest kept node is
    // pulled toward the next one so the ribbon end moves smoothly, tangents recomputed.
    void trail_age(std::int16_t ms) {
        c_trail_age.hit();
        jo::TrailNodes* const list = &jo::self<jo::Trail>(this)->nodes;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_trail_nodes(list)->count()) == 0) return;
        if (jo::self<jo::Trail>(this)->nodes.oldest->time >= now() - static_cast<std::uint32_t>(static_cast<std::int32_t>(ms))) return;
        jo::self<jo::Trail>(this)->dirty = 1;
        if (jo::view<void*>(jo::self<jo::Trail>(this)->nodes.oldest->newer) == nullptr) {
            if (jo::self<jo::Trail>(this)->nodes.oldest->time < now() - static_cast<std::uint32_t>(static_cast<std::int32_t>(ms)))
                joan::orig::as_trail_nodes(list)->pop_oldest();  // 0x006894f0
            return;
        }
        while (jo::self<jo::Trail>(this)->nodes.oldest->newer->time < now() - static_cast<std::uint32_t>(static_cast<std::int32_t>(ms))) {
            joan::orig::as_trail_nodes(list)->pop_oldest();
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_trail_nodes(list)->count()) <= 1) return;
        }
        const std::uint32_t span = jo::self<jo::Trail>(this)->nodes.oldest->newer->time - jo::self<jo::Trail>(this)->nodes.oldest->time;
        const std::uint32_t left = jo::self<jo::Trail>(this)->nodes.oldest->newer->time +
                                   static_cast<std::uint32_t>(static_cast<std::int32_t>(ms)) - now();
        if (span == 0 || left == 0) {
            joan::orig::as_trail_nodes(list)->pop_oldest();
            return;
        }
        const joan::hook::x87::Fpu fpu{true};
        const float t = static_cast<float>(fpu.div(static_cast<double>(left), static_cast<double>(static_cast<std::int32_t>(span))));
        for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i) {
            std::uint8_t* const tail = jo::view<std::uint8_t*>(jo::self<jo::Trail>(this)->nodes.oldest);
            const V3 cur = load3(&jo::as_trail_node(tail)->points[i * 3]);
            const V3 next = load3(&jo::as_trail_node(jo::view<std::uint8_t*>(jo::as_trail_node(tail)->newer))->points[i * 3]);
            joan::orig::as_trail_nodes(list)->lerp(joan::orig::arg::mptr(static_cast<void*>(&jo::as_trail_node(tail)->points[i * 3])), next, cur, joan::orig::arg::f32(t));  // 0x00689a20
        }
        std::uint8_t* b = jo::view<std::uint8_t*>(jo::as_trail_node(jo::self<jo::Trail>(this)->nodes.oldest)->newer);
        std::uint8_t* a = jo::view<std::uint8_t*>(jo::as_trail_node(b)->newer) != nullptr ? jo::view<std::uint8_t*>(jo::as_trail_node(b)->newer) : b;
        std::uint8_t* c = jo::view<std::uint8_t*>(jo::self<jo::Trail>(this)->nodes.oldest);
        for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i)
            joan::orig::as_trail_nodes(list)->tangent(joan::orig::arg::mptr(static_cast<void*>(&jo::as_trail_node(b)->tangents[i * 3])), load3(&jo::as_trail_node(a)->points[i * 3]), load3(&jo::as_trail_node(b)->points[i * 3]), load3(&jo::as_trail_node(c)->points[i * 3]), 0.0f);  // 0x006898d0
        b = jo::view<std::uint8_t*>(jo::self<jo::Trail>(this)->nodes.oldest);
        a = jo::view<std::uint8_t*>(jo::as_trail_node(b)->newer);
        c = b;
        for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i)
            joan::orig::as_trail_nodes(list)->tangent(joan::orig::arg::mptr(static_cast<void*>(&jo::as_trail_node(b)->tangents[i * 3])), load3(&jo::as_trail_node(a)->points[i * 3]), load3(&jo::as_trail_node(b)->points[i * 3]), load3(&jo::as_trail_node(c)->points[i * 3]), 0.0f);
    }

    // 0x006889c0 Trail::build: ribbon vertices (0x18 bytes, plane (+0)->+0x90, vertex array
    // +0x28) from the nodes, newest first; with +0x15 Hermite-interpolated extra rows every
    // 2 units; the unused rows repeat the last one; v = row / rows.
    void trail_build() {
        c_trail_build.hit();
        jo::self<jo::Trail>(this)->node_count = 0;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_trail_nodes(&jo::self<jo::Trail>(this)->nodes)->count()) == 0) return;
        auto vertex = [this](std::int32_t row, std::int32_t i) -> std::uint8_t* {
            const std::int32_t cnt = jo::self<jo::Trail>(this)->point_count;
            return jo::view<std::uint8_t*>(jo::self<jo::Trail>(this)->actor->plane->vertices_f4) + (jo::view<std::int32_t>(jo::self<jo::Trail>(this)->vtx_base) + cnt * row + i) * 0x18;
        };
        for (std::uint8_t* node = jo::view<std::uint8_t*>(jo::self<jo::Trail>(this)->nodes.newest); node != nullptr; node = jo::view<std::uint8_t*>(jo::as_trail_node(node)->older)) {
            for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i) std::memcpy(vertex(jo::self<jo::Trail>(this)->node_count, i), &jo::as_trail_node(node)->points[i * 3], 12);
            jo::self<jo::Trail>(this)->node_count = static_cast<std::int16_t>(jo::self<jo::Trail>(this)->node_count + 1);
            if (jo::self<jo::Trail>(this)->enabled != 0 && jo::view<void*>(jo::as_trail_node(node)->older) != nullptr) {
                float widest = 0.0f;
                for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i) {
                    V3 d;
                    joan::orig::vec_sub(static_cast<void*>(&d), static_cast<void*>(&jo::as_trail_node(node)->points[i * 3]), static_cast<void*>(&jo::as_trail_node(jo::view<std::uint8_t*>(jo::as_trail_node(node)->older))->points[i * 3]));  // 0x0060cc90
                    const float len = v3d_float(d::iat_v3d_87_length, &d);  // length
                    if (len > widest) widest = len;
                }
                if (!(widest > 2.0f)) continue;
                const std::int32_t steps = joan::hook::x87::ftol(widest / 2.0f) - 1;
                if (steps <= 0) continue;
                const float step = 1.0f / static_cast<float>(steps);
                for (float t = step; below_or_nan(t, 1.0f); t = t + step) {
                    std::uint8_t* const older = jo::view<std::uint8_t*>(jo::as_trail_node(node)->older);
                    for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i)
                        joan::orig::as_trail(this)->hermite(joan::orig::arg::mptr(static_cast<void*>(vertex(jo::self<jo::Trail>(this)->node_count, i))), load3(&jo::as_trail_node(node)->points[i * 3]), load3(&jo::as_trail_node(node)->tangents[i * 3]), load3(&jo::as_trail_node(older)->points[i * 3]), load3(&jo::as_trail_node(older)->tangents[i * 3]), joan::orig::arg::f32(t));  // 0x00688fd0
                    jo::self<jo::Trail>(this)->node_count = static_cast<std::int16_t>(jo::self<jo::Trail>(this)->node_count + 1);
                    if (jo::self<jo::Trail>(this)->node_count >= jo::self<jo::Trail>(this)->max_nodes) break;
                }
            }
            if (jo::self<jo::Trail>(this)->node_count >= jo::self<jo::Trail>(this)->max_nodes) break;
        }
        if (jo::self<jo::Trail>(this)->node_count == 0) return;
        for (std::int32_t row = jo::self<jo::Trail>(this)->node_count; row <= jo::self<jo::Trail>(this)->max_nodes; ++row)
            for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i)
                std::memcpy(vertex(row, i), vertex(jo::self<jo::Trail>(this)->node_count - 1, i), 12);
        for (std::int32_t row = 0; row < jo::self<jo::Trail>(this)->node_count; ++row)
            for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i)
                jo::as_vertex_f4(vertex(row, i))->attr[1] = static_cast<float>(row) / static_cast<float>(jo::self<jo::Trail>(this)->node_count);
        void* const plane = jo::view<void*>(jo::self<jo::Trail>(this)->actor->plane);
        joan::orig::as_plane(plane)->invalidate(2u);  // 0x005cf920 Plane::invalidate
        joan::orig::as_plane(plane)->upload();     // 0x005cfa10 Plane::upload
        jo::self<jo::Trail>(this)->dirty = 0;
    }

    // 0x00688fd0 Hermite(out, p0, m0, p1, m1, t).
    void* hermite(void* out, V3 p0, V3 m0, V3 p1, V3 m1, float t) {
        c_hermite.hit();
        const float s = t * t;
        const float c = s * t;
        const float k_p1 = -2.0f * c + 3.0f * s;
        const float k_m1 = c - s;
        const float k_m0 = (c - 2.0f * s) + t;
        const float k_p0 = (2.0f * c - 3.0f * s) + 1.0f;
        const V3 a{k_p1 * p1.x, k_p1 * p1.y, k_p1 * p1.z};
        const V3 b{k_m1 * m1.x, k_m1 * m1.y, k_m1 * m1.z};
        const V3 d{k_m0 * m0.x, k_m0 * m0.y, k_m0 * m0.z};
        const V3 e{k_p0 * p0.x, k_p0 * p0.y, k_p0 * p0.z};
        V3 r{e.x + d.x, e.y + d.y, e.z + d.z};
        r = V3{r.x + b.x, r.y + b.y, r.z + b.z};
        r = V3{r.x + a.x, r.y + a.y, r.z + a.z};
        store3(out, r);
        return out;
    }
    // 0x006898d0 tangent(out, p0, p1, p2, t) = (1 + t) / 2 * (p1 - p0) + (1 - t) / 2 * (p2 - p1).
    void* tangent(void* out, V3 p0, V3 p1, V3 p2, float t) {
        const V3 d21{p2.x - p1.x, p2.y - p1.y, p2.z - p1.z};
        const float ka = (1.0f - t) / 2.0f;
        const V3 a{ka * d21.x, ka * d21.y, ka * d21.z};
        const V3 d10{p1.x - p0.x, p1.y - p0.y, p1.z - p0.z};
        const float kb = (1.0f + t) / 2.0f;
        const V3 b{kb * d10.x, kb * d10.y, kb * d10.z};
        store3(out, V3{b.x + a.x, b.y + a.y, b.z + a.z});
        return out;
    }
    // 0x00689a20 lerp(out, p0, p1, t) = p0 + t * (p1 - p0).
    void* lerp(void* out, V3 p0, V3 p1, float t) {
        const V3 d{p1.x - p0.x, p1.y - p0.y, p1.z - p0.z};
        store3(out, V3{p0.x + t * d.x, p0.y + t * d.y, p0.z + t * d.z});
        return out;
    }
    H* node_ctor() {  // 0x00689250
        std::memset(this, 0, sizeof(jo::TrailNode));  // the whole node (0x84 on x86; newer / older are pointers)
        return this;
    }
    void node_set_points(const void* points) { std::memcpy(this, points, 5 * 12); }  // 0x006892c0
    void node_unlink() {                                  // 0x00689330
        jo::self<jo::TrailNode>(this)->newer = nullptr;
        jo::self<jo::TrailNode>(this)->older = nullptr;
    }
    void node_dtor() { joan::orig::as_trail_node(this)->unlink(); }       // 0x00689290
    H* node_delete(std::uint32_t flags) {                 // 0x006894a0
        joan::orig::as_trail_node(this)->destruct();                       // 0x00689290
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
    void nodes_pop_oldest() {  // 0x006894f0
        if (jo::self<jo::TrailNodes>(this)->count_ == 0) return;
        void* const old = jo::view<void*>(jo::self<jo::TrailNodes>(this)->oldest);
        if (jo::self<jo::TrailNodes>(this)->count_ != 1) {
            jo::view<void*>(jo::self<jo::TrailNodes>(this)->oldest) = jo::view<void*>(jo::as_trail_node(jo::self<jo::TrailNodes>(this)->oldest)->newer);
            jo::self<jo::TrailNodes>(this)->oldest->older = nullptr;
        } else {
            jo::self<jo::TrailNodes>(this)->newest = nullptr;
            jo::self<jo::TrailNodes>(this)->oldest = nullptr;
        }
        joan::orig::as_trail_node(old)->unlink();  // 0x00689330
        if (old != nullptr) joan::orig::as_trail_node(old)->destroy(1u);  // 0x006894a0
        jo::self<jo::TrailNodes>(this)->count_ = jo::self<jo::TrailNodes>(this)->count_ - 1;
    }
    // 0x006895c0 push a new newest node with the points; tangents of it and its older
    // neighbour (Catmull-Rom over all 5 slots).
    void nodes_push(const void* points) {
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::TrailNode)));  // operator new (0x84 on x86)
        std::uint8_t* const node = mem != nullptr ? joan::orig::arg::as<std::uint8_t*>(joan::orig::as_trail_node(mem)->construct()) : nullptr;  // 0x00689250
        joan::orig::as_trail_node(node)->set_points(joan::orig::arg::mptr(points));  // 0x006892c0
        if (jo::self<jo::TrailNodes>(this)->count_ != 0) {
            jo::view<void*>(jo::as_trail_node(node)->older) = jo::view<void*>(jo::self<jo::TrailNodes>(this)->newest);
            jo::view<void*>(jo::self<jo::TrailNodes>(this)->newest->newer) = node;
        } else {
            jo::view<void*>(jo::self<jo::TrailNodes>(this)->oldest) = node;
        }
        jo::view<void*>(jo::self<jo::TrailNodes>(this)->newest) = node;
        jo::as_trail_node(node)->time = now();
        jo::self<jo::TrailNodes>(this)->count_ = jo::self<jo::TrailNodes>(this)->count_ + 1;
        if (jo::view<void*>(jo::as_trail_node(node)->older) != nullptr) {
            std::uint8_t* const older = jo::view<std::uint8_t*>(jo::as_trail_node(node)->older);
            std::uint8_t* const oldest = jo::view<std::uint8_t*>(jo::as_trail_node(older)->older) != nullptr ? jo::view<std::uint8_t*>(jo::as_trail_node(older)->older) : older;
            for (std::int32_t i = 0; i < 5; ++i)
                joan::orig::as_trail_nodes(this)->tangent(joan::orig::arg::mptr(static_cast<void*>(&jo::as_trail_node(older)->tangents[i * 3])), load3(&jo::as_trail_node(node)->points[i * 3]), load3(&jo::as_trail_node(older)->points[i * 3]), load3(&jo::as_trail_node(oldest)->points[i * 3]), 0.0f);
        }
        std::uint8_t* const older = jo::view<std::uint8_t*>(jo::as_trail_node(node)->older) != nullptr ? jo::view<std::uint8_t*>(jo::as_trail_node(node)->older) : node;
        for (std::int32_t i = 0; i < 5; ++i)
            joan::orig::as_trail_nodes(this)->tangent(joan::orig::arg::mptr(static_cast<void*>(&jo::as_trail_node(node)->tangents[i * 3])), load3(&jo::as_trail_node(node)->points[i * 3]), load3(&jo::as_trail_node(node)->points[i * 3]), load3(&jo::as_trail_node(older)->points[i * 3]), 0.0f);
    }
    // 0x006891d0 Trail::set_scene(scene).
    void trail_set_scene(void* scene) {
        if (jo::view<void*>(jo::self<jo::Trail>(this)->actor) != nullptr) {
            if (jo::view<void*>(jo::self<jo::Trail>(this)->actor->scene) != nullptr) thiscall(kNodeRemove, jo::view<void*>(jo::self<jo::Trail>(this)->actor->scene), jo::view<void*>(jo::self<jo::Trail>(this)->actor));
            if (scene != nullptr && jo::self<jo::Trail>(this)->attached != 0) thiscall(kNodeAdd, scene, jo::view<void*>(jo::self<jo::Trail>(this)->actor));
        }
        jo::view<void*>(jo::self<jo::Trail>(this)->scene) = scene;
    }
};

#define FX3(name, addr, fn) JOAN_REPLACE("game/fx3", name, addr, fn)
FX3("Trail::update", 0x00687880, &H::trail_update);
FX3("Trail::release_hold", 0x00687850, &H::trail_release_hold);
FX3("TrailNodes::count", 0x00689af0, &H::nodes_count);
FX3("Trail::add_node", 0x006883d0, &H::trail_add_node);
FX3("Trail::age", 0x00688550, &H::trail_age);
FX3("Trail::build", 0x006889c0, &H::trail_build);
FX3("Trail::hermite", 0x00688fd0, &H::hermite);
FX3("TrailNodes::tangent", 0x006898d0, &H::tangent);
FX3("TrailNodes::lerp", 0x00689a20, &H::lerp);
FX3("TrailNode::TrailNode", 0x00689250, &H::node_ctor);
FX3("TrailNode::set_points", 0x006892c0, &H::node_set_points);
FX3("TrailNode::unlink", 0x00689330, &H::node_unlink);
FX3("TrailNode::~TrailNode", 0x00689290, &H::node_dtor);
FX3("TrailNode::delete", 0x006894a0, &H::node_delete);
FX3("TrailNodes::pop_oldest", 0x006894f0, &H::nodes_pop_oldest);
FX3("TrailNodes::push", 0x006895c0, &H::nodes_push);
FX3("Trail::set_scene", 0x006891d0, &H::trail_set_scene);
#undef FX3

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

JOAN_COUNTER(c_attach, "Attach::update");

struct H {
    // 0x00687a60
    void attach_update() {
        c_attach.hit();
        if (jo::self<jo::Trail>(this)->attached == 0) return;
        if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(jo::self<jo::Trail>(this)->unit_id)) != 0) {  // 0x00425d50
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::Trail>(this)->scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Trail>(this)->actor)));            // 0x005efdb0
            return;
        }
        std::uint8_t* const o = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Trail>(this)->unit_id)));  // 0x00425d20
        if (jo::self<jo::Trail>(this)->actor == nullptr) return;
        if (jo::as_actor(jo::as_map_object(o)->actor)->scene == nullptr) return;
        if (jo::as_map_object(o)->actor == nullptr) return;
        void* first = nullptr;
        void* second = nullptr;  // select_mesh writes a pointer (the bone) here: pointer-sized, or x64 clobbers the neighbouring local
        joan::orig::as_actor(jo::view<void*>(jo::as_map_object(o)->actor))->select_mesh(joan::orig::arg::mptr(global<void*>(d::g_camera)), 1, &first, &second);                                         // 0x005ad060
        void* const t = joan::orig::arg::as<void*>(joan::orig::as_plane(first)->bone_5cf8d0());                      // 0x005cf8d0
        std::uint8_t* m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bone(t)->matrix());                // 0x0060ef80
        if (jo::as_actor(jo::as_map_object(o)->actor)->anim != nullptr)
            m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::as_map_object(o)->actor)->bone))->matrix());
        std::uint32_t pos[3] = {jo::view<std::uint32_t>(jo::self<jo::Matrix44>(m)->m[12]), jo::view<std::uint32_t>(jo::self<jo::Matrix44>(m)->m[13]), jo::view<std::uint32_t>(jo::self<jo::Matrix44>(m)->m[14])};
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::Trail>(this)->actor))->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(pos)));  // 0x005ac310
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::Trail>(this)->scene))->detach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Trail>(this)->actor)));              // 0x005efdb0
        joan::orig::as_scene(jo::view<void*>(jo::as_actor(jo::as_map_object(o)->actor)->scene))->attach(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Trail>(this)->actor)));  // 0x005efc50
        jo::self<jo::Trail>(this)->scene = jo::as_actor(jo::as_map_object(o)->actor)->scene;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Attach::update", 0x00687a60, &H::attach_update);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b103_14.cpp (game/b103_14)
namespace {
namespace game_b103_14 {

struct H {
    // ------------------------------------------------------------------ 0x00687c00
    u32 call_687410() {
        joan::orig::as_trail(this)->release();  // 0x00687410
        return 0;  // 0x00687c00 has no caller (no call or address reference); 0x00687410 returns nothing
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_14", name, addr, fn)
B103("Obj::call_687410", 0x00687c00, &H::call_687410);
#undef B103

}  // namespace game_b103_14
}  // namespace

// ---------------------------------------- from modules/game_b102_14.cpp (game/b102_14)
namespace {
namespace game_b102_14 {

using u8 = std::uint8_t;

using u16 = std::uint16_t;


struct Rec36 {
    u32 v[9];
};
static_assert(sizeof(Rec36) == 36, "Rec36");

struct H {
    // ------------------------------------------------------------------ 0x00687c30
    u8 push_point(Rec36 rec) {
        const i32 n = static_cast<i32>(jo::self<jo::Trail>(this)->point_count);
        if (n >= 5 || jo::view<i32>(jo::self<jo::Trail>(this)->hold_start) != 0) return 0;
        joan::orig::as_vertex_format(&jo::self<jo::Trail>(this)->points[(n - 1) * 6])->from_24(&rec);  // 0x0060f330
        jo::view<u16>(jo::self<jo::Trail>(this)->point_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Trail>(this)->point_count) + 1);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_14", name, addr, fn)
B("Track::push_point", 0x00687c30, &H::push_point);
#undef B

}  // namespace game_b102_14
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {

void* const kTextures = obj(d::g_texture_array);

using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_trail, "Trail::build");

struct H {

    void* ctor_689370() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::TrailNodes));  // memset (0xc on x86)
        return this;
    }

    // ---- 0x00687cc0 trail mesh ----------------------------------------------------------------
    // +0 actor, +4 scene object, +0x16 points (stride 0x18), +0x8e points per row,
    // +0x94 rows.
    void trail_build(void* texture) {
        c_trail.hit();
        if (jo::self<jo::Trail>(this)->point_count < 2) return;
        if (jo::view<void*>(jo::self<jo::Trail>(this)->actor_def) == nullptr) {
            void* const p = game_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            void* const shape = p != nullptr ? joan::orig::as_plane(p)->construct(4u, 1u) : nullptr;  // 0x005cbaf0
            joan::orig::as_plane(shape)->init(0, 0, 0, 1u);                                           // 0x005cc7a0
            void* const q = game_new(static_cast<std::uint32_t>(sizeof(jo::ActorDef)));  // 0x2d8 on x86
            void* const node = q != nullptr ? joan::orig::as_actor_def(q)->construct() : nullptr;  // 0x005af390
            jo::view<void*>(jo::self<jo::Trail>(this)->actor_def) = node;
            joan::orig::as_actor_def(jo::view<void*>(jo::self<jo::Trail>(this)->actor_def))->create(joan::orig::as_plane(shape), nullptr);  // 0x005b02f0
            jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->actor_def->model->sphere[0]) = global<std::uint32_t>(d::g_pos);
            jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->actor_def->model->sphere[1]) = global<std::uint32_t>(d::g_a69cdc);
            jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->actor_def->model->sphere[2]) = global<std::uint32_t>(d::g_a69ce0);
            jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->actor_def->model->sphere[3]) = 0x43160000u;  // 150.0f
        }
        if (jo::view<void*>(jo::self<jo::Trail>(this)->actor) != nullptr) return;
        void* const r = game_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86
        void* const actor = r != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(r)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        jo::view<void*>(jo::self<jo::Trail>(this)->actor) = actor;
        jo::view<void*>(jo::self<jo::Trail>(this)->actor->def) = jo::view<void*>(jo::self<jo::Trail>(this)->actor_def);
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Trail>(this)->actor))->init();  // 0x005aab70
        jo::self<jo::Trail>(this)->actor->plane->transformed = 1;
        {
            jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->actor->aim_sphere[0]) = global<std::uint32_t>(d::g_pos);
            jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->actor->aim_sphere[1]) = global<std::uint32_t>(d::g_a69cdc);
            jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->actor->aim_sphere[2]) = global<std::uint32_t>(d::g_a69ce0);
        }
        jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->actor->aim_sphere[3]) = 0x41200000u;  // 10.0f
        jo::view<std::uint32_t>(jo::self<jo::Trail>(this)->actor->kind) = 0x27;
        jo::self<jo::Trail>(this)->actor->owner = this;
        std::uint8_t* const mesh = jo::view<std::uint8_t*>(jo::self<jo::Trail>(this)->actor->plane);
        const std::int32_t mi = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_materials(1));  // 0x005cd470
        std::uint8_t* const mat = reinterpret_cast<std::uint8_t*>(&jo::as_plane(mesh)->materials[mi]);  // 0x54-byte records on x86 (Plane::materials is a Material array: x64 stride sizeof(Material))
        joan::orig::as_material(mat)->construct();  // 0x005d52a0
        std::uint32_t colour = 0xffffffffu;
        joan::orig::as_material(mat)->set_colour0(joan::orig::arg::mptr(static_cast<void*>(&colour)));  // 0x005d5540
        if (texture == nullptr) {
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kTextures)->load(joan::orig::arg::str(&img::global<const char>(d::s_SLASH_02)), 1, 0u));  // 0x006099c0 "SLASH_02"
            joan::orig::as_material(mat)->set_texture(0, joan::orig::arg::mptr(t));  // 0x005d5650
            joan::orig::texture_ref::release(joan::orig::arg::mptr(t));        // 0x0042cef0
        } else {
            joan::orig::as_material(mat)->set_texture(0, joan::orig::arg::mptr(texture));
        }
        jo::as_material(mat)->blend = 7;
        jo::as_material(mat)->pass_mask = 2;
        jo::as_material(mat)->raster = jo::as_material(mat)->raster | 0x102u;
        const std::int32_t n = jo::self<jo::Trail>(this)->point_count;
        const std::int32_t tris = static_cast<std::int32_t>(static_cast<std::uint32_t>(n + n - 2) *
                                                            static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::self<jo::Trail>(this)->max_nodes)));
        const std::int32_t verts = static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::Trail>(this)->point_count) *
                                                             static_cast<std::uint32_t>(jo::self<jo::Trail>(this)->max_nodes + 1));
        const std::int32_t vbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_vertices(joan::orig::arg::i32(verts)));  // 0x005cd7f0
        const std::int32_t tbase = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(mesh)->add_triangles(joan::orig::arg::i32(tris)));   // 0x005ce6f0
        const joan::hook::x87::Fpu fpu{true};
        for (std::int32_t row = 0; row < verts; row += jo::self<jo::Trail>(this)->point_count) {
            for (std::int32_t i = 0; i < jo::self<jo::Trail>(this)->point_count; ++i) {
                {
                    const float* const src = &jo::self<jo::Trail>(this)->points[i * 6];
                    std::uint8_t* const v = jo::view<std::uint8_t*>(jo::as_plane(mesh)->vertices_f4) + (vbase + row + i) * 0x18;
                    std::memcpy(v, src, 12);
                }
                {
                    const std::int32_t den = verts - jo::self<jo::Trail>(this)->point_count;
                    jo::as_plane(mesh)->vertices_f4[vbase + row + i].attr[1] =
                        static_cast<float>(fpu.div(static_cast<double>(row), static_cast<double>(den)));
                }
                {
                    const std::int32_t den = jo::self<jo::Trail>(this)->point_count - 1;
                    jo::as_plane(mesh)->vertices_f4[vbase + row + i].attr[2] =
                        static_cast<float>(fpu.sub(1.0, fpu.div(static_cast<double>(i), static_cast<double>(den))));  // 0x009cf784
                }
                jo::view<std::uint32_t>(jo::as_plane(mesh)->vertices_f4[vbase + row + i].attr[0]) = colour;
            }
        }
        std::uint8_t flip = 1;
        std::int16_t a = 0, b = 1, c = static_cast<std::int16_t>(jo::self<jo::Trail>(this)->point_count), count = 0;
        for (std::int32_t k = 0; k < tris; ++k) {
            if (!(static_cast<std::int32_t>(c) < verts)) break;
            joan::orig::as_plane(mesh)->set_triangle(joan::orig::arg::i32(tbase + k), joan::orig::arg::u32(mi), joan::orig::arg::u32(vbase + a), joan::orig::arg::u32(vbase + b), joan::orig::arg::u32(vbase + c));  // 0x005ce800
            flip = flip == 0 ? 1 : 0;
            if (flip != 0) {
                b = static_cast<std::int16_t>(a + 1);
                c = static_cast<std::int16_t>(a + jo::self<jo::Trail>(this)->point_count);
            } else {
                a = b;
                b = static_cast<std::int16_t>(a + jo::self<jo::Trail>(this)->point_count);
                c = static_cast<std::int16_t>(b - 1);
            }
            count = static_cast<std::int16_t>(count + 1);
            if (static_cast<std::int32_t>(count) == jo::self<jo::Trail>(this)->point_count * 2 - 2) {
                a = static_cast<std::int16_t>(a + 1);
                b = static_cast<std::int16_t>(a + 1);
                c = static_cast<std::int16_t>(a + jo::self<jo::Trail>(this)->point_count);
                count = 0;
            }
        }
        // (the original then stores dead values into a / b / c / count)
        joan::orig::as_plane(mesh)->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Trail::build", 0x00687cc0, &H::trail_build);
M("Obj689370::Obj", 0x00689370, &H::ctor_689370);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14 {

struct H {
    // 0x006893a0 List::~List() (ret 0).
    std::uint32_t list_dtor() {
        joan::orig::as_list(this)->clear();  // 0x00689400
        return 0;  // callers (Player::~Player 0x00687180, an unwind funclet) discard the result; List::clear returns nothing
    }
};

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("List::~List", 0x006893a0, &H::list_dtor);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

struct H {
    // ------------------------------------------------------------------------ 0x006893d0
    void* clear_8() {
        jo::view<u32>(jo::self<jo::TrailNodes>(this)->count_) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::clear_8", 0x006893d0, &H::clear_8);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Trail::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_04::H::ctor_687130>(this)); }  // 0x687130
auto joan::orig::Player::dtor() -> void { joan::bind::member<&::game_leaves82a::H::player_dtor>(this); }  // 0x687180
auto joan::orig::Trail::init(void* a0) -> void { joan::bind::member<&::game_b83_04::H::task_init>(this, a0); }  // 0x6871f0
auto joan::orig::Trail::release() -> void { joan::bind::member<&::game_leaves81::H::source_release>(this); }  // 0x687410
auto joan::orig::Trail::deform_update(std::int32_t a0) -> void { joan::bind::member<&::game_b89_04::H::deform_update>(this, a0); }  // 0x687500
auto joan::orig::Trail::release_hold() -> void { joan::bind::member<&::game_fx::H::trail_release_hold>(this); }  // 0x687850
auto joan::orig::Trail::update() -> void { joan::bind::member<&::game_fx::H::trail_update>(this); }  // 0x687880
auto joan::orig::Trail::attach_update() -> void { joan::bind::member<&::game_b92_06::H::attach_update>(this); }  // 0x687a60
auto joan::orig::Trail::build_687cc0(void* a0) -> void { joan::bind::member<&::game_b84_04::H::trail_build>(this, a0); }  // 0x687cc0
auto joan::orig::Trail::add_node() -> void { joan::bind::member<&::game_fx::H::trail_add_node>(this); }  // 0x6883d0
auto joan::orig::Trail::age(std::int16_t a0) -> void { joan::bind::member<&::game_fx::H::trail_age>(this, a0); }  // 0x688550
auto joan::orig::Trail::build_6889c0() -> void { joan::bind::member<&::game_fx::H::trail_build>(this); }  // 0x6889c0
auto joan::orig::Trail::set_scene(void* a0) -> void { joan::bind::member<&::game_fx::H::trail_set_scene>(this, a0); }  // 0x6891d0
auto joan::orig::TrailNode::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::node_ctor>(this)); }  // 0x689250
auto joan::orig::TrailNode::destruct() -> void { joan::bind::member<&::game_fx::H::node_dtor>(this); }  // 0x689290
auto joan::orig::TrailNode::set_points(void* a0) -> void { joan::bind::member<&::game_fx::H::node_set_points>(this, a0); }  // 0x6892c0
auto joan::orig::TrailNode::unlink() -> void { joan::bind::member<&::game_fx::H::node_unlink>(this); }  // 0x689330
auto joan::orig::TrailNodes::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_04::H::ctor_689370>(this)); }  // 0x689370
auto joan::orig::List::dtor_6893a0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_14::H::list_dtor>(this)); }  // 0x6893a0
auto joan::orig::List::clear() -> void { joan::bind::member<&::game_leaves82a::H::list_clear>(this); }  // 0x689400
auto joan::orig::TrailNode::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_fx::H::node_delete>(this, a0)); }  // 0x6894a0
auto joan::orig::TrailNodes::pop_oldest() -> void { joan::bind::member<&::game_fx::H::nodes_pop_oldest>(this); }  // 0x6894f0
auto joan::orig::TrailNodes::push(void* a0) -> void { joan::bind::member<&::game_fx::H::nodes_push>(this, a0); }  // 0x6895c0
auto joan::orig::TrailNodes::count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx::H::nodes_count>(this)); }  // 0x689af0
