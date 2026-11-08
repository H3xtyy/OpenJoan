// en3d/scene.cpp: functions of the original d:\ww\en3d\scene.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cmath>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../render/ambient_occlusion.hpp"
#include "../render/shadow_map.hpp"
#include "../render/water.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

constexpr img::Addr kSceneCpp(d::s_d_ww_en3d_scene_cpp);  // "d:\ww\en3d\scene.cpp"
std::int32_t line_base(img::Addr address) { return static_cast<std::int32_t>(global<std::int16_t>(address)); }

using u32 = std::uint32_t;

using joan::img::obj;
using joan::img::str;

using i32 = std::int32_t;

using joan::orig::helpers::game_new;  // operator new

using joan::call::stdcall_call;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/en3d_helpers36.cpp (en3d/helpers36)
//
//      0x005e9b10 Scene::clear()                - Scene::setup(0), frees every scene array
//
//      0x005e9fc0 SceneGroup scalar deleting destructor (dtor 0x005ef850)
//
//      0x005ea010 SkyBox scalar deleting destructor (dtor 0x005f5140)
//
//      0x005ea0d0 Scene::set_1c(p)              - deletes the old +0x1c object
//
//      0x005ea180 Scene::free_71()
//
//      0x005ea390 Scene::release_7d()           - 0x005acf60(0) on +0x7d[n-1..0], count +0x75
//
//      0x005eb030 Scene::delete_30()
//
//      0x005eb820 Scene::delete_lights()        - deletes +0x54[+0x4c - 1 .. 0]
//
//      0x005ed9d0 Scene::set_camera(cam)        - +0xb5 = cam, pushed to the renderer 0xa52597
namespace {
namespace en3d_helpers36 {


// Game memory manager (0xa48838) with file / line tracking.
constexpr img::Addr kMemory(d::g_mem);
std::uint32_t mem_free(void* p, const char* file, std::int32_t line) {
    return thiscall<std::uint32_t>(fn::del, img::obj(kMemory), p, file, line);  // 0x0058ed70
}

JOAN_COUNTER(c_scene_clear, "Scene::clear");

struct H {

    // Frees +off through the memory manager (line base 0xa1742c) when set; returns eax.
    void scene_free(void*& field, std::int32_t line_add) {
        if (field == nullptr) return;
        mem_free(field, img::str(kSceneCpp), line_base(d::g_a1742c) + line_add);
        field = nullptr;
    }

    // 0x005e9b10 Scene::clear(). ret 0. eax: the 0x005edfd0 result, or the last free's.
    std::uintptr_t scene_clear() {
        c_scene_clear.hit();
        joan::orig::as_scene(this)->setup(joan::orig::arg::mptr(static_cast<void*>(nullptr)));  // 0x005e9e50 setup(0)
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->occluder_total) = 0;
        scene_free(jo::view<void*>(jo::self<jo::Scene>(this)->occluders), 0x12);
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->occluder_capacity) = 0;
        scene_free(jo::view<void*>(jo::self<jo::Scene>(this)->buffer_d1), 0x19);
        jo::self<jo::Scene>(this)->u32_cd = 0;
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->visible_count) = 0;
        scene_free(jo::view<void*>(jo::self<jo::Scene>(this)->visible), 0x21);
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->visible_capacity) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->list_40_count) = 0;
        scene_free(jo::view<void*>(jo::self<jo::Scene>(this)->list_40), 0x29);
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->list_40_capacity) = 0;
        scene_free(jo::view<void*>(jo::self<jo::Scene>(this)->sectors), 0x2f);
        jo::self<jo::Scene>(this)->sectors = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->sector_capacity) = 0;
        scene_free(jo::view<void*>(jo::self<jo::Scene>(this)->lights), 0x34);
        jo::self<jo::Scene>(this)->lights = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->light_capacity) = 0;
        scene_free(jo::view<void*>(jo::self<jo::Scene>(this)->actors), 0x3a);
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->actor_capacity) = 0;
        if (jo::view<void*>(jo::self<jo::Scene>(this)->plane_81) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Scene>(this)->plane_81);
            joan::orig::as_plane(p)->destroy(1u);  // 0x00459c40 deleting dtor
            jo::view<void*>(jo::self<jo::Scene>(this)->plane_81) = nullptr;
        }
        jo::self<jo::Scene>(this)->u32_68 = 0;
        jo::self<jo::Scene>(this)->u32_6c = 1;
        std::uintptr_t r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_scene(this)->free_c5());  // 0x005edfd0
        if (jo::self<jo::Scene>(this)->region_buffer != nullptr) {
            r = mem_free(jo::self<jo::Scene>(this)->region_buffer, img::str(kSceneCpp), line_base(d::g_a1742c) + 0x4b);
            jo::self<jo::Scene>(this)->region_buffer = nullptr;
        }
        return r;
    }

    // 0x005e9fc0 SceneGroup scalar deleting destructor. ret 4.
    void* group_delete(std::uint32_t flags) {
        joan::orig::as_scene_s(this)->destruct();  // 0x005ef850
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }

    // 0x005ea010 SkyBox scalar deleting destructor. ret 4.
    void* sky_delete(std::uint32_t flags) {
        joan::orig::as_sky_box(this)->dtor();  // 0x005f5140
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }

    // 0x005ea0d0 Scene::set_1c(p). ret 4. eax: the deleting dtor's result, or this.
    std::uintptr_t scene_set_1c(void* p) {
        if (jo::view<void*>(jo::self<jo::Scene>(this)->plane_1c) != nullptr) {
            const std::uintptr_t r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_1c))->destroy(1u));  // 0x00459c40
            jo::view<void*>(jo::self<jo::Scene>(this)->plane_1c) = p;
            return r;
        }
        jo::view<void*>(jo::self<jo::Scene>(this)->plane_1c) = p;
        return reinterpret_cast<std::uintptr_t>(this);
    }

    // 0x005ea180 Scene::free_71(). ret 0. eax: the free's result, or this.
    std::uintptr_t scene_free_71() {
        if (jo::view<void*>(jo::self<jo::Scene>(this)->plane_71) == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_71))->destroy(0u);  // 0x00459c40 (destructor only)
        const std::uint32_t r = mem_free(jo::view<void*>(jo::self<jo::Scene>(this)->plane_71), img::str(kSceneCpp), line_base(d::g_a17430) + 4);
        jo::view<void*>(jo::self<jo::Scene>(this)->plane_71) = nullptr;
        return r;
    }

    // 0x005ea390 Scene::release_7d(). ret 0, eax = this.
    void* scene_release_7d() {
        for (std::int32_t i = jo::self<jo::Scene>(this)->actor_count; i > 0; --i)
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::Scene>(this)->actors[i - 1].p))->set_scene(nullptr);  // 0x005acf60
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->actor_count) = 0;
        return this;
    }

    // 0x005eb030 Scene::delete_30(). ret 0, eax = this.
    void* scene_delete_30() {
        void* const p = jo::view<void*>(jo::self<jo::Scene>(this)->tree);
        if (p != nullptr) joan::orig::as_sector_node(p)->vector_destroy(3u);  // 0x005ea910 deleting dtor (array)
        jo::view<void*>(jo::self<jo::Scene>(this)->tree) = nullptr;
        return this;
    }

    // 0x005eb820 Scene::delete_lights(). ret 0, eax = this.
    void* scene_delete_lights() {
        for (; jo::self<jo::Scene>(this)->light_count > 0; jo::self<jo::Scene>(this)->light_count = jo::self<jo::Scene>(this)->light_count - 1) {
            void* const light = jo::view<void*>(jo::self<jo::Scene>(this)->lights[jo::self<jo::Scene>(this)->light_count - 1].p);
            if (light != nullptr) joan::orig::as_light(light)->scalar_deleting_destructor(1u);  // 0x005eb7d0 deleting dtor
        }
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->light_count) = 0;
        return this;
    }

    // 0x005ed9d0 Scene::set_camera(cam). ret 4. eax = the renderer call's.
    std::uintptr_t scene_set_camera(void* cam) {
        jo::view<void*>(jo::self<jo::Scene>(this)->camera) = cam;
        const std::uint32_t v = joan::orig::arg::as<std::uint32_t>(joan::orig::as_light(cam)->c6());  // 0x005c6710
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_vs_cache(global<void*>(d::g_vs_cache))->set_tint_source(static_cast<std::int32_t>(v)));  // 0x0061c250
    }
};

#define HELPER36(name, addr, fn) JOAN_REPLACE("en3d/helpers36", name, addr, fn)
HELPER36("Scene::deinit", 0x005e9b10, &H::scene_clear);
HELPER36("SceneGroup::scalar_deleting_destructor", 0x005e9fc0, &H::group_delete);
HELPER36("SkyBox::scalar_deleting_destructor", 0x005ea010, &H::sky_delete);
HELPER36("Scene::unset_effect_plane", 0x005ea0d0, &H::scene_set_1c);
HELPER36("Scene::unset_shadow_plane", 0x005ea180, &H::scene_free_71);
HELPER36("Scene::remove_all_effect_actors", 0x005ea390, &H::scene_release_7d);
HELPER36("Scene::invalidate_sector_struct", 0x005eb030, &H::scene_delete_30);
HELPER36("Scene::del_all_light", 0x005eb820, &H::scene_delete_lights);
HELPER36("Scene::set_camera", 0x005ed9d0, &H::scene_set_camera);
#undef HELPER36

}  // namespace en3d_helpers36
}  // namespace

// ---------------------------------------- from modules/en3d_helpers31.cpp (en3d/helpers31)
//
//  * module en3d/helpers31:
//      Scene::Scene                (0x005e9a00)  ctor: sub-object at +0x85, then memset 0xd5
//      Scene::reset                (0x005e9ac0)
//      Scene::setup                (0x005e9e50)  clears groups / lights, copies the 24-byte
//                                                 setup block, rebuilds
//      Scene::set_sky              (0x005ea060)
//      Scene::add_light            (0x005eb5d0)  new Light (0xce bytes), appended to +0x54
//      Scene::set_fog_flag         (0x005ee140)  flags +0xbd bit 0x10
//      Scene::set_mode             (0x005ef730)  +0x20
//      Scene::register_material    (0x005eddc0)  1-based index in the +0xc5 list (appends)
//      SceneNode::collect          (0x005eb370)  recursive box query over a binary tree
//      Scene::nop                  (0x005eb590)  empty, ret 4
//      SkyBox::SkyBox              (0x005f5100)  Plane ctor + +0x120 = 0
//      SkyBox::init                (0x005f51b0)
//      SkyBox::build               (0x005f5240)  the 6-face (or 8-vertex cube) sky geometry
namespace {
namespace en3d_helpers31 {


// Game memory manager (0xa48838) with file / line tracking.
constexpr img::Addr kMemory(d::g_mem);
void* mem_realloc(void* p, std::uint32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}

JOAN_COUNTER(c_scene_setup, "Scene::setup");
JOAN_COUNTER(c_add_light, "Scene::add_light");
JOAN_COUNTER(c_register_material, "Scene::register_material");
JOAN_COUNTER(c_collect, "SceneNode::collect");

struct H {
    // 0x005e9e50 Scene::setup(const 24-byte block*). ret 4.
    std::uintptr_t scene_setup(const void* block) {  // eax = last call's eax
        c_scene_setup.hit();
        joan::orig::as_scene(this)->delete_30();  // 0x005eb030
        for (std::int32_t i = jo::self<jo::Scene>(this)->occluder_total - 1; i >= 0; --i) joan::orig::as_scene(this)->remove_5c(joan::orig::arg::i32(i));  // 0x005eedc0
        while (jo::self<jo::Scene>(this)->sector_count > 0) {
            void* const item = jo::view<void*>(jo::self<jo::Scene>(this)->sectors[jo::self<jo::Scene>(this)->sector_count - 1]);
            if (item != nullptr) joan::orig::as_scene_group(item)->destroy(1u);  // 0x005e9fc0 deleting dtor
            jo::view<void*>(jo::self<jo::Scene>(this)->sectors[jo::self<jo::Scene>(this)->sector_count - 1]) = nullptr;
            jo::self<jo::Scene>(this)->sector_count = jo::self<jo::Scene>(this)->sector_count - 1;
        }
        joan::orig::as_scene(this)->delete_lights();  // 0x005eb820
        if (jo::view<void*>(jo::self<jo::Scene>(this)->sky) != nullptr) {
            void* const sky = jo::view<void*>(jo::self<jo::Scene>(this)->sky);
            if (sky != nullptr) joan::orig::as_sky_box(sky)->scalar_deleting_destructor(1u);  // 0x005ea010 deleting dtor
            jo::view<void*>(jo::self<jo::Scene>(this)->sky) = nullptr;
        }
        joan::orig::as_scene(this)->set_1c(joan::orig::arg::mptr(0));  // 0x005ea0d0
        if (block != nullptr) std::memcpy(jo::self<jo::Scene>(this)->params, block, sizeof(jo::Scene::params));  // 24 on x86
        joan::orig::as_scene(this)->free_71();  // 0x005ea180
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_scene(this)->release_7d());  // 0x005ea390
    }

    // 0x005ea060 Scene::set_sky(sky). ret 4.
    // eax on exit: this when unchanged, else sky.
    void* scene_set_sky(void* sky) {
        if (jo::view<void*>(jo::self<jo::Scene>(this)->sky) == sky) return this;
        void* const old = jo::view<void*>(jo::self<jo::Scene>(this)->sky);
        if (old != nullptr) joan::orig::as_sky_box(old)->scalar_deleting_destructor(1u);  // 0x005ea010 deleting dtor
        jo::view<void*>(jo::self<jo::Scene>(this)->sky) = sky;
        return sky;
    }

    // 0x005eb5d0 Scene::add_light. ret 0. (SEH frame only frees the `new` if the ctor throws.)
    void* scene_add_light() {
        c_add_light.hit();
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Light)));                  // operator new (0xce on x86)
        void* const light = mem != nullptr ? joan::orig::as_light(mem)->construct() : nullptr;  // 0x005c5830
        if (jo::self<jo::Scene>(this)->light_count >= jo::self<jo::Scene>(this)->light_capacity) {
            jo::self<jo::Scene>(this)->light_capacity = jo::self<jo::Scene>(this)->light_count + 8;
            const std::int32_t line = line_base(d::g_a17440) + 7;
            jo::view<void*>(jo::self<jo::Scene>(this)->lights) = mem_realloc(jo::view<void*>(jo::self<jo::Scene>(this)->lights), static_cast<std::uint32_t>(jo::self<jo::Scene>(this)->light_capacity * static_cast<std::int32_t>(sizeof(jo::LightPtr))), img::str(kSceneCpp), line);
        }
        jo::view<void*>(jo::self<jo::Scene>(this)->lights[jo::self<jo::Scene>(this)->light_count]) = light;
        jo::self<jo::Scene>(this)->light_count = jo::self<jo::Scene>(this)->light_count + 1;
        return light;
    }

    // 0x005eb590 empty. ret 4.
    void scene_nop(void*) {}  // the facade passes a pointer (Phase 2 unplaced-b-5)

    // 0x005ee140 Scene::set_fog_flag(on). ret 4.
    void* scene_set_flag10(std::int32_t on) {  // eax = this
        if (on != 0)
            jo::self<jo::Scene>(this)->fog_flags = jo::self<jo::Scene>(this)->fog_flags | 0x10u;
        else
            jo::self<jo::Scene>(this)->fog_flags = jo::self<jo::Scene>(this)->fog_flags & 0xffffffefu;
        return this;
    }

    // 0x005eddc0 Scene::register_material(p): 1-based index of p in the +0xc5 list (count +0xc1),
    // appended when absent. Returns the full eax (index + 1 or the new count). ret 4.
    std::int32_t scene_register(void* item) {
        c_register_material.hit();
        if (jo::view<void*>(jo::self<jo::Scene>(this)->materials) != nullptr) {
            for (std::int32_t i = 0; i < jo::self<jo::Scene>(this)->material_count; ++i)
                if (jo::view<void*>(jo::self<jo::Scene>(this)->materials[i]) == item) return i + 1;
        }
        jo::self<jo::Scene>(this)->material_count = jo::self<jo::Scene>(this)->material_count + 1;
        const std::int32_t line = line_base(d::g_a1744c) + 0xd;
        jo::view<void*>(jo::self<jo::Scene>(this)->materials) = mem_realloc(jo::view<void*>(jo::self<jo::Scene>(this)->materials), static_cast<std::uint32_t>(jo::self<jo::Scene>(this)->material_count * static_cast<std::int32_t>(sizeof(jo::BillboardPtr))), img::str(kSceneCpp), line);
        jo::view<void*>(jo::self<jo::Scene>(this)->materials[jo::self<jo::Scene>(this)->material_count - 1]) = item;
        return jo::self<jo::Scene>(this)->material_count;
    }

    // 0x005eb370 SceneNode::collect(box, int* count, out[]): nodes whose bounds (+0x10) pass the
    // box test add their payload (+0x34) and recurse into +0x8 / +0xc. ret 0xc.
    // The nodes are the sector tree's SectorNode objects (Scene::build_tree 0x005ea730; the x86 picture: left +0x8,
    // right +0xc, box +0x10, sector +0x34), not the SceneNode view, whose x64 offsets differ: through the view x64
    // read the box and the payload from the wrong words, so World::read_scene_object (0x00551930) found no node for
    // its effects. out[] holds pointers (Scene::nodes_in_box 0x005ef610: region_buffer, sector_capacity * sizeof(SceneSPtr)).
    void node_collect(void* box, std::int32_t* count, void** out) {
        c_collect.hit();
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_box(box)->overlaps(joan::orig::arg::mptr(jo::self<jo::SectorNode>(this)->box))) == 0) return;  // 0x0060d6b0
        if (jo::view<void*>(jo::self<jo::SectorNode>(this)->sector) != nullptr) {
            out[*count] = jo::view<void*>(jo::self<jo::SectorNode>(this)->sector);
            *count = *count + 1;
        }
        if (jo::view<void*>(jo::self<jo::SectorNode>(this)->left) != nullptr) joan::orig::as_scene_node(jo::view<void*>(jo::self<jo::SectorNode>(this)->left))->collect(joan::orig::arg::mptr(box), joan::orig::arg::mptr(count), joan::orig::arg::mptr(out));  // 0x005eb370
        if (jo::view<void*>(jo::self<jo::SectorNode>(this)->right) != nullptr) joan::orig::as_scene_node(jo::view<void*>(jo::self<jo::SectorNode>(this)->right))->collect(joan::orig::arg::mptr(box), joan::orig::arg::mptr(count), joan::orig::arg::mptr(out));
    }
};

