// en3d/scene_s.cpp: functions of the original d:\ww\en3d\scene_s.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include "../call.hpp"
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>
#include "../lib/crt/crt.hpp"
#include "../lib/crt/qsort_vc6.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../render/shadow_map.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../render/overrides.hpp"
#include <cmath>
#include <limits>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "shadow_volume_gpu.hpp"
#include "scene_lists.hpp"
#include "../render/grass_batch.hpp"
#include "../game/stream_cache_api.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::call::cdecl_call;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i16 = std::int16_t;
using i32 = std::int32_t;

void* const kHeap = obj(d::g_mem);

using joan::orig::helpers::op_new;

using Device = joan::d3d8::IDirect3DDevice8;

Device* game_device() { return global<Device*>(d::g_game_device); }
constexpr img::Addr kSysEn(d::g_sys3d);

using joan::call::import_call;

using joan::orig::helpers::op_delete;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
//
//      0x005ef900 SceneSource::clear()              - 0x005eff10 / 0x005f0270, frees +0xda /
//                 +0xc6 table, deletes +0x38 / +0x40 / +0x3c
//
//      0x005efbe0 SceneSource::set_38(p) (ret 4)    - delete old +0x38 when different
namespace {
namespace en3d_b83_03 {


void* const kMemory = obj(d::g_mem);  // allocator (0x0058e7b0 / 0x0058eb30 / 0x0058ed70)

void mem_free(void* p, const char* file, std::int32_t line) { joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

JOAN_COUNTER(c_source_clear, "SceneSource::clear");

struct H {
    // Scalar delete through 0x00459c40 (thunk 0x0040903e).
    static void delete_object(void* p) {
        if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);
    }

    // 0x005ef900 (eax = this)
    void* source_clear() {
        c_source_clear.hit();
        const char* const kSceneS = str(d::s_d_ww_en3d_scene_s_cpp);  // "d:\ww\en3d\scene_s.cpp"
        joan::orig::as_scene_s(this)->release_actors();  // 0x005eff10
        joan::orig::as_scene_s(this)->clear();  // 0x005f0270
        if (jo::view<void*>(jo::self<jo::SceneS>(this)->lights) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::SceneS>(this)->lights), kSceneS, static_cast<std::int32_t>(global<std::int16_t>(d::g_a17950)) + 0x11);
            jo::view<void*>(jo::self<jo::SceneS>(this)->lights) = nullptr;
        }
        jo::view<std::uint32_t>(jo::self<jo::SceneS>(this)->light_cap) = 0;
        if (jo::view<void*>(jo::self<jo::SceneS>(this)->plane) != nullptr) {
            delete_object(jo::view<void*>(jo::self<jo::SceneS>(this)->plane));
            jo::view<void*>(jo::self<jo::SceneS>(this)->plane) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::SceneS>(this)->plane2) != nullptr) {
            delete_object(jo::view<void*>(jo::self<jo::SceneS>(this)->plane2));
            jo::view<void*>(jo::self<jo::SceneS>(this)->plane2) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::SceneS>(this)->object) != nullptr) {
            void* const inner = jo::view<void*>(jo::self<jo::SceneS>(this)->object->def);
            if (inner != nullptr) joan::orig::as_actor_def(inner)->destroy(1u);  // 0x004b0c20
            void* const outer = jo::view<void*>(jo::self<jo::SceneS>(this)->object);
            if (outer != nullptr) joan::orig::as_scene_object(outer)->destroy(1u);  // 0x00420840
            // +0x3c is not cleared by the original.
        }
        jo::self<jo::SceneS>(this)->u32_ba = 0;
        if (jo::view<void*>(jo::self<jo::SceneS>(this)->nodes) != nullptr) {
            for (std::int32_t i = 0; i < jo::self<jo::SceneS>(this)->node_count; ++i) {
                void* const e = jo::view<void**>(jo::self<jo::SceneS>(this)->nodes)[i];
                if (e != nullptr) joan::orig::as_scene_node(e)->scalar_delete(1u);  // 0x005efb90
            }
            mem_free(jo::view<void*>(jo::self<jo::SceneS>(this)->nodes), kSceneS, static_cast<std::int32_t>(global<std::int16_t>(d::g_a17950)) + 0x2f);
            jo::view<void*>(jo::self<jo::SceneS>(this)->nodes) = nullptr;
        }
        jo::view<std::uint32_t>(jo::self<jo::SceneS>(this)->node_cap) = 0;
        jo::view<std::uint32_t>(jo::self<jo::SceneS>(this)->node_count) = 0;
        return this;
    }

    // 0x005efbe0 (eax = this when unchanged, else p)
    std::uintptr_t set_38(void* p) {
        if (jo::view<void*>(jo::self<jo::SceneS>(this)->plane) == p) return reinterpret_cast<std::uintptr_t>(this);
        delete_object(jo::view<void*>(jo::self<jo::SceneS>(this)->plane));
        jo::view<void*>(jo::self<jo::SceneS>(this)->plane) = p;
        return reinterpret_cast<std::uintptr_t>(p);
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("SceneSector::deinit", 0x005ef900, &H::source_clear);
B83_03("SceneSource::set_38", 0x005efbe0, &H::set_38);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x005f39a0 SceneS::push(p) (ret 4)    - +0xc6 array (+0xc2 count, +0xbe capacity)
//
//      0x005f0270 SceneS::clear()            - deletes the +0xda entries from the back
//
//      0x005efb90 SceneNode scalar deleting destructor (0x005e5570) (ret 4)
namespace {
namespace en3d_b84_01 {


void* const kMemory = obj(d::g_mem);     // allocator (0x0058e7b0 alloc / 0x0058eb30 realloc)

using joan::orig::helpers::game_delete;                      // operator delete

struct H {
    // 0x005f39a0
    void push_scene_s(jo::SceneNode* p) {
        if (!(jo::self<jo::SceneS>(this)->node_count < jo::self<jo::SceneS>(this)->node_cap)) {
            jo::self<jo::SceneS>(this)->node_cap = jo::self<jo::SceneS>(this)->node_count + 5;
            jo::self<jo::SceneS>(this)->nodes = static_cast<jo::SceneNodePtr*>(thiscall<void*>(
                fn::resize_58eb30, kMemory, static_cast<void*>(jo::self<jo::SceneS>(this)->nodes), static_cast<std::uint32_t>(jo::self<jo::SceneS>(this)->node_cap) * static_cast<std::uint32_t>(sizeof(jo::SceneNodePtr)),
                str(d::s_d_ww_en3d_scene_s_cpp), static_cast<std::int32_t>(global<std::int16_t>(d::g_a17968)) + 4));  // 0x0058eb30 scene_s.cpp
        }
        jo::self<jo::SceneS>(this)->nodes[jo::self<jo::SceneS>(this)->node_count].p = p;
        jo::self<jo::SceneS>(this)->node_count = jo::self<jo::SceneS>(this)->node_count + 1;
    }

    // 0x005f0270
    void clear_scene_s() {
        while (jo::self<jo::SceneS>(this)->light_count > 0) {
            void* const e = jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[jo::self<jo::SceneS>(this)->light_count - 1];
            if (e != nullptr) joan::orig::as_light(e)->scalar_deleting_destructor(1u);  // 0x005eb7d0
            jo::self<jo::SceneS>(this)->light_count = jo::self<jo::SceneS>(this)->light_count - 1;
        }
        jo::self<jo::SceneS>(this)->light_count = 0;
    }

    // 0x005efb90
    void* node_delete(std::uint32_t flags) {
        joan::orig::as_tex_projector(this)->destruct();  // 0x005e5570
        if ((flags & 1u) != 0) game_delete(this);
        return this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("SceneSector::add_reflective_plane", 0x005f39a0, &H::push_scene_s);
E3D("SceneSector::del_all_light", 0x005f0270, &H::clear_scene_s);
E3D("SceneNode::scalar_delete", 0x005efb90, &H::node_delete);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx3)
//
//  * module game/fx3: weapon trails (0x00687880 update, 0x006883d0 add node, 0x00688550
//    age, 0x006889c0 build ribbon, Hermite / Catmull-Rom helpers, node list), scene-node
//    linking (0x005efc50 / 0x005efdb0 / 0x005acf10, 0x006891d0).
namespace {
namespace game_fx {


JOAN_COUNTER(c_node_add, "SceneNode::attach");

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // 0x005efc50 Scene::attach(node): unlink from its scene (+0x18; prev +0x20, next +0x24;
    // scene list +0xca tail / +0xce head), link at the head of this one.
    void scene_attach(std::uint8_t* node) {
        c_node_add.hit();
        std::uint8_t* const old = jo::view<std::uint8_t*>(jo::as_actor(node)->scene);
        if (self() != old) {
            ++joan::en3d::g_scene_list_edits;
            joan::en3d::unit_index_touch(old);  // [Debug] UnitIndex
            joan::en3d::unit_index_touch(self());
            if (old != nullptr) {
                std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_actor(node)->scene_prev);
                std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_actor(node)->scene_next);
                if (prev != nullptr) jo::view<std::uint8_t*>(jo::as_actor(prev)->scene_next) = next; else jo::view<std::uint8_t*>(jo::as_scene_s(old)->nodes_head) = next;
                if (next != nullptr) jo::view<std::uint8_t*>(jo::as_actor(next)->scene_prev) = prev; else jo::view<std::uint8_t*>(jo::as_scene_s(old)->nodes_tail) = prev;
                jo::as_actor(node)->scene_prev = nullptr;
                jo::as_actor(node)->scene_next = nullptr;
                jo::as_actor(node)->scene = nullptr;
            }
            if (self() != nullptr) {
                std::uint8_t* const prev = nullptr;
                std::uint8_t* const next = jo::view<std::uint8_t*>(jo::self<jo::SceneS>(this)->nodes_head);
                jo::view<std::uint8_t*>(jo::self<jo::SceneS>(this)->nodes_head) = node;  // (prev is always null)
                if (next != nullptr) jo::view<std::uint8_t*>(jo::as_actor(next)->scene_prev) = node; else jo::view<std::uint8_t*>(jo::self<jo::SceneS>(this)->nodes_tail) = node;
                jo::view<std::uint8_t*>(jo::as_actor(node)->scene_prev) = prev;
                jo::view<std::uint8_t*>(jo::as_actor(node)->scene_next) = next;
                jo::view<std::uint8_t*>(jo::as_actor(node)->scene) = self();
            }
        }
        joan::orig::as_scene_node(node)->set_scene(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x005acf10
    }
    // 0x005efdb0 Scene::detach(node): unlink from its scene (this is not used).
    // eax: node (0x005acf10 with a null scene leaves its `this` in eax).
    std::uintptr_t scene_detach(std::uint8_t* node) {
        std::uint8_t* const old = jo::view<std::uint8_t*>(jo::as_actor(node)->scene);
        if (old != nullptr) {
            ++joan::en3d::g_scene_list_edits;
            joan::en3d::unit_index_touch(old);  // [Debug] UnitIndex
            std::uint8_t* const prev = jo::view<std::uint8_t*>(jo::as_actor(node)->scene_prev);
            std::uint8_t* const next = jo::view<std::uint8_t*>(jo::as_actor(node)->scene_next);
            if (prev != nullptr) jo::view<std::uint8_t*>(jo::as_actor(prev)->scene_next) = next; else jo::view<std::uint8_t*>(jo::as_scene_s(old)->nodes_head) = next;
            if (next != nullptr) jo::view<std::uint8_t*>(jo::as_actor(next)->scene_prev) = prev; else jo::view<std::uint8_t*>(jo::as_scene_s(old)->nodes_tail) = prev;
            jo::as_actor(node)->scene_prev = nullptr;
            jo::as_actor(node)->scene_next = nullptr;
            jo::as_actor(node)->scene = nullptr;
        }
        joan::orig::as_scene_node(node)->set_scene(joan::orig::arg::mptr(static_cast<void*>(nullptr)));
        return reinterpret_cast<std::uintptr_t>(node);
    }
};

#define FX3(name, addr, fn) JOAN_REPLACE("game/fx3", name, addr, fn)
FX3("SceneSector::add_actor", 0x005efc50, &H::scene_attach);
FX3("SceneSector::remove_actor", 0x005efdb0, &H::scene_detach);
#undef FX3

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/en3d_b82_09.cpp (en3d/b82_09)
//
//      0x005eff10 SceneGroup::release_ce()       - unlinks the +0xce chain (next +0x24).
namespace {
namespace en3d_b82_09 {

using joan::orig::helpers::addr_of;

JOAN_COUNTER(c_release_ce, "SceneGroup::release_ce");

struct H {
    // 0x005eff10 SceneGroup::release_ce(). ret 0. eax: the last 0x005acf10 result, or this.
    std::uintptr_t release_ce() {
        c_release_ce.hit();
        ++joan::en3d::g_scene_list_edits;
        joan::en3d::unit_index_touch(this);  // [Debug] UnitIndex
        std::uintptr_t r = addr_of(this);
        void* node = jo::view<void*>(jo::self<jo::SceneS>(this)->nodes_head);
        while (node != nullptr) {
            void* const next = jo::as_actor(node)->scene_next;
            jo::as_actor(node)->scene_prev = nullptr;
            jo::as_actor(node)->scene_next = nullptr;
            r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_scene_node(node)->set_scene(joan::orig::arg::mptr(0)));  // 0x005acf10
            node = next;
        }
        jo::self<jo::SceneS>(this)->nodes_head = nullptr;
        jo::self<jo::SceneS>(this)->nodes_tail = nullptr;
        return r;
    }
};

#define B82_09(name, addr, fn) JOAN_REPLACE("en3d/b82_09", name, addr, fn)
B82_09("SceneSector::remove_all_actor", 0x005eff10, &H::release_ce);
#undef B82_09

}  // namespace en3d_b82_09
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {


// Every helper shares one class so `this` is just the object's address.
struct H {
    // --- SceneS (en3d/scene_s.cpp) ---------------------------------------------------------------
    void* scenes_first_actor() { return jo::view<void*>(jo::self<jo::SceneS>(this)->nodes_tail); }  // 0x005effa0
    std::uint32_t scenes_casts_shadows() {                  // 0x005f0370
        return jo::self<jo::SceneS>(this)->casts_shadows_ != 0 && jo::self<jo::SceneS>(this)->scene != nullptr ? 1 : 0;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("SceneS::first_actor", 0x005effa0, &H::scenes_first_actor);
HELPER("SceneS::casts_shadows", 0x005f0370, &H::scenes_casts_shadows);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
//
//    - get_20_5effd0 0x005effd0 (cdecl, p) -> p->+0x20. Dead code.
namespace {
namespace game_b105_16 {


void* JOAN_CDECL get_20_5effd0(const u8* p) { return jo::as_actor(p)->scene_prev; }  // the previous actor of the scene list

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("get_20_5effd0", 0x005effd0, &get_20_5effd0);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
//
//  * module game/leaves80a:
//      Font::~Font 0x00623e70 (empty), String::c_str 0x00417d90 (returns this),
//      Table::last_slot 0x00417520 (+0x08), SoundEntry::id 0x004d0440 (+0x04),
//      SceneNode::attachments 0x005efff0 (+0xce), Textures::mode / set_mode 0x0060a720 /
//      0x0060a740 (+0x30), File::File 0x004179c0 (+0x105 = -1), Bytes::at 0x0041bf10 (byte
//      [this + i + 1]), SoundList::entry 0x004cf690 (this + i * 0x18 - 4), Sound::play / pause
//      0x0059f620 / 0x0059f650 (OpenAL alSourcePlay / alSourcePause), Reader::done 0x005b4140
//      (byte +1 == 0), Light::params 0x0061ddc0 (copies 0x68 bytes from +0x5e), Actor::position
//      0x005ac270 (bone matrix translation), Loader::Loader 0x00577be0 (zeroed, +0x13c = -1),
//      `vector constructor iterator' 0x00449af0 (stdcall), scalar deleting destructor 0x00459c40
//      (destructor 0x005cc750), Progress::redraw 0x0075f4f0 (Game::draw 0x004d48f0 on the
//      loading screen), Model::update_parts 0x005b77b0, Unit::attach_a / attach_b 0x00421960 /
//      0x005227f0 (adds the +0x4e / +0x4a object to the world's scene list +0x6e),
//      Texture::set_name 0x00605ad0, the float sqrt wrapper 0x004256d0 (CRT sqrt 0x007b2344
//      runs fsqrt under the caller's precision control, so its st0 is exactly a double: the
//      C++ double return is exact; the negative-argument result is the x87 indefinite NaN,
//      which also survives the double).
namespace {
namespace game_leaves80a {


struct H {
    // 0x005efff0 on a SceneS (the sector, size 0xe4): its node list head +0xce; the SceneNode view put the word at
    // +0xce behind two pointers, which x64 widens differently from SceneS's eleven.
    void* attachments() { return jo::self<jo::SceneS>(this)->nodes_head; }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("SceneNode::attachments", 0x005efff0, &H::attachments);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
//
//  * Obj::flag59 0x005c6790, VbAlloc::reset 0x005e4430, VbAlloc::alloc 0x005e8780 (node
//    from the free list or 0x005e8c40), Obj::get4c 0x005eeba0, get24 0x005f0020 (cdecl).
namespace {
namespace game_b94_06 {


// 0x005f0020
void* JOAN_CDECL get24(void* p) { return jo::as_actor(p)->scene_next; }

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("get24", 0x005f0020, &get24);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Scene::new_child 0x005f0040 (ret 0): new 0xce, 0x5c5830, push into +0xda (grow by 8
//      with 0x58eb30 "scene_s.cpp").
//
//  - SEH frames (0x5e6000, 0x5f0040, 0x6035c0, 0x605d80, 0x609620) only matter on exceptions;
//    normal paths reproduced.
namespace {
namespace game_b99_07 {


JOAN_COUNTER(c_new_child, "Scene::new_child");

struct H {
    // ------------------------------------------------------------------ 0x005f0040
    void* new_child() {
        c_new_child.hit();
        void* const p = op_new(static_cast<u32>(sizeof(jo::Light)));  // 0xce on x86
        void* const o = p != nullptr ? joan::orig::as_light(p)->construct() : nullptr;  // 0x005c5830
        if (jo::self<jo::SceneS>(this)->light_count >= jo::self<jo::SceneS>(this)->light_cap) {
            jo::self<jo::SceneS>(this)->light_cap = jo::self<jo::SceneS>(this)->light_count + 8;
            jo::view<void*>(jo::self<jo::SceneS>(this)->lights) = joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SceneS>(this)->lights)), joan::orig::arg::u32(jo::self<jo::SceneS>(this)->light_cap * static_cast<std::int32_t>(sizeof(jo::LightPtr))), str(d::s_d_ww_en3d_scene_s_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a17954)) + 7));  // 0x0058eb30
        }
        jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[jo::self<jo::SceneS>(this)->light_count] = o;
        jo::self<jo::SceneS>(this)->light_count = jo::self<jo::SceneS>(this)->light_count + 1;
        return o;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("SceneSector::create_light", 0x005f0040, &H::new_child);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
//
//      0x005f0190 SceneS::remove_d2(p)      (thiscall, ret 4) - finds p in the +0xda / +0xd2 array,
//                 deletes it (0x005eb7d0, flag 1), removes the slot (0x00591070). Dead code.
//
//      0x005f36a0 SceneS::remove_48(p)      (thiscall, ret 4) - same on +0x44 / +0x48 (0x005f3770).
//                 Dead code. eax of both: see the functions.
//
//      0x005f0780 compare_a8_desc           (cdecl(elem**, elem**)) - qsort callback.
//
//   - 0x005f0190 / 0x005f36a0: when the count is < 1 the original returns the caller's eax
//     (undefined, no callers exist); 1 is returned there.
namespace {
namespace game_b102_11 {


JOAN_COUNTER(c_remove, "SceneS::remove");

// 0x005f0780 (cdecl)
i32 compare_vis_time(float a, float b) {
    const float d = a - b;
    if (!(d >= 0.0f)) return 1;  // test ah,1: less or unordered
    if (d > 0.0f) return -1;     // test ah,0x41 jne -> 0
    return 0;
}
i32 JOAN_CDECL compare_a8_desc(void** pa, void** pb) { return compare_vis_time((jo::as_actor(*pa))->vis_time, (jo::as_actor(*pb))->vis_time); }

}  // namespace game_b102_11
}  // namespace

namespace joan::en3d {
// qsort(actors, n, 4, compare_a8_desc) - the transparent pass's far-to-near order: the same algorithm on (vis_time,
// actor) pairs (lib/crt/qsort_vc6.hpp: same comparisons and swaps, so the same order, ties included), the distances
// read once instead of through two actor pointers per comparison.
void sort_by_vis_time(void** actors, std::int32_t n) {
    struct E {
        float key;
        void* actor;
    };
    static std::vector<E> v;
    v.resize(static_cast<std::size_t>(n > 0 ? n : 0));
    for (std::int32_t i = 0; i < n; ++i) v[static_cast<std::size_t>(i)] = {jo::as_actor(actors[i])->vis_time, actors[i]};
    joan::crt::qsort_vc6(v.data(), static_cast<std::uint32_t>(v.size()), [](const E& a, const E& b) { return ::game_b102_11::compare_vis_time(a.key, b.key); });
    for (std::int32_t i = 0; i < n; ++i) actors[i] = v[static_cast<std::size_t>(i)].actor;
}
}  // namespace joan::en3d

namespace {
namespace game_b102_11 {

struct H {

    // Shared body of 0x005f0190 / 0x005f36a0.
    u32 remove_common(void* p, i32& count, void*& array, std::uintptr_t delete_thunk) {
        c_remove.hit();
        u32 eax = 1;  // undefined in the original when the count is < 1
        for (i32 i = 1; i <= count; ++i) {
            if (static_cast<void**>(array)[i - 1] != p) {
                eax = static_cast<u32>(i + 1);
                continue;
            }
            if (p != nullptr) thiscall<void*>(delete_thunk, p, 1u);  // scalar deleting dtor
            joan::orig::as_misc(obj(d::g_misc))->del_array_rec_591070(joan::orig::arg::str(array), joan::orig::arg::i32(count), static_cast<i32>(sizeof(void*)), joan::orig::arg::i32(i));  // 0x00591070
            count = count - 1;
            return static_cast<u32>(count);
        }
        return eax;
    }
    // 0x005f0190 (ret 4)
    u32 remove_d2(void* p) { return remove_common(p, jo::self<jo::SceneS>(this)->light_count, jo::view<void*>(jo::self<jo::SceneS>(this)->lights), fn::code_value(fn::Light_scalar_deleting_destructor)); }  // 0x005eb7d0
    // 0x005f36a0 (ret 4)
    u32 remove_48(void* p) { return remove_common(p, jo::self<jo::SceneS>(this)->item_count, jo::view<void*>(jo::self<jo::SceneS>(this)->items), fn::code_value(fn::Obj_sdtor_5f3770)); }  // 0x005f3770
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("SceneSector::del_light", 0x005f0190, &H::remove_d2);
B11("SceneS::compare_a8_desc", 0x005f0780, &compare_a8_desc);
B11("SceneSector::remove_portal", 0x005f36a0, &H::remove_48);
#undef B11

}  // namespace game_b102_11
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
//
//      0x005f0310 Obj::byte_df                    (thiscall, ret 0) - al (upper eax = this).
namespace {
namespace game_b103_11 {


struct H {
    // 0x005f0310
    u32 byte_df() { return (joan::call::register_low_bits(this) & 0xffffff00u) | jo::self<jo::SceneS>(this)->casts_shadows_; }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Obj::byte_df", 0x005f0310, &H::byte_df);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
//
//      0x005f0340 Obj::set_flag_df      (thiscall(char), ret 4) - eax = this.
namespace {
namespace game_b93_03 {


struct H {
    void* set_flag_df(u8 v) {  // 0x005f0340
        jo::self<jo::SceneS>(this)->casts_shadows_ = v;
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Obj::set_flag_df", 0x005f0340, &H::set_flag_df);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/en3d_helpers5.cpp (en3d/helpers5)
namespace {
namespace en3d_helpers5 {

constexpr img::Addr kMemory(d::g_mem);

// A light's intensity: (int) sqrt(r^2 + g^2 + b^2) of its colour bytes (+0x34..+0x36), CRT sqrt.
double light_length(const void* light) {
    const std::int32_t r = jo::as_light(light)->colour.r, g = jo::as_light(light)->colour.g, b = jo::as_light(light)->colour.b;
    return joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(r * r + g * g + b * b))));
}

struct ShadowVertex {  // D3DFVF_XYZRHW | DIFFUSE | TEX1 (0x1c bytes)
    float x, y, z, rhw;
    std::uint32_t colour;
    float u, v;
};

JOAN_COUNTER(c_sector_draw, "SceneS::draw");
JOAN_COUNTER(c_receivers, "SceneS::draw_receivers");

// SceneS (en3d/scene_s.cpp): +0x00 Scene*, +0x08 box, +0x38 / +0x40 planes, +0x3c actor,
// +0xd2 light count, +0xda lights, +0xe0 light state.
struct SceneS {
    // 0x005f03b0: register the sector's lights with the light manager, draw its planes (+0x38;
    // +0x40 and the actor +0x3c only with 0xa03834), then its actors (0x005effa0 list, next
    // +0x20) when 0xa69ab0 is set and the box is in front of a clipping camera (+0xcc): pass 1
    // in list order, pass 2 sorted far to near (qsort, comparator 0x005f0780 on actor +0xa8)
    // through the Scene's growable array (+0xcd capacity, +0xd1 entries).
    void draw(void* lights, void* camera, std::int32_t pass) {
        c_sector_draw.hit();
        for (std::int32_t i = 1; i <= jo::self<jo::SceneS>(this)->light_count; ++i) {
            void* const light = jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i - 1];
            jo::as_light(light)->u32_ca = joan::orig::arg::as<std::uint32_t>(joan::orig::as_reorder(lights)->light_slot(joan::orig::arg::i32(jo::as_light(light)->u32_ca), joan::orig::arg::mptr(light)));  // 0x005e76f0
        }
        jo::self<jo::SceneS>(this)->u32_e0 = joan::orig::arg::as<std::uint32_t>(joan::orig::as_reorder(lights)->sector_slot(joan::orig::arg::i32(jo::self<jo::SceneS>(this)->u32_e0), joan::orig::arg::mptr(this)));  // 0x005e7870
        *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_reorder(lights)->state()) = jo::self<jo::SceneS>(this)->u32_e0;                             // 0x005e7b80
        if (global<std::uint32_t>(d::g_use_reorder_queue) != 0) {
            if (jo::view<void*>(jo::self<jo::SceneS>(this)->plane) && joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::SceneS>(this)->plane))->visible()) != 0)  // 0x005d0480
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::SceneS>(this)->plane))->submit_5d35b0(joan::orig::arg::mptr(lights), joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass));                // 0x005d35b0
            if (jo::view<void*>(jo::self<jo::SceneS>(this)->plane2) && global<std::uint32_t>(d::g_draw_overlay_mesh) != 0 && joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::SceneS>(this)->plane2))->visible()) != 0)
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::SceneS>(this)->plane2))->submit_5d35b0(joan::orig::arg::mptr(lights), joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass));
            if (jo::view<void*>(jo::self<jo::SceneS>(this)->object) && global<std::uint32_t>(d::g_draw_overlay_mesh) != 0 && joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(jo::view<void*>(jo::self<jo::SceneS>(this)->object))->visible_5ab620()) != 0)  // 0x005ab620
                joan::orig::as_actor(jo::view<void*>(jo::self<jo::SceneS>(this)->object))->draw_entry(joan::orig::arg::mptr(lights), joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass));  // 0x005abc50
        }
        if (global<std::uint32_t>(d::g_actor_draw_enable) == 0) return;
        if (jo::view<std::int8_t>(jo::as_camera(camera)->u8_cc) != 0 && joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(camera)->box_near(joan::orig::arg::mptr(jo::self<jo::SceneS>(this)->desc))) == 0) return;  // 0x00615d70
        if (pass == 1) {
            for (void* actor = joan::orig::arg::as<void*>(joan::orig::as_scene_s(this)->first_actor()); actor; actor = jo::as_actor(actor)->scene_prev)  // 0x005effa0
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(actor)->visible_5ab470(joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass))) != 0)  // 0x005ab470
                    joan::orig::as_actor(actor)->draw_entry(joan::orig::arg::mptr(lights), joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass));
            return;
        }
        if (pass != 2) return;
        std::int32_t count = 0;
        for (void* actor = joan::orig::arg::as<void*>(joan::orig::as_scene_s(this)->first_actor()); actor; actor = jo::as_actor(actor)->scene_prev) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(actor)->visible_5ab470(joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass))) == 0) continue;
            void* const scene = jo::view<void*>(jo::self<jo::SceneS>(this)->scene);
            if (count >= jo::view<std::int32_t>(jo::as_scene(scene)->u32_cd)) {
                jo::view<std::int32_t>(jo::as_scene(scene)->u32_cd) = jo::view<std::int32_t>(jo::as_scene(scene)->u32_cd) + 0x80;
                const std::int32_t line = global<std::int16_t>(d::g_a17958) + 0xd5;
                jo::view<void**>(jo::as_scene(scene)->buffer_d1) = joan::orig::arg::as<void**>(joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(jo::view<void**>(jo::as_scene(scene)->buffer_d1)), joan::orig::arg::u32(static_cast<std::uint32_t>(jo::view<std::int32_t>(jo::as_scene(scene)->u32_cd)) * static_cast<std::uint32_t>(sizeof(void*))), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_en3d_scene_s_cpp)), joan::orig::arg::i32(line)));  // 0x0058eb30
            }
            jo::view<void**>(jo::self<jo::SceneS>(this)->scene->buffer_d1)[count] = actor;
            ++count;
        }
        joan::en3d::sort_by_vis_time(jo::view<void**>(jo::self<jo::SceneS>(this)->scene->buffer_d1), count);  // qsort(..., compare_a8_desc)
        void** list = jo::view<void**>(jo::self<jo::SceneS>(this)->scene->buffer_d1);
        for (std::int32_t k = count; k > 0; --k, ++list) joan::orig::as_actor(*list)->draw_entry(joan::orig::arg::mptr(lights), joan::orig::arg::mptr(camera), joan::orig::arg::i32(pass));
    }

    // 0x005f0f80: shadow receivers - for each shadow-casting sector light, count the sector's
    // shadow volumes into a cleared stencil (0x005f1cc0) and darken the viewport where the
    // count is above the reference with a black triangle list whose alpha is the light's
    // share: 255 * |light| / lit_sum * (1 - unlit_sum / (unlit_sum + lit_sum)) + 0.5, where the
    // sums are half the scene lights' intensities (+0 != 0: shadow lights) plus this sector's.
    // A light whose volumes fail leaves COLORWRITE off (as the original).
    void draw_receivers(void* camera, void** scene_lights, std::int32_t scene_light_count, std::int32_t stencil_bits) {
        c_receivers.hit();
        if (joan::render::shadow_params().mode != 1) return;  // [Graphics] Shadows: the original stencil shadows only at 1
        Device* const d = game_device();
        void* stack_list[32];
        const std::int32_t needed = scene_light_count + jo::self<jo::SceneS>(this)->light_count;
        void** const list = needed > 32 ? joan::orig::arg::as<void**>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(needed) * static_cast<std::uint32_t>(sizeof(void*))))) : stack_list;
        std::int32_t listed = 0;
        for (std::int32_t i = 1; i <= jo::self<jo::SceneS>(this)->light_count; ++i) {
            void* const light = jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i - 1];
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_light(light)->active()) == 0) continue;  // 0x005c6070
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_light(light)->casts()) == 0) continue;  // 0x005c6770
            list[listed++] = light;
        }
        d->SetRenderState(52, 1);  // STENCILENABLE
        for (std::uint32_t stage = 0; stage < 4; ++stage) d->SetTexture(stage, nullptr);

        // Two triangles over the viewport (camera +0x10 x, +0x14 y, +0x18 right, +0x1c bottom).
        const float left = static_cast<float>(jo::as_camera(camera)->view_x1);
        const float top = static_cast<float>(jo::as_camera(camera)->view_y1);
        const float right = static_cast<float>(jo::as_camera(camera)->view_x2) + 1.0f;
        const float bottom = static_cast<float>(jo::as_camera(camera)->view_y2) + 1.0f;
        ShadowVertex tri[6];
        tri[0].x = left, tri[0].y = top;
        tri[1].x = right, tri[1].y = bottom;
        tri[2].x = right, tri[2].y = top;
        tri[3].x = left, tri[3].y = top;
        tri[4].x = left, tri[4].y = bottom;
        tri[5].x = right, tri[5].y = bottom;
        for (ShadowVertex& v : tri) {
            v.z = 1.0f;
            v.rhw = 1.0f;
            v.u = 0.0f;
            v.v = 0.0f;
        }

        std::int32_t lit = 0, unlit = 0;
        for (std::int32_t i = 0; i < scene_light_count; ++i) {
            const std::int32_t length = static_cast<std::int32_t>(static_cast<std::int64_t>(light_length(scene_lights[i])));  // _ftol
            if (jo::as_light(scene_lights[i])->type == 0)
                unlit += length;
            else
                lit += length;
        }
        lit /= 2;
        unlit /= 2;
        for (std::int32_t i = 0; i < listed; ++i) {
            const std::int32_t length = static_cast<std::int32_t>(static_cast<std::int64_t>(light_length(list[i])));
            if (jo::as_light(list[i])->type == 0)
                unlit += length;
            else
                lit += length;
        }

        void* const saved = joan::orig::arg::as<void*>(joan::orig::as_camera(camera)->light());  // 0x00616b40
        joan::orig::as_camera(camera)->set_light(joan::orig::arg::mptr(nullptr));                     // 0x006169d0
        joan::orig::as_fog(jo::as_camera(camera)->fog)->off();             // 0x005c09a0
        const bool color_write = (global<std::uint32_t>(d::g_a52490) & 0x80) != 0;  // caps: COLORWRITEENABLE
        const std::uint32_t full = 1u << ((stencil_bits - 1) & 31);
        for (std::int32_t i = 1; i <= jo::self<jo::SceneS>(this)->light_count; ++i) {
            void* const light = jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i - 1];
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_light(light)->casts()) == 0 || jo::as_light(light)->type == 0) continue;
            d->Clear(0, nullptr, 4, 0, 0.0f, full);  // stencil
            d->SetRenderState(56, 8);           // STENCILFUNC = ALWAYS
            d->SetRenderState(54, 1);           // STENCILZFAIL = KEEP
            d->SetRenderState(53, 1);           // STENCILFAIL = KEEP
            d->SetRenderState(57, 1);           // STENCILREF
            d->SetRenderState(58, 0xffffffff);  // STENCILMASK
            d->SetRenderState(59, 0xffffffff);  // STENCILWRITEMASK
            if (color_write) d->SetRenderState(168, 0);
            std::int32_t volumes = 0;
            if (joan::orig::arg::as<char>(joan::orig::as_scene_s(this)->shadow_volumes(joan::orig::arg::mptr(camera), joan::orig::arg::mptr(light), &volumes)) == 0) continue;  // 0x005f1cc0
            if (color_write) d->SetRenderState(168, 0xf);
            d->SetRenderState(22, 2);  // CULLMODE = CW
            joan::orig::as_sys_en(img::obj(kSysEn))->set_blend(0xeu);      // SysEn::set_blend
            joan::orig::as_sys_en(img::obj(kSysEn))->set_texture_mode(0x20011u);  // SysEn::set_texture_mode
            // sqrt's st0 result times 255.0 (a double constant), rounded once to the 24-bit precision.
            float share = static_cast<float>(light_length(light) * 255.0);
            share = share / static_cast<float>(lit);
            float rest = static_cast<float>(unlit) / static_cast<float>(unlit + lit);
            rest = 1.0f - rest;
            share = share * rest;
            share = share + 0.5f;
            const std::uint8_t alpha = static_cast<std::uint8_t>(static_cast<std::int64_t>(share));  // _ftol, low byte
            for (ShadowVertex& v : tri) v.colour = static_cast<std::uint32_t>(alpha) << 24;
            joan::orig::as_sys_en(img::obj(kSysEn))->set_blend(0xeu);
            joan::orig::as_sys_en(img::obj(kSysEn))->set_texture_mode(0x20011u);
            joan::orig::as_sys_en(img::obj(kSysEn))->set_raster(0x10u);   // SysEn::set_raster
            joan::orig::as_sys_en(img::obj(kSysEn))->set_flags(0u);      // SysEn::set_flags
            joan::orig::as_sys_en(img::obj(kSysEn))->user_vertices(joan::orig::arg::mptr(tri), 6u);  // 0x00600310 user vertices
            joan::orig::as_sys_en(img::obj(kSysEn))->apply(0u);      // SysEn::apply
            d->SetRenderState(57, full - static_cast<std::uint32_t>(volumes));  // STENCILREF
            d->SetRenderState(56, 2);  // STENCILFUNC = LESS
            d->SetRenderState(55, 3);  // STENCILPASS = REPLACE
            d->SetRenderState(53, 3);  // STENCILFAIL = REPLACE
            d->SetRenderState(54, 3);  // STENCILZFAIL = REPLACE
            d->SetRenderState(23, 5);  // ZFUNC = GREATER
            joan::orig::as_sys_en(img::obj(kSysEn))->draw(4, 0u, 2u);  // 0x00600a00 triangle list, 2 triangles
            d->SetRenderState(23, 4);  // ZFUNC = LESSEQUAL
        }
        if (list != stack_list) joan::crt::operator_delete(joan::orig::arg::mptr(list));  // operator delete
        d->SetRenderState(52, 0);
        joan::orig::as_camera(camera)->set_light(joan::orig::arg::mptr(saved));
        if (color_write) d->SetRenderState(168, 0xf);
    }
};

