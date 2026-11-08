// game/modelres.cpp: functions of the original d:\ww\modelres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../runtime/hitch.hpp"

#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../original/records.hpp"
#include <cmath>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

using joan::call::cdecl_call;

// Record sizes of the tables below (x86: 0x2b / 0xa4 bytes).
constexpr std::int32_t kEntry = static_cast<std::int32_t>(sizeof(jo::ModelEntry));
constexpr std::int32_t kMorph = static_cast<std::int32_t>(sizeof(jo::ModelMorphRec));
// The i-th morph record of a model entry.
jo::ModelMorphRec* morph_rec(const void* m, std::int32_t i) { return jo::as_model_morph_rec(jo::view<std::uint8_t*>(jo::as_model_entry(m)->morphs) + i * kMorph); }

}  // namespace

// ---------------------------------------- from modules/game_b107_13.cpp (game/b107_13)
//
//    - ModelRes::free_all 0x004b0a10 (ret 0): when byte +0xc is set, for each entry
//      j = 1..[+0] of the 0x2b-byte table [+8]: scalar-delete +0x1f (0x004b0c20), and when
//      the 0xa4-byte sub-array +0x27 exists, loop i < +0x23 testing sub[i]+0xa0 but deleting
//      sub[j]+0xa0 (0x004b0c70) - the original indexes the delete with the OUTER index j,
//      kept as is; then free sub (0x0058ed70 on 0xa48838, 'd:\ww\modelres.cpp', line
//      [0xa01730] + 0x1a). Then +0 = 0, free +8 (line + 0x24), +4 = 0, 0x005a17d0(+0xd),
//      byte +0xc = 0. Both callers ignore eax.
namespace {
namespace game_b107_13 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_free_all, "ModelRes::free_all");

struct H {
    // 0x004b0a10
    void free_all() {
        if (static_cast<i32>(jo::view<std::int8_t>(jo::self<jo::ModelRes>(this)->loaded)) == 0) return;
        c_free_all.hit();
        for (i32 j = 1; j <= jo::self<jo::ModelRes>(this)->count; ++j) {
            u8* e = reinterpret_cast<u8*>(&jo::self<jo::ModelRes>(this)->entries[j - 1]);
            if (jo::as_model_entry(e)->actor_def != nullptr) {
                jo::ActorDef* const p = jo::as_model_entry(e)->actor_def;
                if (p != nullptr) joan::orig::as_actor_def(p)->destroy(1u);  // 0x004b0c20
                jo::as_model_entry(e)->actor_def = nullptr;
            }
            if (jo::as_model_entry(e)->morphs != nullptr) {
                for (i32 i = 0; i < jo::as_model_entry(e)->morph_count; ++i) {
                    if (jo::as_model_entry(e)->morphs[i].morph == nullptr) continue;
                    jo::Morph* const p = jo::as_model_entry(e)->morphs[j].morph;  // outer index
                    if (p != nullptr) joan::orig::as_morph(p)->destroy(1u);             // 0x004b0c70
                }
                const i32 line = static_cast<i32>(global<i16>(d::g_a01730)) + 0x1a;
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::as_model_entry(e)->morphs), str(d::s_d_ww_modelres_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
                jo::as_model_entry(e)->morphs = nullptr;
            }
        }
        jo::view<u32>(jo::self<jo::ModelRes>(this)->count) = 0;
        if (jo::self<jo::ModelRes>(this)->entries != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a01730)) + 0x24;
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<jo::ModelRes>(this)->entries), str(d::s_d_ww_modelres_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::ModelRes>(this)->entries = nullptr;
        }
        jo::view<u32>(jo::self<jo::ModelRes>(this)->capacity) = 0;
        joan::orig::as_list(&jo::self<jo::ModelRes>(this)->strings)->clear_5a17d0();  // 0x005a17d0
        jo::self<jo::ModelRes>(this)->loaded = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_13", name, addr, fn)
B("ModelRes::free_all", 0x004b0a10, &H::free_all);
#undef B

}  // namespace game_b107_13
}  // namespace