#define HELPER31(name, addr, fn) JOAN_REPLACE("en3d/helpers31", name, addr, fn)
HELPER31("Scene::clear_scene", 0x005e9e50, &H::scene_setup);
HELPER31("Scene::set_sky", 0x005ea060, &H::scene_set_sky);
HELPER31("Scene::create_light", 0x005eb5d0, &H::scene_add_light);
HELPER31("Scene::nop", 0x005eb590, &H::scene_nop);
HELPER31("Scene::set_fog_flag", 0x005ee140, &H::scene_set_flag10);
HELPER31("Scene::register_material", 0x005eddc0, &H::scene_register);
HELPER31("SceneNode::collect", 0x005eb370, &H::node_collect);
#undef HELPER31

}  // namespace en3d_helpers31
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
//
//      0x005ea140 Obj::set_71(v)                  (thiscall, ret 4) - 0x005ea180 when changed.
//
//      0x005eb0f0 Item::Item                      (thiscall, ret 0) - (vector ctor).
//
//      0x005ee1a0 Obj::field_bd                   (thiscall, ret 0)
//
//    0x005eb0f0 are passed to the vector ctor iterator (0x007b2f98, return ignored); 0x005ba990 /
namespace {
namespace game_b103_11 {


using joan::orig::helpers::u;

struct H {
    // 0x005ea140
    std::uintptr_t set_71(jo::Plane* v) {
        if (jo::self<jo::Scene>(this)->plane_71 == v) return u(this);
        joan::orig::as_scene(this)->free_71();  // 0x005ea180
        jo::self<jo::Scene>(this)->plane_71 = v;
        return u(v);
    }
    // 0x005eb0f0
    H* item_ctor() {
        jo::self<jo::SectorNode>(this)->parent = nullptr;
        jo::view<u32>(jo::self<jo::SectorNode>(this)->axis) = 3;
        jo::self<jo::SectorNode>(this)->right = nullptr;
        jo::self<jo::SectorNode>(this)->left = nullptr;
        jo::self<jo::SectorNode>(this)->sector = nullptr;
        return this;
    }
    // 0x005ee1a0
    u32 field_bd() { return jo::self<jo::Scene>(this)->fog_flags; }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Scene::set_shadow_plane", 0x005ea140, &H::set_71);
B103("Item::Item_5eb0f0", 0x005eb0f0, &H::item_ctor);
B103("Obj::field_bd", 0x005ee1a0, &H::field_bd);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
//
//      0x005ea1f0 Scene::add_actor     (thiscall(actor), ret 4)
namespace {
namespace game_b92_04 {


using i16 = std::int16_t;

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_add_actor, "Scene::add_actor");

struct H {
    // ------------------------------------------------------------------------ 0x005ea1f0
    void add_actor(void* actor) {
        c_add_actor.hit();
        for (i32 i = 1; i <= jo::self<jo::Scene>(this)->actor_count; ++i) {
            if (jo::view<void**>(jo::self<jo::Scene>(this)->actors)[i - 1] == actor) return;
        }
        if (jo::self<jo::Scene>(this)->actor_count >= jo::self<jo::Scene>(this)->actor_capacity) {
            jo::self<jo::Scene>(this)->actor_capacity = jo::self<jo::Scene>(this)->actor_count + 0x10;
            jo::view<void**>(jo::self<jo::Scene>(this)->actors) = joan::orig::arg::as<void**>(joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Scene>(this)->actors)), joan::orig::arg::u32(jo::self<jo::Scene>(this)->actor_capacity * static_cast<std::int32_t>(sizeof(jo::ActorPtr))), str(d::s_d_ww_en3d_scene_cpp), joan::orig::arg::i32(// 0x0058eb30 "d:\ww\en3d\scene.cpp"
                                               static_cast<i32>(global<i16>(d::g_a17434)) + 0xd)));
        }
        jo::view<void**>(jo::self<jo::Scene>(this)->actors)[jo::self<jo::Scene>(this)->actor_count] = actor;
        jo::self<jo::Scene>(this)->actor_count = jo::self<jo::Scene>(this)->actor_count + 1;
        joan::orig::as_actor(actor)->set_scene(jo::self<jo::Scene>(this));  // 0x005acf60
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Scene::add_actor", 0x005ea1f0, &H::add_actor);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
//
//    - List::remove 0x005ea2f0 (obj), ret 4; List::count 0x005ea3f0.
namespace {
namespace game_b90_04 {


JOAN_COUNTER(c_remove, "List::remove");

struct H {
    // 0x005ea2f0
    void list_remove(void* o) {
        c_remove.hit();
        joan::orig::as_actor(o)->set_scene(nullptr);  // 0x005acf60
        for (std::int32_t i = 1; i <= jo::self<jo::Scene>(this)->actor_count; ++i) {
            if (jo::view<void**>(jo::self<jo::Scene>(this)->actors)[i - 1] != o) continue;
            joan::orig::as_misc(obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Scene>(this)->actors)), joan::orig::arg::i32(jo::self<jo::Scene>(this)->actor_count), static_cast<std::int32_t>(sizeof(jo::ActorPtr)), joan::orig::arg::i32(i));  // 0x00591070
            jo::self<jo::Scene>(this)->actor_count = jo::self<jo::Scene>(this)->actor_count - 1;
            return;
        }
    }

    // 0x005ea3f0
    std::uint32_t list_count() { return jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->actor_count); }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Scene::remove_effect_actor", 0x005ea2f0, &H::list_remove);
M("List::count", 0x005ea3f0, &H::list_count);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
//
//  * module game/b91_03:
//    TextCursor::advanced 0x0058c120, to_lower 0x0058e330, empty methods 0x0058e720 /
//    0x0059d1b0, Stream::forward 0x005939f0, Joystick::in_rect 0x00594360,
//    Joystick::pressed_kind 0x00594a10, Key::repeat_count 0x00595fb0, word getters
//    0x005979a0 / 0x005979c0, Pad::axis 0x00597c80, Pad::axis_range 0x00597e00,
//    same16 0x00598700, Pair::clear 0x005a1730, Bitmap::address 0x005a93b0,
//    MusicStream::release 0x005a98a0, MusicStream::read_header 0x005a9ad0,
//    MusicStream::reset 0x005a9b40, MusicStream::rewind 0x005aa040, Actor::set_byte_fd
//    0x005ad000, Table::get 0x005af330, Table::missing 0x005af360, AnimDef::lookup_word
//    0x005b4a00, Names::get 0x005ba910, Rect::set 0x005bed90 / 0x00612ee0, Array::at1
//    0x005ea410, RenderTarget::create_depth 0x006032d0, RenderTarget::release_depth
//    0x00603380, z_plane_hit 0x0060e6a0, BitWriter::put 0x0060ff70, BitWriter::drop
//    0x006100a0, Viewport::set_int 0x00614110, Sound3d::set 0x00616a90, getter 0x00616b60,
//    Dialog::reset_name 0x00616f80, base dtor bodies 0x00628ad0 / 0x0062c720,
//    TextBox::init 0x0062b8d0, Scroll::set 0x0062be10 / 0x0062cd40, Scroll::at_end
//    0x0062c040 / 0x0062cf70, Scroll::set_end 0x0062ced0, TextBox::cursor_x 0x00630770,
//    TextBox::line_y 0x00630ab0, getter 0x00631770, Table16::entry 0x00638150.
namespace {
namespace game_b91_03 {


struct H {
    // 0x005ea410 1-based element of the array +0x7d (count +0x75).
    void* at1(std::int32_t i) {
        if (i > 0 && i <= jo::self<jo::Scene>(this)->actor_count) return joan::orig::arg::as<void*>(jo::self<jo::Scene>(this)->actors[i - 1].p);
        return nullptr;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Array::at1", 0x005ea410, &H::at1);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x005ea460 Scene::add_node()          - new 0xe4 node (0x005ef800), +0x2c / +0xc9 arrays
//
//      0x005eeca0 Scene::add_entry()         - zeroed 0x94 entry in +0x58 (index returned)
namespace {
namespace en3d_b84_01 {


void* const kMemory = obj(d::g_mem);     // allocator (0x0058e7b0 alloc / 0x0058eb30 realloc)

void game_memset(void* p, std::int32_t v, std::uint32_t n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_add_node, "Scene::add_node");
JOAN_COUNTER(c_add_entry, "Scene::add_entry");

struct H {
    // 0x005ea460
    void* add_node() {
        c_add_node.hit();
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::SceneS)));  // 0xe4 on x86: the sector SceneS that 0x005ef800 builds
        void* const node = mem != nullptr ? joan::orig::as_scene_node(mem)->construct() : nullptr;  // 0x005ef800
        jo::view<void*>(jo::as_scene_s(node)->scene) = this;
        if (!(jo::self<jo::Scene>(this)->sector_count < jo::self<jo::Scene>(this)->sector_capacity)) {
            jo::self<jo::Scene>(this)->sector_capacity = jo::self<jo::Scene>(this)->sector_count + 8;
            jo::self<jo::Scene>(this)->sectors = static_cast<jo::SceneSPtr*>(thiscall<void*>(
                fn::resize_58eb30, kMemory, static_cast<void*>(jo::self<jo::Scene>(this)->sectors), static_cast<std::uint32_t>(jo::self<jo::Scene>(this)->sector_capacity) * static_cast<std::uint32_t>(sizeof(jo::SceneSPtr)),
                str(d::s_d_ww_en3d_scene_cpp), static_cast<std::int32_t>(global<std::int16_t>(d::g_a17438)) + 8));  // 0x0058eb30
            jo::self<jo::Scene>(this)->region_buffer = thiscall<void*>(
                fn::resize_58eb30, kMemory, jo::self<jo::Scene>(this)->region_buffer, static_cast<std::uint32_t>(jo::self<jo::Scene>(this)->sector_capacity) * static_cast<std::uint32_t>(sizeof(jo::SceneSPtr)),  /* the collected sector pointers */
                str(d::s_d_ww_en3d_scene_cpp), static_cast<std::int32_t>(global<std::int16_t>(d::g_a17438)) + 9);  // 0x0058eb30
        }
        jo::view<void*>(jo::self<jo::Scene>(this)->sectors[jo::self<jo::Scene>(this)->sector_count].p) = node;
        jo::self<jo::Scene>(this)->sector_count = jo::self<jo::Scene>(this)->sector_count + 1;
        joan::orig::as_scene(this)->delete_30();  // 0x005eb030
        return node;
    }

    // 0x005eeca0 (eax = the new entry's index)
    std::int32_t add_entry() {
        c_add_entry.hit();
        if (!(jo::self<jo::Scene>(this)->occluder_total > jo::self<jo::Scene>(this)->occluder_capacity)) {
            jo::self<jo::Scene>(this)->occluder_capacity = jo::self<jo::Scene>(this)->occluder_total + 8;
            jo::self<jo::Scene>(this)->occluders = static_cast<jo::Occluder*>(thiscall<void*>(
                fn::resize_58eb30, kMemory, static_cast<void*>(jo::self<jo::Scene>(this)->occluders), static_cast<std::uint32_t>(jo::self<jo::Scene>(this)->occluder_capacity) * 0x94u,
                str(d::s_d_ww_en3d_scene_cpp), static_cast<std::int32_t>(global<std::int16_t>(d::g_a17454)) + 4));  // 0x0058eb30
        }
        game_memset(jo::view<std::uint8_t*>(jo::self<jo::Scene>(this)->occluders) + static_cast<std::uint32_t>(jo::self<jo::Scene>(this)->occluder_total) * 0x94u, 0, 0x94u);
        const std::int32_t index = jo::self<jo::Scene>(this)->occluder_total;
        jo::self<jo::Scene>(this)->occluder_total = index + 1;
        return index;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("Scene::create_sector", 0x005ea460, &H::add_node);
E3D("Scene::create_occluder", 0x005eeca0, &H::add_entry);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/en3d_b82_09.cpp (en3d/b82_09)
//
//      0x005edfd0 Scene::free_c5()              - 0x006356c0(0) + virtual delete per entry.
//
//      0x005ea5e0 Scene::remove_group(g)        - delete_30, deleting dtor, array erase +0x2c.
//
//      0x005eb700 Scene::remove_light(l)        - deleting dtor, array erase +0x54.
//
//      0x005ea6e0 Scene::group(i)               - +0x2c[i - 1] for 1 <= i <= +0x24, else 0.
//
//      0x005ea910 SceneSub-array (0x38) vector deleting destructor (dtor 0x005eb150).
//
//      0x005eb7d0 Light scalar deleting destructor (dtor 0x005c5870).
//
//      0x005ea9b0 trivial constructor (returns this).
//
// 0x005eb700 leave the caller's eax when the list is empty (count < 1); the replacements
namespace {
namespace en3d_b82_09 {

using joan::orig::helpers::at;

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kArrays(d::g_misc);

JOAN_COUNTER(c_free_c5, "Scene::free_c5");
JOAN_COUNTER(c_remove_group, "Scene::remove_group");
JOAN_COUNTER(c_remove_light, "Scene::remove_light");

struct H {
    // 0x005edfd0 Scene::free_c5(). ret 0, eax = this.
    void* free_c5() {
        c_free_c5.hit();
        if (jo::view<void*>(jo::self<jo::Scene>(this)->materials) == nullptr) return this;
        for (std::int32_t i = 0; i < jo::self<jo::Scene>(this)->material_count; ++i) {
            void* const e = jo::view<void**>(jo::self<jo::Scene>(this)->materials)[i];
            joan::orig::as_billboard(e)->release(0u);  // 0x006356c0
            if (e != nullptr) {
                const std::uintptr_t slot0 = (*static_cast<std::uintptr_t**>(e))[0];
                thiscall<void*>(slot0, e, 1);  // virtual deleting destructor
            }
        }
        joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Scene>(this)->materials)), joan::orig::arg::str(img::str(kSceneCpp)), joan::orig::arg::i32(line_base(d::g_a17450) + 0xc));  // 0x0058ed70
        jo::self<jo::Scene>(this)->materials = nullptr;
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->material_count) = 0;
        return this;
    }

    // 0x005ea5e0 Scene::remove_group(g). ret 4.
    std::int32_t remove_group(void* g) {
        c_remove_group.hit();
        std::int32_t i = 1;
        for (; i <= jo::self<jo::Scene>(this)->sector_count; ++i) {
            if (jo::view<void**>(jo::self<jo::Scene>(this)->sectors)[i - 1] != g) continue;
            joan::orig::as_scene(this)->delete_30();  // 0x005eb030
            if (g != nullptr) joan::orig::as_scene_group(g)->destroy(1u);  // 0x005e9fc0
            joan::orig::as_misc(img::obj(kArrays))->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Scene>(this)->sectors)), joan::orig::arg::i32(jo::self<jo::Scene>(this)->sector_count), static_cast<std::int32_t>(sizeof(jo::SceneSPtr)), joan::orig::arg::i32(i));  // 0x00591070
            jo::self<jo::Scene>(this)->sector_count = jo::self<jo::Scene>(this)->sector_count - 1;
            return jo::self<jo::Scene>(this)->sector_count;
        }
        return i;
    }

    // 0x005eb700 Scene::remove_light(l). ret 4.
    std::int32_t remove_light(void* l) {
        c_remove_light.hit();
        std::int32_t i = 1;
        for (; i <= jo::self<jo::Scene>(this)->light_count; ++i) {
            if (jo::view<void**>(jo::self<jo::Scene>(this)->lights)[i - 1] != l) continue;
            if (l != nullptr) joan::orig::as_light(l)->scalar_deleting_destructor(1u);  // 0x005eb7d0
            joan::orig::as_misc(img::obj(kArrays))->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Scene>(this)->lights)), joan::orig::arg::i32(jo::self<jo::Scene>(this)->light_count), static_cast<std::int32_t>(sizeof(jo::LightPtr)), joan::orig::arg::i32(i));  // 0x00591070
            jo::self<jo::Scene>(this)->light_count = jo::self<jo::Scene>(this)->light_count - 1;
            return jo::self<jo::Scene>(this)->light_count;
        }
        return i;
    }