#define HELPER5(name, addr, fn) JOAN_REPLACE("en3d/helpers5", name, addr, fn)
HELPER5("SceneSector::render_ordered", 0x005f03b0, &SceneS::draw);
HELPER5("SceneS::draw_receivers", 0x005f0f80, &SceneS::draw_receivers);
#undef HELPER5

}  // namespace en3d_helpers5
}  // namespace

// ---------------------------------------- from modules/game_b101_11.cpp (game/b101_11)
//
//    - Picker::scan_5eda20 0x005eda20 (cam), ret 4 / Picker::scan_5f0d40 0x005f0d40 (cam),
//      ret 4: closest-angle search over the object list (+0x4c/+0x54 resp. +0xd2/+0xda) into a
//      record (this+0x85 resp. 0x005edd90(0xa483f8)), then 0x0061c250([0xa52597], id).
//
//    - Mesh::draw_split 0x005f2470 (dev, a, b, verts, idx, nv, ni), ret 0x1c: splits a draw
//      into 0x005e7ec0 calls (< 0x3fff indices per call, index range < 0x1000).
//
//    - Shadow::draw_obj 0x005f3340 (obj, a, b), ret 0xc.
//
//  - No direct callers / data references (dead code): 0x005e4c30, 0x005eda20,
//    0x005ef430, 0x005f0d40, 0x005f2470, 0x005f3340, 0x0060c8e0.
namespace {
namespace game_b101_11 {

using u16 = std::uint16_t;

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

JOAN_COUNTER(c_scan_5f0d40, "Picker::scan_5f0d40");
JOAN_COUNTER(c_draw_split, "Mesh::draw_split");
JOAN_COUNTER(c_draw_obj, "Shadow::draw_obj");

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

// 0x005f2470 helper: 0x005e7ec0 (10 args), ret 0x28
void draw_part(void* dev, void* verts, i32 nv, i32 lo, i32 hi, u16* idx, i32 first, i32 last, i32 tris, joan::orig::Bone* a1,
               u32 a2) {
    joan::orig::as_reorder(dev)->forward(verts, joan::orig::arg::u32(nv), joan::orig::arg::u16(lo), joan::orig::arg::u16(hi), idx, joan::orig::arg::u32(first), joan::orig::arg::u32(last), joan::orig::arg::u32(tris), a1, joan::orig::arg::u32(a2));  // 0x005e7ec0
}

struct H {
    // ------------------------------------------------------------------ 0x005f0d40
    void scan_5f0d40(void* cam) {
        c_scan_5f0d40.hit();
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_scene(obj(d::g_scene))->pick());  // 0x005edd90
        u8* const c = joan::orig::arg::as<u8*>(joan::orig::as_camera(cam)->matrix());                // 0x00616e90
        u32 dir[3];
        dir[0] = reinterpret_cast<const u32*>(c)[8];  // the matrix's third row (view direction)
        dir[1] = reinterpret_cast<const u32*>(c)[9];
        dir[2] = reinterpret_cast<const u32*>(c)[10];
        const i32 id = scan_list(jo::self<jo::SceneS>(this)->light_count, jo::self<jo::SceneS>(this)->lights, rec, dir, -1);
        if (id != -1) {
            joan::orig::as_vs_cache(global<void*>(d::g_vs_cache))->set_tint_source(joan::orig::arg::i32(id));  // 0x0061c250
        }
    }