// ---------------------------------------- from modules/game_units4.cpp (game/units4)
//
//      - small direct callees: Modifiers::slot 0x004974f0, PtrList::is_empty_at 0x00457f60,
//        PtrList::get 0x00457e90, ModTable::value_own 0x004a41c0, ModTable::value 0x004a4200,
//        EffectRegistry::remove 0x0041bbf0, SoundSet::remove 0x006adf70, scalar deleting
//        destructors 0x004b0c20 / 0x004b0c70, Holder::release 0x006356c0.
namespace {
namespace game_units4 {

using joan::orig::helpers::game_delete;                                                     // operator delete

struct H {
    // 0x004b0c20 / 0x004b0c70 scalar deleting destructors
    void* sdd_4b0c20(std::uint32_t flags) {
        joan::orig::as_actor_def(this)->destruct();  // 0x0047b5b0
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
    void* sdd_4b0c70(std::uint32_t flags) {
        joan::orig::as_camera(this)->wrap_5c7b80();  // 0x005c67f0
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
};

#define U4(name, addr, fn) JOAN_REPLACE("game/units4", name, addr, fn)
U4("Object::sdd_4b0c20", 0x004b0c20, &H::sdd_4b0c20);
U4("Object::sdd_4b0c70", 0x004b0c70, &H::sdd_4b0c70);
#undef U4

}  // namespace game_units4
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
//
//    - ModelResMgr::release 0x004b16a0 (id) ret 4 (modelres.cpp).
//
//    - ModelResMgr::set_params 0x004b0ec0 (id, b, b, w) ret 0x10.
//
//    - 0x004d0d80, 0x004afa80, 0x0041baa0, 0x00416ed0, 0x004b1640, 0x004d09c0, WORKSHOP texts
//      0x004ddf90 / 0x004dde10 / 0x004ddde0 / 0x004dddb0 / 0x004ddd80 / 0x004ddd50, 0x0041b440,
//      0x00416f20, 0x004156a0, 0x00430410, 0x004d06b0, 0x004d0550, 0x00497580, 0x0042ad30.
//
//  * Word / byte arguments pushed with junk in the upper bits are only read as word / byte by
//    the callees (checked: 0x00417db0, 0x00417b60, 0x00417af0, 0x00417f20, 0x004b1640,
//    0x004d2b10, 0x004c59d0, 0x004d1220, 0x004d0820, 0x004cf690, 0x0041a740).
namespace {
namespace game_b83_07 {

using joan::call::stdcall_call;

JOAN_COUNTER(c_model_release, "ModelResMgr::release");

constexpr img::Addr kAnimRes = d::g_anim_res_mgr;

void sleep_5() { stdcall_call<std::uint32_t>(global<std::uintptr_t>(d::iat_KERNEL32_SleepEx), 5u, 1u); }  // SleepEx(5, TRUE)

struct H {
    // ------------------------------------------------------------------ 0x004b16a0
    std::int32_t model_release(std::int16_t id) {
        c_model_release.hit();
        std::uint8_t* const m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(this)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x0041a180
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_model_res_mgr(this)->ready(joan::orig::arg::i16(id))) == 0) {  // 0x004b1640
            sleep_5();
            joan::orig::as_model_res_mgr(this)->update_anim(joan::orig::arg::i16(id));  // 0x004d2b10
        }
        --jo::as_model_entry(m)->ref_count;
        if (jo::as_model_entry(m)->ref_count != 0) return jo::as_model_entry(m)->ref_count;
        if (jo::as_model_entry(m)->actor_def != nullptr) {
            const std::int8_t b1b = jo::as_model_entry(m)->param_a;
            const std::int8_t b1c = jo::as_model_entry(m)->param_b;
            const std::int16_t w1d = jo::as_model_entry(m)->param_anim;
            void* const p = jo::as_model_entry(m)->actor_def;
            if (p != nullptr) joan::orig::as_actor_def(p)->destroy(1u);  // 0x004b0c20
            jo::as_model_entry(m)->actor_def = nullptr;
            if (b1b != 0 || b1c == 0) {
                if (jo::view<std::int16_t>(jo::as_model_entry(m)->anim) != 0) {
                    void* const a = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(kAnimRes))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_model_entry(m)->anim)))));  // 0x004b2730
                    joan::orig::as_anim_set(a)->release();  // 0x004156d0
                }
            } else if (w1d != 0) {
                joan::orig::as_anim_res_mgr(obj(kAnimRes))->release(joan::orig::arg::i16(w1d));  // 0x00417e00
            }
            for (std::int32_t i = 0; i < jo::as_model_entry(m)->morph_count; ++i) {
                void* const t = morph_rec(m, i)->morph;
                if (t != nullptr) joan::orig::as_morph(t)->destroy(1u);  // 0x004b0c70
                morph_rec(m, i)->morph = nullptr;
            }
        }
        for (std::int32_t i = 0; i < jo::as_model_entry(m)->morph_count; ++i) {
            if (morph_rec(m, i)->morph == nullptr) continue;
            void* const t = morph_rec(m, i)->morph;
            if (t != nullptr) joan::orig::as_morph(t)->destroy(1u);
            morph_rec(m, i)->morph = nullptr;
        }
        return jo::as_model_entry(m)->ref_count;
    }

    // ------------------------------------------------------------------ 0x004b0ec0
    void* set_params(std::int16_t id, std::int8_t a, std::int8_t b, std::int16_t w) {
        std::uint8_t* const m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(this)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x0041a180
        if (jo::as_model_entry(m)->i8_1a != 0) {
            jo::as_model_entry(m)->param_anim = w;
        } else {
            jo::as_model_entry(m)->param_a = a;
            jo::as_model_entry(m)->param_b = b;
            jo::as_model_entry(m)->param_anim = w;
            jo::as_model_entry(m)->i8_1a = 1;
        }
        return m;
    }

    // ------------------------------------------------------------------ 0x004b1640
    std::int32_t model_ready(std::int16_t id) {
        std::uint8_t* const m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(this)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x0041a180
        return jo::as_model_entry(m)->load_state == 0 ? 1 : 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("ModelResMgr::release", 0x004b16a0, &H::model_release);
B("ModelResMgr::set_params", 0x004b0ec0, &H::set_params);
B("ModelResMgr::ready", 0x004b1640, &H::model_ready);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
//
//    - ModelRes::acquire 0x004b0f40 (modelres.cpp, id): reference count, first use loads the
//      model, textures and animations (ANIM\).
namespace {
namespace game_b82_16 {

// Path string (0x190 chars + NUL, 0x005a11a0 / 0x005a1550 / 0x00417d90).
struct alignas(4) Path {
    char b[0x194];
};

JOAN_COUNTER(c_acquire, "ModelRes::acquire");

using joan::orig::helpers::game_new;  // operator new
void sleep_ex() {
    reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(std::uint32_t, std::int32_t)>(global<std::uintptr_t>(d::iat_KERNEL32_SleepEx))(5u, 1);
}
using joan::orig::helpers::pump;

struct H {
    // ------------------------------------------------------------------ 0x004b0f40
    std::int32_t model_acquire(std::int16_t id) {
        joan::hitch::Span hitch_span("model_acquire");  // [Debug] HitchLog
        c_acquire.hit();
        std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_model_res_mgr(this)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x0041a180
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_model_res_mgr(this)->ready(joan::orig::arg::i16(id))) == 0) {  // 0x004b1640
            sleep_ex();
            joan::orig::as_model_res_mgr(this)->update_anim(joan::orig::arg::i16(id));  // 0x004d2b10
        }
        jo::as_model_entry(p)->ref_count += 1;
        if (jo::as_model_entry(p)->ref_count == 1 && jo::as_model_entry(p)->actor_def == nullptr) {
            const std::int8_t b18 = jo::as_model_entry(p)->param_a;
            const std::int8_t b1c = jo::as_model_entry(p)->param_b;
            const std::int16_t w20 = jo::as_model_entry(p)->param_anim;
            void* const mem = game_new(sizeof(jo::ActorDef));  // 0x2d8 on x86
            jo::as_model_entry(p)->actor_def = static_cast<jo::ActorDef*>(mem != nullptr ? joan::orig::as_actor_def(mem)->construct() : nullptr);  // 0x005af390
            if (b18 != 0 || b1c == 0) {
                if (jo::view<std::int16_t>(jo::as_model_entry(p)->anim) != 0) {
                    void* const r = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_model_entry(p)->anim)))));  // 0x004b2730
                    joan::orig::as_anim_set(r)->acquire();  // 0x00415090
                    pump();
                }
                joan::orig::as_actor_def(jo::as_model_entry(p)->actor_def)->load(joan::orig::arg::str(jo::as_model_entry(p)->dir), joan::orig::arg::str(jo::as_model_entry(p)->file), joan::orig::arg::i16(jo::view<std::int16_t>(jo::as_model_entry(p)->anim)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x005afdd0
                pump();
            } else {
                joan::orig::as_anim_set(obj(d::g_anim_res_mgr))->acquire_417540(joan::orig::arg::u32(w20));  // 0x00417540
                pump();
                void* const v =
                    *joan::orig::arg::as<void**>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(static_cast<std::int32_t>(w20))));  // 0x004174f0
                joan::orig::as_actor_def(jo::as_model_entry(p)->actor_def)->load2(joan::orig::arg::str(jo::as_model_entry(p)->dir), joan::orig::arg::str(jo::as_model_entry(p)->file), static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<std::uint8_t>(0))), jo::as_model_entry(p)->file, v);  // 0x005af3d0
                pump();
            }
            if (jo::as_model_entry(p)->anim_db_id != 0) {
                std::uint8_t* const r =
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_db(obj(d::g_texture_effect_res))->entry(joan::orig::arg::u32(*reinterpret_cast<const std::int16_t*>(&jo::as_model_entry(p)->anim_db_id))));  // 0x004b27b0
                auto mesh = [&]() { return jo::view<std::uint8_t*>(jo::as_model(jo::view<void*>(jo::as_actor_def(jo::as_model_entry(p)->actor_def)->model))->mesh); };
                for (std::int32_t i = jo::as_plane(mesh())->material_count; i > 0; --i) {
                    std::uint8_t* const e = reinterpret_cast<std::uint8_t*>(&jo::as_plane(mesh())->materials[i - 1]);  // 0x54-byte Materials on x86
                    std::int32_t s = 0;
                    for (; s < 6; ++s)
                        if (i == jo::view<std::int16_t>(jo::as_tex_script_rec(r)->material_indices[s])) break;
                    if (s == 6) continue;
                    for (std::int32_t k = jo::view<std::int16_t>(jo::as_tex_script_rec(r)->stage_count[s]); k > 0; --k) {
                        void* const tex = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(jo::as_tex_script_rec(r)->stage_names + s * 0x84), 1, 0u));  // 0x006099c0
                        joan::orig::as_material(e)->set_texture(joan::orig::arg::i32(k - 1), joan::orig::arg::mptr(tex));              // 0x005d5650
                        joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(tex));       // 0x0060a550
                        jo::as_material(e)->raster |= 0x100u;
                        jo::as_material(e)->texture_mode =
                            jo::view<std::uint32_t>(jo::as_model_entry(p)->anim_db_id) | 0x1000000u | (static_cast<std::uint32_t>(s) << 16);
                    }
                }
                pump();
            }
            Path path;
            joan::orig::as_string(&path)->construct(joan::orig::arg::str(jo::as_model_entry(p)->dir));  // 0x005a11a0
            joan::orig::as_string(&path)->append(str(d::s_ANIM));        // 0x005a1550 append "ANIM\"
            for (std::int32_t i = 0; i < jo::as_model_entry(p)->morph_count; ++i) {
                jo::ModelMorphRec* const rec = morph_rec(p, i);
                void* const am = game_new(sizeof(jo::Morph));  // 0x84 on x86
                rec->morph = static_cast<jo::Morph*>(am != nullptr ? joan::orig::as_morph(am)->construct() : nullptr);  // 0x005c67b0
                joan::orig::as_morph(rec->morph)->load(joan::orig::arg::str(rec->name), joan::orig::arg::str(jo::as_model_entry(p)->dir), joan::orig::arg::str(rec->file), joan::orig::arg::mptr(0));  // 0x005c6820
                if (joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(morph_rec(p, i)->anim))) != 0) {  // strlen
                    void* const sm = game_new(sizeof(jo::MorphAnim));  // 0x10 on x86
                    void* const snd = sm != nullptr ? joan::orig::as_morph_anim(sm)->construct() : nullptr;  // 0x005cae20
                    rec->morph->anim = static_cast<jo::MorphAnim*>(snd);
                    void* const anim = rec->morph;
                    char* const name = morph_rec(p, i)->anim;
                    const char* const dir = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
                    joan::orig::as_morph_anim(rec->morph->anim)->load(joan::orig::arg::str(dir), joan::orig::arg::str(name), joan::orig::arg::mptr(anim));  // 0x005cae80
                }
            }
            pump();
        }
        return jo::as_model_entry(p)->ref_count;
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("ModelRes::acquire", 0x004b0f40, &H::model_acquire);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
//
//    - ModelRes::add 0x004b18d0 (dir, file, flags): splits ".SMO" / ".AM1", looks up an
//      existing entry (0x004b20e0), else appends a 0x2b-byte record (strings 0x005a18d0, unit
//      type match through 0xa47c01, "MORPH" table records 0xa4 bytes each).
//
//  - 0x004b18d0: the "MORPH" name buffer (ebp-0x248, 0x20 bytes) is followed by the table
//    pointer (ebp-0x228); 0x00590210 writes its NUL at buf[n] (n <= 0x20), which can zero the
//    pointer's low byte. The frame layout of those two locals is kept so this happens the same.
namespace {
namespace game_b85_06 {

void* const kMemory = obj(d::g_mem);     // allocator (0x0058eb30 realloc)
void* const kFiles = obj(d::g_misc);      // file / text helpers
const char* const kModelresCpp = str(d::s_d_ww_modelres_cpp);  // "d:\ww\modelres.cpp"

using joan::orig::helpers::memset_;
std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
using joan::orig::helpers::strcpy_;
char* strcat_(char* d, const char* s) { return std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
char* strncpy_(char* d, const char* s, std::uint32_t n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
char* strchr_(const char* s, std::int32_t c) { return joan::orig::arg::as<char*>(joan::orig::libc::strchr(joan::orig::arg::str(s), joan::orig::arg::i32(c))); }
using joan::orig::helpers::stricmp_;
void* mem_realloc(void* p, std::int32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(kMemory)->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}
std::int32_t line_base() { return static_cast<std::int32_t>(global<std::int16_t>(d::g_a01738)); }

JOAN_COUNTER(c_model_add, "ModelRes::add");

struct H {
    // ---- 0x004b18d0 ModelRes::add ----------------------------------------------------------
    std::int32_t model_add(const char* dir, const char* file, std::int32_t flags) {
        c_model_add.hit();
        char name[0x104];  // ebp-0x108
        char path[0x104];  // ebp-0x218
        struct {
            char text[0x20];  // ebp-0x248
            void* table;      // ebp-0x228 (0x00590210's NUL can land on its low byte)
        } morph;
        static_assert(sizeof(morph) == 0x20 + sizeof(void*));  // x86: 0x24

        const char* const dot = strchr_(file, 0x2e);
        if (dot != nullptr) {
            strncpy_(name, file, static_cast<std::uint32_t>(dot - file));
            name[dot - file] = 0;
        } else {
            strcpy_(name, file);
        }
        char type = 0x53;  // 'S'
        if (dot != nullptr) {
            if (stricmp_(dot, str(d::s_SMO)) == 0) {  // ".SMO"
                type = 0x53;
            } else if (stricmp_(dot, str(d::s_AM1)) == 0) {  // ".AM1"
                type = 0x41;
            }
        }
        const std::int32_t found = joan::orig::arg::as<std::int32_t>(joan::orig::as_table(this)->find_4b20e0(joan::orig::arg::str(dir), joan::orig::arg::str(static_cast<const char*>(name)), static_cast<std::int8_t>(joan::orig::arg::i32(type)), joan::orig::arg::i32(flags)));  // 0x004b20e0
        if (found != 0) return found;

        jo::self<jo::ModelRes>(this)->count += 1;
        if (jo::self<jo::ModelRes>(this)->count > jo::self<jo::ModelRes>(this)->capacity) {
            jo::self<jo::ModelRes>(this)->capacity = jo::self<jo::ModelRes>(this)->count + 0x3f;
            jo::view<void*>(jo::self<jo::ModelRes>(this)->entries) = mem_realloc(jo::view<void*>(jo::self<jo::ModelRes>(this)->entries), jo::self<jo::ModelRes>(this)->capacity * kEntry, kModelresCpp, line_base() + 0x22);
        }
        std::uint8_t* const e = jo::view<std::uint8_t*>(jo::self<jo::ModelRes>(this)->entries) + jo::self<jo::ModelRes>(this)->count * kEntry - kEntry;
        memset_(e, 0, sizeof(jo::ModelEntry));
        if (dir != nullptr) {
            strcpy_(path, dir);
            strcat_(path, name);
            jo::as_model_entry(e)->path = joan::orig::arg::as<char*>(joan::orig::as_string_pool(&jo::self<jo::ModelRes>(this)->strings)->add(joan::orig::arg::str(static_cast<const char*>(path)), joan::orig::arg::i32(-1)));  // 0x005a18d0
            jo::as_model_entry(e)->file = jo::as_model_entry(e)->path + strlen_(dir);
            if (jo::self<jo::ModelRes>(this)->count >= 2 &&
                stricmp_((jo::as_model_entry(jo::view<std::uint8_t*>(jo::self<jo::ModelRes>(this)->entries) + (jo::self<jo::ModelRes>(this)->count - 2) * kEntry))->dir, dir) == 0) {
                jo::as_model_entry(e)->dir = (jo::as_model_entry(jo::view<std::uint8_t*>(jo::self<jo::ModelRes>(this)->entries) + (jo::self<jo::ModelRes>(this)->count - 2) * kEntry))->dir;
            } else {
                jo::as_model_entry(e)->dir = joan::orig::arg::as<char*>(joan::orig::as_string_pool(&jo::self<jo::ModelRes>(this)->strings)->add(joan::orig::arg::str(dir), joan::orig::arg::i32(-1)));
            }
        } else {
            jo::as_model_entry(e)->path = joan::orig::arg::as<char*>(joan::orig::as_string_pool(&jo::self<jo::ModelRes>(this)->strings)->add(joan::orig::arg::str(static_cast<const char*>(name)), joan::orig::arg::i32(-1)));
            jo::as_model_entry(e)->file = jo::as_model_entry(e)->path;
            jo::view<const char*>(jo::as_model_entry(e)->dir) = str(d::kEmpty);
        }
        jo::as_model_entry(e)->id = static_cast<std::uint16_t>(jo::view<std::uint32_t>(jo::self<jo::ModelRes>(this)->count));
        jo::as_model_entry(e)->type = type;
        jo::as_model_entry(e)->anim = static_cast<std::uint16_t>(flags);
        for (std::int32_t k = global<std::int16_t>(d::g_a47c05); k > 0; --k) {
            std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_db(obj(d::g_texture_effect_res))->entry(joan::orig::arg::u32(static_cast<std::int16_t>(k))));  // 0x004b27b0
            if (stricmp_(jo::as_model_entry(e)->file, jo::as_tex_script_rec(rec)->name) == 0) {
                jo::as_model_entry(e)->anim_db_id = jo::as_tex_script_rec(rec)->id;
                break;
            }
        }
        morph.table = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_MORPH)));  // 0x0058b180 "MORPH"
        jo::as_model_entry(e)->morph_count = 0;
        for (std::int32_t i = 0; i < joan::orig::arg::as<std::int32_t>(joan::orig::as_database(morph.table)->rec_count()); ++i) {  // 0x00415280
            const std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_database(morph.table)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(reinterpret_cast<char*>(&morph)), joan::orig::arg::str(rec), 0x20);           // 0x00590210
            if (stricmp_(reinterpret_cast<const char*>(&morph), jo::as_model_entry(e)->file) != 0) continue;
            jo::as_model_entry(e)->morph_count += 1;
            jo::view<std::uint8_t*>(jo::as_model_entry(e)->morphs) = static_cast<std::uint8_t*>(
                mem_realloc(jo::as_model_entry(e)->morphs, jo::as_model_entry(e)->morph_count * kMorph, kModelresCpp, line_base() + 0x5d));
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(morph_rec(e, jo::as_model_entry(e)->morph_count - 1)->name), joan::orig::arg::str(rec + 0x20), 0x20);
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(morph_rec(e, jo::as_model_entry(e)->morph_count - 1)->file), joan::orig::arg::str(rec + 0x40), 0x20);
            joan::orig::as_misc(kFiles)->copy_field_590210(joan::orig::arg::str(morph_rec(e, jo::as_model_entry(e)->morph_count - 1)->anim), joan::orig::arg::str(rec + 0x60), 0x20);
            morph_rec(e, jo::as_model_entry(e)->morph_count - 1)->morph = nullptr;
        }
        joan::orig::as_database(morph.table)->deinit();  // 0x00580da0
        return jo::view<std::int16_t>(jo::as_model_entry(e)->id);
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("ModelRes::add", 0x004b18d0, &H::model_add);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
//
//    - Obj::init_4b09c0 0x004b09c0 (ret 0): 0x004b0a10, 0x005a17a0(+0xd), 0x004b0cc0, byte +0xc =
//      1; returns this.
namespace {
namespace game_b108_02 {

using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::orig::helpers::at;

struct H {
    // 0x004b09c0
    H* init_4b09c0() {
        joan::orig::as_model_res(this)->free_all();           // 0x004b0a10
        joan::orig::as_string_pool(&jo::self<jo::ModelRes>(this)->strings)->reset();  // 0x005a17a0
        joan::orig::as_model_res(this)->load();           // 0x004b0cc0
        jo::self<jo::ModelRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Obj::init_4b09c0", 0x004b09c0, &H::init_4b09c0);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
//
//    - Table::find 0x004b20e0 (name, sub, kind, number) ret 0x10: 1-based index or 0.
namespace {
namespace game_b86_06 {

using joan::call::cdecl_call;
using joan::orig::helpers::at;

using joan::orig::helpers::stricmp_;

JOAN_COUNTER(c_find, "Table::find");

struct H {
    // ---- 0x004b20e0 Table::find(name, sub, kind, number) ret 0x10 --------------------------
    std::int32_t find(const char* name, const char* sub, std::int8_t kind, std::int32_t number) {
        c_find.hit();
        for (std::int32_t i = 0; i < jo::self<jo::ModelRes>(this)->count; ++i) {
            std::uint8_t* const e = jo::view<std::uint8_t*>(jo::self<jo::ModelRes>(this)->entries) + i * kEntry;
            if (static_cast<std::int32_t>(jo::view<std::int8_t>(jo::as_model_entry(e)->type)) != static_cast<std::int32_t>(kind)) continue;
            if (static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_model_entry(e)->anim)) != number) continue;
            if (stricmp_(jo::as_model_entry(e)->file, sub) != 0) continue;
            if (stricmp_(jo::as_model_entry(e)->dir, name) != 0) continue;
            return i + 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Table::find", 0x004b20e0, &H::find);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

using joan::orig::helpers::at;

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* anim_db_entry(std::uint32_t id) {  // 0x004b27b0
        return jo::view<std::uint8_t*>(jo::self<jo::AnimDb>(this)->records) + static_cast<std::int16_t>(id) * 0xd51 - 0xd51;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("AnimDb::entry", 0x004b27b0, &H::anim_db_entry);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/game_b100_04.cpp (game/b100_04)
//
//    - ModelRes::load 0x004b0cc0 (ret 0): "MODEL" section -> 0x2b-byte records.
namespace {
namespace game_b100_04 {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

void* const kHeap = obj(d::g_mem);
void* const kTokens = obj(d::g_misc);

using joan::orig::helpers::crt_memset;

JOAN_COUNTER(c_model_load, "ModelRes::load");

struct H {
    // ------------------------------------------------------------------ 0x004b0cc0
    i32 model_load() {
        c_model_load.hit();
        void* const section = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_MODEL_9d52a0)));  // 0x0058b180 "MODEL"
        jo::self<jo::ModelRes>(this)->count = joan::orig::arg::as<i32>(joan::orig::as_database(section)->rec_count());                                       // 0x00415280
        jo::self<jo::ModelRes>(this)->capacity = jo::self<jo::ModelRes>(this)->count;
        jo::view<u8*>(jo::self<jo::ModelRes>(this)->entries) = joan::orig::arg::as<u8*>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(jo::self<jo::ModelRes>(this)->capacity * kEntry), str(d::s_d_ww_modelres_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a01734)) + 0xa)));  // 0x0058e7b0
        crt_memset(jo::view<void*>(jo::self<jo::ModelRes>(this)->entries), 0, static_cast<u32>(jo::self<jo::ModelRes>(this)->count * kEntry));
        i32 i = 0;
        for (; i < jo::self<jo::ModelRes>(this)->count; ++i) {
            char buf_name[0x104];
            char buf_file[0x44];
            u8* const row = joan::orig::arg::as<u8*>(joan::orig::as_database(section)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            u8* const rec = jo::view<u8*>(jo::self<jo::ModelRes>(this)->entries) + i * kEntry;
            jo::as_model_entry(rec)->id = static_cast<u16>(i + 1);
            joan::orig::as_misc(kTokens)->copy_field_590210(joan::orig::arg::str(buf_name), joan::orig::arg::str(row), 0x20);                 // 0x00590210
            jo::as_model_entry(rec)->path = joan::orig::arg::as<char*>(joan::orig::as_string_pool(&jo::self<jo::ModelRes>(this)->strings)->add(joan::orig::arg::str(buf_name), joan::orig::arg::i32(-1)));  // 0x005a18d0
            jo::as_model_entry(rec)->dir = reinterpret_cast<char*>(img::addr(d::kEmpty));
            joan::orig::as_misc(kTokens)->copy_field_590210(joan::orig::arg::str(buf_file), joan::orig::arg::str(row + 0x20), 0x20);
            jo::as_model_entry(rec)->file = joan::orig::arg::as<char*>(joan::orig::as_string_pool(&jo::self<jo::ModelRes>(this)->strings)->add(joan::orig::arg::str(buf_file), joan::orig::arg::i32(-1)));
            jo::view<u8>(jo::as_model_entry(rec)->type) = row[0x40];
            if (static_cast<i32>(jo::view<i8>(jo::as_model_entry(rec)->type)) == 0x41) {
                char* const name = joan::orig::arg::as<char*>(joan::orig::as_misc(kTokens)->rtrim_field_590170(joan::orig::arg::str(row + 0x41), 0x20));  // 0x00590170
                jo::as_model_entry(rec)->anim = joan::orig::arg::as<u16>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->index_of(joan::orig::arg::str(name)));    // 0x00418430
            } else {
                jo::as_model_entry(rec)->anim = 0;
            }
            jo::view<u32>(jo::as_model_entry(rec)->anim_db_id) = 0;
        }
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_04", name, addr, fn)
B("ModelRes::load", 0x004b0cc0, &H::model_load);
#undef B

}  // namespace game_b100_04
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04 {

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::cdecl_call;
using joan::orig::helpers::at;

JOAN_COUNTER(c_name_find, "NameTable::find");

struct H {
    // ------------------------------------------------------------------ 0x004b1e30
    i32 name_find(const char* name) {
        c_name_find.hit();
        for (i32 i = 0; i < jo::self<jo::ModelRes>(this)->count; ++i) {
            const char* const s = jo::as_model_entry(jo::view<u8*>(jo::self<jo::ModelRes>(this)->entries) + i * kEntry)->path;
            if (joan::crt::stricmp(joan::orig::arg::str(s), joan::orig::arg::str(name)) == 0) return i + 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("NameTable::find", 0x004b1e30, &H::name_find);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {

using i32 = std::int32_t;
using u8 = std::uint8_t;

using u16 = std::uint16_t;
using u32 = std::uint32_t;

using i16 = std::int16_t;

using joan::call::thiscall;
using joan::img::str;

JOAN_COUNTER(c_set_key, "Anim::set_key_4b21a0");

struct H {
    // ------------------------------------------------------------------ 0x004b21a0
    static void delta(const float& s, const float& b, float& d, float dur) {
        d = (s - b) / dur;
    }
    void set_key_4b21a0(i32 key, u32 duration_bits) {
        c_set_key.hit();
        float dur;
        std::memcpy(&dur, &duration_bits, sizeof dur);
        jo::LightSys* const ls = jo::self<jo::LightSys>(this);
        std::memset(static_cast<void*>(&ls->state_a), 0, sizeof(ls->state_a));  // memset
        joan::orig::as_light_params(&ls->state_a.preset.params)->construct();  // 0x00617ee0
        jo::view<u32>(ls->state_a.fade) = duration_bits;
        ls->state_a.scale = 1.0f - ls->state_b.scale;
        const jo::LightPreset* const src = reinterpret_cast<jo::LightPreset*>(jo::view<u8*>(ls->sets) + static_cast<i32>((static_cast<u32>(key) - 1u) * static_cast<u32>(sizeof(jo::LightPreset))));
        jo::view<u16>(ls->state_a.preset.light_count) = jo::view<u16>(src->light_count);
        std::strncpy(ls->state_a.preset.name, joan::orig::arg::str(static_cast<const void*>(src->name)), 0x40u);  // memcpy
        for (i32 i = 0; i < static_cast<i32>(ls->state_a.preset.light_count); ++i) {
            const jo::LightDef& s = src->lights[i];
            const jo::LightDef& b = ls->current.lights[i];
            jo::LightDef& d = ls->state_a.preset.lights[i];
            delta(s.colour[0], b.colour[0], d.colour[0], dur);
            delta(s.colour[1], b.colour[1], d.colour[1], dur);
            delta(s.colour[2], b.colour[2], d.colour[2], dur);
            delta(s.colour[3], b.colour[3], d.colour[3], dur);
            delta(s.angle_a, b.angle_a, d.angle_a, dur);
            delta(s.angle_b, b.angle_b, d.angle_b, dur);
            d.enabled = s.enabled;
        }
        const jo::LightParams& sp = src->params;
        const jo::LightParams& bp = ls->current.params;
        jo::LightParams& dp = ls->state_a.preset.params;
        for (i32 k = 0; k < 5; ++k) delta(sp.scale_a[k], bp.scale_a[k], dp.scale_a[k], dur);
        for (i32 k = 0; k < 3; ++k) delta(sp.scale_b[k], bp.scale_b[k], dp.scale_b[k], dur);
        for (i32 k = 0; k < 3; ++k) delta(sp.sun_dir[k], bp.sun_dir[k], dp.sun_dir[k], dur);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Anim::set_key_4b21a0", 0x004b21a0, &H::set_key_4b21a0);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

using joan::orig::helpers::at;

struct H {
    std::uint8_t* elem_4b2730(std::uint32_t i) {  // 0x004b2730
        return jo::self<jo::AnimRes>(this)->entries + (i * sizeof(jo::AnimSet) - sizeof(jo::AnimSet));  // 0x3a-byte records on x86
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("elem_004b2730", 0x004b2730, &H::elem_4b2730);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

using joan::call::cdecl_call;
using joan::orig::helpers::at;

using u16 = std::uint16_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_clear_4b28a0, "Obj::clear_4b28a0");

struct H {
    // ------------------------------------------------------------------ 0x004b28a0
    H* clear_4b28a0() {
        c_clear_4b28a0.hit();
        if (jo::self<jo::MsgEntry>(this)->text != nullptr) {
            joan::crt::operator_delete(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::MsgEntry>(this)->text)));  // operator delete
            jo::self<jo::MsgEntry>(this)->text = nullptr;
        }
        if (jo::self<jo::MsgEntry>(this)->extra != nullptr) {
            joan::crt::operator_delete(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::MsgEntry>(this)->extra)));
            jo::self<jo::MsgEntry>(this)->extra = nullptr;
        }
        jo::view<u16>(jo::self<jo::MsgEntry>(this)->text_len) = 0;
        jo::view<u16>(jo::self<jo::MsgEntry>(this)->extra_len) = 0;
        jo::view<u32>(jo::self<jo::MsgEntry>(this)->value) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("Obj::clear_4b28a0", 0x004b28a0, &H::clear_4b28a0);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ModelRes::free_all() -> void { joan::bind::member<&::game_b107_13::H::free_all>(this); }  // 0x4b0a10
auto joan::orig::ActorDef::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_units4::H::sdd_4b0c20>(this, a0)); }  // 0x4b0c20
auto joan::orig::Morph::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_units4::H::sdd_4b0c70>(this, a0)); }  // 0x4b0c70
auto joan::orig::ModelRes::load() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_04::H::model_load>(this)); }  // 0x4b0cc0
auto joan::orig::ModelResMgr::set_params(std::int16_t a0, std::int8_t a1, std::int8_t a2, std::int16_t a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::set_params>(this, a0, a1, a2, a3)); }  // 0x4b0ec0
auto joan::orig::ModelRes::acquire(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_16::H::model_acquire>(this, a0)); }  // 0x4b0f40
auto joan::orig::ModelResMgr::ready(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::model_ready>(this, a0)); }  // 0x4b1640
auto joan::orig::ModelResMgr::release(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::model_release>(this, a0)); }  // 0x4b16a0
auto joan::orig::ModelRes::add(const char* a0, const char* a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_06::H::model_add>(this, a0, a1, a2)); }  // 0x4b18d0
auto joan::orig::Table::find_4b20e0(const char* a0, const char* a1, std::int8_t a2, std::int32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b86_06::H::find>(this, a0, a1, a2, a3)); }  // 0x4b20e0
auto joan::orig::Anim::set_key_4b21a0(std::int32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b104_03::H::set_key_4b21a0>(this, a0, a1); }  // 0x4b21a0
auto joan::orig::AnimResMgr::entry(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18::H::elem_4b2730>(this, a0)); }  // 0x4b2730
auto joan::orig::AnimDb::entry(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::anim_db_entry>(this, a0)); }  // 0x4b27b0
