// en3d/actordeg.cpp: functions of the original d:\ww\en3d\actordeg.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/ambient_occlusion.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers27)
//
//  * module en3d/helpers27: load time - ActorDef::create_lods (0x005b0ab0), Plane tangents
//    (0x005d5770).
namespace {
namespace en3d_helpers25 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

JOAN_COUNTER(c_create_lods, "ActorDef::create_lods");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- en3d/helpers27 ---------------------------------------------------------------------
    // 0x005b0ab0 ActorDef::create_lods: per LOD level 0..4 of the root plane (def +0xc, +0x28)
    // an own plane (+0x178) with detail +0x24 / +0x29 (and +0x2e cleared): kind 1 (def +0xc,
    // +2) non-skinned planes get batching copies sized by vertex / triangle counts, kind 1
    // skinned and kind 0 planes get material-merged copies; kind 0 also allocates +0x34.
    void actordef_create_lods() {
        c_create_lods.hit();
        if (jo::self<jo::ActorDef>(this)->lods_created != 0) return;
        jo::self<jo::ActorDef>(this)->lods_created = 1;
        std::uint8_t* const root = jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->model->mesh);
        for (std::int32_t i = 0; i <= 4; ++i) {
            jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]) = nullptr;
            jo::self<jo::ActorDef>(this)->lod_limit[i] = 0;
            jo::self<jo::ActorDef>(this)->lod_merged[i] = 0;
            jo::self<jo::ActorDef>(this)->lod_count[i] = 0;
        }
        jo::self<jo::ActorDef>(this)->matrices = nullptr;
        auto lod_plane = [&](std::int32_t* i) -> std::uint8_t* {
            if (*i == 0) return root;
            return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(root)->level(joan::orig::arg::mptr(i), 0u));  // 0x005d45e0
        };
        auto new_plane = [&](std::int32_t i, std::uint32_t flags) {
            void* p = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            if (p != nullptr) p = joan::orig::as_plane(p)->construct(joan::orig::arg::u32(static_cast<std::uint32_t>(static_cast<std::uint8_t>(jo::as_plane(root)->vertex_format)))  /* +4 on x86 */, joan::orig::arg::u32(flags));  // Plane::Plane
            jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]) = p;
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]))->init(0, 0, 0, joan::orig::arg::u32(static_cast<std::uint32_t>(jo::as_plane(root)->bone_count))  /* +6 on x86 */);  // Plane::init
        };
        const std::int16_t kind = jo::self<jo::ActorDef>(this)->model->kind;
        if (kind == 1 && jo::as_plane(root)->vertex_format != 7) {
            for (std::int32_t i = 0; i <= 4; ++i) {
                std::uint8_t* const plane = lod_plane(&i);
                std::int32_t detail = 0;
                if (plane != nullptr) {
                    const std::int32_t a = jo::as_plane(plane)->vertex_count > 0 ? 0x1000 / jo::as_plane(plane)->vertex_count : 0x10;
                    const std::int32_t b = jo::as_plane(plane)->triangle_count > 0 ? 0x4000 / (jo::as_plane(plane)->triangle_count * 3) : 0x10;
                    detail = a < b ? a : b;
                    detail = detail < 0x10 ? detail : 0x10;
                }
                if (detail < 2 || jo::as_plane(plane)->material_count != 1 || jo::view<std::int32_t>(jo::as_plane(plane)->materials->pass_mask) != 1 ||
                    jo::as_plane(plane)->vertex_format == 7)
                    continue;
                new_plane(i, 1);
                jo::self<jo::ActorDef>(this)->lod_limit[i] = static_cast<std::uint8_t>(detail);
                jo::self<jo::ActorDef>(this)->lod_merged[i] = 0;
            }
            return;
        }
        if (kind == 1 && jo::as_plane(root)->vertex_format == 7) {
            for (std::int32_t i = 0; i <= 4; ++i) {
                std::uint8_t* const plane = lod_plane(&i);
                const std::int32_t detail = plane != nullptr ? 0x10 : 0;
                if (!(detail > 0)) continue;
                new_plane(i, 0);
                void* const lod = jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]);
                joan::orig::as_plane(lod)->merge(joan::orig::arg::mptr(static_cast<void*>(plane)), 1, 0);  // 0x005cea30 merge
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]))->copy_extras(joan::orig::arg::mptr(static_cast<void*>(plane)));  // 0x005cf050
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]))->invalidate(joan::orig::arg::u32(-1));                          // 0x005cf920
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]))->sort_by_material(0);                           // 0x005cf1d0
                jo::self<jo::ActorDef>(this)->lod_limit[i] = static_cast<std::uint8_t>(detail);
                jo::self<jo::ActorDef>(this)->lod_merged[i] = static_cast<std::uint8_t>(detail);
            }
            return;
        }
        if (kind != 0) return;
        for (std::int32_t i = 0; i <= 4; ++i) {
            std::uint8_t* const plane = lod_plane(&i);
            const std::int32_t detail = plane != nullptr ? 0x10 : 0;
            if (!(detail > 0) || jo::as_plane(plane)->vertex_format == 7) continue;
            new_plane(i, 0);
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]))->merge(joan::orig::arg::mptr(static_cast<void*>(plane)), 1, 0);
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]))->copy_extras(joan::orig::arg::mptr(static_cast<void*>(plane)));
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]))->invalidate(joan::orig::arg::u32(-1));
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]))->sort_by_material(0);
            jo::self<jo::ActorDef>(this)->lod_limit[i] = static_cast<std::uint8_t>(detail);
            jo::self<jo::ActorDef>(this)->lod_merged[i] = 0x10;
        }
        jo::self<jo::ActorDef>(this)->matrices = joan::orig::as_mem(img::obj(d::g_mem))->add(0x1400u, joan::orig::arg::str(&img::global<const char>(d::s_d_ww_en3d_actordeg_cpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e74)) + 0x82));  // actordeg.cpp
    }
};

