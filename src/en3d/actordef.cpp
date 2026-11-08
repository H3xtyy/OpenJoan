// en3d/actordef.cpp: functions of the original d:\ww\en3d\actordef.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;

using joan::img::obj;
using joan::img::str;

void* const kUnitTypes = obj(d::g_anim_res_mgr);  // unit type table (0x004b2730 / 0x004174f0)

}  // namespace

// ---------------------------------------- from modules/en3d_helpers37.cpp (en3d/helpers37)
//
//      0x005b03d0 ActorDef::release()            - waits for the loader (SleepEx(5, 1) +
namespace {
namespace en3d_helpers37 {

JOAN_COUNTER(c_actordef_release, "ActorDef::release");

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kActordefFile(d::s_d_ww_en3d_actordef_cpp);  // "d:\ww\en3d\actordef.cpp"

void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

// The original returns whatever eax holds (here `this`, or the deleting dtor's result); no caller uses it as a
// pointer, so the register value is kept as a u32 (its low 32 bits on x64).
std::uintptr_t eax_this(const void* p) { return reinterpret_cast<std::uintptr_t>(p); }

struct H {
    // 0x005b03d0 ActorDef::release().
    std::uintptr_t actordef_release() {
        c_actordef_release.hit();
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_actor_def(this)->loaded()) == 0) {  // 0x005afda0
            reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(std::uint32_t, std::int32_t)>(
                global<std::uintptr_t>(d::iat_KERNEL32_SleepEx))(5, 1);  // SleepEx
            joan::orig::as_actor_def(this)->poll_load();  // 0x005af890
        }
        if (jo::view<std::int8_t>(jo::self<jo::ActorDef>(this)->state) == 0) return eax_this(this);
        for (std::int32_t i = 0; i <= 4; ++i) {
            if (jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]) != nullptr) {
                void* const p = jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]);
                if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);  // 0x00459c40 scalar deleting dtor
                jo::view<void*>(jo::self<jo::ActorDef>(this)->batch[i]) = nullptr;
            }
            jo::self<jo::ActorDef>(this)->lod_count[i] = 0;
        }
        if (jo::self<jo::ActorDef>(this)->matrices != nullptr) mem_free(jo::self<jo::ActorDef>(this)->matrices, img::str(kActordefFile), global<std::int16_t>(d::g_a16e5c) + 0x15);
        jo::self<jo::ActorDef>(this)->matrices = nullptr;
        jo::self<jo::ActorDef>(this)->lods_created = 0;
        if (jo::self<jo::ActorDef>(this)->bone_map != nullptr) {
            mem_free(jo::self<jo::ActorDef>(this)->bone_map, img::str(kActordefFile), global<std::int16_t>(d::g_a16e5c) + 0x1b);
            jo::self<jo::ActorDef>(this)->bone_map = nullptr;
        }
        if (jo::self<jo::ActorDef>(this)->bone_count != 0) {
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(img::obj(d::g_anim_res_mgr))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count))));  // 0x004b2730
            joan::orig::as_anim_set(slot)->release();  // 0x004156d0
        }
        std::uintptr_t result = eax_this(this);
        if (jo::view<void*>(jo::self<jo::ActorDef>(this)->model) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::ActorDef>(this)->model);
            if (p != nullptr) result = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_actor_def(p)->scalar_deleting_destructor(1u));  // 0x005b05d0 scalar deleting dtor
            jo::view<void*>(jo::self<jo::ActorDef>(this)->model) = nullptr;
        }
        jo::self<jo::ActorDef>(this)->state = 0;
        return result;
    }
};