    // ------------------------------------------------------------------ 0x005f2470
    void draw_split(void* dev, joan::orig::Bone* a1, u32 a2, void* verts, u16* idx, i32 nv, i32 ni) {
        c_draw_split.hit();
        const i32 kMaxV = 0x1000;
        const i32 kMaxI = 0x3fff;
        if (verts == nullptr || idx == nullptr || nv <= 0 || ni <= 0) return;
        if (nv < kMaxV && ni < kMaxI) {
            draw_part(dev, verts, nv, 0, nv, idx, 0, ni, ni / 3, a1, a2);
            return;
        }
        if (nv < kMaxV) {
            i32 start = 0;
            do {
                const i32 cnt = (ni - start > kMaxI) ? kMaxI : ni - start;
                draw_part(dev, verts, nv, 0, nv, idx, start, start + cnt, cnt / 3, a1, a2);
                start += cnt;
            } while (start != ni);
            return;
        }
        i32 start = 0;
        do {
            i32 cnt = 0;
            i32 mn = nv;
            i32 mx = 0;
            for (i32 i = start; i < ni; i += 3) {
                const i32 m0 = mn;
                const i32 m1 = mx;
                for (i32 j = i; j < i + 3; ++j) {
                    const i32 v = static_cast<i32>(idx[j]);
                    if (v < m0) {
                        mn = v;
                    } else if (v > m1) {
                        mx = v;
                    }
                }
                if (mx - mn + 1 < kMaxV) {
                    cnt += 3;
                } else {
                    break;
                }
            }
            draw_part(dev, verts, nv, mn, mx, idx, start, start + cnt, cnt / 3, a1, a2);
            start += cnt;
        } while (start != ni);
    }

    // ------------------------------------------------------------------ 0x005f3340
    void draw_obj(void* o, u32 a, u32 b) {
        c_draw_obj.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_plane(o)->in_pass(joan::orig::arg::u32(b))) == 0) return;  // 0x005ce970
        i32 st = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::SceneS>(this)->scene))->shadows_on()) != 0) {  // 0x005ee0d0
            st = global<i32>(d::g_stencil_bits);
            if (st < 4) return;
        }
        if (st != 0 && jo::self<jo::SceneS>(this)->nodes_tail != nullptr) stencil_states(st);
        joan::orig::as_plane(o)->draw(joan::orig::arg::mptr(a), joan::orig::arg::i32(b), 0u);  // 0x005d0570
        if (st == 0 || jo::self<jo::SceneS>(this)->nodes_tail == nullptr) return;
        const i32 n0 = joan::orig::arg::as<i32>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::SceneS>(this)->scene))->get_4c());  // 0x005eeba0
        void** const arr = static_cast<void**>(op_new(static_cast<u32>(n0) * static_cast<u32>(sizeof(void*))));  // the scene's light pointers
        for (i32 i = 0; i < joan::orig::arg::as<i32>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::SceneS>(this)->scene))->get_4c()); ++i) {
            arr[i] = joan::orig::arg::as<void*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::SceneS>(this)->scene))->get_5eeb50(joan::orig::arg::i32(i)));  // 0x005eeb50
        }
        const i32 n1 = joan::orig::arg::as<i32>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::SceneS>(this)->scene))->get_4c());
        joan::orig::as_scene_s(this)->draw_receivers(joan::orig::arg::mptr(a), joan::orig::arg::mptr(static_cast<void*>(arr)), joan::orig::arg::i32(n1), joan::orig::arg::i32(st));  // 0x005f0f80
        op_delete(arr);
        set_rs(0x34, 0);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_11", name, addr, fn)
B("Picker::scan_5f0d40", 0x005f0d40, &H::scan_5f0d40);
B("Mesh::draw_split", 0x005f2470, &H::draw_split);
B("Shadow::draw_obj", 0x005f3340, &H::draw_obj);
#undef B

}  // namespace game_b101_11
}  // namespace

