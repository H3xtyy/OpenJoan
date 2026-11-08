// game/unknown_wavelist.cpp: functions whose original source file is unknown (formerly game/_unplaced/wavelist.cpp;
// re/attribution.csv: between ww/world_s.cpp and ww/ilib/angle.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
namespace {
namespace game_b98_05 {

struct H {
    // 0x0056eca0
    void* ctor_56eca0() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);   // 0x005856a0 (SceneObject pointers: 4 bytes on x86)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("Obj::ctor_56eca0", 0x0056eca0, &H::ctor_56eca0);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
namespace {
namespace game_b95_05 {

struct H {
    // 0x0056ecd0
    std::uint32_t dtor_56ecd0() {
        joan::orig::as_wave_list(this)->clear_all();         // 0x0056f010
        return thiscall<std::uint32_t>(fn::dtor_DynArrayB, this);  // 0x00585710
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("Obj::dtor_56ecd0", 0x0056ecd0, &H::dtor_56ecd0);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b101_09.cpp (game/b101_09)
namespace {
namespace game_b101_09 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

using i8 = std::int8_t;

using joan::img::str;

JOAN_COUNTER(c_wave_load, "WaterWave::load");

// ---------------------------------------------------------------- Database::write helpers
struct alignas(4) Path {
    u8 bytes[0x194];
};
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
void path_set(Path& p, const char* s) { joan::orig::as_string(&p)->construct(joan::orig::arg::str(s)); }  // 0x005a11a0
char* crt_strcat(const char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
using joan::orig::helpers::game_new;  // operator new

struct H {
    // 0x0056ed40
    H* wave_load() {
        c_wave_load.hit();
        if (jo::self<jo::WaterWave>(this)->loaded != 0) return this;
        jo::self<jo::WaterWave>(this)->life_ms = 0x320;
        if (jo::self<jo::WaterWave>(this)->anim == nullptr) {
            Path path;
            path_set(path, str(d::s_ITEM_9cf648));                    // "ITEM\\"
            crt_strcat(c_str(path), str(d::s_Object_Models_Items_WaterWav_9d8600));          // "...SmallWave\\anim\\"
            void* const mem_ = game_new(static_cast<std::uint32_t>(sizeof(jo::AnimDef)));  // 0x18b on x86
            void* const anim = mem_ != nullptr ? joan::orig::as_anim_def(mem_)->construct() : nullptr;  // 0x005b2390
            jo::view<void*>(jo::self<jo::WaterWave>(this)->anim) = anim;
            joan::orig::as_anim_def(jo::view<void*>(jo::self<jo::WaterWave>(this)->anim))->load(joan::orig::arg::str(c_str(path)), str(d::s_WAVE_EFF), 0u);  // 0x005b23d0
        }
        if (jo::self<jo::WaterWave>(this)->model == nullptr) {
            void* const mem_ = game_new(static_cast<std::uint32_t>(sizeof(jo::ActorDef)));  // 0x2d8 on x86
            void* const model = mem_ != nullptr ? joan::orig::as_actor_def(mem_)->construct() : nullptr;  // 0x005af390
            jo::view<void*>(jo::self<jo::WaterWave>(this)->model) = model;
            Path path;
            path_set(path, str(d::s_ITEM_9cf648));            // "ITEM\\"
            crt_strcat(c_str(path), str(d::s_Object_Models_Items_WaterWav_9d85c0));  // "...SmallWave\\"
            void* const anim = jo::view<void*>(jo::self<jo::WaterWave>(this)->anim);
            joan::orig::as_actor_def(jo::view<void*>(jo::self<jo::WaterWave>(this)->model))->load2(joan::orig::arg::str(c_str(path)), str(d::s_WAVE_EFF), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), str(d::s_WAVE_EFF), anim);  // 0x005af3d0
            for (i32 i = 0;; ++i) {
                u8* const mesh = jo::view<u8*>(jo::self<jo::WaterWave>(this)->model->model->mesh);
                if (!(i < jo::as_plane(mesh)->material_count)) break;
                u32& flags = jo::as_plane(mesh)->materials[i].raster;
                flags = flags | 0x100u;
            }
        }
        jo::view<u8>(jo::self<jo::WaterWave>(this)->loaded) = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_09", name, addr, fn)
B("WaterWave::load", 0x0056ed40, &H::wave_load);
#undef B

}  // namespace game_b101_09
}  // namespace

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
namespace {
namespace game_b84_02 {

JOAN_COUNTER(c_clear_56f010, "Mgr56f010::clear_all");

struct H {
    // 0x0056f010 clear all (0xa48aa8).
    void clear_56f010() {
        c_clear_56f010.hit();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i >= 1; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->empty_at_56f3f0(joan::orig::arg::i32(i))) != 0) continue;           // 0x0056f3f0
            std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(this)->get_56f320(joan::orig::arg::i32(i)));      // 0x0056f320
            if (jo::view<void*>(jo::as_scene_object(p)->scene) != nullptr) joan::orig::as_scene(jo::view<void*>(jo::as_scene_object(p)->scene))->detach(joan::orig::arg::mptr(static_cast<void*>(p)));  // 0x005efdb0
            if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);                       // 0x00420840
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));                    // 0x005858c0
        }
        if (jo::view<void*>(jo::self<jo::WaterWave>(this)->model) != nullptr) {
            void* const q = jo::view<void*>(jo::self<jo::WaterWave>(this)->model);
            if (q != nullptr) joan::orig::as_actor_def(q)->destroy(1u);  // 0x004b0c20
            jo::view<void*>(jo::self<jo::WaterWave>(this)->model) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::WaterWave>(this)->anim) != nullptr) {
            void* const q = jo::view<void*>(jo::self<jo::WaterWave>(this)->anim);
            if (q != nullptr) joan::orig::as_anim(q)->scalar_delete(1u);  // 0x00417250
            jo::view<void*>(jo::self<jo::WaterWave>(this)->anim) = nullptr;
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        jo::view<std::uint8_t>(jo::self<jo::WaterWave>(this)->loaded) = 0;
    }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("Mgr56f010::clear_all", 0x0056f010, &H::clear_56f010);
#undef B84

}  // namespace game_b84_02
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

void* list_get(void* list, std::int32_t i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

struct H {
    void* get_56f320(std::int32_t i) { return list_get(this, i); }
    std::int32_t empty_56f3f0(std::int32_t i) { return list_get(this, i) == nullptr ? 1 : 0; }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("List::get_56f320", 0x0056f320, &H::get_56f320);
L82("List::empty_at_56f3f0", 0x0056f3f0, &H::empty_56f3f0);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::WaveList::clear_all() -> void { joan::bind::member<&::game_b84_02::H::clear_56f010>(this); }  // 0x56f010
auto joan::orig::List::get_56f320(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::get_56f320>(this, a0)); }  // 0x56f320
auto joan::orig::List::empty_at_56f3f0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82b::H::empty_56f3f0>(this, a0)); }  // 0x56f3f0