#define HELPER37(name, addr, fn) JOAN_REPLACE("en3d/helpers37", name, addr, fn)
HELPER37("ActorDef::deinit", 0x005b03d0, &H::actordef_release);
#undef HELPER37

}  // namespace en3d_helpers37
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
//
//      0x005b05d0 ActorDef scalar deleting destructor (0x005b0a50)
namespace {
namespace en3d_b82_17 {

using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;                       // operator delete

struct H {
    // 0x005b05d0
    void* actordef_delete(std::uint32_t flags) {
        joan::orig::as_actor_def(this)->dtor();  // 0x005b0a50
        if ((flags & 1u) != 0) game_delete(this);
        return this;
    }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("ActorDef::scalar_deleting_destructor", 0x005b05d0, &H::actordef_delete);
#undef B82_17E

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x005b0620 ActorDef::load_bones(dir, name, rec) (ret 0xc) - <dir><name>.AM3 (0x005c5400) or
//                 the record's bone table (0x005c5610), 0x005c56d0, bone map +0x14 (0x005ba1d0 /
//                 0x0058e7b0 / 0x005ba300); 0 / 1
namespace {
namespace en3d_b84_01 {

void* const kFiles = obj(d::g_misc);      // file system helper (0x00590b80 exists)
void* const kMemory = obj(d::g_mem);     // allocator (0x0058e7b0 alloc / 0x0058eb30 realloc)

// String / path object (0x194 bytes on the stack, no destructor).
struct alignas(4) Path {
    std::uint8_t bytes[0x194];
};
static_assert(sizeof(Path) == 0x194);
void path_ctor(Path& p) { joan::orig::as_string(&p)->construct(); }                             // 0x005a1170
void path_assign(Path& p, const char* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }    // 0x005a1440
void path_append(Path& p, const char* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }        // 0x005a1550
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
bool file_exists(Path& p) { return joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(p))) != 0; }  // 0x00590b80

JOAN_COUNTER(c_load_bones, "ActorDef::load_bones");

struct H {
    // 0x005b0620
    std::int32_t load_bones(const char* dir, const char* name, void* rec) {
        c_load_bones.hit();
        if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count) == 0 && rec == nullptr) return 0;
        if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count) != 0) {
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(kUnitTypes)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count))));  // 0x004b2730
            void* const entry = joan::orig::arg::as<void*>(joan::orig::as_anim_set(type)->entry(1u));  // 0x004168d0
            const std::uint32_t id = jo::view<std::uint32_t>(jo::as_anim_group_rec(entry)->anim_id);
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(kUnitTypes)->slot_4174f0(joan::orig::arg::u32(id)));  // 0x004174f0
            rec = jo::as_anim_slot(slot)->anim;
        }
        if (rec == nullptr) return 0;
        Path path;
        path_ctor(path);
        path_assign(path, dir);
        path_append(path, name);
        path_append(path, str(d::s_AM3));  // ".AM3"
        if (file_exists(path)) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->load_bones_path(joan::orig::arg::str(c_str(path)))) == 0) return 0;  // 0x005c5400
        } else {
            if (jo::as_anim_def(rec)->bones_resolved == 0) return 0;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->load_bones_table(joan::orig::arg::mptr(jo::as_anim_def(rec)->bones))) == 0) return 0;  // 0x005c5610
        }
        if (jo::as_anim_def(rec)->bones_resolved != 0) joan::orig::as_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->link_bones();  // 0x005c56d0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_table(jo::view<void*>(jo::self<jo::ActorDef>(this)->model->bones))->differs(joan::orig::arg::mptr(jo::as_anim_def(rec)->bones))) != 0) {  // 0x005ba1d0
            const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e60)) + 0x43;
            jo::self<jo::ActorDef>(this)->bone_map = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_anim_def(rec)->track_count) << 2), str(d::s_d_ww_en3d_actordef_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0 "d:\ww\en3d\actordef.cpp"
            joan::orig::as_bone_table(jo::view<void*>(jo::self<jo::ActorDef>(this)->model->bones))->map(joan::orig::arg::mptr(jo::self<jo::ActorDef>(this)->bone_map), joan::orig::arg::mptr(jo::as_anim_def(rec)->bones));  // 0x005ba300
        }
        return 1;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("ActorDef::load_bone_hierarchy_5b0620", 0x005b0620, &H::load_bones);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (en3d/b83_08)