// ---------------------------------------- from modules/game_b115_13.cpp (fix/uninit)
//
//    - SceneS::shadow_volumes 0x005f1cc0 (thiscall(camera, light, int* count), ret 0xc, returns al;
//      thunk 0x00404962, called by SceneS::draw_receivers 0x005f0f80 / Scene shadow pass, both
//      C++ now). For every actor of the sector (0x005effa0, next +0x20) that is drawable
//      (0x005ad030, 0x005ab620, plane +0x90 0x005d0480), whose bounding sphere (0x005acc80) with
//      3 x radius is visible to the camera (0x006159f0): distance from the camera position (+0xa4)
//      to the actor's matrix translation (0x0060c310 / 0x0060cc90, v3d length) and a height limit
//      (100; 300 / 0 for a unit whose formation lookup 0x006af310 / 0x00637da0 says so). Unless
//      0xa5b9f8 == 4 the actor is skipped when `z - camera z > limit`. Then either the actor's
//      shadow plane (0x005ad830, near or forced by 0xa58054 / 0xa5272b..) is turned into a
//      stencil shadow volume (0x005e3040) and drawn twice (CULL CW + INCR, CULL CCW + DECR,
//      identity or the plane bone's world matrix), or - within 2000 - the actor's own geometry
//      (0x005d7380) is drawn twice with its materials' op 0x10 / 0x11. Ends with CULL CW; al =
//      something drawn. *count = number of volumes whose 0x005e3040 flag byte was set.
//
//  - 0x005f1cc0 [ebp-0x28]: never written (0x005acc80 fills [ebp-0x24..-0x18] = centre, radius);
//    it is the minuend of `[ebp-0x28] - camera z > limit` that skips the actor. Its value is
//    whatever the caller's earlier calls left 0x38 bytes below the caller's stack pointer. Both
//    callers are C++ replacements now, so the garbage depends on the hook's own compiled frames
//    and cannot be traced to a stable value. As a float, the typical stack contents (pointers
//    0x0012xxxx / 0x00xxxxxx, small integers, 0) are denormals ~ +0, and negative integers are
//    NaN (the compare then also draws); only a leftover genuine float coordinate higher than
//    camera z + limit would skip a shadow. We use 0.0f: the shadow is skipped only for a camera
//    below z = -limit (never in the game - camera z is the eye height above the terrain), i.e.
//    every shadow is drawn, which is how they look today (no flickering shadows are known).
//    (The intended value was probably an actor height such as the sphere top; using it would
//    change which shadows appear for low cameras, so it is not guessed.)
namespace {
namespace game_b115_13 {

// fix/uninit values (see the header).
constexpr float kShadowTopZ = 0.0f;  // 0x005f1cc0 [ebp-0x28]

JOAN_COUNTER(c_shadow, "SceneS::shadow_volumes");

// [Graphics] PerfLog: what the shadow pass spends its time on (per call, summed; logged every 30000 calls, ~7 s).
struct ShadowStats {
    unsigned calls = 0, actors = 0, casting = 0, in_view = 0, volumes = 0, both = 0, meshes = 0, far_sectors = 0;
    void done() {
        if (++calls < 30000 || joan::hook::settings().perf_log <= 0) return;
        joan::hook::log_line("[Graphics] shadow pass (per call, %u calls): %.0f%% sectors skipped as far, %.0f actors walked, %.0f casting + visible, "
                             "%.0f within 2000 and in view, %.1f live volumes (%.1f as one draw), %.1f prebuilt shadow meshes (2 Plane::draw each)",
                             calls, 100.0 * far_sectors / double(calls), actors / double(calls), casting / double(calls), in_view / double(calls),
                             volumes / double(calls), both / double(calls), meshes / double(calls));
        *this = {};
    }
};
ShadowStats g_shadow_stats;

struct H {
    // ---------------------------------------------------------------------------------------
    // 0x005f1cc0 SceneS::shadow_volumes(camera, light, count)
    char shadow_volumes(void* camera, void* light, i32* count) {
        if (jo::as_light(light)->type == 0) return 0;
        c_shadow.hit();
        char drawn = 0;
        *count = 0;
        game_device()->SetRenderState(0x2f, 1);
        // A sector whose box lies farther than 2000 from the camera (+ 1000 for actors reaching out of their sector)
        // holds no actor the pass draws a shadow for (unless the scripted mode below, which ignores the distance):
        // the walk is skipped - most sectors lit by the sun are that far at the widened view distance, and each actor
        // walked costs its bone matrix and an x87 length. (The walk only reads besides the matrices it brings up to
        // date, which are worked out again wherever they are needed.)
        if (!(global<i32>(d::g_stencil_bits) >= 4 && global<i32>(d::g_mission) == 1 && static_cast<i8>(global<u8>(d::g_a5d384)) != 0)) {
            const float* const box = reinterpret_cast<const float*>(&jo::self<jo::SceneS>(this)->desc);
            const float* const eye = jo::as_camera(camera)->params.position;
            float d2 = 0.0f;
            for (int k = 0; k < 3; ++k) {
                const float below = box[k] - eye[k], above = eye[k] - box[k + 3];
                const float out = below > 0.0f ? below : above > 0.0f ? above : 0.0f;
                d2 += out * out;
            }
            if (d2 > 3000.0f * 3000.0f) {  // (NaN compares false: walked)
                ++g_shadow_stats.far_sectors;
                game_device()->SetRenderState(0x16, 2);
                g_shadow_stats.done();
                return 0;
            }
        }
        void* bone = nullptr;  // [ebp-0x48]: written by 0x005ad830 together with [ebp-0x44]
        for (void* actor = joan::orig::arg::as<void*>(joan::orig::as_scene_s(this)->first_actor()); actor != nullptr; actor = jo::as_actor(actor)->scene_prev) {  // 0x005effa0
            ++g_shadow_stats.actors;
            if (joan::orig::arg::as<i32>(joan::orig::as_actor(actor)->flag_fd()) == 0) continue;                // 0x005ad030
            if (joan::orig::arg::as<i32>(joan::orig::as_actor(actor)->visible_5ab620()) == 0) continue;                // 0x005ab620
            if (joan::orig::arg::as<i32>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(actor)->plane))->visible()) == 0) continue;  // 0x005d0480
            ++g_shadow_stats.casting;
            // The distance to the camera first (the original works it out after the view test below): an actor
            // beyond 2000 gets no shadow unless the scripted mode below picks its plane anyway, and everything in
            // between (bounding sphere, view test, the unit's zone) only reads. Most actors a cell walks are that
            // far: the pass walks every cell's actors once per cell lit by the sun (~65 calls a frame).
            float position[3] = {0.0f, 0.0f, 0.0f};  // [ebp-0x30]
            void* const matrix = joan::orig::arg::as<void*>(joan::orig::as_bone(jo::view<void*>(jo::as_actor(actor)->bone))->matrix());  // 0x0060ef80
            joan::orig::matrix_position(static_cast<void*>(position), joan::orig::arg::ptr(matrix));       // 0x0060c310
            float temp[3] = {0.0f, 0.0f, 0.0f};                                       // [ebp-0xc4]
            const float* const diff = joan::orig::arg::as<const float*>(joan::orig::vec_sub(static_cast<void*>(temp), static_cast<void*>(position), static_cast<void*>(jo::as_camera(camera)->params.position)));  // 0x0060cc90
            float delta[3] = {diff[0], diff[1], diff[2]};  // [ebp-0xb8]
            const float distance = static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<void*>(delta)));  // length
            const bool scripted = global<i32>(d::g_stencil_bits) >= 4 && global<i32>(d::g_mission) == 1 && static_cast<i8>(global<u8>(d::g_a5d384)) != 0;
            if (distance > 2000.0f && !scripted) continue;  // NaN goes on, as below
            float sphere[4] = {0.0f, 0.0f, 0.0f, 0.0f};                          // [ebp-0x24]
            joan::orig::as_actor(actor)->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), 1u);  // 0x005acc80
            if (joan::orig::arg::as<i32>(joan::orig::as_camera(camera)->sphere_in_view(joan::orig::arg::mptr(static_cast<void*>(sphere)), joan::orig::arg::f32(sphere[3] * 3.0f))) == 0) continue;  // 0x006159f0
            ++g_shadow_stats.in_view;
            double limit = 100.0;
            if (jo::view<i32>(jo::as_actor(actor)->record) != 0) {
                u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_players(img::obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<i32>(jo::as_actor(actor)->record))));  // 0x006af310
                if (jo::as_shot(unit)->state.sub != nullptr && jo::as_shot(unit)->state.zone != 0) {  // the 0x006af310 list holds Shot records (Body +0x212 / +0x20e = state.sub / state.zone)
                    u8* const entry = joan::orig::arg::as<u8*>(joan::orig::as_billboard(jo::view<void*>(static_cast<jo::PathSpace*>(jo::as_shot(unit)->state.sub)->zones))->item(joan::orig::arg::i32(static_cast<std::int32_t>(jo::as_shot(unit)->state.zone))));  // 0x00637da0
                    limit = jo::as_pfd_zone(entry)->kind == 0 ? 300.0 : 0.0;
                }
            }
            if (global<i32>(d::g_control_mode) != 4) {
                // fix/uninit: [ebp-0x28] (never written) -> kShadowTopZ.
                const float height = kShadowTopZ - jo::as_camera(camera)->params.position[2];
                if (static_cast<double>(height) > limit) continue;  // test ah,0x41 / jne: NaN draws
            }
            void* plane = nullptr;  // [ebp-0x44]
            bool find_plane = false;
            if (!(distance > 1000.0f)) {
                const i32 mode = static_cast<i8>(global<u8>(d::g_a58054));
                if (mode == 3) {
                    find_plane = true;
                } else if (mode == 2 && jo::as_actor(actor)->kind == 0xb) {
                    u8* const object = jo::view<u8*>(jo::as_actor(actor)->owner);
                    if (static_cast<i32>(jo::as_unit(object)->id) == global<i32>(d::g_player_unit_recno) || jo::as_ground_object(object)->rider_id == global<i32>(d::g_player_unit_recno))
                        find_plane = true;
                }
            }
            if (find_plane || scripted)
                joan::orig::as_anim(actor)->select_frame_5ad830(joan::orig::arg::mptr(static_cast<void*>(&plane)), joan::orig::arg::mptr(static_cast<void*>(&bone)));  // 0x005ad830
            if (plane != nullptr) {
                float world[16];  // [ebp-0x9c]
                std::memcpy(world, img::obj(d::g_identity_matrix), sizeof(world));
                u8* const storage = joan::orig::arg::as<u8*>(joan::orig::as_plane(plane)->bone_5cf8d0());  // 0x005cf8d0
                static_cast<void>(*reinterpret_cast<volatile const u32*>(storage + 0x40));  // [ebp-0xa0], never used
                if (bone != nullptr)
                    std::memcpy(world, joan::orig::arg::as<const void*>(joan::orig::as_bone(bone)->matrix()), sizeof(world));  // 0x0060ef80
                u32 extent = 0;  // [ebp-0xa4], float bits
                u8* const shared = jo::view<u8*>(jo::as_actor(actor)->def->model);
                if (shared != nullptr) {
                    const float sum = jo::as_model(shared)->sphere[3] + jo::as_model(shared)->sphere[2];
                    std::memcpy(&extent, &sum, 4);
                }
                if (joan::hook::settings().gpu_shadow_volumes != 0) {
                    // [Graphics] GpuShadowVolumes: the same volume, made by the GPU from the plane's mesh (render/
                    // shadow_volumes.hpp); drawn with the states below (the flag ShadowPlane::build returns is
                    // always 0: *count unchanged).
                    joan::render::VolumeSource src;
                    float dir[3] = {0.0f, 0.0f, 0.0f}, length = 0.0f, extent_f = 0.0f;
                    std::memcpy(&extent_f, &extent, 4);
                    if (joan::en3d::gpu_shadow_volume(plane, light, world, extent_f, src, dir, length)) {
                        joan::orig::as_sys_en(img::obj(kSysEn))->set_blend(0xdu);
                        joan::orig::as_sys_en(img::obj(kSysEn))->set_texture_mode(1u);
                        joan::orig::as_sys_en(img::obj(kSysEn))->set_vertex_type(static_cast<char>(joan::orig::arg::i32(1)));
                        joan::orig::as_sys_en(img::obj(kSysEn))->apply(0u);
                        game_device()->SetTransform(0x100, world);
                        game_device()->SetRenderState(0x16, 2);
                        game_device()->SetRenderState(0x37, 4);
                        bool both = false;  // the device drew both passes at once (two-sided stencil)
                        const bool front = joan::render::shadow_volume_draw(src, dir, length, true, &both);
                        game_device()->SetRenderState(0x16, 3);
                        game_device()->SetRenderState(0x37, 5);
                        const bool back = front && !both && joan::render::shadow_volume_draw(src, dir, length, false);
                        if (front || back) drawn = 1;
                        ++g_shadow_stats.volumes;
                        if (both) ++g_shadow_stats.both;
                    }
                    continue;
                }
                std::uint8_t* vertices = nullptr;  // [ebp-0x4c] .. [ebp-0x5c]; the two buffers are pointers (ShadowPlane::build writes u8** / u16**: pointer-sized on x64)
                u32 vertex_count = 0;
                std::uint16_t* indices = nullptr;
                u32 triangles = 0, flag = 0;
                // 0x005e3040 writes -0x4c / -0x54 first and -0x50 / -0x58 / -0x5c (byte) whenever it
                // stores a non-zero -0x4c, so the reads below see written values only.
                joan::orig::as_plane(plane)->build_shadow_volume(joan::orig::arg::mptr(camera), joan::orig::arg::mptr(light), joan::orig::arg::mptr(static_cast<void*>(&vertices)), joan::orig::arg::mptr(static_cast<void*>(&vertex_count)), joan::orig::arg::mptr(static_cast<void*>(&indices)), joan::orig::arg::mptr(static_cast<void*>(&triangles)), joan::orig::arg::mptr(static_cast<void*>(&flag)), joan::orig::arg::mptr(static_cast<void*>(world)), joan::orig::arg::f32(extent));  // 0x005e3040
                if (vertices != nullptr && vertex_count != 0 && indices != nullptr && triangles != 0) {
                    if ((flag & 0xff) != 0) *count = *count + 1;
                    joan::orig::as_sys_en(img::obj(kSysEn))->set_blend(0xdu);  // 0x005f8250
                    joan::orig::as_sys_en(img::obj(kSysEn))->set_texture_mode(1u);    // 0x005fa250
                    joan::orig::as_sys_en(img::obj(kSysEn))->set_vertex_type(static_cast<char>(joan::orig::arg::i32(1)));    // 0x006024e0
                    joan::orig::as_sys_en(img::obj(kSysEn))->apply(0u);    // 0x006014d0
                    game_device()->SetTransform(0x100, world);
                    game_device()->SetRenderState(0x16, 2);
                    game_device()->SetRenderState(0x37, 4);
                    joan::orig::as_sys_en(img::obj(kSysEn))->fill_vertices(static_cast<char>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(vertices), joan::orig::arg::u32(vertex_count));  // 0x00600490
                    joan::orig::as_sys_en(img::obj(kSysEn))->fill_indices(joan::orig::arg::mptr(indices), joan::orig::arg::u32(triangles * 3), 0u);  // 0x00600510
                    joan::orig::as_sys_en(img::obj(kSysEn))->draw_indexed(4, 0u, joan::orig::arg::u32(triangles));            // 0x00600a90
                    game_device()->SetRenderState(0x16, 3);
                    game_device()->SetRenderState(0x37, 5);
                    joan::orig::as_sys_en(img::obj(kSysEn))->draw_indexed(4, 0u, joan::orig::arg::u32(triangles));  // 0x00600a90
                    drawn = 1;
                }
                continue;
            }
            if (distance > 2000.0f) continue;  // test ah,0x41 / je: NaN goes on
            u8* const geometry = joan::orig::arg::as<u8*>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(actor)->def->model->mesh))->field_f0());  // 0x005d7380
            if (geometry == nullptr) continue;
            ++g_shadow_stats.meshes;
            joan::orig::as_bone(jo::view<void*>(jo::as_plane(geometry)->bones))->set_parent(joan::orig::arg::mptr(jo::view<void*>(jo::as_actor(actor)->bone)));  // 0x0060ec70
            joan::orig::as_bone(jo::view<void*>(jo::as_plane(geometry)->bones))->invalidate();                           // 0x0060ee10
            for (i32 i = 0; i < jo::as_plane(geometry)->material_count; ++i) jo::as_plane(geometry)->materials[i].raster = 0x10;
            joan::orig::as_plane(geometry)->invalidate(1u);  // 0x005cf920
            game_device()->SetRenderState(0x37, 4);
            joan::orig::as_plane(geometry)->draw(joan::orig::arg::mptr(camera), 1, 0u);  // 0x005d0570
            for (i32 i = 0; i < jo::as_plane(geometry)->material_count; ++i) jo::as_plane(geometry)->materials[i].raster = 0x11;
            joan::orig::as_plane(geometry)->invalidate(1u);  // 0x005cf920
            game_device()->SetRenderState(0x37, 5);
            joan::orig::as_plane(geometry)->draw(joan::orig::arg::mptr(camera), 1, 0u);  // 0x005d0570
            joan::orig::as_bone(jo::view<void*>(jo::as_plane(geometry)->bones))->set_parent(joan::orig::arg::mptr(static_cast<void*>(nullptr)));  // 0x0060ec70
            joan::orig::as_bone(jo::view<void*>(jo::as_plane(geometry)->bones))->invalidate();                               // 0x0060ee10
            drawn = 1;
        }
        game_device()->SetRenderState(0x16, 2);
        g_shadow_stats.done();
        return drawn;
    }
};