    // 0x005ea6e0 Scene::group(i). ret 4.
    void* group(std::int32_t i) {
        if (i > 0 && i <= jo::self<jo::Scene>(this)->sector_count) return jo::view<void**>(jo::self<jo::Scene>(this)->sectors)[i - 1];
        return nullptr;
    }

    // 0x005ea910 vector deleting destructor (elements 0x38, dtor 0x005eb150). ret 4.
    void* sub_vector_delete(std::uint32_t flags) {
        if ((flags & 2) != 0) {
            joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(this)), static_cast<std::uint32_t>(sizeof(jo::SectorNode)), joan::orig::arg::i32(at<std::int32_t>(this, -4)), fn::code_value(fn::Scene_clear_34_4));  // eh vector destructor iterator (0x38-byte nodes on x86; count: the array cookie)
            void* const block = static_cast<std::uint8_t*>(static_cast<void*>(this)) - 4;
            if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(block));  // operator delete
            return block;
        }
        joan::orig::as_scene(this)->clear_34_4();  // 0x005eb150
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }

    // 0x005eb7d0 Light scalar deleting destructor. ret 4.
    void* light_delete(std::uint32_t flags) {
        joan::orig::as_shadow_model(this)->dtor();  // 0x005c5870
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }

    // 0x005ea9b0 trivial constructor. ret 0.
    void* trivial_ctor() { return this; }
};

#define B82_09(name, addr, fn) JOAN_REPLACE("en3d/b82_09", name, addr, fn)
B82_09("Scene::remove_all_billboard", 0x005edfd0, &H::free_c5);
B82_09("Scene::del_sector", 0x005ea5e0, &H::remove_group);
B82_09("Scene::del_light", 0x005eb700, &H::remove_light);
B82_09("Scene::group", 0x005ea6e0, &H::group);
B82_09("SceneSub::vector_deleting_destructor", 0x005ea910, &H::sub_vector_delete);
B82_09("Light::scalar_deleting_destructor", 0x005eb7d0, &H::light_delete);
B82_09("SceneSub20::ctor", 0x005ea9b0, &H::trivial_ctor);
#undef B82_09

}  // namespace en3d_b82_09
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
//
//    - small: Obj::get_4 0x005c5be0, Obj::call_5e8ab0 0x005e61a0, Obj::get_24 0x005ea6c0,
//      init_5eb450 0x005eb450, Obj::get_byte_58 0x005f4440, Obj::get_36 0x005f50e0,
//      table_a17ee4 0x00600c00 (cdecl), init_603550 0x00603550, Obj::nop_603ad0 0x00603ad0,
//      init_60baf0 0x0060baf0.
namespace {
namespace game_b104_10 {


// ---------------------------------------------------------------------------------------
// small functions
// ---------------------------------------------------------------------------------------
std::uintptr_t JOAN_CDECL init_5eb450() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_plane_equation(obj(d::g_a69a98))->init_425700()); }  // 0x00425700

struct H {
    u32 get_24() { return jo::view<u32>(jo::self<jo::Scene>(this)->sector_count); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("Obj::get_24_5ea6c0", 0x005ea6c0, &H::get_24);
B("init_5eb450", 0x005eb450, &init_5eb450);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers23)
//
//  * module en3d/helpers23: Scene::build_tree (0x005ea730), Camera::water_receivers (0x00617370).
namespace {
namespace en3d_helpers22 {

using joan::orig::helpers::at;

constexpr img::Addr kMemory(d::g_mem);
void* mem_alloc(std::uint32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058e7b0
}
void mem_free(void* p, const char* file, std::int32_t line) { joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

JOAN_COUNTER(c_build_tree, "Scene::build_tree");

struct H {
    // --- en3d/helpers23 ---------------------------------------------------------------------
    // 0x005ea730 Scene::build_tree: sector tree (+0x30, 2n-1 nodes of 0x38) from the n sectors
    // (+0x24, +0x2c) through 0x005ea9d0, with the globals 0xa69a90 / 0xa69aac as scratch.
    void scene_build_tree() {
        c_build_tree.hit();
        if (jo::view<void*>(jo::self<jo::Scene>(this)->tree) != nullptr || !(jo::self<jo::Scene>(this)->sector_count > 0)) return;
        const char* const file = &img::global<const char>(d::s_d_ww_en3d_scene_cpp);  // "d:\ww\en3d\scene.cpp"
        global<void**>(d::g_a69a90) = static_cast<void**>(mem_alloc(static_cast<std::uint32_t>(jo::self<jo::Scene>(this)->sector_count * static_cast<std::int32_t>(sizeof(jo::SceneSPtr))), file, line_base(d::g_a1743c) + 5));
        for (std::int32_t i = 0; i < jo::self<jo::Scene>(this)->sector_count; ++i) global<void**>(d::g_a69a90)[i] = jo::view<void**>(jo::self<jo::Scene>(this)->sectors)[i];
        const std::int32_t nodes = jo::self<jo::Scene>(this)->sector_count * 2 - 1;
        std::uint8_t* const block = static_cast<std::uint8_t*>(game_new(static_cast<std::uint32_t>(nodes * static_cast<std::int32_t>(sizeof(jo::SectorNode)) + 4)));
        std::uint8_t* tree = nullptr;
        if (block != nullptr) {
            at<std::int32_t>(block, 0) = nodes;  // array cookie (4-byte count, our own layout on every architecture)
            joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(block + 4)), static_cast<std::uint32_t>(sizeof(jo::SectorNode)), joan::orig::arg::i32(nodes), fn::code_value(fn::Item_Item_5eb0f0), fn::code_value(fn::Scene_clear_34_4));  // eh vector ctor 0x005eb0f0 / dtor 0x005eb150
            tree = block + 4;
        }
        jo::view<void*>(jo::self<jo::Scene>(this)->tree) = tree;
        global<void*>(d::g_a69aac) = jo::view<void*>(jo::self<jo::Scene>(this)->tree);
        joan::orig::as_scene(this)->build_node(joan::orig::arg::mptr(0), 0, joan::orig::arg::i32(jo::self<jo::Scene>(this)->sector_count));  // 0x005ea9d0
        global<void*>(d::g_a69aac) = nullptr;
        mem_free(global<void*>(d::g_a69a90), file, line_base(d::g_a1743c) + 0x14);
        global<void*>(d::g_a69a90) = nullptr;
    }
};

#define HELPER23(name, addr, fn) JOAN_REPLACE("en3d/helpers23", name, addr, fn)
HELPER23("Scene::build_sector_struct", 0x005ea730, &H::scene_build_tree);
#undef HELPER23

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers25)
//
//  * module en3d/helpers25: SysEn resource list link / unlink, Bone array delete, Scene region
//    query, Plane flags, small stubs; Scene::build_node (0x005ea9d0, recursive sector tree) and
//    the projected-texture matrix (0x005e5660).
namespace {
namespace en3d_helpers25 {


JOAN_COUNTER(c_build_node, "Scene::build_node");

struct H {
    // 0x005ea9d0 Scene::build_node(parent, first, last): node (0xa69aac bump allocator, 0x38
    // bytes: +0 parent, +4 split axis, +8 / +0xc children, +0x10 bounds, +0x28 centre, +0x34
    // leaf sector) over the sectors 0xa69a90[first..last), split at the median centre.
    void* scene_build_node(void*, std::int32_t first, std::int32_t last) {
        c_build_node.hit();
        if (!(last > first)) return nullptr;
        std::uint8_t** const sectors = global<std::uint8_t**>(d::g_a69a90);
        std::int32_t axis = 3;
        float bounds[6];
        std::memcpy(bounds, jo::as_scene_s(sectors[first])->desc, sizeof bounds);  // sector + 8: its box
        float centres[6];
        float tmp[3];
        joan::orig::as_aabb(centres)->set_point(joan::orig::arg::ptr(joan::orig::arg::as<void*>(joan::orig::as_box(jo::as_scene_s(sectors[first])->desc)->centre(joan::orig::arg::mptr(tmp)))));  // 0x0060d430 box = point(centre 0x0060d3a0)
        for (std::int32_t i = first + 1; i < last; ++i) {
            joan::orig::as_box(bounds)->merge(joan::orig::arg::mptr(static_cast<void*>(jo::as_scene_s(global<std::uint8_t**>(d::g_a69a90)[i])->desc)));  // 0x0060d030 union
            float c[3];
            joan::orig::as_aabb(centres)->extend(joan::orig::arg::ptr(joan::orig::arg::as<void*>(joan::orig::as_box(jo::as_scene_s(global<std::uint8_t**>(d::g_a69a90)[i])->desc)->centre(joan::orig::arg::mptr(c)))));  // 0x0060d490 extend
        }
        float centre[3];
        {
            float c[3];
            const float* const p = joan::orig::arg::as<float*>(joan::orig::as_box(centres)->centre(joan::orig::arg::mptr(c)));
            std::memcpy(centre, p, 12);
        }
        const float dx = centres[3] - centres[0];
        const float dy = centres[4] - centres[1];
        const float dz = centres[5] - centres[2];
        if (!(dx < dy) && !(dx < dz))
            axis = 0;
        else if (!(dy < dz))
            axis = 1;
        else
            axis = 2;
        std::uint8_t* const node = global<std::uint8_t*>(d::g_a69aac);
        global<std::uint8_t*>(d::g_a69aac) = node + sizeof(jo::SectorNode);  // 0x38 on x86
        jo::as_sector_node(node)->axis = axis;
        jo::view<void*>(jo::as_sector_node(node)->left) = nullptr;
        jo::view<void*>(jo::as_sector_node(node)->right) = nullptr;
        std::memcpy(jo::as_sector_node(node)->box, bounds, sizeof bounds);  // +0x10
        std::memcpy(jo::as_sector_node(node)->centre, centre, sizeof centre);  // +0x28
        if (last - first == 1) {
            jo::view<std::uint8_t*>(jo::as_sector_node(node)->sector) = global<std::uint8_t**>(d::g_a69a90)[first];
            jo::view<std::uint8_t*>((jo::as_scene_s(global<std::uint8_t**>(d::g_a69a90)[first]))->node) = node;
            return node;
        }
        std::int32_t mid = 0;
        if (!(dx < 0.0f || dx > 0.0f) && !(dy < 0.0f || dy > 0.0f) && !(dz < 0.0f || dz > 0.0f)) {
            mid = (first + last) / 2;
        } else {
            static const std::uintptr_t kCompare[3] = {fn::code_value(fn::Scene_compare_x), fn::code_value(fn::Scene_compare_y), fn::code_value(fn::Scene_compare_z)};
            if (axis <= 2) {
                std::int32_t scratch;
                mid = first + joan::orig::arg::as<std::int32_t>(joan::orig::scene::partition(joan::orig::arg::mptr(global<std::uint8_t**>(d::g_a69a90) + first), joan::orig::arg::i32(last - first), static_cast<std::int32_t>(sizeof(void*)), joan::orig::arg::mptr(static_cast<void*>(centre)), reinterpret_cast<void*>(kCompare[axis]), &scratch));
            }
        }
        jo::view<void*>(jo::as_sector_node(node)->left) = joan::orig::arg::as<void*>(joan::orig::as_scene(this)->build_node(joan::orig::arg::mptr(static_cast<void*>(node)), joan::orig::arg::i32(first), joan::orig::arg::i32(mid)));
        if (jo::view<void*>(jo::as_sector_node(node)->left) != nullptr) jo::view<void*>(jo::as_sector_node(node)->left->parent) = node;
        jo::view<void*>(jo::as_sector_node(node)->right) = joan::orig::arg::as<void*>(joan::orig::as_scene(this)->build_node(joan::orig::arg::mptr(static_cast<void*>(node)), joan::orig::arg::i32(mid), joan::orig::arg::i32(last)));
        if (jo::view<void*>(jo::as_sector_node(node)->right) != nullptr) jo::view<void*>(jo::as_sector_node(node)->right->parent) = node;
        return node;
    }
};

#define HELPER25(name, addr, fn) JOAN_REPLACE("en3d/helpers25", name, addr, fn)
HELPER25("Scene::build_sector_struct_recur", 0x005ea9d0, &H::scene_build_node);
#undef HELPER25

}  // namespace en3d_helpers25
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
//
//    - Scene::partition (0x005eadd0, en3d/scene.cpp): moves the elements with
//      cmp(e, key) >= 0 to the end (swapping with the last), returns the remaining count.
namespace {
namespace game_leaves80b {

JOAN_COUNTER(c_partition, "Scene::partition");

// 0x005eadd0 Scene::partition(base, count, size, key, cmp, tmp): every element with
// cmp(e, key) >= 0 is swapped with the last one (through tmp) and the count shrinks.
using PartitionCmp = std::int32_t(JOAN_CDECL*)(void*, void*);
std::int32_t JOAN_CDECL scene_partition(std::uint8_t* base, std::int32_t count, std::int32_t size, void* key, PartitionCmp cmp,
                                     void* tmp) {
    c_partition.hit();
    std::int32_t i = 0;
    while (i < count) {
        std::uint8_t* const e = base + i * size;
        if (cmp(e, key) >= 0) {
            std::uint8_t* const last = base + (count - 1) * size;
            if (last != e) {
                std::memcpy(joan::orig::arg::mptr(tmp), static_cast<void*>(e), joan::orig::arg::u32(size));  // memcpy
                std::memcpy(static_cast<void*>(e), static_cast<void*>(last), joan::orig::arg::u32(size));
                std::memcpy(static_cast<void*>(last), joan::orig::arg::ptr(tmp), joan::orig::arg::u32(size));
            }
            --count;
        } else {
            ++i;
        }
    }
    return count;
}

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Scene::partition", 0x005eadd0, &scene_partition);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
//
//                 - selection callbacks of 0x005eadd0 (0x5eaec0 itself is unreferenced).
//
//      0x005eb4d0 compare_plane_distance (cdecl(elem**, elem**)) - qsort callback, via 0x0060da80.
namespace {
namespace game_b102_11 {


// Shared body of 0x005eaec0 / 0x005eaf30 / 0x005eafb0: centre of (*elem)+8 (0x0060d3a0) minus key.
i32 compare_axis(void** elem, const float* key, i32 axis) {
    float centre[3];
    const float* const c = joan::orig::arg::as<float*>(joan::orig::as_box(&jo::as_scene_s(*elem)->desc)->centre(joan::orig::arg::mptr(centre)));  // 0x0060d3a0
    const float d = c[axis] - key[axis];
    // test ah,0x41 jne: 1 only when d > 0; then test ah,1: -1 when d < 0 or unordered.
    if (d > 0.0f) return 1;
    if (!(d >= 0.0f)) return -1;
    return 0;
}
i32 JOAN_CDECL compare_x(void** elem, const float* key) { return compare_axis(elem, key, 0); }  // 0x005eaec0
i32 JOAN_CDECL compare_y(void** elem, const float* key) { return compare_axis(elem, key, 1); }  // 0x005eaf30
i32 JOAN_CDECL compare_z(void** elem, const float* key) { return compare_axis(elem, key, 2); }  // 0x005eafb0

// 0x005eb4d0 (cdecl)
i32 JOAN_CDECL compare_plane_distance(void** pa, void** pb) {
    void* const a = *pa;
    void* const b = *pb;
    const float da = joan::orig::arg::as<float>(joan::orig::box_plane_distance(joan::orig::arg::mptr(obj(d::g_a69a98)), joan::orig::arg::mptr(&jo::as_scene_s(a)->desc), joan::orig::arg::mptr(static_cast<void*>(nullptr))));  // 0x0060da80
    const float db = joan::orig::arg::as<float>(joan::orig::box_plane_distance(joan::orig::arg::mptr(obj(d::g_a69a98)), joan::orig::arg::mptr(&jo::as_scene_s(b)->desc), joan::orig::arg::mptr(static_cast<void*>(nullptr))));  // 0x0060da80
    // test ah,1: -1 when da < db or unordered; test ah,0x41 jne: 1 only when da > db.
    if (!(da >= db)) return -1;
    if (da > db) return 1;
    return 0;
}

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Scene::compare_x", 0x005eaec0, &compare_x);
B11("Scene::compare_y", 0x005eaf30, &compare_y);
B11("Scene::compare_z", 0x005eafb0, &compare_z);
B11("Scene::compare_plane_distance", 0x005eb4d0, &compare_plane_distance);
#undef B11

}  // namespace game_b102_11
}  // namespace

// ---------------------------------------- from modules/game_b111_10.cpp (game/b111_10)
//
//      0x005eb090 Group::set_all_df(char)          (thiscall, ret 4)    - dead code (no references)
namespace {
namespace game_b111_10 {


struct H {