//
//      0x005b0850 ActorDef::setup_bones(src, rec) (ret 8) - rec from the unit type when +0x10,
//                 0x005c5560 / 0x005c5610 / 0x005c56d0 on the model, +0x14 = bone map
//                 (0x005ba1d0 / 0x0058e7b0 / 0x005ba300); 0 / 1
namespace {
namespace en3d_b83_08 {

void* const kMemory = obj(d::g_mem);     // allocator

JOAN_COUNTER(c_setup_bones, "ActorDef::setup_bones");

struct H {
    // 0x005b0850
    std::int32_t setup_bones(void* src, void* rec) {
        c_setup_bones.hit();
        if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count) == 0 && rec == nullptr) return 0;
        if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count) != 0) {
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(kUnitTypes)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count))));  // 0x004b2730
            void* const entry = joan::orig::arg::as<void*>(joan::orig::as_anim_set(type)->entry(1u));  // 0x004168d0
            const std::uint32_t id = jo::view<std::uint32_t>(jo::as_anim_group_rec(entry)->anim_id);
            void* const slot = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(kUnitTypes)->slot_4174f0(joan::orig::arg::u32(id)));  // 0x004174f0
            rec = jo::as_anim_slot(slot)->anim;
        }
        if (rec == nullptr) return 0;
        if (src != nullptr) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->load_bones_file(joan::orig::arg::str(src))) == 0) return 0;  // 0x005c5560
        } else {
            if (jo::as_anim_def(rec)->bones_resolved == 0) return 0;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->load_bones_table(joan::orig::arg::mptr(jo::as_anim_def(rec)->bones))) == 0) return 0;  // 0x005c5610
        }
        if (jo::as_anim_def(rec)->bones_resolved != 0) joan::orig::as_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->link_bones();  // 0x005c56d0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_table(jo::view<void*>(jo::self<jo::ActorDef>(this)->model->bones))->differs(joan::orig::arg::mptr(jo::as_anim_def(rec)->bones))) != 0) {  // 0x005ba1d0
            const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a16e64)) + 0x3f;
            jo::self<jo::ActorDef>(this)->bone_map = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::as_anim_def(rec)->track_count) << 2), str(d::s_d_ww_en3d_actordef_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0 "d:\ww\en3d\actordef.cpp"
            joan::orig::as_bone_table(jo::view<void*>(jo::self<jo::ActorDef>(this)->model->bones))->map(joan::orig::arg::mptr(jo::self<jo::ActorDef>(this)->bone_map), joan::orig::arg::mptr(jo::as_anim_def(rec)->bones));  // 0x005ba300
        }
        return 1;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b83_08", name, addr, fn)
E3D("ActorDef::load_bone_hierarchy_5b0850", 0x005b0850, &H::setup_bones);
#undef E3D

}  // namespace en3d_b83_08
}  // namespace