#define HELPER27(name, addr, fn) JOAN_REPLACE("en3d/helpers27", name, addr, fn)
HELPER27("ActorDef::create_groupable_actor_plane", 0x005b0ab0, &H::actordef_create_lods);
#undef HELPER27

}  // namespace en3d_helpers25
}  // namespace

// ---------------------------------------- from modules/en3d_helpers11.cpp (en3d/helpers12)
namespace {
namespace en3d_helpers11 {

using joan::hook::global;

using joan::call::thiscall;

using joan::orig::helpers::differs;

// Actor fields used here: +0x00 ActorDef* (+0x0c -> +0x2c mesh state, +0x14, +0x18 frame planes
// per animation, +0x1c their counts), +0x04 animation set, +0x08 clock select (-1: 0xa500b2,
// else 0xa500ba), +0x30 fade rate, +0x3c, +0x44 / +0x48 animation / sub, +0x4c start time,
// +0x50 looping, +0x54 last update time, +0x78 / +0x7c, +0x80, +0x84, +0x88, +0x8c bone,
// +0x90 plane, +0x94 current plane, +0x9c last LOD, +0xb0 / +0xb4 attachments.
std::uint32_t actor_clock(const void* actor) {
    return jo::as_actor(actor)->kind == -1 ? global<std::uint32_t>(d::g_game_time_ms) : global<std::uint32_t>(d::g_now);
}
float elapsed_float(std::uint32_t now, const void* actor) {  // fild qword of the unsigned difference
    return static_cast<float>(static_cast<double>(now - jo::as_actor(actor)->start_time));
}
// 0x005b5e40: skin / animate a plane for the actor at time t.
void animate(const void* actor, void* plane, float t, std::uint32_t looping, const void* a, const void* b,
             std::uint32_t c, const void* d, const void* e) {
    joan::orig::as_anim_def(jo::as_actor(actor)->anim)->animate(joan::orig::arg::mptr(plane), joan::orig::arg::i32(jo::as_actor(actor)->action), joan::orig::arg::u32(static_cast<std::uint32_t>(static_cast<std::uint16_t>(jo::as_actor(actor)->sub_action))), joan::orig::arg::f32(t), joan::orig::arg::u32(looping), joan::orig::arg::mptr(a), joan::orig::arg::mptr(b), joan::orig::arg::f32(c), joan::orig::arg::mptr(d), joan::orig::arg::mptr(e));
}

JOAN_COUNTER(c_queue, "ActorDef::queue_instance");

// [Graphics] PerfLog: why actors are or are not batched (queue_instance results by reason and kind).
struct QueueStats {
    enum Reason { kQueued, kFaded, kDisabled, kLoading, kFade, kMorph, kNoBatch, kNotInPass, kCount };
    std::uint32_t by[kCount][3] = {};  // [reason][0 rigid, 1 CPU-skinned, 2 vertex-shader skinned]
    std::uint32_t calls = 0;
    void note(Reason r, std::int16_t kind, std::int8_t vtype) {
        ++by[r][kind == 0 ? 0 : vtype == 7 ? 2 : 1];
        if (++calls < 4000000 || joan::hook::settings().perf_log <= 0) return;  // ~every 10 s in a big battle
        joan::hook::log_line("[Graphics] queue_instance (last %u calls; rigid / CPU-skinned / VS-skinned): queued %u/%u/%u, "
                             "faded %u/%u/%u, kind not batched %u/%u/%u (switches %u %u %u), loading %u/%u/%u, fading %u/%u/%u, "
                             "morphs %u/%u/%u, no batch %u/%u/%u, not in pass %u/%u/%u",
                             calls, by[kQueued][0], by[kQueued][1], by[kQueued][2], by[kFaded][0], by[kFaded][1], by[kFaded][2],
                             by[kDisabled][0], by[kDisabled][1], by[kDisabled][2], global<std::uint32_t>(d::g_a16e68),
                             global<std::uint32_t>(d::g_a67680), global<std::uint32_t>(d::g_a16e6c), by[kLoading][0], by[kLoading][1],
                             by[kLoading][2], by[kFade][0], by[kFade][1], by[kFade][2], by[kMorph][0], by[kMorph][1], by[kMorph][2],
                             by[kNoBatch][0], by[kNoBatch][1], by[kNoBatch][2], by[kNotInPass][0], by[kNotInPass][1], by[kNotInPass][2]);
        *this = {};
    }
};
QueueStats g_queue_stats;

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- ActorDef (en3d/actordeg.cpp): per LOD l: +0x24+l max batch, +0x29+l prepared,
    //     +0x2e+l queued, +0x33 prepared mask, +0x34 matrices (16 per LOD), +0x38+l*64 actors,
    //     +0x178+l*4 batch plane, +0x18c+l*64 bones, +0x2cc / +0x2d0 / +0x2d4 light list link.
    // 0x005b1180: 0 when the actor cannot be batched (the caller draws it alone).
    std::int32_t queue_instance(std::uint8_t* actor, void* camera, std::int32_t pass, std::uint8_t* lights) {
        c_queue.hit();
        const std::int16_t kind = jo::self<jo::ActorDef>(this)->model->kind;
        std::int8_t vtype = (jo::as_plane(jo::self<jo::ActorDef>(this)->model->mesh))->vertex_format;
        if (jo::view<std::uint32_t>(jo::as_actor(actor)->faded) != 0) return g_queue_stats.note(QueueStats::kFaded, kind, vtype), 0;
        const bool enabled = (kind == 0 && global<std::uint32_t>(d::g_a16e68) != 0) ||
                             (kind == 1 && vtype != 7 && global<std::uint32_t>(d::g_a67680) != 0) ||
                             (kind == 1 && vtype == 7 && global<std::uint32_t>(d::g_a16e6c) != 0);
        if (!enabled) return g_queue_stats.note(QueueStats::kDisabled, kind, vtype), 0;
        if (jo::view<std::int8_t>((jo::as_plane(jo::as_actor(actor)->plane))->load_state) != 0) return g_queue_stats.note(QueueStats::kLoading, kind, vtype), 0;
        if (differs(jo::as_actor(actor)->fade, 0.0f)) return g_queue_stats.note(QueueStats::kFade, kind, vtype), 0;
        if (jo::as_actor(actor)->morph_count > 0) return g_queue_stats.note(QueueStats::kMorph, kind, vtype), 0;
        if (jo::self<jo::ActorDef>(this)->lods_created == 0) joan::orig::as_actor_def(this)->create_lods();  // 0x005b0ab0
        std::int32_t lod = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::as_actor(actor)->plane)->lod(joan::orig::arg::mptr(camera)));  // 0x005d4760
        if (lod != 0) joan::orig::as_plane(jo::as_actor(actor)->plane)->level(&lod, 1u);              // 0x005d45e0
        if (jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[lod]) == nullptr || jo::self<jo::ActorDef>(this)->lod_limit[lod] == 0) return g_queue_stats.note(QueueStats::kNoBatch, kind, vtype), 0;
        if (kind == 0 && joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::as_actor(actor)->plane)->in_pass(joan::orig::arg::u32(pass))) == 0) return g_queue_stats.note(QueueStats::kNotInPass, kind, vtype), 1;  // 0x005ce970
        std::uint8_t* const plane = lod == 0 ? jo::view<std::uint8_t*>(jo::as_actor(actor)->plane)
                                             : joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(jo::as_actor(actor)->plane)->level(&lod, 0u));
        std::uint8_t* const shared = lod == 0 ? jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->model->mesh)
                                              : joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(jo::self<jo::ActorDef>(this)->model->mesh)->level(&lod, 0u));
        vtype = jo::as_plane(plane)->vertex_format;
        std::int32_t limit = jo::self<jo::ActorDef>(this)->lod_limit[lod];
        if (pass != 1) limit = limit >= 1 ? 1 : limit;
        if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->lod_count[lod]) >= limit)
            joan::orig::as_actor_def(this)->draw_instances(joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass), joan::orig::arg::i32(lod));  // ActorDef::draw_instances 0x005b1cb0
        if (jo::self<jo::ActorDef>(this)->lod_count[lod] >= jo::self<jo::ActorDef>(this)->lod_merged[lod] && kind == 1 && vtype != 7) {
            void* const batch = jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[lod]);
            joan::orig::as_plane(batch)->merge(joan::orig::arg::mptr(shared), 1, 2);  // 0x005cea30
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[lod]))->copy_extras(joan::orig::arg::mptr(shared));  // 0x005cf050
            jo::self<jo::ActorDef>(this)->merged_mask = static_cast<std::uint8_t>(jo::self<jo::ActorDef>(this)->merged_mask | static_cast<std::uint8_t>(1u << (lod & 31)));
            jo::self<jo::ActorDef>(this)->lod_merged[lod] = static_cast<std::uint8_t>(jo::self<jo::ActorDef>(this)->lod_merged[lod] + 1);
        }
        const std::int32_t count = jo::self<jo::ActorDef>(this)->lod_count[lod];
        std::uint8_t* const batch = jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->batch[lod]);
        if (kind == 1 && vtype != 7) {
            if (pass == 1) {
                const std::uint32_t now = actor_clock(actor);
                const void* const p78 = jo::as_actor(actor)->key_count != 0 ? static_cast<const void*>(jo::as_actor(actor)->keys) : nullptr;
                animate(actor, plane, elapsed_float(now, actor), jo::as_actor(actor)->anim_flags, jo::self<jo::ActorDef>(this)->bone_map, p78,
                        jo::as_actor(actor)->blend_ms, &jo::as_actor(actor)->cursor_owner, jo::as_actor(actor)->bone_mats);  // actor + 0x84 (the key-cache owner)
                jo::Bone* const saved0 = jo::as_plane(batch)->bones;
                jo::as_plane(batch)->bones = jo::as_plane(plane)->bones;
                const std::uint8_t saved6 = jo::as_plane(batch)->bone_count;
                jo::as_plane(batch)->bone_count = jo::as_plane(plane)->bone_count;
                void* const state = lod == 0 ? jo::view<void*>(jo::self<jo::ActorDef>(this)->model->skin)
                                             : joan::orig::arg::as<void*>(joan::orig::as_mesh(jo::view<void*>(jo::self<jo::ActorDef>(this)->model->skin))->lod(joan::orig::arg::i32(lod)));  // 0x005f4fb0
                std::int32_t field = 0, size = 0;  // the vertex array of the format and its element size
                switch (vtype - 1) {
                    case 0: field = static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f1)), size = static_cast<std::int32_t>(sizeof(jo::VertexF1)); break;
                    case 1: field = static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f2)), size = static_cast<std::int32_t>(sizeof(jo::VertexF2)); break;
                    case 3: field = static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f4)), size = static_cast<std::int32_t>(sizeof(jo::VertexF4)); break;
                    case 7: field = static_cast<std::int32_t>(offsetof(jo::Plane, vertices_f8)), size = static_cast<std::int32_t>(sizeof(jo::VertexF8)); break;
                    default: break;
                }
                if (field != 0) {
                    std::uint8_t*& vertices = *reinterpret_cast<std::uint8_t**>(batch + field);
                    std::uint8_t* const saved_ptr = vertices;
                    const std::uint32_t saved_count = jo::view<std::uint32_t>(jo::as_plane(batch)->vertex_count);
                    vertices = vertices + static_cast<std::uint32_t>(count * jo::as_plane(shared)->vertex_count * size);
                    jo::view<std::uint32_t>(jo::as_plane(batch)->vertex_count) = jo::view<std::uint32_t>(jo::as_plane(shared)->vertex_count);
                    joan::orig::as_skin(state)->deform(joan::orig::arg::mptr(batch));  // 0x005f4c10
                    vertices = saved_ptr;
                    jo::view<std::uint32_t>(jo::as_plane(batch)->vertex_count) = saved_count;
                }
                jo::as_plane(batch)->bones = saved0;
                jo::as_plane(batch)->bone_count = saved6;
            }
        } else if (kind == 1 && vtype == 7) {
            if (pass == 1) {
                const std::uint32_t now = actor_clock(actor);
                const void* const p78 = jo::as_actor(actor)->key_count != 0 ? static_cast<const void*>(jo::as_actor(actor)->keys) : nullptr;
                animate(actor, plane, elapsed_float(now, actor), jo::as_actor(actor)->anim_flags, jo::self<jo::ActorDef>(this)->bone_map, p78,
                        jo::as_actor(actor)->blend_ms, &jo::as_actor(actor)->cursor_owner, jo::as_actor(actor)->bone_mats);  // actor + 0x84 (the key-cache owner)
            }
            jo::self<jo::ActorDef>(this)->batch_planes[lod * 16 + jo::self<jo::ActorDef>(this)->lod_count[lod]] = jo::as_plane(plane)->bones;
        } else if (kind == 0) {
            std::uint8_t* const matrix = jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->matrices) + ((lod << 4) + jo::self<jo::ActorDef>(this)->lod_count[lod]) * 64;
            if (jo::as_plane(plane)->transformed == 0) {
                std::memcpy(matrix, joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_plane(plane)->bones))->matrix()), 64);  // 0x0060ef80
                jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->instances[lod * 16 + jo::self<jo::ActorDef>(this)->lod_count[lod]]) = actor;
            } else {
                std::memcpy(matrix, img::obj(d::g_identity_matrix), 64);
            }
        }
        jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->instances[lod * 16 + jo::self<jo::ActorDef>(this)->lod_count[lod]]) = actor;
        jo::self<jo::ActorDef>(this)->lod_count[lod] = static_cast<std::uint8_t>(jo::self<jo::ActorDef>(this)->lod_count[lod] + 1);

        std::uint8_t* const old = jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->instance_list);
        if (lights != old) {
            if (old != nullptr) {
                std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->prev);
                std::uint8_t* const next = jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->next);
                if (prev != nullptr)
                    jo::view<std::uint8_t*>(jo::as_actor_def(prev)->next) = next;
                else
                    jo::view<std::uint8_t*>(jo::as_instance_list(old)->head) = next;
                if (next != nullptr)
                    jo::view<std::uint8_t*>(jo::as_actor_def(next)->prev) = prev;
                else
                    jo::view<std::uint8_t*>(jo::as_instance_list(old)->tail) = prev;
                jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->prev) = nullptr;
                jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->next) = nullptr;
                jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->instance_list) = nullptr;
            }
            if (lights != nullptr) {
                std::uint8_t* const prev = nullptr;
                std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_instance_list(lights)->head);
                if (prev != nullptr)
                    jo::view<std::uint8_t*>(jo::as_actor_def(prev)->prev) = self();
                else
                    jo::view<std::uint8_t*>(jo::as_instance_list(lights)->head) = self();
                if (next != nullptr)
                    jo::view<std::uint8_t*>(jo::as_actor_def(next)->prev) = self();
                else
                    jo::view<std::uint8_t*>(jo::as_instance_list(lights)->tail) = self();
                jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->prev) = prev;
                jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->next) = next;
                jo::view<std::uint8_t*>(jo::self<jo::ActorDef>(this)->instance_list) = lights;
            }
        }
        g_queue_stats.note(QueueStats::kQueued, kind, vtype);
        if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->lod_count[lod]) >= limit) joan::orig::as_actor_def(this)->draw_instances(joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass), joan::orig::arg::i32(lod));
        return 1;
    }
};