    // ------------------------------------------------------------------ 0x005eb090
    // Dead code (no code / data reference): the eax the original leaves (the caller's eax or the
    // final counter) has no reader. 0x005f0340 reads its argument as a byte.
    void set_all_df(u8 value) {
        for (i32 i = 0; i < jo::self<jo::Scene>(this)->sector_count; ++i) joan::orig::as_scene_s(jo::view<void*>(jo::self<jo::Scene>(this)->sectors[i].p))->set_casts_shadows(static_cast<std::uint8_t>(joan::orig::arg::u32(value)));  // 0x005f0340
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b111_10", name, addr, fn)
B("Group::set_all_df_5eb090", 0x005eb090, &H::set_all_df);
#undef B

}  // namespace game_b111_10
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
//
//      0x005eb150 Scene::clear_34_4()               - +0x34->+4 = 0 when +0x34
namespace {
namespace en3d_b83_03 {


struct H {
    // 0x005eb150 (eax = this)
    void* clear_34_4() {
        if (jo::view<void*>(jo::self<jo::SectorNode>(this)->sector) != nullptr) jo::self<jo::SectorNode>(this)->sector->node = nullptr;
        return this;
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("Scene::clear_34_4", 0x005eb150, &H::clear_34_4);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/en3d_helpers3.cpp (en3d/helpers3)
namespace {
namespace en3d_helpers3 {


struct Vec3 {
    float x, y, z;
};

struct H {

    // --- Vec3 / box (min xyz, max xyz) --------------------------------------------------------
    H* vec3_copy(const Vec3* v) {  // 0x005edce0
        *reinterpret_cast<Vec3*>(jo::self<jo::Vec3f>(this)) = *v;
        return this;
    }

    // 0x005eb190: tree node (+0x08 / +0x0c children, +0x10 box, +0x34 sector): append the sector
    // to the visible list (0xa69a94, count 0xa69aa8) when its box is in view and not occluded.
    // camera == null: everything below counts as visible.
    void node_collect(void* camera) {
        std::int32_t visible;
        if (camera == nullptr)
            visible = 2;
        else
            visible = joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(camera)->box_in_view(joan::orig::arg::mptr(jo::self<jo::SectorNode>(this)->box))) != 0 &&  // 0x006156b0
                              joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(camera)->box_occluded(joan::orig::arg::mptr(jo::self<jo::SectorNode>(this)->box))) == 0  // 0x00616030
                          ? 1
                          : 0;
        if (visible == 0) return;
        if (void* const sector = jo::view<void*>(jo::self<jo::SectorNode>(this)->sector); sector && joan::orig::arg::as<std::int32_t>(joan::orig::as_scene_s(sector)->casts_shadows()) != 0) {  // 0x005f0370
            global<void**>(d::g_a69a94)[global<std::int32_t>(d::g_a69aa8)] = sector;
            global<std::int32_t>(d::g_a69aa8) = global<std::int32_t>(d::g_a69aa8) + 1;
        }
        if (visible >= 2) camera = nullptr;
        if (jo::view<void*>(jo::self<jo::SectorNode>(this)->left)) joan::orig::as_sector_node(jo::view<void*>(jo::self<jo::SectorNode>(this)->left))->collect_visible(joan::orig::arg::mptr(camera));
        if (jo::view<void*>(jo::self<jo::SectorNode>(this)->right)) joan::orig::as_sector_node(jo::view<void*>(jo::self<jo::SectorNode>(this)->right))->collect_visible(joan::orig::arg::mptr(camera));
    }
};

#define HELPER3(name, addr, fn) JOAN_REPLACE("en3d/helpers3", name, addr, fn)
HELPER3("Vec3::Vec3", 0x005edce0, &H::vec3_copy);
HELPER3("SectorNode::collect", 0x005eb190, &H::node_collect);
#undef HELPER3

}  // namespace en3d_helpers3
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
//
//    - Tree::collect_5eb2b0 0x005eb2b0 (ret 0xc, dead: only its own recursive calls): when
//      0x0060dfd0(a, this + 0x10): out[(*n)++] = +0x34 if set, recurse into +8 / +0xc.
namespace {
namespace game_b108_04 {


JOAN_COUNTER(c_collect, "Tree::collect_5eb2b0");

struct H {
    // ------------------------------------------------------------------ 0x005eb2b0
    void collect_5eb2b0(void* a, void** out, i32* n) {
        c_collect.hit();
        if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(joan::orig::arg::ptr(a), joan::orig::arg::ptr(jo::self<jo::SectorNode>(this)->box))) == 0) return;  // 0x0060dfd0
        if (jo::self<jo::SectorNode>(this)->sector != nullptr) {
            out[*n] = jo::self<jo::SectorNode>(this)->sector;
            *n = *n + 1;
        }
        if (jo::self<jo::SectorNode>(this)->left != nullptr) joan::orig::as_sector_node(jo::self<jo::SectorNode>(this)->left)->collect_on_segment(joan::orig::arg::mptr(a), joan::orig::arg::mptr(out), joan::orig::arg::mptr(n));
        if (jo::self<jo::SectorNode>(this)->right != nullptr) joan::orig::as_sector_node(jo::self<jo::SectorNode>(this)->right)->collect_on_segment(joan::orig::arg::mptr(a), joan::orig::arg::mptr(out), joan::orig::arg::mptr(n));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Tree::collect_5eb2b0", 0x005eb2b0, &H::collect_5eb2b0);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
//
//    - empty thiscall stubs (never write eax; no references at all - dead code):
//      0x00597870 / 0x005af040 / 0x005b0a10 / 0x005b49e0 / 0x005c50d0 / 0x005e4410 /
//      0x005eb5b0 (ret 0), 0x005a0290 / 0x005ac250 / 0x005af220 / 0x005b0a30 / 0x005eb570
//      (ret 4), 0x0059fda0 / 0x0059fdc0 / 0x005a0250 / 0x005a0270 (ret 0xc).
//
//    - CRT initialiser-table entries (0x009f48fc..0x009f491c) that call their body directly:
//      0x005a80c0 -> 0x005a80e0, 0x005aeec0 -> 0x005aeee0, 0x005aef20 -> 0x005aef40,
//      0x005aef70 -> 0x005aef90, 0x005aefc0 -> 0x005aefe0, 0x005eb430 -> 0x005eb450,
//      0x005f44a0 -> 0x005f44c0, 0x005f4500 -> 0x005f4520 (callee's eax passed on).
namespace {
namespace game_b105_16 {

template <fn::Code Body>
u32 JOAN_CDECL crt_init() {
    return cdecl_call<u32>(Body);  // direct call in the original (no thunk)
}

struct H {
    // ---------------------------------------------------------------- stubs / getters
    void stub0() {}
    void stub1(u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("crt_init_5eb430", 0x005eb430, &crt_init<fn::init_5eb450>);
B("Obj::stub_5eb570", 0x005eb570, &H::stub1);
B("Obj::stub_5eb5b0", 0x005eb5b0, &H::stub0);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {


// Every helper shares one class so `this` is just the object's address.
struct H {
    // --- Scene (en3d/scene.cpp) ----------------------------------------------------------------
    std::uint32_t scene_active() { return 1; }  // 0x005eb8b0
    std::uint32_t scene_shadows_on() {          // 0x005ee0d0
        if (jo::self<jo::Scene>(this)->fog_flags & 0x10) return 1;
        return global<std::int32_t>(d::g_stencil_bits) >= 4 && global<std::uint32_t>(d::g_mission) == 1 &&
                       global<std::int8_t>(d::g_a5d384) != 0
                   ? 1
                   : 0;
    }
    void scene_lights_off() {  // 0x005eebc0
        for (std::int32_t i = 1; i <= jo::self<jo::Scene>(this)->light_count; ++i) {
            void* const light = jo::self<jo::Scene>(this)->lights[i - 1].p;
            if (joan::orig::arg::as<int>(joan::orig::as_light(light)->active()) != 0) joan::orig::as_light(light)->enable();  // active / 0x005c6110
        }
    }
    void scene_lights_on() {  // 0x005eec30
        for (std::int32_t i = jo::self<jo::Scene>(this)->light_count; i > 0; --i) {
            void* const light = jo::self<jo::Scene>(this)->lights[i - 1].p;
            if (joan::orig::arg::as<int>(joan::orig::as_light(light)->active()) != 0) joan::orig::as_light(light)->disable();  // 0x005c6250
        }
    }
};

// 0x005eb470: sort sectors front to back along the view ray (comparator 0x005eb4d0 reads the globals).
void JOAN_CDECL sort_sectors(void** sectors, std::uint32_t count, const std::uint32_t* ray) {
    for (int i = 0; i < 4; ++i) global<std::uint32_t>(d::g_a69a98 + i * 4) = ray[i];
    joan::orig::crt_qsort(sectors, count, static_cast<std::uint32_t>(sizeof(void*)), reinterpret_cast<joan::orig::QsortCmp>(fn::code_value(fn::Scene_compare_plane_distance)));
}

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Scene::can_render", 0x005eb8b0, &H::scene_active);
HELPER("Scene::get_shadow_enabled", 0x005ee0d0, &H::scene_shadows_on);
HELPER("Scene::lights_off", 0x005eebc0, &H::scene_lights_off);
HELPER("Scene::lights_on", 0x005eec30, &H::scene_lights_on);
HELPER("sort_sectors", 0x005eb470, &sort_sectors);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04.cpp (en3d/b86_04)
//
//      0x005eb8d0 Scene::draw(a1, camera) (ret 8) - grows the sector list (+0x34, capacity +0x3c
//                 from +0x24 / +0x28), 0x005ea730, collects the visible sectors (0x005eb190 on
//                 +0x30, global list 0xa69a94 / count 0xa69aa8), camera plane (0x0060d990 from the
//                 bone matrix axis / position), 0x005eb470 sort, camera +0xd4 = 0, 0x005f03b0 each.
namespace {
namespace en3d_b86_04 {


void* const kMemory = obj(d::g_mem);    // allocator (0x0058eb30 realloc, 0x0058ed70 free)

JOAN_COUNTER(c_scene_draw, "Scene::draw");

struct H {
    // 0x005eb8d0
    void scene_draw(void* a1, void* camera) {
        c_scene_draw.hit();
        if (jo::self<jo::Scene>(this)->visible_capacity < jo::self<jo::Scene>(this)->sector_count && jo::self<jo::Scene>(this)->sector_count > 0) {
            jo::self<jo::Scene>(this)->visible_capacity = jo::self<jo::Scene>(this)->sector_capacity;
            jo::view<void*>(jo::self<jo::Scene>(this)->visible) = joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Scene>(this)->visible)), joan::orig::arg::u32(jo::self<jo::Scene>(this)->visible_capacity * static_cast<std::int32_t>(sizeof(jo::SceneSPtr))), str(d::s_d_ww_en3d_scene_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a17444)) + 7));  // 0x0058eb30
        }
        jo::self<jo::Scene>(this)->visible_count = 0;
        joan::orig::as_scene(this)->build_tree();  // 0x005ea730
        global<void*>(d::g_a69a94) = jo::view<void*>(jo::self<jo::Scene>(this)->visible);
        global<std::int32_t>(d::g_a69aa8) = 0;
        if (jo::view<void*>(jo::self<jo::Scene>(this)->tree) != nullptr) {
            if (global<void*>(d::g_a69ab4) != nullptr && camera == global<void*>(d::g_a69ab4)) {
                joan::orig::as_sector_node(jo::view<void*>(jo::self<jo::Scene>(this)->tree))->collect_visible(joan::orig::arg::mptr(global<void*>(d::g_a69ab8)));  // 0x005eb190
            } else {
                joan::orig::as_sector_node(jo::view<void*>(jo::self<jo::Scene>(this)->tree))->collect_visible(joan::orig::arg::mptr(camera));
            }
        }
        jo::self<jo::Scene>(this)->visible_count = global<std::int32_t>(d::g_a69aa8);
        std::uint32_t plane[4];
        joan::orig::as_plane_equation(static_cast<void*>(plane))->init_425700();  // 0x00425700 Plane4 ctor (no-op)
        void* m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_camera(camera)->bone_))->matrix());  // 0x0060ef80 bone matrix
        std::uint32_t axis[3];
        void* const p_axis = joan::orig::arg::as<void*>(joan::orig::matrix_axis_z(static_cast<void*>(axis), joan::orig::arg::ptr(m)));  // 0x0060c220
        m = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_camera(camera)->bone_))->matrix());
        std::uint32_t pos[3];
        void* const p_pos = joan::orig::arg::as<void*>(joan::orig::matrix_position(static_cast<void*>(pos), joan::orig::arg::ptr(m)));  // 0x0060c310
        joan::orig::vec_cross(joan::orig::arg::mptr(static_cast<void*>(plane)), joan::orig::arg::mptr(p_pos), joan::orig::arg::mptr(p_axis));          // 0x0060d990
        joan::orig::sort_sectors(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Scene>(this)->visible)), joan::orig::arg::u32(jo::self<jo::Scene>(this)->visible_count), joan::orig::arg::mptr(static_cast<void*>(plane)));  // 0x005eb470
        jo::as_camera(camera)->i32_d4 = 0;
        for (std::int32_t i = 0; i < jo::self<jo::Scene>(this)->visible_count; ++i) {
            joan::orig::as_scene_s(jo::view<void**>(jo::self<jo::Scene>(this)->visible)[i])->draw(joan::orig::arg::mptr(a1), joan::orig::arg::mptr(camera), 1);  // 0x005f03b0
        }
    }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b86_04", name, addr, fn)
EN3D("Scene::add_billboard", 0x005eb8d0, &H::scene_draw);
#undef EN3D

}  // namespace en3d_b86_04
}  // namespace

// ---------------------------------------- from modules/game_b101_11.cpp (game/b101_11)
//
//    - Picker::scan_5eda20 0x005eda20 (cam), ret 4 / Picker::scan_5f0d40 0x005f0d40 (cam),
//      ret 4: closest-angle search over the object list (+0x4c/+0x54 resp. +0xd2/+0xda) into a
//      record (this+0x85 resp. 0x005edd90(0xa483f8)), then 0x0061c250([0xa52597], id).
//
//  - No direct callers / data references (dead code): 0x005e4c30, 0x005eda20,
//    0x005ef430, 0x005f0d40, 0x005f2470, 0x005f3340, 0x0060c8e0.
//
//  - 0x005eda20 compares the found id with 0x0fffffff (not -1) before calling 0x0061c250, so
//    it calls it with -1 when nothing was found; kept.
namespace {
namespace game_b101_11 {

using joan::call::import_call;

JOAN_COUNTER(c_scan_5eda20, "Picker::scan_5eda20");

// Loop body shared by 0x005eda20 / 0x005f0d40. rec layout: +0 count, +8 count2, +0xc best,
// +0x10 second, +0x14 12-byte vector (0x005edce0), +0x20 16 bytes (0x005edd30).
i32 scan_list(const i32& count, jo::LightPtr* const& list, u8* rec, const u32 (&dir)[3], i32 id) {
    for (i32 i = 1; i <= count; ++i) {
        u8* const e = jo::view<u8*>(list[i - 1].p);
        if (jo::view<i32>(jo::as_light(e)->type) != 1) continue;
        if (joan::orig::arg::as<i32>(joan::orig::as_light(e)->active()) == 0) continue;  // 0x005c6070
        const float d = static_cast<float>(import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(&jo::as_light(e)->d3d_light[16]),
                                           static_cast<const void*>(dir)));  // dot_product
        const float a = joan::orig::arg::as<float>(joan::orig::libc::fabs_f(joan::orig::arg::f32(d)));                 // 0x00427d60
        u32 a_bits;
        std::memcpy(&a_bits, &a, 4);
        if (!(a >= jo::as_light_pick(rec)->best)) {  // test ah,1 je: a < best or unordered
            if (jo::as_light_pick(rec)->count > 0) {
                jo::view<u32>(jo::as_light_pick(rec)->second) = jo::view<u32>(jo::as_light_pick(rec)->best);
                if (jo::as_light_pick(rec)->count2 < 2) jo::as_light_pick(rec)->count2 = jo::as_light_pick(rec)->count2 + 1;
            } else {
                jo::as_light_pick(rec)->count = jo::as_light_pick(rec)->count + 1;
                jo::as_light_pick(rec)->count2 = jo::as_light_pick(rec)->count2 + 1;
            }
            u32 v3[3];
            joan::orig::as_vec3(v3)->construct_copy(joan::orig::arg::mptr(static_cast<void*>(&jo::as_light(e)->d3d_light[16])));  // 0x005edce0
            std::memcpy(jo::as_light_pick(rec)->direction, v3, 12);
            u32 v4[4];
            joan::orig::as_vector4(v4)->assign(joan::orig::arg::mptr(static_cast<void*>(&jo::as_light(e)->d3d_light[1])));  // 0x005edd30
            std::memcpy(&jo::as_light_pick(rec)->colour, v4, 16);
            jo::view<u32>(jo::as_light_pick(rec)->best) = a_bits;
            id = joan::orig::arg::as<i32>(joan::orig::as_light(e)->c6());  // 0x005c6710
        } else if (!(a >= jo::as_light_pick(rec)->second)) {
            jo::view<u32>(jo::as_light_pick(rec)->second) = a_bits;
        }
    }
    return id;
}

struct H {
    // ------------------------------------------------------------------ 0x005eda20
    void scan_5eda20(void* cam) {
        c_scan_5eda20.hit();
        jo::view<u32>(jo::self<jo::Scene>(this)->light_pick.count) = 0;
        jo::self<jo::Scene>(this)->light_pick.u32_4 = 0;
        jo::view<u32>(jo::self<jo::Scene>(this)->light_pick.count2) = 0;
        jo::view<u32>(jo::self<jo::Scene>(this)->light_pick.best) = 0x497423f0u;
        jo::view<u32>(jo::self<jo::Scene>(this)->light_pick.second) = 0x497423f0u;
        u8* const c = joan::orig::arg::as<u8*>(joan::orig::as_camera(cam)->matrix());  // 0x00616e90
        u32 dir[3];
        dir[0] = reinterpret_cast<const u32*>(c)[8];  // the matrix's third row (view direction)
        dir[1] = reinterpret_cast<const u32*>(c)[9];
        dir[2] = reinterpret_cast<const u32*>(c)[10];
        const i32 id = scan_list(jo::self<jo::Scene>(this)->light_count, jo::self<jo::Scene>(this)->lights, reinterpret_cast<u8*>(&jo::self<jo::Scene>(this)->light_pick), dir, -1);
        if (id != 0x0fffffff) {  // sic: not -1
            joan::orig::as_vs_cache(global<void*>(d::g_vs_cache))->set_tint_source(joan::orig::arg::i32(id));  // 0x0061c250
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("Picker::scan_5eda20", 0x005eda20, &H::scan_5eda20);
#undef B

}  // namespace game_b101_11
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x005edd30 V4::assign            (thiscall(src), ret 4).
namespace {
namespace game_b93_03 {

struct H {
    void* v4_assign(const void* src) {  // 0x005edd30
        std::memcpy(this, src, 16);
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("V4::assign", 0x005edd30, &H::v4_assign);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
//
//      0x005edd90 Obj5ed::name               (thiscall) - returns this + 0x85.
namespace {
namespace game_b95_06 {

struct H {
    // ------------------------------------------------------------------------ 0x005edd90
    void* name_85() { return &jo::self<jo::Scene>(this)->light_pick; }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Obj5ed::name", 0x005edd90, &H::name_85);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
//
//  * module game/world1: small leaves - camera-motion state copies, scene / unit list
//    wrappers, string-table lookup (0x005a3090), device lost check (0x005ff420), scalar
//    deleting destructors, the scene's sorted pointer array (0x005eded0), streamed-sound
//    pause / resume (0x0063c6d0, 0x00470c90, 0x004ced90).
namespace {
namespace game_world {


struct H {
    void scene_remove(void* p) {                                                         // 0x005eded0
        if (jo::view<void*>(jo::self<jo::Scene>(this)->materials) == nullptr) return;
        for (std::int32_t i = 0; i < jo::self<jo::Scene>(this)->material_count; ++i) {
            void** const items = jo::view<void**>(jo::self<jo::Scene>(this)->materials);
            if (items[i] != p) continue;
            if (i < jo::self<jo::Scene>(this)->material_count - 1)
                std::memmove(&items[i], &items[i + 1], static_cast<std::size_t>((jo::self<jo::Scene>(this)->material_count - i) * static_cast<std::int32_t>(sizeof(void*)) - static_cast<std::int32_t>(sizeof(void*))));
            jo::self<jo::Scene>(this)->material_count = jo::self<jo::Scene>(this)->material_count - 1;
            return;
        }
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Scene::remove_sorted", 0x005eded0, &H::scene_remove);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/scene_draw.cpp (en3d/scene.cpp)
namespace {
namespace scene_draw {


// --- Scene::draw_shadows (0x005ee1d0, en3d/scene.cpp) --------------------------------
// Stencil shadows: per light (Scene +0x54 lights, +0x4c count; light +0x00 casts shadows),
// each visible sector's shadow volumes are counted into the stencil (0x005f1cc0, left
// original: it reads an uninitialised stack float in its distance cull), then a
// half-transparent black quad over the viewport darkens where the count is above the
// reference. Finally the sectors' receivers (0x005f0f80).
// The original also sums the lights' intensities (sqrt of r^2+g^2+b^2) and derives the
// quad's alpha from them, then overwrites it with 0x80 - that computation has no effect
// and is left out.
struct SceneShadows {
    void draw_shadows(void* ctx, void** sectors, std::int32_t sector_count, std::int32_t stencil_bits);
};
JOAN_COUNTER(c_shadows, "Scene::draw_shadows");

struct ShadowVertex {  // D3DFVF_XYZRHW | DIFFUSE | TEX1 (0x1c bytes)
    float x, y, z, rhw;
    std::uint32_t colour;
    float u, v;
};

void SceneShadows::draw_shadows(void* ctx, void** sectors, std::int32_t sector_count, std::int32_t stencil_bits) {
    c_shadows.hit();
    // [Graphics] Shadows: the original stencil shadows (volumes, the darkening quad and the sector
    // receivers) only at 1; 0 = none, 2 = shadow maps instead (render/shadow_map.hpp).
    if (joan::render::shadow_params().mode != 1) return;
    auto* const self = reinterpret_cast<std::uint8_t*>(this);
    auto* const d = global<joan::d3d8::IDirect3DDevice8*>(d::g_game_device);
    void* const sysen = img::obj(d::g_sys3d);
    d->SetRenderState(52, 1);  // STENCILENABLE
    for (std::uint32_t stage = 0; stage < 4; ++stage) d->SetTexture(stage, nullptr);
    // Viewport quad (camera +0x10 x, +0x14 y, +0x18 right, +0x1c bottom, inclusive).
    ShadowVertex quad[4];
    quad[0].x = static_cast<float>(jo::as_camera(ctx)->view_x1);
    quad[0].y = static_cast<float>(jo::as_camera(ctx)->view_y1);
    quad[1].x = static_cast<float>(jo::as_camera(ctx)->view_x1);
    quad[1].y = static_cast<float>(jo::as_camera(ctx)->view_y2) + 1.0f;
    quad[2].x = static_cast<float>(jo::as_camera(ctx)->view_x2) + 1.0f;
    quad[2].y = static_cast<float>(jo::as_camera(ctx)->view_y1);
    quad[3].x = static_cast<float>(jo::as_camera(ctx)->view_x2) + 1.0f;
    quad[3].y = static_cast<float>(jo::as_camera(ctx)->view_y2) + 1.0f;
    for (ShadowVertex& v : quad) {
        v.z = 1.0f;
        v.rhw = 1.0f;
        v.u = 0.0f;
        v.v = 0.0f;
    }
    void* const saved = joan::orig::arg::as<void*>(joan::orig::as_camera(ctx)->light());  // 0x00616b40
    joan::orig::as_camera(ctx)->set_light(joan::orig::arg::mptr(nullptr));                     // 0x006169d0
    joan::orig::as_fog(jo::as_camera(ctx)->fog)->off();             // 0x005c09a0
    for (std::int32_t i = 1; i <= jo::as_scene(self)->light_count; ++i) {
        void* const light = jo::view<void*>(jo::as_scene(self)->lights[i - 1]);
        if (joan::orig::arg::as<int>(joan::orig::as_light(light)->casts()) == 0 || jo::as_light(light)->type == 0) continue;  // 0x005c6770
        d->SetRenderState(56, 8);           // STENCILFUNC = ALWAYS
        d->SetRenderState(54, 1);           // STENCILZFAIL = KEEP
        d->SetRenderState(53, 1);           // STENCILFAIL = KEEP
        d->SetRenderState(57, 1);           // STENCILREF
        d->SetRenderState(58, 0xffffffff);  // STENCILMASK
        d->SetRenderState(59, 0xffffffff);  // STENCILWRITEMASK
        const bool color_write = (global<std::uint32_t>(d::g_a52490) & 0x80) != 0;  // SysEn caps: COLORWRITEENABLE
        if (color_write) d->SetRenderState(168, 0);
        std::int32_t volumes = 0;
        bool any = false;
        for (std::int32_t j = 1; j <= sector_count; ++j) {
            void* const sector = sectors[j - 1];
            if (joan::orig::arg::as<int>(joan::orig::as_scene_s(sector)->casts_shadows()) == 0) continue;  // 0x005f0370
            std::int32_t n = 0;
            if (joan::orig::arg::as<char>(joan::orig::as_scene_s(sector)->shadow_volumes(joan::orig::arg::mptr(ctx), joan::orig::arg::mptr(light), &n)) != 0) {  // SceneS shadow volumes 0x005f1cc0
                any = true;
                volumes += n;
            }
        }
        if (color_write) d->SetRenderState(168, 0xf);
        if (!any) continue;
        d->SetRenderState(22, 2);  // CULLMODE = CW
        for (ShadowVertex& v : quad) v.colour = 0x80000000u;  // black, alpha 0x80
        joan::orig::as_sys_en(sysen)->set_blend(0xeu);       // SysEn::set_blend
        joan::orig::as_sys_en(sysen)->set_texture_mode(0x20011u);   // SysEn::set_texture_mode
        joan::orig::as_sys_en(sysen)->set_raster(0x10u);      // SysEn::set_raster
        joan::orig::as_sys_en(sysen)->set_flags(0u);         // SysEn::set_flags
        joan::orig::as_sys_en(sysen)->user_vertices(joan::orig::arg::mptr(quad), 4u);    // 0x00600310 user vertices
        joan::orig::as_sys_en(sysen)->apply(0u);         // SysEn::apply
        d->SetRenderState(57, static_cast<std::uint32_t>((1 << ((stencil_bits - 1) & 31)) - volumes));  // STENCILREF
        d->SetRenderState(56, 2);  // STENCILFUNC = LESS
        d->SetRenderState(55, 3);  // STENCILPASS = REPLACE
        d->SetRenderState(53, 3);  // STENCILFAIL = REPLACE
        d->SetRenderState(54, 3);  // STENCILZFAIL = REPLACE
        joan::orig::as_sys_en(sysen)->draw(5, 0u, 2u);  // 0x00600a00 strip, 2 triangles
    }
    joan::orig::as_camera(ctx)->set_light(joan::orig::arg::mptr(saved));
    d->SetRenderState(52, 0);
    for (std::int32_t j = 1; j <= sector_count; ++j)
        if (joan::orig::arg::as<int>(joan::orig::as_scene_s(sectors[j - 1])->casts_shadows()) != 0)
            joan::orig::as_scene_s(sectors[j - 1])->draw_receivers(joan::orig::arg::mptr(ctx), joan::orig::arg::mptr(jo::view<void*>(jo::as_scene(self)->lights)), joan::orig::arg::i32(jo::as_scene(self)->light_count), joan::orig::arg::i32(stencil_bits));  // 0x005f0f80
}

JOAN_REPLACE("en3d/scene.cpp", "Scene::draw_shadows", 0x005ee1d0, &SceneShadows::draw_shadows);

}  // namespace scene_draw
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
//
//    - Scene::each_5eeaf0 0x005eeaf0 (ret 4, no references: dead): 0x005f31c0(a) on every
//      element of +0x2c (count +0x24).
namespace {
namespace game_b108_11 {


struct H {
    // ------------------------------------------------------------------ 0x005eeaf0 (dead)
    void each_5eeaf0(u32 a) {
        for (i32 i = 0; i < jo::self<jo::Scene>(this)->sector_count; ++i) {
            joan::orig::as_scene(jo::view<void**>(jo::self<jo::Scene>(this)->sectors)[i])->update_5f31c0(joan::orig::arg::mptr(a));  // 0x005f31c0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Scene::each_5eeaf0", 0x005eeaf0, &H::each_5eeaf0);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
//
//    - Scene::get 0x005eeb50 (i), ret 4: (+0x54)[i] when 0 <= i < +0x4c, else 0.
namespace {
namespace game_b97_06 {


struct H {
    // 0x005eeb50: a Light* in eax (x64: needs facade Scene::get_5eeb50 returning std::uintptr_t / void*)
    std::uintptr_t get_5eeb50(std::int32_t i) {
        if (i >= 0 && i < jo::self<jo::Scene>(this)->light_count) return reinterpret_cast<std::uintptr_t>(jo::self<jo::Scene>(this)->lights[i].p);
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Scene::get_5eeb50", 0x005eeb50, &H::get_5eeb50);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
//
//  * Obj::flag59 0x005c6790, VbAlloc::reset 0x005e4430, VbAlloc::alloc 0x005e8780 (node
//    from the free list or 0x005e8c40), Obj::get4c 0x005eeba0, get24 0x005f0020 (cdecl).
namespace {
namespace game_b94_06 {


struct H {
    // 0x005eeba0
    std::uint32_t get4c() { return jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->light_count); }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Obj::get4c", 0x005eeba0, &H::get4c);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers_2 {


// Every helper shares one class so `this` is just the object's address.
struct H {

    H* scene_end_frame(void*) {  // 0x005ef380
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->occluder_count_) = 0;
        return this;
    }
    float object_radius_a() { return 0.5f * jo::view<float>(jo::self<jo::Billboard>(this)->u32_15); }  // 0x005ef780
    float object_radius_b() { return 0.5f * jo::view<float>(jo::self<jo::Billboard>(this)->u32_19); }  // 0x005ef7b0
    char object_flag() { return jo::view<char>(jo::self<jo::CloudLayers>(this)->built); }                 // 0x005ef7e0
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Scene::turn_off_occluder", 0x005ef380, &H::scene_end_frame);
HELPER("Object::radius_a", 0x005ef780, &H::object_radius_a);
HELPER("Object::radius_b", 0x005ef7b0, &H::object_radius_b);
HELPER("Object::flag", 0x005ef7e0, &H::object_flag);
#undef HELPER

}  // namespace en3d_helpers_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers31.cpp (en3d/helpers31)
//
//  * module en3d/helpers31:
//      Scene::Scene                (0x005e9a00)  ctor: sub-object at +0x85, then memset 0xd5
//      Scene::reset                (0x005e9ac0)
//      Scene::setup                (0x005e9e50)  clears groups / lights, copies the 24-byte
//                                                 setup block, rebuilds
//      Scene::set_sky              (0x005ea060)
//      Scene::add_light            (0x005eb5d0)  new Light (0xce bytes), appended to +0x54
//      Scene::set_fog_flag         (0x005ee140)  flags +0xbd bit 0x10
//      Scene::set_mode             (0x005ef730)  +0x20
//      Scene::register_material    (0x005eddc0)  1-based index in the +0xc5 list (appends)
//      SceneNode::collect          (0x005eb370)  recursive box query over a binary tree
//      Scene::nop                  (0x005eb590)  empty, ret 4
//      SkyBox::SkyBox              (0x005f5100)  Plane ctor + +0x120 = 0
//      SkyBox::init                (0x005f51b0)
//      SkyBox::build               (0x005f5240)  the 6-face (or 8-vertex cube) sky geometry
namespace {
namespace en3d_helpers31_2 {

using joan::call::thiscall;

struct H {
    // 0x005e9a00 Scene::Scene. ret 0, returns this.
    void* scene_ctor() {
        joan::orig::as_scene_pick(&jo::self<jo::Scene>(this)->light_pick)->construct();  // 0x005e9a50
        std::memset(this, 0, sizeof(jo::Scene));  // 0x007b1690, 0xd5 (x86)
        return this;
    }

    // 0x005e9ac0 Scene::reset. ret 0. (eax = this on exit.)
    void* scene_reset() {
        joan::orig::as_scene(this)->clear();  // 0x005e9b10
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->sector_count) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->light_count) = 0;
        jo::self<jo::Scene>(this)->fog_flags = 0;
        return this;
    }

    // 0x005ef730 Scene::set_mode(value). ret 4.
    void* scene_set_20(std::uint32_t value) {  // eax = this
        jo::self<jo::Scene>(this)->mode = value;
        return this;
    }
};

#define HELPER31(name, addr, fn) JOAN_REPLACE("en3d/helpers31", name, addr, fn)
HELPER31("Scene::Scene", 0x005e9a00, &H::scene_ctor);
HELPER31("Scene::init", 0x005e9ac0, &H::scene_reset);
HELPER31("Scene::set_mode", 0x005ef730, &H::scene_set_20);
#undef HELPER31

}  // namespace en3d_helpers31_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers36.cpp (en3d/helpers36)
//
//      0x005eed70 Scene::count_5c()
//
//      0x005eedc0 Scene::remove_5c(i)           - array erase in the +0x58 list (0x94 bytes)
//
//      0x005e9a90 Scene::~Scene()               - Scene::clear
namespace {
namespace en3d_helpers36_2 {

using joan::call::thiscall;

struct H {
    // 0x005eed70
    std::int32_t scene_count_5c() { return jo::self<jo::Scene>(this)->occluder_total; }

    // 0x005eedc0 Scene::remove_5c(i). ret 4. eax: i when i >= +0x64, else this.
    std::uintptr_t scene_remove_5c(std::int32_t i) {
        joan::orig::as_misc(img::obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(jo::view<void*>(jo::self<jo::Scene>(this)->occluders)), joan::orig::arg::i32(jo::self<jo::Scene>(this)->occluder_total), 0x94, joan::orig::arg::i32(i + 1));  // 0x00591070
        jo::self<jo::Scene>(this)->occluder_total = jo::self<jo::Scene>(this)->occluder_total - 1;
        if (i >= jo::self<jo::Scene>(this)->occluder_count_) return static_cast<std::uint32_t>(i);
        jo::self<jo::Scene>(this)->occluder_count_ = jo::self<jo::Scene>(this)->occluder_count_ - 1;
        return reinterpret_cast<std::uintptr_t>(this);
    }

    // 0x005e9a90 Scene::~Scene(). eax = Scene::clear's.
    std::uintptr_t scene_dtor() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_scene(this)->clear()); }  // 0x005e9b10
};

#define HELPER36(name, addr, fn) JOAN_REPLACE("en3d/helpers36", name, addr, fn)
HELPER36("Scene::count_5c", 0x005eed70, &H::scene_count_5c);
HELPER36("Scene::del_occluder", 0x005eedc0, &H::scene_remove_5c);
HELPER36("Scene::~Scene", 0x005e9a90, &H::scene_dtor);
#undef HELPER36

}  // namespace en3d_helpers36_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers3.cpp (en3d/helpers3)
namespace {
namespace en3d_helpers3_2 {


struct H {
    // --- Scene occluders / sector tree ----------------------------------------------------------
    std::int32_t scene_occluder_count() { return jo::self<jo::Scene>(this)->occluder_count_; }                 // 0x005ef3b0
    std::uint8_t* scene_occluder(std::int32_t i) { return jo::view<std::uint8_t*>(jo::self<jo::Scene>(this)->occluders) + i * 0x94; }  // 0x005eed90
};

#define HELPER3(name, addr, fn) JOAN_REPLACE("en3d/helpers3", name, addr, fn)
HELPER3("Scene::occluder_count", 0x005ef3b0, &H::scene_occluder_count);
HELPER3("Scene::get_occluder", 0x005eed90, &H::scene_occluder);
#undef HELPER3

}  // namespace en3d_helpers3_2
}  // namespace

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
//
//    - Scene::nodes_in_box (0x005ef610): builds the sector tree, then collects the sector nodes
//      overlapping a box into the tree's buffer (+0xc9), count to *out.
namespace {
namespace game_leaves80b_2 {

using joan::call::thiscall;

JOAN_COUNTER(c_nodes_in_box, "Scene::nodes_in_box");

struct H {
    // 0x005ef610
    void** nodes_in_box(void* box, std::int32_t* count) {
        c_nodes_in_box.hit();
        joan::orig::as_scene(this)->build_tree();  // 0x005ea730 build_tree
        *count = 0;
        if (jo::self<jo::Scene>(this)->sector_count == 0 || jo::view<void*>(jo::self<jo::Scene>(this)->tree) == nullptr) return nullptr;
        joan::orig::as_scene_node(jo::view<void*>(jo::self<jo::Scene>(this)->tree))->collect(joan::orig::arg::mptr(box), joan::orig::arg::mptr(count), joan::orig::arg::mptr(jo::self<jo::Scene>(this)->region_buffer));  // 0x005eb370
        return joan::orig::arg::as<void**>(jo::self<jo::Scene>(this)->region_buffer);
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Scene::nodes_in_box", 0x005ef610, &H::nodes_in_box);
#undef L80B

}  // namespace game_leaves80b_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers25)
namespace {
namespace en3d_helpers25_2 {

using joan::hook::global;

using joan::call::thiscall;

JOAN_COUNTER(c_regions, "Scene::regions");

struct H {
    // 0x005ef690 Scene::regions(camera, &count): receiver regions near the camera (sector tree
    // walk 0x005eb190 fills the list +0xc9 and the count 0xa69aa8).
    void* scene_regions(void* camera, std::int32_t* count) {
        c_regions.hit();
        joan::orig::as_scene(this)->build_tree();  // 0x005ea730 build_tree
        *count = 0;
        if (jo::self<jo::Scene>(this)->sector_count == 0 || jo::view<void*>(jo::self<jo::Scene>(this)->tree) == nullptr) return nullptr;
        global<void*>(d::g_a69a94) = jo::self<jo::Scene>(this)->region_buffer;
        global<std::int32_t>(d::g_a69aa8) = 0;
        joan::orig::as_sector_node(jo::view<void*>(jo::self<jo::Scene>(this)->tree))->collect_visible(joan::orig::arg::mptr(camera));  // 0x005eb190
        *count = global<std::int32_t>(d::g_a69aa8);
        return jo::self<jo::Scene>(this)->region_buffer;
    }
};

#define HELPER25(name, addr, fn) JOAN_REPLACE("en3d/helpers25", name, addr, fn)
HELPER25("Scene::regions", 0x005ef690, &H::scene_regions);
#undef HELPER25

}  // namespace en3d_helpers25_2
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
//
//      0x005ef800 SceneNode::SceneNode() - 6 x 0x10 vector ctor at +0x5a (0x00425700), memset 0xe4
namespace {
namespace en3d_b85_04 {

using joan::call::stdcall_call;

void* game_memset(void* p, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x005ef800 (registry name SceneNode::SceneNode): the sector SceneS (0xe4 bytes on x86; six Plane4 clip planes at +0x5a)
    H* scene_node_ctor() {
        joan::orig::vector(joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::SceneS>(this)->clip_planes)), static_cast<std::int32_t>(sizeof(jo::Plane4)), 6, joan::orig::arg::mptr(fn::code_value(fn::Plane4_Plane4_425700)));  // 0x00449af0 vector ctor (0x00425700)
        game_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::SceneS)));  // 0xe4 (x86)
        return this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("SceneNode::SceneNode", 0x005ef800, &H::scene_node_ctor);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/scene_draw.cpp (en3d/scene.cpp)
//
// en3d scene drawing above Plane::draw: Scene::render 0x005ebac0 (en3d/scene.cpp).
//
// Statement-by-statement C++ of the MSVC 6 code (read with re/lift.py, checked
// against the assembly). Other en3d functions are called through their
// incremental-link thunks, so replacements of them are picked up.
namespace {
namespace scene_draw_2 {


JOAN_COUNTER(c_render, "Scene::render");

// Thunks of the functions called (original addresses in comments).
void plane_draw(void* plane, void* ctx, std::int32_t pass, std::uint32_t arg) {
    joan::orig::as_plane(plane)->draw(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), joan::orig::arg::u32(arg));  // Plane::draw 0x005d0570
}
void actor_draw(void* actor, void* ctx, std::int32_t pass) { joan::orig::as_actor(actor)->draw(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass)); }  // 0x005aba20

// --- Scene::render (0x005ebac0, en3d/scene.cpp) --------------------------------------
// Scene fields: +0x18 Plane* sky dome  +0x20 layer-major drawing  +0x24 sector count
//   +0x28 sector capacity  +0x2c Sector** sectors  +0x30 sector tree (no portals)
//   +0x34 Sector** visible  +0x38 visible count  +0x3c visible capacity  +0x68 flag
//   +0x71 Plane* (drawn after the sectors, both passes)  +0x75 / +0x7d actor count / Actor**
//   +0x81 Plane*  +0xb5 tint source  +0xc1 / +0xc5 object count / objects (bounded planes)
// Sector: +0x08 bounds  +0x44 Portal** / +0x48 count  +0x54 references  +0x58 u8 done
//   +0x59 u8 reached  +0x5a clip planes (16 bytes each) / +0xba count  +0xde u8 lit state
// Portal: +0x58 / +0x5c the two sectors  +0x62 u8 handled  +0x63 u8 open  +0x64 side
// Camera (ctx): +0x00 Bone*  +0xa0  +0xa4 position  +0xd4  +0xdc mirror  +0xe0 u8 portals
//   on  +0xe1 Sector* camera sector  +0xe5 u8 / +0xe6 sky height override
struct Scene {
    std::uint32_t render(void* ctx);
};

using joan::call::cdecl_call;
const float* bone_matrix(void* bone) { return joan::orig::arg::as<const float*>(joan::orig::as_bone(bone)->matrix()); }  // 0x0060ef80

// An object with a bounding sphere (0x00635760 update, 0x005ef780 / 0x005ef7b0 radii,
// 0x00635ea0 centre, 0x00635e80 its plane): drawn in pass 1 when the camera sees it.
void draw_bounded(void* object, void* ctx) {
    joan::orig::as_billboard(object)->place(joan::orig::arg::mptr(ctx));
    const float a = joan::orig::arg::as<float>(joan::orig::as_billboard(object)->half_width());
    const float b = joan::orig::arg::as<float>(joan::orig::as_billboard(object)->half_height());
    const float radius = (a <= b || a != a || b != b) ? b : a;  // x87: unordered picks b
    std::uint8_t centre[16];
    const void* const c = joan::orig::arg::as<const void*>(joan::orig::as_billboard(object)->centre(joan::orig::arg::mptr(centre), joan::orig::arg::mptr(ctx)));
    if (joan::orig::arg::as<int>(joan::orig::as_camera(ctx)->sphere_visible(joan::orig::arg::mptr(c), joan::orig::arg::f32(radius))) != 0)  // Camera::sphere_visible 0x00615840
        plane_draw(joan::orig::arg::as<void*>(joan::orig::as_billboard(object)->plane()), ctx, 1, 0);
}

std::uint32_t Scene::render(void* ctx) {
    c_render.hit();
    joan::render::water_scene_begin();  // [Graphics] Water: nothing of this scene drawn by the water pass yet
    auto* const self = reinterpret_cast<std::uint8_t*>(this);
    auto visible_count = [&]() -> std::int32_t& { return jo::as_scene(self)->visible_count; };
    auto visible = [&](std::int32_t i) { return jo::view<void**>(jo::as_scene(self)->visible)[i]; };
    const bool sky_on = global<std::uint32_t>(d::g_a03830) != 0;

    // Sky dome, centred on the camera.
    if (sky_on && jo::view<std::uint32_t>(jo::as_camera(ctx)->params.projection) == 0 && jo::view<void*>(jo::as_scene(self)->sky) &&
        joan::orig::arg::as<int>(joan::orig::as_plane(jo::view<void*>(jo::as_scene(self)->sky))->visible()) != 0 && jo::as_camera(ctx)->mirror == 0) {
        std::uint32_t pos[3];
        joan::orig::matrix_position(joan::orig::arg::mptr(pos), joan::orig::arg::ptr(bone_matrix(joan::orig::arg::as<void*>(joan::orig::as_camera(ctx)->bone()))));  // 0x0060c310, 0x00613de0
        void* const sky_bone = jo::view<void*>(jo::as_scene(self)->sky->bones);
        jo::view<std::uint32_t>(jo::as_bone(sky_bone)->local[12]) = pos[0];
        jo::view<std::uint32_t>(jo::as_bone(sky_bone)->local[13]) = pos[1];
        jo::view<std::uint32_t>(jo::as_bone(sky_bone)->local[14]) = jo::as_camera(ctx)->mirror ? jo::view<std::uint32_t>(jo::as_camera(ctx)->mirror_z) : pos[2];
        joan::orig::as_bone(jo::view<void*>(jo::as_scene(self)->sky->bones))->invalidate();  // 0x0060ee10
        const bool lightning_on = joan::orig::arg::as<int>(joan::orig::as_weather_system(img::obj(d::g_weather_system))->active_lightning()) != 0;  // 0x0068c570
        if ((jo::as_material(jo::view<void*>(jo::as_scene(self)->sky->materials)))->texture_mode == 0x10) {
            if (lightning_on) {
                const std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_table(img::obj(d::g_light_sys))->find_4a9530(joan::orig::arg::str(&img::global<const char>(d::s_Lightning))));  // "Lightning"
                joan::orig::as_vs_cache(global<void*>(d::g_vs_cache))->set_constants(joan::orig::arg::mptr(static_cast<void*>(global<std::uint8_t*>(d::g_a47e10) + (i - 1) * 0x18c + 0x12c)));  // 0x0061d200
            } else {
                joan::orig::as_vs_cache(global<void*>(d::g_vs_cache))->set_constants(joan::orig::arg::mptr(img::obj(d::g_a47f40)));
            }
        }
        plane_draw(jo::view<void*>(jo::as_scene(self)->sky), ctx, 1, 0);
    }
    if (void* const moon = global<void*>(d::g_a483b4)) draw_bounded(moon, ctx);
    if (jo::view<void*>(jo::as_scene(self)->sky) && sky_on && joan::orig::arg::as<char>(joan::orig::as_cloud_layers(img::obj(d::g_cloud_layers))->flag_08()) != 0)  // 0x005ef7e0
        joan::orig::as_cloud_layers(img::obj(d::g_cloud_layers))->draw_follow(joan::orig::arg::mptr(ctx), static_cast<char>(joan::orig::arg::i32(1)));  // 0x00459cd0
    joan::orig::as_weather_system(img::obj(d::g_weather_system))->draw_lightning(joan::orig::arg::mptr(ctx));  // 0x0068c440
    // The registered billboards (unit name labels and health plates, ...): drawn before the world, with
    // their own depth; the screen-space passes (shadows, ambient occlusion, water) leave their pixels alone
    // (render::overlay_begin, not part of the original game).
    joan::render::overlay_begin();
    for (std::int32_t i = 0; i < jo::as_scene(self)->material_count; ++i) draw_bounded(jo::view<void**>(jo::as_scene(self)->materials)[i], ctx);
    joan::render::overlay_end();

    joan::orig::as_camera(ctx)->apply_clip_planes();        // 0x00617af0
    joan::orig::as_scene_s(self)->occluders(joan::orig::arg::mptr(ctx));  // 0x005eee40
    joan::orig::as_scene(self)->lights_off();       // 0x005eebc0 lighting off
    joan::orig::as_scene(self)->set_lit(0u);    // 0x005e8fe0
    if (jo::as_scene(self)->visible_capacity < jo::as_scene(self)->sector_count && jo::as_scene(self)->sector_count > 0) {
        jo::as_scene(self)->visible_capacity = jo::as_scene(self)->sector_capacity;
        jo::view<void**>(jo::as_scene(self)->visible) = thiscall<void**>(
            fn::resize_58eb30, img::obj(d::g_mem), jo::view<void*>(jo::as_scene(self)->visible),
            static_cast<std::uint32_t>(jo::as_scene(self)->visible_capacity * static_cast<std::int32_t>(sizeof(jo::SceneSPtr))), &img::global<const char>(d::s_d_ww_en3d_scene_cpp),
            static_cast<std::int32_t>(global<std::int16_t>(d::g_a17448)) + 0x82);  // Mem::realloc
    }

    // [Graphics] Shadows=2 (render/shadow_map.hpp; not part of the original game): with the game's shadows on, the
    // sun of the shadow map - the scene's first shadow-casting directional light, its direction as
    // ShadowPlane::build (0x005e3040) takes it: the z axis of its bone, or of its orbit angles.
    float sun[3] = {0.0f, 0.0f, 0.0f};
    bool has_sun = false;
    // (Water=1 takes the same sun for its glint, with or without shadows.)
    const bool shadow_sun = joan::render::shadow_params().mode == 2 && joan::orig::arg::as<int>(joan::orig::as_scene(self)->shadows_on()) != 0;
    if (shadow_sun || joan::render::water_params().mode == 1) {
        for (std::int32_t i = 1; i <= jo::as_scene(self)->light_count && !has_sun; ++i) {
            void* const light = jo::view<void*>(jo::as_scene(self)->lights[i - 1]);
            if (joan::orig::arg::as<int>(joan::orig::as_light(light)->casts()) == 0 || jo::as_light(light)->type != 1) continue;  // 0x005c6770
            const auto* const words = static_cast<const std::uint32_t*>(light);
            if (joan::orig::arg::as<void*>(joan::orig::as_light(light)->bone()) != nullptr) {  // 0x005c5be0
                void* const m = joan::orig::arg::as<void*>(joan::orig::as_bone(joan::orig::arg::as<void*>(joan::orig::as_light(light)->bone()))->matrix());  // 0x0060ef80
                joan::orig::matrix_axis_z(static_cast<void*>(sun), joan::orig::arg::ptr(m));  // 0x0060c220
            } else {
                std::uint8_t rot[0x40];
                joan::orig::orbit_matrix(joan::orig::arg::mptr(static_cast<void*>(rot)), joan::orig::arg::f32(words[6]), joan::orig::arg::f32(words[7]), joan::orig::arg::f32(0u), joan::orig::arg::f32(words[8]));  // 0x0060bd00
                joan::orig::matrix_axis_z(static_cast<void*>(sun), static_cast<void*>(rot));  // 0x0060c220
            }
            has_sun = true;
        }
    }
    // Shadow casters just outside the picture (a tree above the top edge whose shadow falls into view):
    // for this render only, the camera's four side planes and its frustum box are moved out by
    // ShadowCasterMargin world units, so the sector and object culling keeps them; the GPU clips what
    // is off screen. Restored after the transparent pass (the game's own visibility tests are unchanged).
    float saved_planes[24], saved_bounds[6];
    bool widened = false;
    if (const float margin = joan::render::shadow_params().caster_margin;
        has_sun && shadow_sun && margin > 0.0f && jo::view<std::uint32_t>(jo::as_camera(ctx)->params.projection) == 0 && jo::as_camera(ctx)->mirror == 0) {
        float* const planes = jo::as_camera(ctx)->frustum_planes;
        float* const bounds = jo::as_camera(ctx)->frustum_bounds;
        std::memcpy(saved_planes, planes, sizeof saved_planes);
        std::memcpy(saved_bounds, bounds, sizeof saved_bounds);
        // The near plane too (casters behind the camera, with the sun behind it); the camera's near-range
        // tests keep the real one (render::near_plane_override). 1 is the far plane (Camera::build_frustum).
        joan::render::NearPlaneOverride& near = joan::render::near_plane_override();
        near.camera = ctx;
        std::memcpy(near.plane, planes, sizeof near.plane);
        for (int p = 0; p < 6; ++p) {
            if (p == 1) continue;
            float* const q = planes + p * 4;
            q[3] += margin * std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);  // inside: n.x + d >= 0
        }
        for (int a = 0; a < 3; ++a) {
            bounds[a] -= margin;
            bounds[3 + a] += margin;
        }
        widened = true;
    }

    // --- visible sectors -------------------------------------------------------------
    visible_count() = 0;
    if (jo::as_camera(ctx)->use_portals != 0) {  // which sector holds the camera
        void* found = nullptr;
        for (std::int32_t i = 0; i < jo::as_scene(self)->sector_count; ++i)
            if (joan::orig::arg::as<char>(joan::orig::as_aabb(&jo::as_scene_s(jo::view<void**>(jo::as_scene(self)->sectors)[i])->desc)->contains(joan::orig::arg::ptr(jo::as_camera(ctx)->params.position))) != 0)  // 0x0060d5f0 contains
                found = jo::view<void**>(jo::as_scene(self)->sectors)[i];
        jo::view<void*>(jo::as_camera(ctx)->portal_sector) = found;
    }
    if (jo::as_camera(ctx)->use_portals == 0 || jo::view<void*>(jo::as_camera(ctx)->portal_sector) == nullptr) {
        // No portals: the sector tree decides (0x005eb190), then ray order (0x005eb470).
        joan::orig::as_scene(self)->build_tree();  // 0x005ea730
        global<void*>(d::g_a69a94) = jo::view<void*>(jo::as_scene(self)->visible);
        global<std::uint32_t>(d::g_a69aa8) = 0;
        if (jo::view<void*>(jo::as_scene(self)->tree)) {
            if (global<void*>(d::g_a69ab4) && ctx == global<void*>(d::g_a69ab4))
                joan::orig::as_sector_node(jo::view<void*>(jo::as_scene(self)->tree))->collect_visible(joan::orig::arg::mptr(global<void*>(d::g_a69ab8)));
            else
                joan::orig::as_sector_node(jo::view<void*>(jo::as_scene(self)->tree))->collect_visible(joan::orig::arg::mptr(ctx));
        }
        visible_count() = global<std::int32_t>(d::g_a69aa8);
        std::uint8_t ray[16];
        joan::orig::as_plane_equation(ray)->init_425700();  // 0x00425700 (constructor, empty)
        std::uint8_t dir[12], pos[12];
        void* const d = joan::orig::arg::as<void*>(joan::orig::matrix_axis_z(joan::orig::arg::mptr(dir), joan::orig::arg::ptr(bone_matrix(jo::view<void*>(jo::as_camera(ctx)->bone_)))));  // 0x0060c220 view axis
        void* const p = joan::orig::arg::as<void*>(joan::orig::matrix_position(joan::orig::arg::mptr(pos), joan::orig::arg::ptr(bone_matrix(jo::view<void*>(jo::as_camera(ctx)->bone_)))));  // 0x0060c310 position
        joan::orig::vec_cross(joan::orig::arg::mptr(ray), joan::orig::arg::mptr(p), joan::orig::arg::mptr(d));                                               // 0x0060d990
        joan::orig::sort_sectors(joan::orig::arg::mptr(jo::view<void*>(jo::as_scene(self)->visible)), joan::orig::arg::u32(visible_count()), joan::orig::arg::mptr(ray));                    // 0x005eb470
    } else {
        // Portals: flood from the camera's sector through open portals, narrowing the clip planes.
        void* start = jo::view<void*>(jo::as_camera(ctx)->portal_sector);
        for (std::int32_t i = 0; i < jo::as_scene(self)->sector_count; ++i) {
            void* const s = jo::view<void**>(jo::as_scene(self)->sectors)[i];
            jo::as_scene_s(s)->u8_58 = 0;
            jo::as_scene_s(s)->planes_ready = 0;
            jo::as_scene_s(s)->portal_refs = 0;
            for (std::int32_t j = 0; j < jo::as_scene_s(s)->item_count; ++j) {
                jo::as_portal(jo::view<void**>(jo::as_scene_s(s)->items)[j])->clipped = 0;
                jo::as_portal(jo::view<void**>(jo::as_scene_s(s)->items)[j])->open = 0;
            }
            if (joan::orig::arg::as<char>(joan::orig::as_aabb(jo::as_scene_s(s)->desc)->contains(joan::orig::arg::ptr(jo::as_camera(ctx)->params.position))) != 0)
                start = s;
        }
        jo::view<void**>(jo::as_scene(self)->visible)[visible_count()] = start;
        jo::view<std::int32_t>(jo::as_scene_s(start)->u32_ba) = joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(ctx)->plane_count());  // 0x006171f0 frustum planes
        for (std::int32_t k = 0; k < jo::view<std::int32_t>(jo::as_scene_s(start)->u32_ba); ++k)
            joan::orig::as_camera(ctx)->plane_6171a0(joan::orig::arg::i32(k), joan::orig::arg::mptr(jo::as_scene_s(start)->clip_planes + (k << 2)));  // 0x006171a0
        jo::as_scene_s(start)->portal_refs = 1;
        jo::as_scene_s(start)->planes_ready = 1;
        ++visible_count();
        joan::orig::as_scene(self)->build_tree();  // 0x005ea730

        auto neighbour = [](void* portal, void* sector) { return joan::orig::arg::as<void*>(joan::orig::as_portal(portal)->other_side(joan::orig::arg::mptr(sector))); };  // 0x005e4460
        auto from_side = [](void* portal, bool second) { return second ? jo::as_portal(portal)->side_b : jo::as_portal(portal)->side_a; };
        // 1. Every sector reachable through a portal facing the camera (0x005e44b0).
        for (std::int32_t i = 0; i != visible_count(); ++i) {
            void* const s = visible(i);
            for (std::int32_t j = 0; j < jo::as_scene_s(s)->item_count; ++j) {
                void* const portal = jo::view<void**>(jo::as_scene_s(s)->items)[j];
                void* const other = neighbour(portal, s);
                if (other == nullptr || jo::as_portal(portal)->open != 0) continue;
                if (joan::orig::arg::as<char>(joan::orig::as_portal(portal)->facing(joan::orig::arg::mptr(s), joan::orig::arg::mptr(ctx))) == 0) continue;
                if (jo::as_scene_s(other)->portal_refs == 0) {
                    jo::view<void**>(jo::as_scene(self)->visible)[visible_count()] = other;
                    ++visible_count();
                }
                jo::as_portal(portal)->open = 1;
                jo::as_portal(portal)->from_b = other != jo::as_portal(portal)->side_a ? 1u : 0u;
                ++jo::as_scene_s(other)->portal_refs;
            }
        }
        // 2. Clip each sector's planes through its portals (0x005e54b0 / 0x005e4ee0) until stable,
        //    dropping sectors no open portal reaches.
        std::int32_t reached = 1, resume = 0;
        while (reached < visible_count()) {
            std::int32_t progress = 0;
            std::int32_t i = resume;
            resume = -1;
            while (i < visible_count()) {
                void* const s = visible(i);
                if (jo::as_scene_s(s)->portal_refs == 0) {
                    if (i != visible_count() - 1)
                        std::memmove(jo::view<void**>(jo::as_scene(self)->visible) + i, jo::view<void**>(jo::as_scene(self)->visible) + i + 1,
                                     static_cast<std::size_t>(visible_count() - 1 - i) * sizeof(void*));
                    --visible_count();
                    for (std::int32_t j = 0; j < jo::as_scene_s(s)->item_count; ++j) {
                        void* const portal = jo::view<void**>(jo::as_scene_s(s)->items)[j];
                        void* const other = neighbour(portal, s);
                        if (other == nullptr || jo::as_portal(portal)->open == 0) continue;
                        if (from_side(portal, jo::as_portal(portal)->from_b != 0) != other) continue;
                        jo::as_portal(portal)->open = 0;
                        --jo::as_scene_s(other)->portal_refs;
                    }
                    ++progress;
                    continue;  // the next sector moved into slot i
                }
                if (jo::as_scene_s(s)->u8_58 == 0 && jo::as_scene_s(s)->planes_ready != 0) {
                    jo::as_scene_s(s)->u8_58 = 1;
                    for (std::int32_t k = 0; k < jo::as_scene_s(s)->item_count; ++k) {
                        void* const portal = jo::view<void**>(jo::as_scene_s(s)->items)[k];
                        if (jo::as_portal(portal)->clipped != 0) continue;
                        void* const other = neighbour(portal, s);
                        if (other == nullptr) continue;
                        if (jo::as_scene_s(other)->planes_ready == 0 && jo::as_portal(portal)->open != 0 &&
                            from_side(portal, jo::as_portal(portal)->from_b != 0) == other) {
                            if (joan::orig::arg::as<char>(joan::orig::as_portal(portal)->inside(joan::orig::arg::mptr(jo::as_scene_s(s)->clip_planes), joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_scene_s(s)->u32_ba)))) != 0) {
                                if (jo::as_scene_s(other)->portal_refs == 1) {
                                    joan::orig::as_portal(portal)->clip(joan::orig::arg::mptr(s), joan::orig::arg::mptr(ctx), joan::orig::arg::mptr(jo::as_scene_s(s)->clip_planes), joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_scene_s(s)->u32_ba)), joan::orig::arg::mptr(jo::as_scene_s(other)->clip_planes), joan::orig::arg::mptr(static_cast<std::uint8_t*>(other) + 0xba));
                                    jo::as_portal(portal)->clipped = 1;
                                    jo::as_scene_s(other)->planes_ready = 1;
                                    ++reached;
                                }
                            } else {
                                jo::as_portal(portal)->open = 0;
                                --jo::as_scene_s(other)->portal_refs;
                            }
                            ++progress;
                            jo::as_portal(portal)->clipped = 1;
                            continue;
                        }
                        if (jo::as_portal(portal)->open != 0 &&
                            from_side(portal, jo::as_portal(portal)->from_b != 1) == other &&
                            jo::as_scene_s(other)->portal_refs == 0) {
                            --jo::as_scene_s(s)->portal_refs;
                            jo::as_portal(portal)->open = 0;
                            ++progress;
                            jo::as_portal(portal)->clipped = 1;
                            continue;
                        }
                        jo::as_scene_s(s)->u8_58 = 0;
                    }
                } else if (jo::as_scene_s(s)->u8_58 == 0 && resume == -1) {
                    resume = i;
                }
                ++i;
            }
            if (progress == 0) {
                void* const s = visible(resume);
                jo::view<std::int32_t>(jo::as_scene_s(s)->u32_ba) = 0;
                jo::as_scene_s(s)->planes_ready = 1;
                ++reached;
                ++resume;
            }
        }
    }

    joan::render::shadow_scene_begin(has_sun && shadow_sun ? sun : nullptr);  // [Graphics] Shadows=2 (see above)

    // --- drawing ---------------------------------------------------------------------------
    if (joan::orig::arg::as<int>(joan::orig::as_scene(self)->shadows_on()) != 0 && global<std::int32_t>(d::g_stencil_bits) >= 4) {  // 0x005ee0d0 shadows on
        auto* const d = global<joan::d3d8::IDirect3DDevice8*>(d::g_game_device);
        d->SetRenderState(52, 1);           // STENCILENABLE
        d->SetRenderState(56, 8);           // STENCILFUNC = ALWAYS
        d->SetRenderState(55, 3);           // STENCILPASS = REPLACE
        d->SetRenderState(53, 1);           // STENCILFAIL = KEEP
        d->SetRenderState(54, 1);           // STENCILZFAIL = KEEP
        d->SetRenderState(58, 0xffffffff);  // STENCILMASK
        d->SetRenderState(59, 0xffffffff);  // STENCILWRITEMASK
    }
    auto use_portals = [&] { return jo::as_camera(ctx)->use_portals != 0 && jo::view<void*>(jo::as_camera(ctx)->portal_sector) != nullptr; };
    // Per-sector setup: its clip planes (0x006170a0), the tint, the lit / unlit state.
    auto begin_sector = [&](void* s, bool tint) {
        if (use_portals())
            for (std::int32_t k = 0; k < jo::view<std::int32_t>(jo::as_scene_s(s)->u32_ba); ++k)
                joan::orig::as_camera(ctx)->push_plane(joan::orig::arg::mptr(jo::as_scene_s(s)->clip_planes + (k << 2)));
        if (tint && global<void*>(d::g_vs_cache) && jo::view<void*>(jo::as_scene(self)->camera))
            joan::orig::as_vs_cache(global<void*>(d::g_vs_cache))->set_tint_source(joan::orig::arg::as<std::int32_t>(joan::orig::as_light(jo::view<void*>(jo::as_scene(self)->camera))->c6()));  // 0x0061c250, 0x005c6710
        if (tint && joan::orig::arg::as<char>(joan::orig::as_scene_s(s)->lit()) != joan::orig::arg::as<char>(joan::orig::as_scene(self)->lit())) {  // 0x005e8f90 / 0x005e8fc0
            if (joan::orig::arg::as<char>(joan::orig::as_scene_s(s)->lit()) != 0)
                joan::orig::as_scene(self)->lights_on();  // 0x005eec30 lighting on
            else
                joan::orig::as_scene(self)->lights_off();  // 0x005eebc0 lighting off
            joan::orig::as_scene(self)->set_lit(joan::orig::arg::u32(joan::orig::arg::as<char>(joan::orig::as_scene_s(s)->lit())));  // 0x005e8fe0
        }
    };
    auto end_sector = [&] {
        if (use_portals()) joan::orig::as_camera(ctx)->clear_planes();  // 0x00617120
    };
    auto draw_lists = [&](void* s, std::int32_t pass, std::int32_t layer) {
        joan::orig::as_scene_s(s)->draw_lists(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), joan::orig::arg::i32(layer));  // SceneS::draw_lists 0x005f07f0
    };

    jo::view<std::uint32_t>(jo::as_camera(ctx)->i32_d4) = 0;
    if (void* const mirror = jo::view<void*>(jo::as_camera(ctx)->reorder)) {
        joan::orig::as_reorder(mirror)->reset_batches();  // 0x005e6430
        if (global<std::uint32_t>(d::g_use_reorder_queue) != 0 || global<std::uint32_t>(d::g_actor_draw_enable) != 0) {
            for (std::int32_t i = 0; i < visible_count(); ++i) {
                begin_sector(visible(i), false);
                joan::orig::as_scene_s(visible(i))->draw(joan::orig::arg::mptr(jo::view<void*>(jo::as_camera(ctx)->reorder)), joan::orig::arg::mptr(ctx), 1);  // 0x005f03b0
                end_sector();
            }
        }
        joan::orig::as_reorder(jo::view<void*>(jo::as_camera(ctx)->reorder))->purge();                // 0x005e64e0
        joan::orig::as_reorder(jo::view<void*>(jo::as_camera(ctx)->reorder))->draw(joan::orig::arg::mptr(self), joan::orig::arg::mptr(ctx), 1);  // 0x005e67b0
        if (global<std::uint32_t>(d::g_use_reorder_queue) == 0 || global<std::uint32_t>(d::g_actor_draw_enable) == 0) {
            for (std::int32_t i = 0; i < visible_count(); ++i) {
                begin_sector(visible(i), true);
                draw_lists(visible(i), 1, 1);
                draw_lists(visible(i), 1, 2);
                draw_lists(visible(i), 1, 3);
                end_sector();
            }
        }
    } else if (jo::as_scene(self)->mode != 0) {
        for (std::int32_t layer = 1; layer <= 3; ++layer)
            for (std::int32_t i = 0; i < visible_count(); ++i) {
                begin_sector(visible(i), true);
                draw_lists(visible(i), 1, layer);
                end_sector();
            }
    } else {
        for (std::int32_t i = 0; i < visible_count(); ++i) {
            begin_sector(visible(i), true);
            for (std::int32_t layer = 1; layer <= 3; ++layer) draw_lists(visible(i), 1, layer);
            end_sector();
        }
    }
    if (joan::orig::arg::as<int>(joan::orig::as_scene(self)->shadows_on()) != 0 && global<std::int32_t>(d::g_stencil_bits) >= 4)
        joan::orig::as_scene(self)->draw_shadows(joan::orig::arg::mptr(ctx), joan::orig::arg::mptr(jo::view<void*>(jo::as_scene(self)->visible)), joan::orig::arg::i32(visible_count()), joan::orig::arg::i32(global<std::int32_t>(d::g_stencil_bits)));  // 0x005ee1d0 shadows
    if (void* const p = jo::view<void*>(jo::as_scene(self)->plane_71); p && joan::orig::arg::as<int>(joan::orig::as_plane(p)->visible()) != 0) plane_draw(p, ctx, 1, 0);
    for (std::int32_t i = 1; i <= jo::as_scene(self)->actor_count; ++i) {
        void* const a = jo::view<void**>(jo::as_scene(self)->actors)[i - 1];
        if (joan::orig::arg::as<int>(joan::orig::as_actor(a)->visible_5ab620()) != 0) actor_draw(a, ctx, 1);  // 0x005ab620
    }
    if (jo::as_scene(self)->u32_68 != 0 && jo::view<void*>(jo::as_scene(self)->plane_81) && joan::orig::arg::as<int>(joan::orig::as_plane(jo::view<void*>(jo::as_scene(self)->plane_81))->visible()) != 0)
        plane_draw(jo::view<void*>(jo::as_scene(self)->plane_81), ctx, 1, 0);
    // [Graphics] Shadows=2: the opaque world is complete - its shadows (not part of the original game).
    joan::render::shadow_opaque_done(jo::as_camera(ctx)->d3d_projection);
    // [Graphics] AmbientOcclusion: the opaque world is complete (not part of the original game).
    joan::render::opaque_scene_done(jo::as_camera(ctx)->d3d_projection);
    // [Graphics] Water=1 (not part of the original game): the modern water over the opaque world.
    joan::render::water_scene_done(jo::as_camera(ctx)->d3d_projection, has_sun ? sun : nullptr);
    if (void* const p = jo::view<void*>(jo::as_scene(self)->plane_71); p && joan::orig::arg::as<int>(joan::orig::as_plane(p)->visible()) != 0) plane_draw(p, ctx, 2, 0);

    // Pass 2 (transparent), back to front.
    jo::view<std::uint32_t>(jo::as_camera(ctx)->i32_d4) = 0;
    if (jo::as_scene(self)->mode != 0) {
        for (std::int32_t layer = 1; layer <= 3; ++layer)
            for (std::int32_t i = visible_count() - 1; i >= 0; --i) {
                begin_sector(visible(i), true);
                draw_lists(visible(i), 2, layer);
                end_sector();
            }
    } else {
        for (std::int32_t i = visible_count() - 1; i >= 0; --i) {
            begin_sector(visible(i), true);
            for (std::int32_t layer = 1; layer <= 3; ++layer) draw_lists(visible(i), 2, layer);
            end_sector();
        }
    }
    for (std::int32_t i = 1; i <= jo::as_scene(self)->actor_count; ++i) {
        void* const a = jo::view<void**>(jo::as_scene(self)->actors)[i - 1];
        if (joan::orig::arg::as<int>(joan::orig::as_actor(a)->visible_5ab620()) != 0) actor_draw(a, ctx, 2);
    }
    // [Graphics] AmbientOcclusion: the transparent world is complete - adds the occlusion of the
    // alpha-tested foliage the game draws in pass 2 (not part of the original game).
    joan::render::transparent_scene_done(jo::as_camera(ctx)->d3d_projection);
    if (widened) {
        std::memcpy(jo::as_camera(ctx)->frustum_planes, saved_planes, sizeof saved_planes);
        std::memcpy(jo::as_camera(ctx)->frustum_bounds, saved_bounds, sizeof saved_bounds);
        joan::render::near_plane_override().camera = nullptr;
    }
    joan::render::water_scene_begin();  // [Graphics] Water: other draws after this scene draw the water planes again
    if (jo::as_camera(ctx)->use_portals != 0)
        for (std::int32_t i = visible_count() - 1; i >= 0; --i) jo::view<std::int32_t>(jo::as_scene_s(visible(i))->u32_ba) = 0;
    if (jo::view<void*>(jo::as_scene(self)->sky) && global<std::uint32_t>(d::g_a03830) != 0 &&
        joan::orig::arg::as<char>(joan::orig::as_cloud_layers(img::obj(d::g_cloud_layers))->flag_08()) != 0)
        joan::orig::as_cloud_layers(img::obj(d::g_cloud_layers))->draw_follow(joan::orig::arg::mptr(ctx), static_cast<char>(joan::orig::arg::i32(2)));
    if (joan::orig::arg::as<char>(joan::orig::as_scene(self)->lit()) == 0) joan::orig::as_scene(self)->lights_on();
    joan::orig::as_scene(self)->end_frame(joan::orig::arg::mptr(ctx));  // 0x005ef380
    joan::orig::as_camera(ctx)->end_clip();        // 0x00617cf0
    return joan::orig::arg::as<std::uint32_t>(joan::orig::as_markers(img::obj(d::g_slots_a3ce90))->draw(joan::orig::arg::mptr(ctx)));  // 0x00487b00
}

JOAN_REPLACE("en3d/scene.cpp", "Scene::render", 0x005ebac0, &Scene::render);

}  // namespace scene_draw_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers36.cpp (en3d/helpers36)
namespace {
namespace en3d_helpers36_3 {

using joan::call::thiscall;

struct H {
    // 0x005e9a50 SceneSub::SceneSub(). ret 0, eax = this.
    void* sub_ctor() {
        joan::orig::as_vector(jo::self<jo::LightPick>(this)->direction)->construct();  // 0x005bd1c0
        joan::orig::as_vector4(&jo::self<jo::LightPick>(this)->colour)->construct_default();  // 0x005ea9b0
        return this;
    }
};

#define HELPER36(name, addr, fn) JOAN_REPLACE("en3d/helpers36", name, addr, fn)
HELPER36("SceneSub::SceneSub", 0x005e9a50, &H::sub_ctor);
#undef HELPER36

}  // namespace en3d_helpers36_3
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

using u32 = std::uint32_t;


struct H {
    // ------------------------------------------------------------------ 0x005ef3d0
    void set_68(u32 v) { jo::self<jo::Scene>(this)->u32_68 = v; }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::set_68", 0x005ef3d0, &H::set_68);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/en3d_b86_04_2.cpp (en3d/b86_04)
namespace {
namespace en3d_b86_04_2 {


struct H {
    // 0x005ef400
    void set_6c(std::uint32_t v) { jo::self<jo::Scene>(this)->u32_6c = v; }
};

#define EN3D(name, addr, fn) JOAN_REPLACE("en3d/b86_04", name, addr, fn)
EN3D("Object::set_6c", 0x005ef400, &H::set_6c);
#undef EN3D

}  // namespace en3d_b86_04_2
}  // namespace

// ---------------------------------------- from modules/game_b101_11_2.cpp (game/b101_11)
namespace {
namespace game_b101_11_2 {

using u32 = std::uint32_t;

using u8 = std::uint8_t;

using joan::hook::global;
using i32 = std::int32_t;

using joan::call::thiscall;
// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

using joan::orig::helpers::device;
void set_rs(u32 state, u32 value) { com_call(device(), 0xc8, state, value); }  // SetRenderState

// Stencil setup shared by 0x005ef430 / 0x005f3340 (n = [0xa5272b] >= 4).
void stencil_states(i32 n) {
    set_rs(0x34, 1);                                         // STENCILENABLE
    com_call(device(), 0x90, 0u, static_cast<void*>(nullptr), 4u, 0u, 0u, 0u);  // Clear(stencil)
    set_rs(0x38, 8);
    set_rs(0x37, 3);
    set_rs(0x36, 1);
    set_rs(0x35, 1);
    set_rs(0x39, 1u << ((static_cast<u32>(n) - 1u) & 31u));  // shl edx,cl
    set_rs(0x3a, 0xffffffffu);
    set_rs(0x3b, 0xffffffffu);
}

JOAN_COUNTER(c_draw_list, "Shadow::draw_list");

struct H {
    // ------------------------------------------------------------------ 0x005ef430
    void draw_list(u32 a, u8** list, i32 n) {
        c_draw_list.hit();
        i32 st = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_scene(this)->shadows_on()) != 0) {  // 0x005ee0d0
            st = global<i32>(d::g_stencil_bits);
            if (st < 4) return;
        }
        if (st != 0) stencil_states(st);
        for (i32 i = 0; i < n; ++i) {
            joan::orig::as_scene(list[i])->update_5f37c0(joan::orig::arg::mptr(a), 1);  // 0x005f37c0
        }
        if (st != 0) {
            joan::orig::as_scene(this)->draw_shadows(joan::orig::arg::mptr(a), joan::orig::arg::mptr(static_cast<void*>(list)), joan::orig::arg::i32(n), joan::orig::arg::i32(st));  // 0x005ee1d0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("Shadow::draw_list", 0x005ef430, &H::draw_list);
#undef B

}  // namespace game_b101_11_2
}  // namespace

// ---------------------------------------- from modules/game_b105_16_2.cpp (game/b105_16)
namespace {
namespace game_b105_16_2 {

using u32 = std::uint32_t;

using u8 = std::uint8_t;


struct H {
    u32 get_20_5ef760() { return jo::self<jo::Scene>(this)->mode; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::get_20_5ef760", 0x005ef760, &H::get_20_5ef760);
#undef B

}  // namespace game_b105_16_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Scene::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers31_2::H::scene_ctor>(this)); }  // 0x5e9a00
auto joan::orig::ScenePick::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36_3::H::sub_ctor>(this)); }  // 0x5e9a50
auto joan::orig::Scene::dtor() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers36_2::H::scene_dtor>(this)); }  // 0x5e9a90
auto joan::orig::Scene::reset() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers31_2::H::scene_reset>(this)); }  // 0x5e9ac0
auto joan::orig::Scene::clear() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::en3d_helpers36::H::scene_clear>(this)); }  // 0x5e9b10
auto joan::orig::Scene::setup(void* a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::en3d_helpers31::H::scene_setup>(this, a0)); }  // 0x5e9e50
auto joan::orig::SceneGroup::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::group_delete>(this, a0)); }  // 0x5e9fc0
auto joan::orig::SkyBox::scalar_deleting_destructor(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::sky_delete>(this, a0)); }  // 0x5ea010
auto joan::orig::Scene::set_sky(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers31::H::scene_set_sky>(this, a0)); }  // 0x5ea060
auto joan::orig::Scene::set_1c(void* a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::en3d_helpers36::H::scene_set_1c>(this, a0)); }  // 0x5ea0d0
auto joan::orig::Scene::free_71() -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::en3d_helpers36::H::scene_free_71>(this)); }  // 0x5ea180
auto joan::orig::Scene::add_actor(void* a0) -> void { joan::bind::member<&::game_b92_04::H::add_actor>(this, a0); }  // 0x5ea1f0
auto joan::orig::List::remove_5ea2f0(void* a0) -> void { joan::bind::member<&::game_b90_04::H::list_remove>(this, a0); }  // 0x5ea2f0
auto joan::orig::Scene::release_7d() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::scene_release_7d>(this)); }  // 0x5ea390
auto joan::orig::List::count_5ea3f0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_04::H::list_count>(this)); }  // 0x5ea3f0
auto joan::orig::Scene::actor_at(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_03::H::at1>(this, a0)); }  // 0x5ea410
auto joan::orig::Scene::add_node() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01::H::add_node>(this)); }  // 0x5ea460
auto joan::orig::Scene::remove_group(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b82_09::H::remove_group>(this, a0)); }  // 0x5ea5e0
auto joan::orig::Scene::get_24() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_10::H::get_24>(this)); }  // 0x5ea6c0
auto joan::orig::Scene::group(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::group>(this, a0)); }  // 0x5ea6e0
auto joan::orig::Scene::build_tree() -> void { joan::bind::member<&::en3d_helpers22::H::scene_build_tree>(this); }  // 0x5ea730
auto joan::orig::SectorNode::vector_destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::sub_vector_delete>(this, a0)); }  // 0x5ea910
auto joan::orig::Vector4::construct_default() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::trivial_ctor>(this)); }  // 0x5ea9b0
auto joan::orig::Scene::build_node(void* a0, std::int32_t a1, std::int32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers25::H::scene_build_node>(this, a0, a1, a2)); }  // 0x5ea9d0
auto joan::orig::Scene::delete_30() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::scene_delete_30>(this)); }  // 0x5eb030
auto joan::orig::Scene::clear_34_4() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_03::H::clear_34_4>(this)); }  // 0x5eb150
auto joan::orig::SectorNode::collect_visible(void* a0) -> void { joan::bind::member<&::en3d_helpers3::H::node_collect>(this, a0); }  // 0x5eb190
auto joan::orig::SectorNode::collect_on_segment(void* a0, void* a1, void* a2) -> void { joan::bind::member<&::game_b108_04::H::collect_5eb2b0>(this, a0, a1, a2); }  // 0x5eb2b0
auto joan::orig::SceneNode::collect(void* a0, void* a1, void* a2) -> void { joan::bind::member<&::en3d_helpers31::H::node_collect>(this, a0, a1, a2); }  // 0x5eb370
auto joan::orig::sort_sectors(void* a0, std::uint32_t a1, void* a2) -> void { joan::bind::free<&::en3d_helpers::sort_sectors>(a0, a1, a2); }  // 0x5eb470
auto joan::orig::Scene::nop(void* a0) -> void { joan::bind::member<&::en3d_helpers31::H::scene_nop>(this, a0); }  // 0x5eb590
auto joan::orig::Scene::add_light() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers31::H::scene_add_light>(this)); }  // 0x5eb5d0
auto joan::orig::Scene::remove_light(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b82_09::H::remove_light>(this, a0)); }  // 0x5eb700
auto joan::orig::Light::scalar_deleting_destructor(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::light_delete>(this, a0)); }  // 0x5eb7d0
auto joan::orig::Scene::delete_lights() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::scene_delete_lights>(this)); }  // 0x5eb820
auto joan::orig::Scene::active() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::scene_active>(this)); }  // 0x5eb8b0
auto joan::orig::Scene::draw(void* a0, void* a1) -> void { joan::bind::member<&::en3d_b86_04::H::scene_draw>(this, a0, a1); }  // 0x5eb8d0
auto joan::orig::Scene::set_camera(void* a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::en3d_helpers36::H::scene_set_camera>(this, a0)); }  // 0x5ed9d0
auto joan::orig::Vec3::construct_copy(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers3::H::vec3_copy>(this, a0)); }  // 0x5edce0
auto joan::orig::Vector4::assign(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_03::H::v4_assign>(this, a0)); }  // 0x5edd30
auto joan::orig::Scene::pick() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_06::H::name_85>(this)); }  // 0x5edd90
auto joan::orig::Scene::register_material(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers31::H::scene_register>(this, a0)); }  // 0x5eddc0
auto joan::orig::Scene::remove_sorted(void* a0) -> void { joan::bind::member<&::game_world::H::scene_remove>(this, a0); }  // 0x5eded0
auto joan::orig::Scene::free_c5() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::free_c5>(this)); }  // 0x5edfd0
auto joan::orig::Scene::shadows_on() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::scene_shadows_on>(this)); }  // 0x5ee0d0
auto joan::orig::Scene::set_fog_flag(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers31::H::scene_set_flag10>(this, a0)); }  // 0x5ee140
auto joan::orig::Scene::get_5eeb50(std::int32_t a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b97_06::H::get_5eeb50>(this, a0)); }  // 0x5eeb50
auto joan::orig::Scene::get_4c() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_06::H::get4c>(this)); }  // 0x5eeba0
auto joan::orig::Scene::lights_off() -> void { joan::bind::member<&::en3d_helpers::H::scene_lights_off>(this); }  // 0x5eebc0
auto joan::orig::Scene::lights_on() -> void { joan::bind::member<&::en3d_helpers::H::scene_lights_on>(this); }  // 0x5eec30
auto joan::orig::Scene::add_entry() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01::H::add_entry>(this)); }  // 0x5eeca0
auto joan::orig::Scene::count_5c() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers36_2::H::scene_count_5c>(this)); }  // 0x5eed70
auto joan::orig::Scene::occluder(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers3_2::H::scene_occluder>(this, a0)); }  // 0x5eed90
auto joan::orig::Scene::remove_5c(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers36_2::H::scene_remove_5c>(this, a0)); }  // 0x5eedc0
auto joan::orig::Scene::end_frame(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers_2::H::scene_end_frame>(this, a0)); }  // 0x5ef380
auto joan::orig::Scene::occluder_count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers3_2::H::scene_occluder_count>(this)); }  // 0x5ef3b0
auto joan::orig::Scene::set_68(std::uint32_t a0) -> void { joan::bind::member<&::game_b96_06::H::set_68>(this, a0); }  // 0x5ef3d0
auto joan::orig::Scene::set_6c(std::uint32_t a0) -> void { joan::bind::member<&::en3d_b86_04_2::H::set_6c>(this, a0); }  // 0x5ef400
auto joan::orig::Scene::nodes_in_box(void* a0, void* a1) -> void** { return joan::bind::conv<void**>(joan::bind::member<&::game_leaves80b_2::H::nodes_in_box>(this, a0, a1)); }  // 0x5ef610
auto joan::orig::Scene::regions(void* a0, void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers25_2::H::scene_regions>(this, a0, a1)); }  // 0x5ef690
auto joan::orig::Scene::set_mode(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers31_2::H::scene_set_20>(this, a0)); }  // 0x5ef730
auto joan::orig::Billboard::half_width() -> float { return joan::bind::conv<float>(joan::bind::member<&::en3d_helpers_2::H::object_radius_a>(this)); }  // 0x5ef780
auto joan::orig::Billboard::half_height() -> float { return joan::bind::conv<float>(joan::bind::member<&::en3d_helpers_2::H::object_radius_b>(this)); }  // 0x5ef7b0
auto joan::orig::CloudLayers::flag_08() -> char { return joan::bind::conv<char>(joan::bind::member<&::en3d_helpers_2::H::object_flag>(this)); }  // 0x5ef7e0
auto joan::orig::SceneNode::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04::H::scene_node_ctor>(this)); }  // 0x5ef800