// ---------------------------------------- from modules/en3d_b83_08.cpp (en3d/b83_08)
//
//      0x005afdd0 ActorDef::load(dir, name, short kind, byte flag) (ret 0x10) - waits for the
//                 loader (0x005afda0 / SleepEx(5,1) / 0x005af890), <dir><name>.AM1|.SMO must
//                 exist (0x00590b80, else 0x00586cb0 "Error loading 3D Model"), +0x10 = kind,
//                 new 0x69 model (0x005c0ec0) at +0xc loaded by 0x005c0ef0; kind != 0: unit
//                 type (0x004b2730 on 0xa58cb8) 0x00415090 and 0x005b0620(dir, name, 0);
//                 +0x20 = 0, byte +0 = 1
//
//      0x005af3d0 ActorDef::load(dir, name, byte flag, int anim, arg) (ret 0x14) - same without
//                 the +0x10 store; anim != 0: 0x005b0620(dir, name, arg)
//
//      0x005b02f0 ActorDef::create(a, b) (ret 8)       - new 0x69 model, 0x005c4e60(a, b), +0x10 = 0
//
//      0x005af390 ActorDef::ActorDef()                 - memset 0x2d8
//
//      0x005b0a50 ActorDef::~ActorDef()                - 0x005c4f10
namespace {
namespace en3d_b83_08_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
// KERNEL32 / USER32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;
using joan::img::obj;
using joan::img::str;

constexpr img::Addr kSleepEx = d::iat_KERNEL32_SleepEx;

void* const kFiles = obj(d::g_misc);      // file system helper (0x00590b80 exists)
void* const kLog = obj(d::g_err);        // error log
void* const kUnitTypes = obj(d::g_anim_res_mgr);  // unit type table (0x004b2730 / 0x004174f0)

using joan::orig::helpers::game_new;  // operator new

struct alignas(4) Path {
    std::uint8_t bytes[0x194];
};
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

JOAN_COUNTER(c_load, "ActorDef::load");
JOAN_COUNTER(c_load2, "ActorDef::load2");
JOAN_COUNTER(c_create, "ActorDef::create");

struct H {
    // Shared head of the two loaders: wait for the pending load, skip when loaded already.
    bool wait_loaded() {
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_actor_def(this)->loaded()) == 0) {  // 0x005afda0
            win_call<std::uint32_t>(kSleepEx, 5u, 1);
            joan::orig::as_actor_def(this)->poll_load();  // 0x005af890
        }
        return static_cast<std::int8_t>(jo::self<jo::ActorDef>(this)->state) != 0;
    }

    // <dir><name>.AM1 (anim) / .SMO; false (after the error log) when the file is missing.
    bool model_path(Path& path, const char* dir, const char* name, bool anim) {
        joan::orig::as_string(&path)->construct(joan::orig::arg::str(dir));  // 0x005a11a0
        joan::orig::as_string(&path)->append(joan::orig::arg::str(name));  // 0x005a1550
        if (anim)
            joan::orig::as_string(&path)->append(str(d::s_AM1));                // ".AM1"
        else
            joan::orig::as_string(&path)->append(str(d::s_SMO));                // ".SMO"
        if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(path))) == 0) {  // 0x00590b80
            joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Error_loading_3D_Model_1s), c_str(path));     // 0x00586cb0 "Error loading 3D Model : %1s"
            return false;
        }
        return true;
    }

    // new 0x69 model (0x005c0ec0) into +0xc.
    void new_model() {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Model)));  // 0x69 on x86
        void* model = nullptr;
        if (mem != nullptr) model = joan::orig::arg::as<void*>(joan::orig::as_model(mem)->Model_5c0ec0());  // 0x005c0ec0
        jo::view<void*>(jo::self<jo::ActorDef>(this)->model) = model;
    }

    // 0x005afdd0
    void load(const char* dir, const char* name, std::int16_t kind, std::uint8_t flag) {
        c_load.hit();
        if (wait_loaded()) return;
        Path path;
        if (!model_path(path, dir, name, static_cast<std::int32_t>(kind) != 0)) return;
        jo::self<jo::ActorDef>(this)->bone_count = kind;
        new_model();
        const std::int32_t anim = static_cast<std::int32_t>(kind) != 0 ? 1 : 0;
        const char* const file = c_str(path);
        joan::orig::as_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->load(joan::orig::arg::str(file), joan::orig::arg::u32(anim), joan::orig::arg::u32(flag));  // 0x005c0ef0
        if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count) != 0) {
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(kUnitTypes)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count))));  // 0x004b2730
            joan::orig::as_anim_set(type)->acquire();  // 0x00415090
            joan::orig::as_actor_def(this)->load_bones(joan::orig::arg::str(dir), joan::orig::arg::str(name), nullptr);  // 0x005b0620
        }
        jo::self<jo::ActorDef>(this)->lods_created = 0;
        jo::self<jo::ActorDef>(this)->state = 1;
    }

    // 0x005af3d0
    void load2(const char* dir, const char* name, std::uint8_t flag, const char* anim, void* arg) {
        c_load2.hit();
        if (wait_loaded()) return;
        Path path;
        if (!model_path(path, dir, name, anim != nullptr)) return;
        new_model();
        const std::int32_t is_anim = anim != nullptr ? 1 : 0;
        const char* const file = c_str(path);
        joan::orig::as_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->load(joan::orig::arg::str(file), joan::orig::arg::u32(is_anim), joan::orig::arg::u32(flag));  // 0x005c0ef0
        if (anim != nullptr) joan::orig::as_actor_def(this)->load_bones(joan::orig::arg::str(dir), joan::orig::arg::str(name), arg);  // 0x005b0620
        jo::self<jo::ActorDef>(this)->lods_created = 0;
        jo::self<jo::ActorDef>(this)->state = 1;
    }

    // 0x005b02f0 (eax = this)
    void* create(jo::Plane* a, const void* b) {
        c_create.hit();
        if (static_cast<std::int8_t>(jo::self<jo::ActorDef>(this)->state) != 0) return this;
        new_model();
        joan::orig::as_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->create(a, const_cast<void*>(b));  // 0x005c4e60
        jo::self<jo::ActorDef>(this)->bone_count = 0;
        jo::self<jo::ActorDef>(this)->lods_created = 0;
        jo::self<jo::ActorDef>(this)->state = 1;
        return this;
    }

    // 0x005af390
    void* actordef_ctor() {
        std::memset(static_cast<void*>(this), 0, sizeof(jo::ActorDef));  // memset 0x2d8 (x86)
        return this;
    }

    // 0x005b0a50 (eax = 0x005c4f10's)
    std::uintptr_t actordef_dtor() { return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_model(this)->unload()); }  // 0x005c4f10
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b83_08", name, addr, fn)
E3D("ActorDef::init_5afdd0", 0x005afdd0, &H::load);
E3D("ActorDef::init_5af3d0", 0x005af3d0, &H::load2);
E3D("ActorDef::init_5b02f0", 0x005b02f0, &H::create);
E3D("ActorDef::ActorDef", 0x005af390, &H::actordef_ctor);
E3D("ActorDef::~ActorDef", 0x005b0a50, &H::actordef_dtor);
#undef E3D

}  // namespace en3d_b83_08_2
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x005b0000 ActorDef::load(dir, name, short kind, byte flag) (ret 0x10) - model by 0x005c2370,
//                 kind != 0: 0x004152b0 on the unit type, ".AM3" animation (0x00577be0 / 0x00577c70 /
//                 0x00577d10) at +4; +0x20 = 0, byte +3 = 0x15
//
//      0x005af5d0 ActorDef::load(dir, name, byte flag, anim, arg) (ret 0x14) - same, anim != 0:
//                 +8 = arg and the ".AM3" animation; byte +3 = 0xb, +0x20 = 0
namespace {
namespace en3d_b84_01_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

void* const kFiles = obj(d::g_misc);      // file system helper (0x00590b80 exists)
void* const kLog = obj(d::g_err);        // error log
void* const kUnitTypes = obj(d::g_anim_res_mgr);  // unit type table (0x004b2730 / 0x004174f0)

using joan::orig::helpers::game_new;  // operator new

// String / path object (0x194 bytes on the stack, no destructor).
struct alignas(4) Path {
    std::uint8_t bytes[0x194];
};
static_assert(sizeof(Path) == 0x194);
void path_ctor(Path& p) { joan::orig::as_string(&p)->construct(); }                             // 0x005a1170
void path_assign(Path& p, const char* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }    // 0x005a1440
void path_append(Path& p, const char* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }        // 0x005a1550
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
bool file_exists(Path& p) { return joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(p))) != 0; }  // 0x00590b80