// fix/uninit: [ebp-0x28] (never written; minuend of the "actor above camera + limit" skip) = 0.0f
// -> no shadow is skipped by that test (camera z is never below -limit); see the header.
JOAN_REPLACE("fix/uninit", "SceneS::shadow_volumes", 0x005f1cc0, &H::shadow_volumes);

}  // namespace game_b115_13
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers20)
namespace {
namespace en3d_helpers20 {


struct H {
    void* light_list_get(std::int32_t i) {  // 0x005f2730
        if (i >= 0 && i < jo::self<jo::SceneS>(this)->light_count) return jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i];
        return nullptr;
    }
};

#define HELPER20(name, addr, fn) JOAN_REPLACE("en3d/helpers20", name, addr, fn)
HELPER20("LightList::get", 0x005f2730, &H::light_list_get);
#undef HELPER20

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/game_b107_11.cpp (game/b107_11)
//
//    - Scene::light_mesh_5f2780 0x005f2780 (mesh, color), ret 8 (en3d/scene_s.cpp; no
//      references at all: dead code): collects the active lights, then per vertex sets the
//      vertex colour bytes to the base colour and adds each light's contribution (clamped to
//      255, _ftol). Without lights it only writes the base colour.
//
//  - 0x005f2780: jump tables 0x005f2f71 / 0x005f2f91 (type - 1) = {1: 0x24-byte vertices at
//    +0x1c, 2: 0x3c at +0x20, 4: 0x18 at +0x28 (normal = global 0xa69cd8), 5: 0x48 at +0x2c,
//    8: 0x28 at +0x38 (normal = global)}; types 3, 6, 7 and anything else hit the default and
//    use the colour pointer / position / normal locals uninitialised (UB in the original, it
//    would write through a garbage pointer). Since the function is unreferenced, this
//    replacement keeps the locals across vertices like the stack slots do, starts them at
//    zero / null and skips the colour writes while the pointer is null.
//    Colour channel: fild byte + fild light byte * k uses Fpu{true}; dot / length results are
//    taken as double and combined with Fpu{true}; `k > 0` (test ah,0x41) and `v < 255 or NaN`
//    (test ah,1) compares kept with their NaN directions.
//    Return value (dead code): 0x0058ed70's eax; when the light buffer pointer is null the
//    original leaves a stale eax (no caller exists), this replacement returns 0.
namespace {
namespace game_b107_11 {

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;


struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_light_mesh, "Scene::light_mesh_5f2780");

// ---------------------------------------------------------------- 0x005f2780 helpers
// Vertex record of mesh type `type` (byte +4): position, normal, colour pointer.
// Returns false for the types the original's jump table sends to the default (locals kept).
bool mesh_vertex(void* mesh, i32 i, i32 type, Vec3& pos, Vec3& nrm, u8*& col) {
    u8* e = nullptr;
    switch (type) {
        case 1:
            e = jo::view<u8*>(jo::as_plane(mesh)->vertices_f1) + i * 0x24;
            std::memcpy(&pos, e, 12);
            std::memcpy(&nrm, e + 0xc, 12);
            col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f1) + i * 0x24 + 0x18;
            return true;
        case 2:
            e = jo::view<u8*>(jo::as_plane(mesh)->vertices_f2) + i * 0x3c;
            std::memcpy(&pos, e, 12);
            std::memcpy(&nrm, e + 0xc, 12);
            col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f2) + i * 0x3c + 0x18;
            return true;
        case 4:
            e = jo::view<u8*>(jo::as_plane(mesh)->vertices_f4) + i * 0x18;
            std::memcpy(&pos, e, 12);
            std::memcpy(&nrm, obj(d::g_pos), 12);
            col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f4) + i * 0x18 + 0xc;
            return true;
        case 5:
            e = jo::view<u8*>(jo::as_plane(mesh)->vertices_f5) + i * 0x48;
            std::memcpy(&pos, e, 12);
            std::memcpy(&nrm, e + 0xc, 12);
            col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f5) + i * 0x48 + 0x18;
            return true;
        case 8:
            e = jo::view<u8*>(jo::as_plane(mesh)->vertices_f8) + i * 0x28;
            std::memcpy(&pos, e, 12);
            std::memcpy(&nrm, obj(d::g_pos), 12);
            col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f8) + i * 0x28 + 0x1c;
            return true;
        default:
            return false;
    }
}

// Colour pointer only (second, light-less loop: jump table 0x005f2f91).
void mesh_color(void* mesh, i32 i, i32 type, u8*& col) {
    switch (type) {
        case 1: col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f1) + i * 0x24 + 0x18; break;
        case 2: col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f2) + i * 0x3c + 0x18; break;
        case 4: col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f4) + i * 0x18 + 0xc; break;
        case 5: col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f5) + i * 0x48 + 0x18; break;
        case 8: col = jo::view<u8*>(jo::as_plane(mesh)->vertices_f8) + i * 0x28 + 0x1c; break;
        default: break;
    }
}

void set_base_color(u8* col, u32 color) {
    col[2] = static_cast<u8>(color);
    col[1] = static_cast<u8>(color >> 8);
    col[0] = static_cast<u8>(color >> 16);
}

// col[ch] += light byte * k, clamped to 255 (`v < 255.0f` or NaN keeps v), through _ftol.
void add_channel(u8* col, i32 ch, const u8& light_byte, float k) {
    const Fpu fpu{true};
    const double v = fpu.add(static_cast<double>(col[ch]),
                             fpu.mul(static_cast<double>(light_byte), static_cast<double>(k)));
    float t;
    if (!(v >= 255.0)) {  // test ah,1 (C0): less or unordered
        const double v2 = fpu.add(static_cast<double>(col[ch]),
                                  fpu.mul(static_cast<double>(light_byte), static_cast<double>(k)));
        t = static_cast<float>(v2);
    } else {
        t = 255.0f;  // 0x437f0000
    }
    col[ch] = static_cast<u8>(ftol(static_cast<double>(t)));  // 0x007b26cc
}