JOAN_REPLACE("en3d/helpers12", "ActorDef::add_groupable_actor_plane", 0x005b1180, &H::queue_instance);

}  // namespace en3d_helpers11
}  // namespace

// ---------------------------------------- from modules/scene_draw.cpp (en3d/actordeg.cpp)
namespace {
namespace scene_draw {

using joan::call::thiscall;

JOAN_COUNTER(c_instances, "ActorDef::draw_instances");

// Thunks of the functions called (original addresses in comments).
void plane_draw(void* plane, void* ctx, std::int32_t pass, std::uint32_t arg) {
    joan::orig::as_plane(plane)->draw(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), joan::orig::arg::u32(arg));  // Plane::draw 0x005d0570
}
void plane_draw_single(void* plane, void* ctx, std::int32_t pass, std::uint32_t arg) {
    joan::orig::as_plane(plane)->draw_single(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), joan::orig::arg::u32(arg));  // 0x005d2b80 (one-material planes)
}
void plane_set_owner(void* plane, void* owner) { joan::orig::as_plane(plane)->set_owner(joan::orig::arg::mptr(owner)); }  // 0x005d03f0

// Intrusive list of ActorDefs with queued instances (0x005f4460 constructs it).
struct InstanceList {
    void* head = nullptr;
    void* tail = nullptr;
};