JOAN_COUNTER(c_load_kind, "ActorDef::load(kind)");
JOAN_COUNTER(c_load_anim, "ActorDef::load(anim)");

struct H {
    // <dir><name>.AM1 | .SMO; false (after the error log) when the file is missing.
    static bool model_path(Path& path, const char* dir, const char* name, bool anim) {
        joan::orig::as_string(&path)->construct(joan::orig::arg::str(dir));  // 0x005a11a0
        path_append(path, name);
        if (anim)
            path_append(path, str(d::s_AM1));  // ".AM1"
        else
            path_append(path, str(d::s_SMO));  // ".SMO"
        if (!file_exists(path)) {
            joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Error_loading_3D_Model_1s), c_str(path));  // 0x00586cb0 "Error loading 3D Model : %1s"
            return false;
        }
        return true;
    }
    void* new_model() {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Model)));  // 0x69 on x86
        return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_model(mem)->Model_5c0ec0()) : nullptr;  // 0x005c0ec0
    }
    // ".AM3" animation object at +4.
    void load_am3(const char* dir, const char* name) {
        Path path;
        path_ctor(path);
        path_assign(path, dir);
        path_append(path, name);
        path_append(path, str(d::s_AM3));  // ".AM3"
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::FileLoader)));  // 0x164 on x86
        void* const anim = mem != nullptr ? joan::orig::as_file_loader(mem)->construct() : nullptr;  // 0x00577be0
        jo::self<jo::ActorDef>(this)->anim_file = anim;
        joan::orig::as_file_loader(jo::self<jo::ActorDef>(this)->anim_file)->set_path(joan::orig::arg::str(c_str(path)));     // 0x00577c70
        joan::orig::as_file_loader(jo::self<jo::ActorDef>(this)->anim_file)->start(nullptr, 0u, joan::orig::arg::u32(-1), nullptr, nullptr);  // 0x00577d10
    }

    // 0x005b0000
    void load_kind(const char* dir, const char* name, std::uint32_t kind_arg, std::uint32_t flag) {
        c_load_kind.hit();
        if (static_cast<std::int8_t>(jo::self<jo::ActorDef>(this)->state) != 0) return;
        const std::int16_t kind = static_cast<std::int16_t>(kind_arg);
        Path path;
        if (!model_path(path, dir, name, static_cast<std::int32_t>(kind) != 0)) return;
        jo::self<jo::ActorDef>(this)->bone_count = kind;
        jo::view<void*>(jo::self<jo::ActorDef>(this)->model) = new_model();
        joan::orig::as_shadow_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->begin_load(joan::orig::arg::str(c_str(path)), joan::orig::arg::u32(static_cast<std::int32_t>(kind) != 0 ? 1 : 0), joan::orig::arg::u32(static_cast<std::uint8_t>(flag)));  // 0x005c2370
        if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count) != 0) {
            void* const type = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(kUnitTypes)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count))));  // 0x004b2730
            joan::orig::as_anim_set(type)->add_ref();  // 0x004152b0
            load_am3(dir, name);
        }
        jo::self<jo::ActorDef>(this)->lods_created = 0;
        jo::view<std::uint8_t>(jo::self<jo::ActorDef>(this)->loaded_) = 0x15;
    }

    // 0x005af5d0
    void load_anim(const char* dir, const char* name, std::uint32_t flag, const char* anim, void* arg) {
        c_load_anim.hit();
        if (static_cast<std::int8_t>(jo::self<jo::ActorDef>(this)->state) != 0) return;
        Path path;
        if (!model_path(path, dir, name, anim != nullptr)) return;
        jo::view<void*>(jo::self<jo::ActorDef>(this)->model) = new_model();
        joan::orig::as_shadow_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->begin_load(joan::orig::arg::str(c_str(path)), joan::orig::arg::u32(anim != nullptr ? 1 : 0), joan::orig::arg::u32(static_cast<std::uint8_t>(flag)));  // 0x005c2370
        if (anim != nullptr) {
            jo::self<jo::ActorDef>(this)->user = arg;
            load_am3(dir, name);
        }
        jo::view<std::uint8_t>(jo::self<jo::ActorDef>(this)->loaded_) = 0xb;
        jo::self<jo::ActorDef>(this)->lods_created = 0;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("ActorDef::load(kind)", 0x005b0000, &H::load_kind);
E3D("ActorDef::async_init", 0x005af5d0, &H::load_anim);
#undef E3D

}  // namespace en3d_b84_01_2
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
//
//      0x005af890 ActorDef::poll_load()          - loader state machine on byte +3 (0xb / 0xc / 0xd /
//                 0x15 / 0x16): 0x005c3110 / 0x005c4e30 on +0x0c, reader (0x11c bytes) on the
//                 +4 stream -> 0x005b0850, deletes +4 and sets byte +0 when done
//
//      0x005afda0 ActorDef::loaded()             - byte +3 == 0
namespace {
namespace en3d_b82_17_2 {

using joan::call::thiscall;
using joan::img::obj;

void* const kFiles = obj(d::g_misc);   // array helper object (0x005911a0 / 0x005910f0)

std::uint32_t timer() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(kFiles)->get_time_591050()); }  // 0x00591050