struct H {
    // ------------------------------------------------------------------ 0x005f2780
    u32 light_mesh_5f2780(void* mesh, u32 color) {
        c_light_mesh.hit();
        const char* const file = str(d::s_d_ww_en3d_scene_s_cpp);  // "d:\\ww\\en3d\\scene_s.cpp"
        void** lights = joan::orig::arg::as<void**>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(jo::self<jo::SceneS>(this)->light_count * static_cast<i32>(sizeof(void*))), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a17960)) + 2)));  // 0x0058e7b0
        i32 n = 0;
        u8 xform_pos = 0;  // [ebp-0x18]
        u8 xform_nrm = 0;  // [ebp-0x1c]
        for (i32 i = 0; i < jo::self<jo::SceneS>(this)->light_count; ++i) {
            if ((joan::orig::arg::as<u32>(joan::orig::as_light(jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i])->flag_59()) & 0xff) == 0) continue;  // 0x005c6790
            if ((joan::orig::arg::as<u32>(joan::orig::as_light(jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i])->flag58()) & 0xff) == 0) continue;  // 0x005f4440
            joan::orig::as_light(jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i])->update();                               // 0x005c5c30
            if (jo::view<i32>((jo::as_light(jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i]))->type) == 3) {
                if (jo::as_plane(mesh)->list_kind == 4) {
                    void* o = jo::view<void*>(jo::as_plane(mesh)->owner);
                    float sphere[4] = {0.0f, 0.0f, 0.0f, 0.0f};  // [ebp-0x30], written by 0x005acc80
                    joan::orig::as_actor(o)->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), joan::orig::arg::u32(static_cast<u32>(1)));  // 0x005acc80
                    void* lp = static_cast<u8*>(jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i]) + 0x38;
                    if ((joan::orig::arg::as<u32>(joan::orig::as_bound_sphere(static_cast<void*>(sphere))->touches(joan::orig::arg::mptr(lp))) & 0xff) == 0)  // 0x0060d890
                        continue;
                }
                xform_nrm = 1;
                xform_pos = xform_nrm;
            } else if (jo::view<i32>((jo::as_light(jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i]))->type) == 1) {
                xform_nrm = 1;
            }
            lights[n] = jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i];
            ++n;
        }

        if (n != 0) {
            void* tbl = joan::orig::arg::as<void*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<i32>(jo::as_plane(mesh)->vertex_format)))));  // 0x00600200
            void* m0 = joan::orig::arg::as<void*>(joan::orig::as_plane(mesh)->bone_5cf8d0());                                // 0x005cf8d0
            void* mat = joan::orig::arg::as<void*>(joan::orig::as_bone(m0)->matrix());                                 // 0x0060ef80
            // [ebp-0x44] pos, [ebp-0x50] normal, [ebp-0x54] colour pointer: kept across vertices.
            Vec3 pos{0.0f, 0.0f, 0.0f};
            Vec3 nrm{0.0f, 0.0f, 0.0f};
            u8* col = nullptr;
            for (i32 i = 0; i < jo::as_plane(mesh)->vertex_count; ++i) {
                mesh_vertex(mesh, i, static_cast<i32>(jo::as_plane(mesh)->vertex_format), pos, nrm, col);
                if (jo::view<i32>(jo::as_plane(mesh)->transformed) == 0) {
                    if (xform_pos != 0) import_call<void*>(d::iat_v3d_87_point_transform_2, static_cast<void*>(&pos), mat);  // point_transform
                    if (xform_nrm != 0) import_call<void*>(d::iat_v3d_87_vector_transform_2, static_cast<void*>(&nrm), mat);  // vector_transform
                }
                if (col == nullptr) continue;  // undefined in the original (see header)
                set_base_color(col, color);
                for (i32 j = 0; j < n; ++j) {
                    void* L = lights[j];
                    float k = 1.0f;  // 0x3f800000
                    const i32 kind = jo::view<i32>(jo::as_light(L)->type);
                    if (kind == 0) {
                        k = k * jo::as_light(L)->f32_30;
                    } else if (kind == 1) {
                        if (jo::as_vertex_format(tbl)->normal_offset == 0) continue;
                        const double d = import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(&nrm),
                                                             static_cast<void*>(&jo::as_light(L)->d3d_light[16]));  // dot
                        k = static_cast<float>(Fpu{true}.mul(d, static_cast<double>(k)));
                        k = k * jo::as_light(L)->f32_30;
                    } else if (kind == 3) {
                        if (jo::as_vertex_format(tbl)->normal_offset == 0) continue;
                        if ((joan::orig::arg::as<u32>(joan::orig::as_bound_sphere(jo::as_light(L)->bounds)->contains(joan::orig::arg::mptr(static_cast<void*>(&pos)))) & 0xff) !=
                            0) {  // 0x0060d810
                            Vec3 d;
                            d.x = jo::as_light(L)->bounds[0] - pos.x;
                            d.y = jo::as_light(L)->bounds[1] - pos.y;
                            d.z = jo::as_light(L)->bounds[2] - pos.z;
                            Vec3 dn{0.0f, 0.0f, 0.0f};
                            import_call<void*>(d::iat_v3d_87_vector_normalize_2, static_cast<void*>(&dn), static_cast<void*>(&d));  // normalize(out, in)
                            const double dt = import_call<double>(d::iat_v3d_87_dot_product, static_cast<void*>(&nrm),
                                                                  static_cast<void*>(&dn));  // dot
                            const Fpu fpu{true};
                            k = static_cast<float>(fpu.mul(dt, static_cast<double>(k)));
                            const double len = import_call<double>(d::iat_v3d_87_length, static_cast<void*>(&d));  // length
                            const double q = fpu.div(len, static_cast<double>(jo::as_light(L)->bounds[3]));
                            const double f = fpu.sub(1.0, q);  // fsubr [0x9cf784] = 1.0f
                            k = static_cast<float>(fpu.mul(f, static_cast<double>(k)));
                            k = k * jo::as_light(L)->f32_30;
                        } else {
                            k = 0.0f;
                        }
                    }
                    if (!(k > 0.0f)) continue;  // fcomp [0x9cf5c4]; test ah,0x41; jne
                    add_channel(col, 2, reinterpret_cast<const u8*>(&jo::as_light(L)->colour)[0], k);
                    add_channel(col, 1, reinterpret_cast<const u8*>(&jo::as_light(L)->colour)[1], k);
                    add_channel(col, 0, reinterpret_cast<const u8*>(&jo::as_light(L)->colour)[2], k);
                }
            }
            joan::orig::as_plane(mesh)->invalidate(joan::orig::arg::u32(static_cast<u32>(2)));  // 0x005cf920
        } else {
            u8* col = nullptr;  // [ebp-0x78]
            for (i32 i = 0; i < jo::as_plane(mesh)->vertex_count; ++i) {
                mesh_color(mesh, i, static_cast<i32>(jo::as_plane(mesh)->vertex_format), col);
                if (col == nullptr) continue;  // undefined in the original (see header)
                set_base_color(col, color);
            }
        }

        if (lights == nullptr) return 0;
        return thiscall<u32>(fn::del, kHeap, static_cast<void*>(lights), file,
                             static_cast<i32>(global<i16>(d::g_a17960)) + 0xd6);  // 0x0058ed70
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_11", name, addr, fn)
B("SceneSector::calc_light_on_plane", 0x005f2780, &H::light_mesh_5f2780);
#undef B

}  // namespace game_b107_11
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
//
//    - Scene::update_5f31c0 0x005f31c0 (ret 4): 0x006156b0 gate, then 0x005d6050 on +0x38,
//      +0x40 (also needs [0xa03834]) and on each child's +0x90 (list 0x005effa0 / +0x20) whose
//      bound 0x005acc80 passes 0x006159f0. Always leaves eax = 0 (returned).
namespace {
namespace game_b108_04 {


JOAN_COUNTER(c_scene, "Scene::update_5f31c0");

struct H {
    // ------------------------------------------------------------------ 0x005f31c0
    // The original reloads owner[off] before each of the three calls.
    static void maybe_update(jo::Plane* const& field, void* ctx) {
        if (joan::orig::arg::as<i32>(joan::orig::as_plane(field)->visible()) != 0 &&                // 0x005d0480
            (joan::orig::arg::as<u32>(joan::orig::as_plane(field)->has_alpha_material()) & 0xff) != 0)         // 0x005d5fe0
            joan::orig::as_plane(field)->draw_mirror_5d6050(joan::orig::arg::mptr(ctx));                  // 0x005d6050
    }
    u32 scene_update_5f31c0(void* ctx) {
        c_scene.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_camera(ctx)->box_in_view(joan::orig::arg::mptr(jo::self<jo::SceneS>(this)->desc))) == 0) return 0;  // 0x006156b0
        if (jo::self<jo::SceneS>(this)->plane != nullptr) maybe_update(jo::self<jo::SceneS>(this)->plane, ctx);
        if (jo::self<jo::SceneS>(this)->plane2 != nullptr && global<u32>(d::g_draw_overlay_mesh) != 0) maybe_update(jo::self<jo::SceneS>(this)->plane2, ctx);
        for (void* c = joan::orig::arg::as<void*>(joan::orig::as_scene_s(this)->first_actor()); c != nullptr; c = jo::as_actor(c)->scene_prev) {  // 0x005effa0
            if (joan::orig::arg::as<i32>(joan::orig::as_actor(c)->visible_5ab620()) == 0) continue;  // 0x005ab620
            alignas(4) u8 bound[16];
            void* const b = joan::orig::arg::as<void*>(joan::orig::as_actor(c)->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(bound)), joan::orig::arg::u32(i32{0})));  // 0x005acc80
            if (joan::orig::arg::as<i32>(joan::orig::as_camera(ctx)->sphere_in_view(joan::orig::arg::mptr(b), joan::orig::arg::f32(i32{0}))) == 0) continue;                      // 0x006159f0
            maybe_update(jo::as_actor(c)->plane, ctx);
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("SceneSector::render_reflection", 0x005f31c0, &H::scene_update_5f31c0);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
//
//    - Scene::push 0x005f35f0 (p), ret 4: grows +0x44 by 8 (0x58eb30 realloc, "scene_s.cpp").
namespace {
namespace game_b98_06 {


JOAN_COUNTER(c_scene_push, "Scene::push");

struct H {
    // ------------------------------------------------------------------ 0x005f35f0
    void scene_push(void* p) {
        c_scene_push.hit();
        if (!(jo::self<jo::SceneS>(this)->item_count < jo::self<jo::SceneS>(this)->item_cap)) {
            jo::self<jo::SceneS>(this)->item_cap = jo::self<jo::SceneS>(this)->item_count + 8;
            const i32 line = static_cast<i32>(global<i16>(d::g_a17964)) + 4;
            jo::view<void*>(jo::self<jo::SceneS>(this)->items) = joan::orig::as_mem(kHeap)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::SceneS>(this)->items)), joan::orig::arg::u32(static_cast<u32>(jo::self<jo::SceneS>(this)->item_cap) * static_cast<u32>(sizeof(jo::PtrSlot))), str(d::s_d_ww_en3d_scene_s_cpp), joan::orig::arg::i32(line));  // 0x0058eb30
        }
        jo::view<void**>(jo::self<jo::SceneS>(this)->items)[jo::self<jo::SceneS>(this)->item_count] = p;
        jo::self<jo::SceneS>(this)->item_count = jo::self<jo::SceneS>(this)->item_count + 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("SceneSector::add_portal", 0x005f35f0, &H::scene_push);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
//
//    - Obj::sdtor_5f3770 0x005f3770 / Obj::sdtor_5f6ac0 0x005f6ac0 (flags), ret 4: scalar
//      deleting dtors (0x5e43e0 / 0x618090, delete when flags & 1) -> this.
namespace {
namespace game_b100_11 {

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    H* sdtor_5f3770(u32 flags) {
        joan::orig::as_portal(this)->destruct();  // 0x005e43e0
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::sdtor_5f3770", 0x005f3770, &H::sdtor_5f3770);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
//
//    - Scene::update_5f37c0 0x005f37c0 (a, b), ret 8.
namespace {
namespace game_b97_06 {


JOAN_COUNTER(c_update_5f37c0, "Scene::update_5f37c0");

struct H {
    // 0x005f37c0
    void update_5f37c0(void* a, std::int32_t b) {
        c_update_5f37c0.hit();
        for (std::int32_t i = 1; i <= jo::self<jo::SceneS>(this)->light_count; ++i) {
            void* o = jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i - 1];
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_light(o)->active()) != 0) joan::orig::as_light(o)->enable();  // 0x005c6070, 0x005c6110
        }
        if (b == 1 && (jo::as_camera(a)->reorder == nullptr || global<std::uint32_t>(d::g_use_reorder_queue) == 0)) {
            if (jo::view<void*>(jo::self<jo::SceneS>(this)->plane) != nullptr && joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::SceneS>(this)->plane))->visible()) != 0) {  // 0x005d0480
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::SceneS>(this)->plane))->draw(joan::orig::arg::mptr(a), joan::orig::arg::i32(b), 0u);  // 0x005d0570
                if (jo::view<std::uint32_t>(jo::self<jo::SceneS>(this)->node_count) != 0) joan::orig::as_scene_s(this)->draw_projected(joan::orig::arg::mptr(a), joan::orig::arg::i32(b));  // 0x005f3a70
            }
            if (jo::view<void*>(jo::self<jo::SceneS>(this)->plane2) != nullptr && global<std::uint32_t>(d::g_draw_overlay_mesh) != 0 &&
                joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::SceneS>(this)->plane2))->visible()) != 0)  // 0x005d0480
                joan::orig::as_plane(jo::view<void*>(jo::self<jo::SceneS>(this)->plane2))->draw(joan::orig::arg::mptr(a), joan::orig::arg::i32(b), 0u);  // 0x005d0570
            if (jo::view<void*>(jo::self<jo::SceneS>(this)->object) != nullptr && global<std::uint32_t>(d::g_draw_overlay_mesh) != 0 &&
                joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(jo::view<void*>(jo::self<jo::SceneS>(this)->object))->visible_5ab620()) != 0)  // 0x005ab620
                joan::orig::as_actor(jo::view<void*>(jo::self<jo::SceneS>(this)->object))->draw(joan::orig::arg::mptr(a), joan::orig::arg::i32(b));  // 0x005aba20
        }
        for (std::int32_t i = jo::self<jo::SceneS>(this)->light_count; i > 0; --i) {
            void* o = jo::view<void**>(jo::self<jo::SceneS>(this)->lights)[i - 1];
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_light(o)->active()) != 0) joan::orig::as_light(o)->disable();  // 0x005c6070, 0x005c6250
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("SceneSector::render_sector_plane", 0x005f37c0, &H::update_5f37c0);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers9.cpp (en3d/helpers10)
namespace {
namespace en3d_helpers9 {

using joan::hook::global;
using Device = joan::d3d8::IDirect3DDevice8;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::import_call;
Device* game_device() { return global<Device*>(d::g_game_device); }

struct Vec3 {
    float x, y, z;
};
struct Mat16 {
    float m[16];
};

JOAN_COUNTER(c_occluders, "SceneS::occluders");
JOAN_COUNTER(c_projected, "SceneS::draw_projected");

struct H {
    // --- SceneS (en3d/scene_s.cpp) -------------------------------------------------------------------
    // 0x005eee40: move the occluders (+0x58, 0x94 each, count +0x5c) the camera can use
    // (0x00616150) to the front (+0x64 how many), then with +0x68 rebuild the occluder mesh plane
    // (+0x81, created once: 0x11c bytes, 0x005cbaf0(4, 1), 0x005cc7a0, type 8, owner this): one
    // material (blend 5) and per occluder 4 vertices (its corners, colour 0xa69cd0) and 4 triangles.
    void occluders(void* camera) {
        c_occluders.hit();
        if (jo::self<jo::Scene>(this)->u32_6c == 0) {
            jo::self<jo::Scene>(this)->occluder_count_ = 0;
            return;
        }
        joan::orig::as_camera(camera)->update_matrices();  // 0x006141a0
        jo::self<jo::Scene>(this)->occluder_count_ = 0;
        for (std::int32_t i = jo::self<jo::Scene>(this)->occluder_total - 1; i >= jo::self<jo::Scene>(this)->occluder_count_; --i) {
            std::uint8_t* const occ = jo::view<std::uint8_t*>(jo::self<jo::Scene>(this)->occluders) + i * 0x94;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_camera(camera)->occluder_planes(joan::orig::arg::mptr(occ))) == 0) continue;  // 0x00616150
            std::uint8_t* const slot = jo::view<std::uint8_t*>(jo::self<jo::Scene>(this)->occluders) + jo::self<jo::Scene>(this)->occluder_count_ * 0x94;
            if (slot != occ) {
                std::uint8_t tmp[0x94];
                std::memcpy(tmp, slot, 0x94);
                std::memcpy(slot, occ, 0x94);
                std::memcpy(occ, tmp, 0x94);
            }
            jo::self<jo::Scene>(this)->occluder_count_ = jo::self<jo::Scene>(this)->occluder_count_ + 1;
            ++i;
        }
        if (jo::self<jo::Scene>(this)->u32_68 == 0) return;
        if (jo::view<void*>(jo::self<jo::Scene>(this)->plane_81) != nullptr) joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->clear();  // 0x005cd2d0 clear
        if (jo::self<jo::Scene>(this)->occluder_count_ <= 0) return;
        if (jo::view<void*>(jo::self<jo::Scene>(this)->plane_81) == nullptr) {
            void* const memory = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
            void* const mesh = memory ? joan::orig::as_plane(memory)->construct(4u, 1u) : nullptr;  // 0x005cbaf0
            jo::view<void*>(jo::self<jo::Scene>(this)->plane_81) = mesh;
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->init(0, 0, 0, 1u);  // 0x005cc7a0
            jo::view<std::uint32_t>(jo::self<jo::Scene>(this)->plane_81->list_kind) = 8;
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->set_owner(joan::orig::arg::mptr(this));  // 0x005d03f0
        }
        const std::int32_t mi = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->add_materials(1));  // 0x005cd470 add materials
        std::uint8_t* const material = jo::view<std::uint8_t*>(jo::self<jo::Scene>(this)->plane_81->materials) + mi * static_cast<std::int32_t>(sizeof(jo::Material));
        joan::orig::as_material(material)->construct();  // 0x005d52a0
        jo::as_material(material)->blend = 5;
        jo::as_material(material)->ambient = global<std::uint32_t>(d::g_init_black);
        for (std::int32_t i = 0; i < jo::self<jo::Scene>(this)->occluder_count_; ++i) {
            const std::uint8_t* const occ = jo::view<std::uint8_t*>(jo::self<jo::Scene>(this)->occluders) + i * 0x94;
            const std::int32_t v = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->add_vertices(4));  // 0x005cd7f0 add vertices
            std::uint8_t* vert = jo::view<std::uint8_t*>(jo::self<jo::Scene>(this)->plane_81->vertices_f4) + v * 0x18;
            for (std::int32_t k = 0; k < 4; ++k, vert += 0x18) {
                jo::view<Vec3>(jo::as_vertex_f4(vert)->pos) = reinterpret_cast<const Vec3*>(jo::as_occluder(occ)->corners)[k];
                jo::view<std::uint32_t>(jo::as_vertex_f4(vert)->attr[0]) = global<std::uint32_t>(d::g_init_white);
            }
            const std::int32_t t = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->add_triangles(4));  // 0x005ce6f0 add triangles
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->set_triangle(joan::orig::arg::i32(t), 0u, joan::orig::arg::u32(v + 1), joan::orig::arg::u32(v), joan::orig::arg::u32(v + 2));  // 0x005ce800
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->set_triangle(joan::orig::arg::i32(t + 1), 0u, joan::orig::arg::u32(v + 2), joan::orig::arg::u32(v), joan::orig::arg::u32(v + 3));
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->set_triangle(joan::orig::arg::i32(t + 2), 0u, joan::orig::arg::u32(v + 1), joan::orig::arg::u32(v + 2), joan::orig::arg::u32(v));
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->set_triangle(joan::orig::arg::i32(t + 3), 0u, joan::orig::arg::u32(v), joan::orig::arg::u32(v + 2), joan::orig::arg::u32(v + 3));
        }
        joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->invalidate(joan::orig::arg::u32(-1));  // 0x005cf920
        joan::orig::as_plane(jo::view<void*>(jo::self<jo::Scene>(this)->plane_81))->sort_by_material(0);   // 0x005cf1d0
    }

    // 0x005f3a70: projected textures (+0xc6 projectors, count +0xc2; texture +0x24, texture
    // matrix 0x005e5660 from the inverse view) - the plane (+0x38) is drawn once per group of up
    // to min(caps 0xa52508, 0xa524fc & 0xffff, 4) projectors with every material replaced by one
    // using those textures (blend 0x13, flags 0x110, mode = stages | 0x2000000). The materials
    // are saved first and restored after; the original destroys saved[0] on every restore step
    // (not saved[i]) - kept as is.
    void draw_projected(void* lights, std::int32_t pass) {  // pass: the draw pass number (the facade passes an int; Phase 2 engine-2)
        c_projected.hit();
        std::uint8_t* const plane = jo::view<std::uint8_t*>(jo::self<jo::SceneS>(this)->plane);
        std::uint8_t* const saved = joan::orig::arg::as<std::uint8_t*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<std::uint32_t>(jo::as_plane(plane)->material_count * static_cast<std::int32_t>(sizeof(jo::Material))))));
        for (std::int32_t i = 0; i < jo::self<jo::SceneS>(this)->plane->material_count; ++i) {
            joan::orig::as_material(saved + i * static_cast<std::int32_t>(sizeof(jo::Material)))->construct();  // 0x005d52a0
            joan::orig::as_material(saved + i * static_cast<std::int32_t>(sizeof(jo::Material)))->assign(joan::orig::arg::mptr(jo::view<std::uint8_t*>(jo::self<jo::SceneS>(this)->plane->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material))));  // 0x005d53e0 copy
        }
        const std::uint32_t a = global<std::uint32_t>(d::g_a52508), b = global<std::uint32_t>(d::g_a524fc) & 0xffff;
        const std::uint32_t m = a < b ? a : b;
        const std::int32_t stages = static_cast<std::int32_t>(m >= 4 ? 4 : m);
        Mat16 view;
        game_device()->GetTransform(2, view.m);       // VIEW
        import_call<void*>(d::iat_v3d_87_matrix_inverse_1, view.m);       // matrix_inverse in place
        std::int32_t p = 0;
        while (p < jo::self<jo::SceneS>(this)->node_count) {
            std::uint8_t material[sizeof(jo::Material)];  // 0x54 on x86
            joan::orig::as_material(material)->construct();
            jo::as_material(material)->blend = 0x13;
            jo::as_material(material)->raster = 0x110;
            std::int32_t s = 0;
            for (; s < stages && p < jo::self<jo::SceneS>(this)->node_count; ++s, ++p) {
                void* const projector = jo::view<void**>(jo::self<jo::SceneS>(this)->nodes)[p];
                joan::orig::as_material(material)->set_texture(joan::orig::arg::i32(s), joan::orig::arg::mptr(jo::as_tex_projector(projector)->texture));  // 0x005d5650 set texture (TexProjector +0x24)
                float texture_matrix[16];
                std::uint32_t vw[16];  // the view matrix passed by value as 16 dwords
                std::memcpy(vw, view.m, sizeof vw);
                joan::orig::as_tex_projector(jo::view<void**>(jo::self<jo::SceneS>(this)->nodes)[p])->texture_matrix(vw[0], vw[1], vw[2], vw[3], vw[4], vw[5], vw[6], vw[7], vw[8], vw[9], vw[10], vw[11], vw[12], vw[13], vw[14], vw[15], joan::orig::arg::mptr(static_cast<void*>(texture_matrix)));  // 0x005e5660
                game_device()->SetTransform(static_cast<joan::d3d8::Enum>(0x10 + s), texture_matrix);  // TEXTURE0 + s
            }
            jo::as_material(material)->texture_mode = static_cast<std::uint32_t>(s) | 0x2000000;
            for (std::int32_t i = 0; i < jo::self<jo::SceneS>(this)->plane->material_count; ++i)
                joan::orig::as_material(jo::view<std::uint8_t*>(jo::self<jo::SceneS>(this)->plane->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material)))->assign(joan::orig::arg::mptr(material));
            joan::orig::as_material(material)->dtor();  // 0x005d5360
            joan::orig::as_plane(jo::view<void*>(jo::self<jo::SceneS>(this)->plane))->draw(joan::orig::arg::mptr(lights), pass, 0u);  // Plane::draw 0x005d0570 (`camera` is the pass number; the facade takes it as std::int32_t)
        }
        for (std::int32_t i = 0; i < jo::self<jo::SceneS>(this)->plane->material_count; ++i) {
            std::uint8_t* const mat = jo::view<std::uint8_t*>(jo::self<jo::SceneS>(this)->plane->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material));
            joan::orig::as_material(mat)->dtor();
            joan::orig::as_material(mat)->assign(joan::orig::arg::mptr(saved + i * static_cast<std::int32_t>(sizeof(jo::Material))));
            joan::orig::as_material(saved)->dtor();  // sic: saved[0]
        }
        joan::crt::operator_delete(joan::orig::arg::mptr(saved));  // operator delete
    }
};