struct ActorDef {
    void draw_instances(void* ctx, std::int32_t pass, std::int32_t only_slot);
};

// 0x005b1cb0, thiscall, ret 0xc. Draws the copies queued for each LOD slot (all
// slots when `only_slot` is -1), then leaves the instance list it was queued on.
void ActorDef::draw_instances(void* ctx, std::int32_t pass, std::int32_t only_slot) {
    c_instances.hit();
    auto* const self = reinterpret_cast<std::uint8_t*>(this);
    for (std::int32_t slot = 0; slot <= 4; ++slot) {
        void* const inst = jo::view<void*>(jo::as_actor_def(self)->batch[slot]);
        if (inst == nullptr) continue;
        if (only_slot != -1 && only_slot != slot) continue;
        void* const base_plane = jo::view<void*>(jo::as_actor_def(self)->model->mesh);
        void* const level = slot == 0 ? base_plane : joan::orig::arg::as<void*>(joan::orig::as_plane(base_plane)->level(&slot, 0u));  // 0x005d45e0
        if (jo::view<std::int8_t>(jo::as_actor_def(self)->merged_mask) & (1 << slot)) {
            joan::orig::as_plane(inst)->sort_by_material(0);  // 0x005cf1d0
            jo::as_actor_def(self)->merged_mask &= static_cast<std::uint8_t>(~(1 << slot));
        }
        const std::uint32_t copies = jo::as_actor_def(self)->lod_count[slot];
        if (copies == 0) continue;
        const std::int16_t kind = (jo::as_model(jo::view<void*>(jo::as_actor_def(self)->model)))->kind;
        auto** const owners = reinterpret_cast<void**>(&jo::as_actor_def(self)->instances[slot * 16]);
        if (kind == 1 && jo::as_plane(level)->vertex_format != 7) {
            // The copies are pre-merged into one buffer: draw them all at once.
            const std::uint32_t vertices = jo::view<std::uint32_t>(jo::as_plane(inst)->vertex_count), triangles = jo::view<std::uint32_t>(jo::as_plane(inst)->triangle_count);
            jo::view<std::uint32_t>(jo::as_plane(inst)->vertex_count) = copies * jo::view<std::uint32_t>(jo::as_plane(level)->vertex_count);
            jo::view<std::uint32_t>(jo::as_plane(inst)->triangle_count) = copies * jo::view<std::uint32_t>(jo::as_plane(level)->triangle_count);
            jo::view<std::uint32_t>((jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->list_kind) = 4;
            plane_set_owner(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), owners[0]);
            plane_draw(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), ctx, pass, 0);
            jo::view<std::uint32_t>((jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->list_kind) = 0;
            plane_set_owner(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), nullptr);
            jo::view<std::uint32_t>(jo::as_plane(inst)->vertex_count) = vertices;
            jo::view<std::uint32_t>(jo::as_plane(inst)->triangle_count) = triangles;
        } else if (kind == 1) {
            // Vertex-shader skinned (type 7): one draw per copy with its own bone.
            void* const bone = jo::view<void*>(jo::as_actor_def(self)->batch[slot]->bones);
            jo::view<std::uint32_t>((jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->list_kind) = 4;
            for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::as_actor_def(self)->lod_count[slot]); ++i) {
                void* const p = jo::view<void*>(jo::as_actor_def(self)->batch[slot]);
                plane_set_owner(p, owners[i]);
                jo::view<void*>((jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->bones) = jo::view<void*>(jo::as_actor_def(self)->batch_planes[slot * 16 + i]);
                if (i == 0 || (jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->material_count > 1)
                    plane_draw(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), ctx, pass, 0);
                else
                    plane_draw_single(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), ctx, pass, 0);
            }
            jo::view<std::uint32_t>((jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->list_kind) = 0;
            plane_set_owner(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), nullptr);
            jo::view<void*>((jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->bones) = bone;
        } else if (kind == 0) {
            // One draw per copy with its own world matrix (copied into the plane's bone).
            jo::view<std::uint32_t>((jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->list_kind) = 4;
            for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::as_actor_def(self)->lod_count[slot]); ++i) {
                plane_set_owner(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), owners[i]);
                std::memcpy(jo::view<void*>(jo::as_actor_def(self)->batch[slot]->bones),
                            jo::view<std::uint8_t*>(jo::as_actor_def(self)->matrices) + ((slot << 4) + i) * 64, 64);
                joan::orig::as_bone(jo::view<void*>(jo::as_actor_def(self)->batch[slot]->bones))->invalidate();  // 0x0060ee10
                if (i == 0 || (jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->material_count > 1)
                    plane_draw(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), ctx, pass, 0);
                else
                    plane_draw_single(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), ctx, pass, 0);
            }
            jo::view<std::uint32_t>((jo::as_plane(jo::view<void*>(jo::as_actor_def(self)->batch[slot])))->list_kind) = 0;
            plane_set_owner(jo::view<void*>(jo::as_actor_def(self)->batch[slot]), nullptr);
        }
        jo::as_actor_def(self)->lod_count[slot] = 0;
    }
    if (only_slot != -1) return;
    // Leave the instance list (inlined unlink; the following "link into null list" does nothing).
    auto* const list = static_cast<InstanceList*>(jo::view<void*>(jo::as_actor_def(self)->instance_list));
    if (list == nullptr) return;
    void* const next = jo::view<void*>(jo::as_actor_def(self)->prev);  // +0x2d0
    void* const prev = jo::view<void*>(jo::as_actor_def(self)->next);  // +0x2d4
    if (next)
        jo::view<void*>(jo::as_actor_def(next)->next) = prev;
    else
        list->tail = prev;
    if (prev)
        jo::view<void*>(jo::as_actor_def(prev)->prev) = next;
    else
        list->head = next;
    jo::view<void*>(jo::as_actor_def(self)->prev) = nullptr;
    jo::view<void*>(jo::as_actor_def(self)->next) = nullptr;
    jo::view<void*>(jo::as_actor_def(self)->instance_list) = nullptr;
}

JOAN_REPLACE("en3d/actordeg.cpp", "ActorDef::draw_instances", 0x005b1cb0, &ActorDef::draw_instances);

}  // namespace scene_draw
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ActorDef::create_lods() -> void { joan::bind::member<&::en3d_helpers25::H::actordef_create_lods>(this); }  // 0x5b0ab0
auto joan::orig::ActorDef::queue_instance(void* a0, void* a1, std::int32_t a2, void* a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers11::H::queue_instance>(this, a0, a1, a2, a3)); }  // 0x5b1180