// 0x00587f80 reader object (0x11c bytes, see en3d_helpers28.cpp).
struct alignas(alignof(void*)) Reader {  // a FileMem: 0x11c on x86, 0x12c on x64
    std::uint8_t bytes[((sizeof(jo::FileMem) > 0x11c ? sizeof(jo::FileMem) : 0x11c) + alignof(void*) - 1) / alignof(void*) * alignof(void*)];
};

JOAN_COUNTER(c_poll_load, "ActorDef::poll_load");

struct H {
    // Reader on the +4 stream (+0x10c, +0x118) handed to 0x005b0850(reader, arg).
    void load_with_reader(void* arg) {
        Reader reader;
        joan::orig::as_file_mem(&reader)->construct();  // 0x00587f80
        joan::orig::as_file_mem(&reader)->open_mem_5881c0(joan::orig::arg::str(jo::as_file_loader(jo::self<jo::ActorDef>(this)->anim_file)->buffer), joan::orig::arg::i32(jo::as_file_loader(jo::self<jo::ActorDef>(this)->anim_file)->bytes_read), static_cast<char>(joan::orig::arg::i32(1u)), static_cast<char>(joan::orig::arg::i32(0u)));                                                 // 0x005881c0
        joan::orig::as_actor_def(this)->setup_bones(static_cast<void*>(&reader), arg);  // 0x005b0850
        joan::orig::as_file_mem(&reader)->close();                                // 0x005886d0
        joan::orig::as_file_mem(&reader)->destruct();                                // 0x00587fd0 ~Reader
    }