JOAN_REPLACE("en3d/helpers10", "Scene::turn_on_occluder", 0x005eee40, &H::occluders);
JOAN_REPLACE("en3d/helpers10", "SceneS::draw_projected", 0x005f3a70, &H::draw_projected);

}  // namespace en3d_helpers9
}  // namespace

// ---------------------------------------- from modules/en3d_b82_09.cpp (en3d/b82_09)
//
//      0x005ef850 SceneGroup::~SceneGroup()      - 0x005ef900.
namespace {
namespace en3d_b82_09_2 {

using joan::call::thiscall;

struct H {
    // 0x005ef850 SceneGroup::~SceneGroup(). eax = 0x005ef900's.
    std::uintptr_t group_dtor() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_scene_source(this)->clear()); }  // 0x005ef900
};

#define B82_09(name, addr, fn) JOAN_REPLACE("en3d/b82_09", name, addr, fn)
B82_09("SceneGroup::~SceneGroup", 0x005ef850, &H::group_dtor);
#undef B82_09

}  // namespace en3d_b82_09_2
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
//
//      0x005ef880 SceneSource::reset(desc) (ret 4)  - clear, +8 = 24 bytes of desc, fields reset
namespace {
namespace en3d_b83_03_2 {

using joan::call::thiscall;

struct H {
    // 0x005ef880 (eax = this)
    void* source_reset(const void* desc) {
        joan::orig::as_scene_source(this)->clear();  // 0x005ef900
        std::memcpy(jo::self<jo::SceneS>(this)->desc, desc, sizeof(jo::SceneS::desc));  // 0x18 on x86
        jo::self<jo::SceneS>(this)->plane = nullptr;
        jo::self<jo::SceneS>(this)->plane2 = nullptr;
        jo::self<jo::SceneS>(this)->object = nullptr;
        jo::self<jo::SceneS>(this)->lit_ = 0;
        jo::self<jo::SceneS>(this)->casts_shadows_ = 1;
        return this;
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("SceneSource::reset", 0x005ef880, &H::source_reset);
#undef B83_03

}  // namespace en3d_b83_03_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers7.cpp (en3d/helpers7)
namespace {
namespace en3d_helpers7 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

// Distance fade shared by 0x005f3fe0 / 0x005f41f0: by the xy distance from the camera to the
// box centre: up to 1450 fully drawn, beyond 3072 skipped (returns 0), between faded with
// alpha 255 - (d - 1450) / 1622 * 255 (white vertex colour), clearing material bits 8-11.
std::int32_t distance_fade(const std::uint8_t* box, const void* camera, void* plane) {
    const float px = jo::as_camera(camera)->params.position[0], py = jo::as_camera(camera)->params.position[1];
    const float cx = (jo::view<float>(jo::as_scene_s(box)->desc[0]) + jo::view<float>(jo::as_scene_s(box)->desc[3])) / 2.0f;
    const float cy = (jo::view<float>(jo::as_scene_s(box)->desc[1]) + jo::view<float>(jo::as_scene_s(box)->desc[4])) / 2.0f;
    const float dx = px - cx, dy = py - cy;
    const float sum = dx * dx + dy * dy;
    const float d = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sum)))));  // CRT sqrt
    if (!(d > global<float>(d::flt_1450))) {  // 1450
        joan::orig::as_plane(plane)->set_104(0u);       // 0x005d6e80 fade off
        return 1;
    }
    if (d > global<float>(d::flt_3072)) return 0;  // 3072
    joan::orig::as_plane(plane)->set_104(1u);
    float a = d - global<float>(d::flt_1450);
    a = a / global<float>(d::flt_1622);  // 1622
    a = a * global<float>(d::flt_m255);  // -255
    a = a + global<float>(d::flt_255);  // 255
    const std::uint8_t alpha = static_cast<std::uint8_t>(static_cast<std::int64_t>(a));  // _ftol
    std::uint8_t colour_obj[8];
    std::uint32_t d3d = 0;
    void* const c = joan::orig::as_colour(colour_obj)->construct(0xffu, 0xffu, 0xffu, static_cast<std::uint32_t>(alpha));  // 0x0042a260
    joan::orig::as_colour(&d3d)->from_rgba(joan::orig::arg::mptr(c));  // 0x005e95b0
    joan::orig::as_plane(plane)->set_fade(joan::orig::arg::u32(d3d));       // 0x005d5e80 fade colour
    for (std::int32_t i = 0; i < jo::as_plane(plane)->material_count; ++i) {
        std::uint8_t* const material = jo::view<std::uint8_t*>(jo::as_plane(plane)->materials) + i * static_cast<std::int32_t>(sizeof(jo::Material));
        jo::as_material(material)->raster = jo::as_material(material)->raster & 0xfffff0ffu;
    }
    return 1;
}

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- Distance fade (scene_s.cpp) -----------------------------------------------------------------
    std::int32_t sector_fade(void* camera) { return distance_fade(self(), camera, jo::view<void*>(jo::self<jo::SceneS>(this)->plane2)); }  // 0x005f3fe0
    std::int32_t actor_fade(void* camera) {                                                           // 0x005f41f0
        return distance_fade(self(), camera, jo::view<void*>(jo::self<jo::SceneS>(this)->object->plane));
    }
};

#define HELPER7(name, addr, fn) JOAN_REPLACE("en3d/helpers7", name, addr, fn)
HELPER7("SceneSector::modify_grass_plane", 0x005f3fe0, &H::sector_fade);
HELPER7("SceneSector::modify_grass_actor", 0x005f41f0, &H::actor_fade);
#undef HELPER7

}  // namespace en3d_helpers7
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
//
//    - small: Obj::get_4 0x005c5be0, Obj::call_5e8ab0 0x005e61a0, Obj::get_24 0x005ea6c0,
//      init_5eb450 0x005eb450, Obj::get_byte_58 0x005f4440, Obj::get_36 0x005f50e0,
//      table_a17ee4 0x00600c00 (cdecl), init_603550 0x00603550, Obj::nop_603ad0 0x00603ad0,
//      init_60baf0 0x0060baf0.
namespace {
namespace game_b104_10 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;


struct H {
    u32 get_byte_58() {  // mov eax,this; mov al,[eax+0x58]: upper bytes are this's
        return (joan::call::register_low_bits(this) & 0xffffff00u) | jo::self<jo::SceneS>(this)->u8_58;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("Obj::get_byte_58", 0x005f4440, &H::get_byte_58);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers_2 {


// Every helper shares one class so `this` is just the object's address.
struct H {
    H* instance_list_construct() {  // 0x005f4460
        jo::view<void*>(jo::self<jo::InstanceList>(this)->head) = nullptr;
        jo::view<void*>(jo::self<jo::InstanceList>(this)->tail) = nullptr;
        return this;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("InstanceList::InstanceList", 0x005f4460, &H::instance_list_construct);
#undef HELPER

}  // namespace en3d_helpers_2
}  // namespace

// ---------------------------------------- from modules/scene_draw.cpp (en3d/scene_s.cpp)
//
// en3d scene drawing above Plane::draw (decompiling en3d upward, batch 1):
//
//   SceneS::draw_lists   0x005f07f0 (en3d/scene_s.cpp) - one scene layer: sky / ground
//                        planes, then every actor of the layer (pass 2 sorted), with
//                        repeated ActorDefs drawn as instance batches
//
// Statement-by-statement C++ of the MSVC 6 code (read with re/lift.py, checked
// against the assembly). Other en3d functions are called through their
// incremental-link thunks, so replacements of them are picked up.
//
// Fields used (raw offsets):
//   Actor:  +0x00 ActorDef*  +0x04 animation set  +0x08 type (0x27 = ...)  +0x0c object
//           +0x20 next actor in the scene  +0x2c draw argument (Plane::draw arg 3)
//           +0x30 float (non-zero: update before drawing)  +0x44 animation index
//           +0x48 u16 frame  +0x8c bone matrix source  +0x90 Plane* (+0x09 skip in pass 2)
//   SceneS: +0x00 Scene* (+0xcd capacity, +0xd1 Actor** sort buffer)  +0x38 / +0x40 Plane*
//           +0x3c Actor*  +0xc2 flag  +0xca first actor  +0xd2 count, +0xda pointer to effects
namespace {
namespace scene_draw {


JOAN_COUNTER(c_lists, "SceneS::draw_lists");

// Thunks of the functions called (original addresses in comments).
void plane_draw(void* plane, void* ctx, std::int32_t pass, std::uint32_t arg) {
    joan::orig::as_plane(plane)->draw(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), joan::orig::arg::u32(arg));  // Plane::draw 0x005d0570
}
void actor_draw(void* actor, void* ctx, std::int32_t pass) { joan::orig::as_actor(actor)->draw(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass)); }  // 0x005aba20
void draw_instances(void* def, void* ctx, std::int32_t pass, std::int32_t slot) {
    joan::orig::as_actor_def(def)->draw_instances(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), joan::orig::arg::i32(slot));  // 0x005b1cb0
}

// Intrusive list of ActorDefs with queued instances (0x005f4460 constructs it).
struct InstanceList {
    void* head = nullptr;
    void* tail = nullptr;
};

struct SceneS {
    void draw_lists(void* ctx, std::int32_t pass, std::int32_t layer);
};

// draw_lists is called for layers 1, 2, 3 of each sector in each pass, and every call walked the sector's whole actor
// list for the actors of its layer (reading each actor's type: cache misses). The layer 1 walk (as the original's)
// now also notes the layer 2 and 3 actors, in list order; the layer 2 / 3 calls of the same camera and pass use them
// while no sector list was edited since (joan::en3d::g_scene_list_edits), else walk the list as the original.
struct LayerLists {
    const void* ctx = nullptr;
    std::int32_t pass = 0;
    std::uint32_t edits = 0;
    bool valid = false;
    std::vector<void*> layer[2];  // layers 2 and 3
};
std::unordered_map<const void*, LayerLists> g_layer_lists;  // by sector

// [Graphics] GrassBatching (render/grass_batch.hpp): a grass patch (a DynGrass object, holder kind 3) is drawn from the
// device's recording of its draws when there is one, else the normal way while the device records it. (The
// instancing check is skipped: it turns every patch away.)
bool is_grass(void* actor) { return jo::view<std::uint32_t>(jo::as_actor(actor)->holder_kind) == 3; }
void grass_draw(void* actor, void* ctx, std::int32_t pass) {
    std::uint8_t* const plane = jo::view<std::uint8_t*>(jo::as_actor(actor)->plane);
    if (plane == nullptr) {
        joan::render::grass_flush();
        return actor_draw(actor, ctx, pass);
    }
    const bool fading = jo::as_plane(plane)->flags_104 != 0;
    const joan::render::GrassKey key{plane, ctx, static_cast<std::uint32_t>(pass) | (fading ? 4u : 0u)};
    const float* wind = &img::global<const float>(d::g_identity_matrix);
    if (jo::as_plane(plane)->bone_count >= 2)
        wind = reinterpret_cast<const float*>(joan::orig::as_bone(jo::as_plane(plane)->bones + 1)->matrix());  // 0x0060ef80 (WORLD1, as Plane::draw)
    if (joan::render::grass_replay(key, wind, jo::view<std::uint32_t>(jo::as_plane(plane)->colour_), fading)) return;
    joan::render::grass_flush();  // the replays queued so far go first (drawn the normal way from here)
    const bool recording = joan::render::grass_record_begin(key);
    actor_draw(actor, ctx, pass);
    if (recording) joan::render::grass_record_end();
}

// 0x005f07f0, thiscall, ret 0xc. `layer` selects which actors (0x005aed50) are drawn.
void SceneS::draw_lists(void* ctx, std::int32_t pass, std::int32_t layer) {
    c_lists.hit();
    auto* const self = reinterpret_cast<std::uint8_t*>(this);
    for (std::int32_t i = 1; i <= jo::as_scene_s(self)->light_count; ++i) {
        void* const e = jo::view<void*>(jo::as_scene_s(self)->lights[i - 1]);
        if (joan::orig::arg::as<int>(joan::orig::as_light(e)->active()) != 0) joan::orig::as_light(e)->enable();  // 0x005c6070 / 0x005c6110 begin
    }
    const bool reflection = pass == 1 && jo::as_camera(ctx)->reorder != nullptr;
    auto lit = [] { return global<std::uint32_t>(d::g_a17878) != 0; };
    LayerLists& lists = g_layer_lists[self];
    if (layer == 1) lists.valid = false;  // this pass's layer 1 walk (if any) notes them anew
    if (!(reflection && global<std::uint32_t>(d::g_use_reorder_queue) != 0) && layer == 1) {
        if (void* const sky = jo::view<void*>(jo::as_scene_s(self)->plane); sky && joan::orig::arg::as<int>(joan::orig::as_plane(sky)->visible()) != 0 && lit()) {  // 0x005d0480
            plane_draw(sky, ctx, pass, 0);
            if (jo::view<std::uint32_t>(jo::as_scene_s(self)->node_count) != 0) joan::orig::as_scene_s(self)->draw_projected(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass));  // 0x005f3a70
        }
        if (void* const p = jo::view<void*>(jo::as_scene_s(self)->plane2);
            p && global<std::uint32_t>(d::g_draw_overlay_mesh) != 0 && joan::orig::arg::as<int>(joan::orig::as_plane(p)->visible()) != 0 && lit() &&
            joan::orig::arg::as<int>(joan::orig::as_scene_s(self)->sector_fade(joan::orig::arg::mptr(ctx))) != 0)  // 0x005f3fe0
            plane_draw(p, ctx, pass, 0);
        if (void* const a = jo::view<void*>(jo::as_scene_s(self)->object);
            a && global<std::uint32_t>(d::g_draw_overlay_mesh) != 0 && joan::orig::arg::as<int>(joan::orig::as_actor(a)->visible_5ab620()) != 0 && lit() &&  // 0x005ab620
            joan::orig::arg::as<int>(joan::orig::as_scene_s(self)->actor_fade(joan::orig::arg::mptr(ctx))) != 0)  // 0x005f41f0
            actor_draw(a, ctx, pass);
    }
    if (!(reflection && global<std::uint32_t>(d::g_actor_draw_enable) != 0)) {
        auto shown = [&](void* actor) {
            return joan::orig::arg::as<int>(joan::orig::as_actor(actor)->visible_5ab470(joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass))) != 0;  // 0x005ab470 visible
        };
        // Every actor of this layer that is shown, in list order (see LayerLists): use(actor).
        auto each_shown = [&](auto&& use) {
            auto visit = [&](void* a) {
                if (shown(a)) use(a);
            };
            if (layer == 1) {
                lists.layer[0].clear();
                lists.layer[1].clear();
                const std::uint32_t edits = joan::en3d::g_scene_list_edits;
                for (void* a = joan::orig::arg::as<void*>(joan::orig::as_scene_s(self)->first_actor()); a; a = jo::view<void*>(jo::as_actor(a)->scene_prev)) {  // 0x005effa0
                    const std::int32_t l = joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(a)->layer());  // 0x005aed50 layer
                    if (l == 1) {
                        visit(a);
                    } else if (l == 2 || l == 3) {
                        lists.layer[l - 2].push_back(a);
                    }
                }
                lists.ctx = ctx;
                lists.pass = pass;
                lists.edits = edits;
                lists.valid = joan::en3d::g_scene_list_edits == edits;  // no list edited while it drew
                return;
            }
            if ((layer == 2 || layer == 3) && lists.valid && lists.ctx == ctx && lists.pass == pass && lists.edits == joan::en3d::g_scene_list_edits) {
                for (void* a : lists.layer[layer - 2])
                    visit(a);
                return;
            }
            for (void* a = joan::orig::arg::as<void*>(joan::orig::as_scene_s(self)->first_actor()); a; a = jo::view<void*>(jo::as_actor(a)->scene_prev))
                if (layer == joan::orig::arg::as<std::int32_t>(joan::orig::as_actor(a)->layer())) visit(a);
        };
        // 0x005b1180: queues the actor as an instance of its ActorDef (true) or not.
        auto queue = [&](void* actor, InstanceList* list) {
            return joan::orig::arg::as<int>(joan::orig::as_actor_def(jo::view<void*>(jo::as_actor(actor)->def))->queue_instance(joan::orig::arg::mptr(actor), joan::orig::arg::mptr(ctx), joan::orig::arg::i32(pass), joan::orig::arg::mptr(list))) != 0;
        };
        if (pass == 1) {
            InstanceList list;
            joan::orig::as_actor_instance_list(&list)->construct();  // 0x005f4460
            const bool batch = joan::hook::settings().grass_batching != 0;
            each_shown([&](void* a) {
                if (batch && is_grass(a)) return grass_draw(a, ctx, pass);
                joan::render::grass_flush();  // queued grass replays before any other draw
                if (!queue(a, &list)) actor_draw(a, ctx, pass);
            });
            joan::render::grass_flush();
            for (void* d = list.head; d;) {
                void* const next = jo::view<void*>(jo::as_actor_def(d)->prev);
                draw_instances(d, ctx, pass, -1);
                d = next;
            }
        } else if (pass == 2) {
            // Transparent pass: sorted (0x005f0780) into the scene's buffer first.
            void* const scene = jo::view<void*>(jo::as_scene_s(self)->scene);
            std::int32_t n = 0;
            each_shown([&](void* a) {
                if (n >= jo::view<std::int32_t>(jo::as_scene(scene)->u32_cd)) {
                    jo::view<std::int32_t>(jo::as_scene(scene)->u32_cd) += 0x80;
                    // Mem::realloc (0x0058eb30) with the edit-and-continue __LINE__ base 0x00a1795c.
                    jo::view<void**>(jo::as_scene(scene)->buffer_d1) = thiscall<void**>(
                        fn::resize_58eb30, img::obj(d::g_mem), jo::view<void*>(jo::as_scene(scene)->buffer_d1),
                        static_cast<std::uint32_t>(jo::view<std::int32_t>(jo::as_scene(scene)->u32_cd)) * static_cast<std::uint32_t>(sizeof(void*)), &img::global<const char>(d::s_d_ww_en3d_scene_s_cpp),
                        static_cast<std::int32_t>(global<std::int16_t>(d::g_a1795c)) + 0x7c);
                }
                jo::view<void**>(jo::as_scene(scene)->buffer_d1)[n] = a;
                ++n;
            });
            joan::en3d::sort_by_vis_time(jo::view<void**>(jo::as_scene(scene)->buffer_d1), n);  // qsort(..., compare_a8_desc)
            InstanceList list;
            joan::orig::as_actor_instance_list(&list)->construct();
            void** p = jo::view<void**>(jo::as_scene(scene)->buffer_d1);
            const bool batch = joan::hook::settings().grass_batching != 0;
            for (std::int32_t k = n; k > 0; --k, ++p) {
                if (batch && is_grass(*p)) grass_draw(*p, ctx, pass);
                else {
                    joan::render::grass_flush();  // queued grass replays before any other draw
                    if (!queue(*p, &list)) actor_draw(*p, ctx, pass);
                }
            }
            joan::render::grass_flush();
            for (void* d = list.head; d;) {
                void* const next = jo::view<void*>(jo::as_actor_def(d)->prev);
                draw_instances(d, ctx, pass, -1);
                d = next;
            }
        }
    }
    for (std::int32_t i = jo::as_scene_s(self)->light_count; i > 0; --i) {
        void* const e = jo::view<void*>(jo::as_scene_s(self)->lights[i - 1]);
        if (joan::orig::arg::as<int>(joan::orig::as_light(e)->active()) != 0) joan::orig::as_light(e)->disable();  // 0x005c6250 end
    }
}