    // 0x005af890
    void poll_load() {
        c_poll_load.hit();
        const std::uint32_t start = timer();
        std::int32_t timed_out = 0;
        const std::int32_t state = jo::self<jo::ActorDef>(this)->loaded_;
        if (state == 0xb) {
            joan::orig::as_shadow_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->poll_load();                                     // 0x005c3110
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_shadow_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->loaded()) != 0) jo::self<jo::ActorDef>(this)->loaded_ = 0xc;  // 0x005c4e30
        } else if (state == 0xc) {
            if (jo::self<jo::ActorDef>(this)->anim_file == nullptr) {
                jo::self<jo::ActorDef>(this)->loaded_ = 0xd;
            } else if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::ActorDef>(this)->anim_file)->done()) != 0) {  // 0x00578250
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::ActorDef>(this)->anim_file)->ok()) == 0)       // 0x005b0a80
                    joan::orig::as_actor_def(this)->setup_bones(nullptr, jo::self<jo::ActorDef>(this)->user);  // 0x005b0850
                else
                    load_with_reader(jo::self<jo::ActorDef>(this)->user);
                jo::self<jo::ActorDef>(this)->loaded_ = 0xd;
            }
        } else if (state == 0xd) {
            if (jo::self<jo::ActorDef>(this)->anim_file != nullptr) {
                void* const p = jo::self<jo::ActorDef>(this)->anim_file;
                if (p != nullptr) joan::orig::as_file_loader(p)->scalar_deleting_destructor(1u);  // 0x005afd50 scalar delete
                jo::self<jo::ActorDef>(this)->anim_file = nullptr;
            }
            jo::self<jo::ActorDef>(this)->loaded_ = 0;
            jo::view<std::int8_t>(jo::self<jo::ActorDef>(this)->state) = 1;
        } else if (state == 0x15) {
            std::int32_t ready = 1;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_shadow_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->loaded()) == 0) {  // 0x005c4e30
                if (timed_out == 0) {
                    joan::orig::as_shadow_model(jo::view<void*>(jo::self<jo::ActorDef>(this)->model))->poll_load();  // 0x005c3110
                    if (timer() - start > 5u) timed_out = 1;
                }
                ready = 0;
            }
            if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count) != 0) {
                void* const sound = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count))));  // 0x004b2730
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set(sound)->idle()) == 0) {  // 0x004156a0
                    if (timed_out == 0) {
                        void* const s2 = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count))));
                        joan::orig::as_anim_set(s2)->stream();  // 0x00415300
                        if (timer() - start > 5u) timed_out = 1;
                    }
                    ready = 0;
                }
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::ActorDef>(this)->anim_file)->done()) == 0) ready = 0;  // 0x00578250
            }
            if (ready != 0) {
                if (static_cast<std::int32_t>(jo::self<jo::ActorDef>(this)->bone_count) != 0) {
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::self<jo::ActorDef>(this)->anim_file)->ok()) == 0)  // 0x005b0a80
                        joan::orig::as_actor_def(this)->setup_bones(joan::orig::arg::mptr(static_cast<void*>(nullptr)), joan::orig::arg::mptr(0u));  // 0x005b0850
                    else
                        load_with_reader(nullptr);
                }
                jo::self<jo::ActorDef>(this)->loaded_ = 0x16;
            }
        } else if (state == 0x16) {
            if (jo::self<jo::ActorDef>(this)->anim_file != nullptr) {
                joan::orig::as_file_loader(jo::self<jo::ActorDef>(this)->anim_file)->stop();  // 0x00578110
                void* const p = jo::self<jo::ActorDef>(this)->anim_file;
                if (p != nullptr) joan::orig::as_file_loader(p)->scalar_deleting_destructor(1u);  // 0x005afd50 scalar delete
                jo::self<jo::ActorDef>(this)->anim_file = nullptr;
            }
            jo::view<std::int8_t>(jo::self<jo::ActorDef>(this)->state) = 1;
            jo::self<jo::ActorDef>(this)->loaded_ = 0;
        }
    }

    // 0x005afda0
    std::int32_t loaded() { return jo::self<jo::ActorDef>(this)->loaded_ == 0 ? 1 : 0; }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("ActorDef::process_async_init", 0x005af890, &H::poll_load);
B82_17E("ActorDef::is_async_init_finish", 0x005afda0, &H::loaded);
#undef B82_17E

}  // namespace en3d_b82_17_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers32.cpp (en3d/helpers32)
namespace {
namespace en3d_helpers32 {

using joan::call::thiscall;
using joan::call::cdecl_call;

struct H {
    // 0x005afd50 FileLoader scalar deleting destructor.
    void* loader_delete(std::uint32_t flags) {
        joan::orig::as_file_loader(this)->stop_and_reset();  // 0x00577c30
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define HELPER32(name, addr, fn) JOAN_REPLACE("en3d/helpers32", name, addr, fn)
HELPER32("FileLoader::scalar_deleting_destructor", 0x005afd50, &H::loader_delete);
#undef HELPER32

}  // namespace en3d_helpers32
}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

using u32 = std::uint32_t;

struct H {
    // ---------------------------------------------------------------- stubs / getters
    void stub0() {}
    void stub1(u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("Obj::stub_5b0a10", 0x005b0a10, &H::stub0);
B("Obj::stub_5b0a30", 0x005b0a30, &H::stub1);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/en3d_helpers32_2.cpp (en3d/helpers32)
namespace {
namespace en3d_helpers32_2 {


struct H {
    // 0x005b0a80 FileLoader::ok().
    std::int32_t loader_ok() { return jo::view<std::int8_t>(jo::self<jo::FileLoader>(this)->error_code) == 0 ? 1 : 0; }
};

#define HELPER32(name, addr, fn) JOAN_REPLACE("en3d/helpers32", name, addr, fn)
HELPER32("FileLoader::ok", 0x005b0a80, &H::loader_ok);
#undef HELPER32

}  // namespace en3d_helpers32_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ActorDef::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_08_2::H::actordef_ctor>(this)); }  // 0x5af390
auto joan::orig::ActorDef::load2(const char* a0, const char* a1, std::uint8_t a2, const char* a3, void* a4) -> void { joan::bind::member<&::en3d_b83_08_2::H::load2>(this, a0, a1, a2, a3, a4); }  // 0x5af3d0
auto joan::orig::ActorDef::load_anim(const char* a0, const char* a1, std::uint32_t a2, const char* a3, void* a4) -> void { joan::bind::member<&::en3d_b84_01_2::H::load_anim>(this, a0, a1, a2, a3, a4); }  // 0x5af5d0
auto joan::orig::ActorDef::poll_load() -> void { joan::bind::member<&::en3d_b82_17_2::H::poll_load>(this); }  // 0x5af890
auto joan::orig::FileLoader::scalar_deleting_destructor(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers32::H::loader_delete>(this, a0)); }  // 0x5afd50
auto joan::orig::ActorDef::loaded() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b82_17_2::H::loaded>(this)); }  // 0x5afda0
auto joan::orig::ActorDef::load(const char* a0, const char* a1, std::int16_t a2, std::uint8_t a3) -> void { joan::bind::member<&::en3d_b83_08_2::H::load>(this, a0, a1, a2, a3); }  // 0x5afdd0
auto joan::orig::ActorDef::load_kind(const char* a0, const char* a1, std::uint32_t a2, std::uint32_t a3) -> void { joan::bind::member<&::en3d_b84_01_2::H::load_kind>(this, a0, a1, a2, a3); }  // 0x5b0000
auto joan::orig::ActorDef::create(Plane* a0, const void* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_08_2::H::create>(this, a0, a1)); }  // 0x5b02f0
auto joan::orig::ActorDef::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers37::H::actordef_release>(this)); }  // 0x5b03d0
auto joan::orig::ActorDef::scalar_deleting_destructor(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::actordef_delete>(this, a0)); }  // 0x5b05d0
auto joan::orig::ActorDef::load_bones(const char* a0, const char* a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01::H::load_bones>(this, a0, a1, a2)); }  // 0x5b0620
auto joan::orig::ActorDef::setup_bones(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b83_08::H::setup_bones>(this, a0, a1)); }  // 0x5b0850
auto joan::orig::ActorDef::dtor() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b83_08_2::H::actordef_dtor>(this)); }  // 0x5b0a50
auto joan::orig::FileLoader::ok() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers32_2::H::loader_ok>(this)); }  // 0x5b0a80