JOAN_REPLACE("en3d/scene_s.cpp", "SceneSector::render", 0x005f07f0, &SceneS::draw_lists);

}  // namespace scene_draw
}  // namespace

// ---------------------------------------- from modules/game_b105_16_2.cpp (game/b105_16)
namespace {
namespace game_b105_16_2 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

template <fn::Code Body>
u32 JOAN_CDECL crt_init() {
    return cdecl_call<u32>(Body);  // direct call in the original (no thunk)
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("crt_init_5f44a0", 0x005f44a0, &crt_init<fn::Init_pi_5f44c0>);
B("crt_init_5f4500", 0x005f4500, &crt_init<fn::init_a69ac8>);
#undef B

}  // namespace game_b105_16_2
}  // namespace

// ---------------------------------------- from modules/game_b94_06_2.cpp (game/b94_06)
namespace {
namespace game_b94_06_2 {

using joan::call::cdecl_call;

using joan::hook::global;

void JOAN_CDECL init_pi_5f44c0() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));
    global<float>(d::g_pi_a69ac4) = static_cast<float>(a * 4.0);
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Init::pi_5f44c0", 0x005f44c0, &init_pi_5f44c0);
#undef B94

}  // namespace game_b94_06_2
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

using joan::hook::global;

void JOAN_CDECL init_a69ac8() {  // 0x005f4520
    global<float>(d::g_a69ac8) = global<float>(d::g_pi_a69ac4) / global<float>(d::flt_2);
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("init_a69ac8", 0x005f4520, &init_a69ac8);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

using joan::call::cdecl_call;

// ---- static initialisers (direct calls, no thunks in the original) ----
void JOAN_CDECL init_5f4550() { joan::orig::init_a69acc(); }
void JOAN_CDECL init_5f45a0() { joan::orig::set_a69ad0(); }

JOAN_REPLACE("game/b105_17", "init_5f4550", 0x005f4550, &init_5f4550);
JOAN_REPLACE("game/b105_17", "init_5f45a0", 0x005f45a0, &init_5f45a0);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

using joan::hook::global;

// ---------------------------------------------------------------------------- 0x005f4570
void JOAN_CDECL init_a69acc() { global<float>(d::g_a69acc) = global<float>(d::g_pi_a69ac4) / global<float>(d::flt_3); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("init_a69acc", 0x005f4570, &init_a69acc);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b99_07_2.cpp (game/b99_07)
namespace {
namespace game_b99_07_2 {

using joan::hook::global;

// ---------------------------------------------------------------------------- 0x005f45c0
static void JOAN_CDECL set_a69ad0() {
    global<float>(d::g_a69ad0) = global<float>(d::g_pi_a69ac4) / global<float>(d::flt_4);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("set_a69ad0", 0x005f45c0, &set_a69ad0);
#undef B

}  // namespace game_b99_07_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::SceneS::occluders(void* a0) -> void { joan::bind::member<&::en3d_helpers9::H::occluders>(this, a0); }  // 0x5eee40
auto joan::orig::SceneS::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b82_09_2::H::group_dtor>(this)); }  // 0x5ef850
auto joan::orig::SceneSource::reset(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_03_2::H::source_reset>(this, a0)); }  // 0x5ef880
auto joan::orig::SceneSource::clear() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_03::H::source_clear>(this)); }  // 0x5ef900
auto joan::orig::SceneNode::scalar_delete(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01::H::node_delete>(this, a0)); }  // 0x5efb90
auto joan::orig::SceneSource::set_38(void* a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::en3d_b83_03::H::set_38>(this, a0)); }  // 0x5efbe0
auto joan::orig::Scene::attach(void* a0) -> void { joan::bind::member<&::game_fx::H::scene_attach>(this, a0); }  // 0x5efc50
auto joan::orig::Scene::detach(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_fx::H::scene_detach>(this, a0)); }  // 0x5efdb0
auto joan::orig::SceneS::release_actors() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b82_09::H::release_ce>(this)); }  // 0x5eff10
auto joan::orig::SceneS::first_actor() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::scenes_first_actor>(this)); }  // 0x5effa0
auto joan::orig::SceneNode::attachments() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves80a::H::attachments>(this)); }  // 0x5efff0
auto joan::orig::get24(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::game_b94_06::get24>(a0)); }  // 0x5f0020
auto joan::orig::Scene::new_child() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07::H::new_child>(this)); }  // 0x5f0040
auto joan::orig::SceneS::clear() -> void { joan::bind::member<&::en3d_b84_01::H::clear_scene_s>(this); }  // 0x5f0270
auto joan::orig::SceneS::set_casts_shadows(std::uint8_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_03::H::set_flag_df>(this, a0)); }  // 0x5f0340
auto joan::orig::SceneS::casts_shadows() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers::H::scenes_casts_shadows>(this)); }  // 0x5f0370
auto joan::orig::SceneS::shadow_volumes(void* a0, void* a1, void* a2) -> char { return joan::bind::conv<char>(joan::bind::member<&::game_b115_13::H::shadow_volumes>(this, a0, a1, a2)); }  // 0x5f1cc0
auto joan::orig::SceneS::light_at(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers20::H::light_list_get>(this, a0)); }  // 0x5f2730
auto joan::orig::Scene::update_5f31c0(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_04::H::scene_update_5f31c0>(this, a0)); }  // 0x5f31c0
auto joan::orig::Scene::push(void* a0) -> void { joan::bind::member<&::game_b98_06::H::scene_push>(this, a0); }  // 0x5f35f0
auto joan::orig::Scene::update_5f37c0(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_b97_06::H::update_5f37c0>(this, a0, a1); }  // 0x5f37c0
auto joan::orig::SceneS::push(void* a0) -> void { joan::bind::member<&::en3d_b84_01::H::push_scene_s>(this, a0); }  // 0x5f39a0
auto joan::orig::SceneS::draw_projected(void* a0, std::int32_t a1) -> void { joan::bind::member<&::en3d_helpers9::H::draw_projected>(this, a0, a1); }  // 0x5f3a70
auto joan::orig::SceneS::sector_fade(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers7::H::sector_fade>(this, a0)); }  // 0x5f3fe0
auto joan::orig::SceneS::actor_fade(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers7::H::actor_fade>(this, a0)); }  // 0x5f41f0
auto joan::orig::Light::flag58() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_10::H::get_byte_58>(this)); }  // 0x5f4440
auto joan::orig::ActorInstanceList::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers_2::H::instance_list_construct>(this)); }  // 0x5f4460
auto joan::orig::init_a69acc() -> void { joan::bind::free<&::game_b96_06::init_a69acc>(); }  // 0x5f4570
auto joan::orig::set_a69ad0() -> void { joan::bind::free<&::game_b99_07_2::set_a69ad0>(); }  // 0x5f45c0

// ---------------------------------------- [Debug] UnitIndex (scene_lists.hpp)
namespace joan::en3d {
namespace {
struct UnitIndexEntry {
    std::vector<std::uint8_t*> units;
    void* tail = nullptr;  // the list's ends when built: a sector reset or reused without an edit seen here differs
    void* head = nullptr;
    bool valid = false;
};
std::unordered_map<const void*, UnitIndexEntry> g_unit_index;
bool g_unit_index_off = false;
std::uint32_t g_unit_index_lookups = 0;
struct UnitIndexStats {
    std::uint32_t lookups = 0, rebuilt = 0, checked = 0;
} g_unit_index_stats;
void unit_index_stats_log() {
    UnitIndexStats& st = g_unit_index_stats;
    if (st.lookups < 200000) return;
    if (joan::hook::settings().perf_log > 0)
        joan::hook::log_line("[Debug] UnitIndex: %u sector lookups, %u rebuilt, %u checked against the list (0 differences); %zu sectors kept",
                             st.lookups, st.rebuilt, st.checked, g_unit_index.size());
    st = {};
}

// The units of the sector's list in the order the queries walk it (from nodes_tail through scene_prev; the queries'
// `node != first` stop of a circular list kept).
void walk_units(void* sector, std::vector<std::uint8_t*>& out) {
    out.clear();
    std::uint8_t* const first = jo::view<std::uint8_t*>(jo::as_scene_s(sector)->nodes_tail);
    std::uint8_t* node = first;
    if (node == nullptr) return;
    do {
        if (jo::as_actor(node)->kind == 0xb) out.push_back(node);
        node = jo::view<std::uint8_t*>(jo::as_actor(node)->scene_prev);
    } while (node != nullptr && node != first);
}
}  // namespace

void unit_index_touch(const void* sector) {
    if (sector == nullptr) return;
    ++g_unit_list_edits;
    const auto it = g_unit_index.find(sector);
    if (it != g_unit_index.end()) it->second.valid = false;
}
void unit_index_actor_kind(const void* actor) {
    if (actor != nullptr) unit_index_touch(jo::view<void*>(jo::as_actor(actor)->scene));
}
void unit_index_touch_all() {
    ++g_unit_list_edits;
    for (auto& [sector, e] : g_unit_index) e.valid = false;
}

const std::vector<std::uint8_t*>* sector_units(void* sector) {
    const auto& s = joan::hook::settings();
    if (s.unit_index == 0 || g_unit_index_off || sector == nullptr) return nullptr;
    ++g_unit_index_stats.lookups;
    unit_index_stats_log();
    UnitIndexEntry& e = g_unit_index[sector];
    void* const tail = jo::view<void*>(jo::as_scene_s(sector)->nodes_tail);
    void* const head = jo::view<void*>(jo::as_scene_s(sector)->nodes_head);
    if (!e.valid || e.tail != tail || e.head != head) {
        ++g_unit_index_stats.rebuilt;
        walk_units(sector, e.units);
        e.tail = tail;
        e.head = head;
        e.valid = true;
        return &e.units;
    }
    if (s.unit_index_check > 0 && ++g_unit_index_lookups % static_cast<std::uint32_t>(s.unit_index_check) == 0) {
        ++g_unit_index_stats.checked;
        static std::vector<std::uint8_t*> walked;
        walk_units(sector, walked);
        if (walked != e.units) {
            joan::hook::log_line("[Debug] UnitIndex: the units kept for a sector (%zu) differ from its list (%zu) - index switched off, "
                                 "please report", e.units.size(), walked.size());
            g_unit_index_off = true;
            return nullptr;
        }
    }
    return &e.units;
}
}  // namespace joan::en3d
