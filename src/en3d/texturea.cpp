// en3d/texturea.cpp: functions of the original d:\ww\en3d\texturea.cpp (re/attribution.csv).
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
#include <cmath>
#include <limits>
#include <cstdarg>
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

// An index / count that a generated facade returns through `void*` (TextureManager::find, remove_a / remove_b): a small
// integer, never an address, so the narrowing is intended (the facades return std::int32_t since Phase 2 engine-1).
inline std::int32_t as_index(std::int32_t v) { return v; }  // TextureManager::find / remove_a / remove_b return the index (Phase 2 engine-1)
inline std::int32_t as_index(const void* v) { return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(v)); }  // an index that travelled as a pointer

using joan::call::cdecl_call;

using joan::orig::helpers::game_new;   // operator new

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::orig::helpers::op_new;

}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
//
//      0x00609260 TextureArray::clear()                         thiscall
namespace {
namespace game_b92_05 {

void* const kMem = obj(d::g_mem);

// Memory manager free / realloc with source file and line (0x0058ed70 / 0x0058eb30).
void mem_free(void* p, img::Addr file, std::int32_t line) {
    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}
using joan::orig::helpers::line_base;

constexpr img::Addr kTextureACpp = d::s_D_ww_en3d_texturea_cpp;  // 'D:\ww\en3d\texturea.cpp'

JOAN_COUNTER(c_texarray_clear, "TextureArray::clear");

struct H {
    // 0x00609260
    void texarray_clear() {
        c_texarray_clear.hit();
        joan::orig::as_texture_manager(this)->purge(0);  // 0x0060a870
        jo::view<std::int32_t>(jo::self<jo::TextureManager>(this)->requeue) = 0;
        for (std::int32_t i = jo::self<jo::TextureManager>(this)->loading_count; i > 0; --i)
            joan::orig::as_texture_manager(this)->release(joan::orig::arg::mptr(jo::self<jo::TextureManager>(this)->loading[i - 1].p));  // 0x0060a550
        if (jo::self<jo::TextureManager>(this)->loading != nullptr) {
            mem_free(jo::self<jo::TextureManager>(this)->loading, kTextureACpp, line_base(d::g_a18a2c) + 0xe);
            jo::self<jo::TextureManager>(this)->loading = nullptr;
        }
        jo::self<jo::TextureManager>(this)->loading_count = 0;
        jo::self<jo::TextureManager>(this)->loading_cap = 0;
        for (std::int32_t i = jo::self<jo::TextureManager>(this)->anon_count; i > 0; --i)
            joan::orig::as_texture_manager(this)->release(joan::orig::arg::mptr(jo::self<jo::TextureManager>(this)->anon[i - 1].p));
        if (jo::self<jo::TextureManager>(this)->anon != nullptr) {
            mem_free(jo::self<jo::TextureManager>(this)->anon, kTextureACpp, line_base(d::g_a18a2c) + 0x1e);
            jo::self<jo::TextureManager>(this)->anon = nullptr;
        }
        jo::self<jo::TextureManager>(this)->anon_count = 0;
        jo::self<jo::TextureManager>(this)->anon_cap = 0;
        for (std::int32_t i = jo::self<jo::TextureManager>(this)->named_count; i > 0; --i)
            joan::orig::as_texture_manager(this)->release(joan::orig::arg::mptr(jo::self<jo::TextureManager>(this)->named[i - 1].p));
        if (jo::self<jo::TextureManager>(this)->named != nullptr) {
            mem_free(jo::self<jo::TextureManager>(this)->named, kTextureACpp, line_base(d::g_a18a2c) + 0x2e);
            jo::self<jo::TextureManager>(this)->named = nullptr;
        }
        jo::self<jo::TextureManager>(this)->named_count = 0;
        jo::self<jo::TextureManager>(this)->named_cap = 0;
        if (jo::self<jo::TextureManager>(this)->released != nullptr) {
            mem_free(jo::self<jo::TextureManager>(this)->released, kTextureACpp, line_base(d::g_a18a2c) + 0x3a);
            jo::self<jo::TextureManager>(this)->released = nullptr;
        }
        jo::self<jo::TextureManager>(this)->released_count = 0;
        jo::self<jo::TextureManager>(this)->released_cap = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("TextureArray::deinit", 0x00609260, &H::texarray_clear);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
//
//      0x006094c0 TextureManager::create_2d()    - new 0x5c (0x00605ce0), +0x14 = this,
namespace {
namespace en3d_b82_17 {

struct H {
    // 0x006094c0
    void* create_2d() {
        void* mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Texture2D)));  // 0x5c on x86
        void* t = mem != nullptr ? joan::orig::as_texture2d(mem)->construct() : nullptr;  // 0x00605ce0
        jo::view<void*>(jo::as_texture(t)->manager) = this;
        joan::orig::as_texture_manager(this)->add_sorted(reinterpret_cast<std::uintptr_t>(t));  // 0x0060ad40 (the texture pointer, whole)
        return t;
    }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("TextureArray::create_Texture2DImage", 0x006094c0, &H::create_2d);
#undef B82_17E

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
//
//    - TextureList::add_new 0x00609570 (ret 0, SEH frame: normal path only): new 0x5c
//      (0x606980), +0x14 = this, 0x60ad40 -> the object.
//
//    - TextureList::find 0x0060b050 (key), ret 4: 0x5911a0 over +0x2c[+0x24] (stride 4).
//
//  - 0x00601140 / 0x00609570: SEH frames only destroy the local file / free the new block on
//    an exception; normal path reproduced.
namespace {
namespace game_b100_11 {

void* const kSearch = obj(d::g_misc);  // binary search / insert helpers (0x5911a0 / 0x5910f0)

i32 bsearch_(const void* key, void* base, i32 count, i32 stride, const void* cmp, i32* out) {
    return joan::orig::arg::as<i32>(joan::orig::as_misc(kSearch)->bin_search_5911a0(joan::orig::arg::mptr(key), joan::orig::arg::str(base), joan::orig::arg::i32(count), joan::orig::arg::i32(stride), joan::orig::arg::as<std::uintptr_t>(cmp), joan::orig::arg::mptr(out)));  // 0x005911a0
}

struct H {
    // ------------------------------------------------------------------ 0x00609570
    u8* texlist_add_new() {
        void* const p = op_new(static_cast<u32>(sizeof(jo::TextureCube)));  // 0x5c on x86
        u8* const t = p != nullptr ? joan::orig::arg::as<u8*>(joan::orig::as_texture_cube(p)->construct()) : nullptr;  // 0x00606980
        jo::view<void*>(jo::as_texture(t)->manager) = this;
        joan::orig::as_texture_manager(this)->add_sorted(reinterpret_cast<std::uintptr_t>(t));  // 0x0060ad40 (the texture pointer, whole)
        return t;
    }

    // ------------------------------------------------------------------ 0x0060b050
    i32 texlist_find(void* key) {
        return bsearch_(key, jo::self<jo::TextureManager>(this)->loading, jo::self<jo::TextureManager>(this)->loading_count, static_cast<std::int32_t>(sizeof(jo::TexturePtr)), fn::code_ptr(fn::cmp_diff_60b0a0), static_cast<i32*>(nullptr));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("TextureList::add_new", 0x00609570, &H::texlist_add_new);
B("TextureArray::search_loading_array", 0x0060b050, &H::texlist_find);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Obj::new_child_609620 0x00609620 (ret 0): new 0x5c, 0x606ef0, +0x14 = this, 0x60ad40.
//
//  - SEH frames (0x5e6000, 0x5f0040, 0x6035c0, 0x605d80, 0x609620) only matter on exceptions;
//    normal paths reproduced.
namespace {
namespace game_b99_07 {

struct H {
    // ------------------------------------------------------------------ 0x00609620
    void* new_child_609620() {
        void* const p = op_new(static_cast<u32>(sizeof(jo::TextureBump)));  // 0x5c on x86
        void* const o = p != nullptr ? joan::orig::as_texture_bump(p)->construct() : nullptr;  // 0x00606ef0
        jo::view<void*>(jo::as_texture(o)->manager) = this;
        joan::orig::as_texture_manager(this)->add_sorted(reinterpret_cast<std::uintptr_t>(o));  // 0x0060ad40 (the texture pointer, whole)
        return o;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::new_child_609620", 0x00609620, &H::new_child_609620);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/en3d_b84_01.cpp (en3d/b84_01)
//
//      0x006096d0 TextureManager::new_render_target() - new 0x74 (0x00607f10), +0x14 = this, 0x0060ad40
namespace {
namespace en3d_b84_01 {

struct H {
    // 0x006096d0
    void* new_render_target() {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::RenderTexture)));  // 0x74 on x86
        void* const rt = mem != nullptr ? joan::orig::as_render_texture(mem)->construct() : nullptr;  // 0x00607f10
        jo::view<void*>(jo::as_texture(rt)->manager) = this;
        joan::orig::as_texture_manager(this)->add_sorted(reinterpret_cast<std::uintptr_t>(rt));  // 0x0060ad40 (the texture pointer, whole)
        return rt;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("TextureManager::new_render_target", 0x006096d0, &H::new_render_target);
#undef E3D

}  // namespace en3d_b84_01
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
//
//    - find_file_609780 0x00609780 (ret 0xc, only called by itself: dead): extension table
//      0xa18948 tried with 0x00590b80, then recursion into subdirectories (_findfirst etc.).
//      Returns al.
namespace {
namespace game_b108_02 {

JOAN_COUNTER(c_find_file, "find_file_609780");

// _finddata_t as laid out by the MSVC 6 CRT (0x118 bytes, attrib +0, name +0x14).
struct FindData {
    u32 attrib;
    u8 times_size[0x10];
    char name[0x104];
};
static_assert(sizeof(FindData) == 0x118, "_finddata_t");
// 0x00609780 frame below the locals (buffers as in the original, lowest address first).
struct FindFrame {
    char sub[0x100];   // [ebp-0x328]
    i32 handle;        // [ebp-0x228]
    char spec[0x100];  // [ebp-0x224]
    FindData fd;       // [ebp-0x124]
};
static_assert(sizeof(FindFrame) == 0x31c, "frame");

struct H {
    // 0x00609780 (only called by itself)
    u8 find_file_609780(const char* name, const char* dir, char* out) {
        c_find_file.hit();
        for (const char* const* ext = &img::global<const char*>(d::g_a18948); *ext != nullptr; ++ext) {
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(out)), joan::orig::arg::str(dir));  // strcpy
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(out)), str(d::s_bslash));  // strcat "\\"
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(out)), joan::orig::arg::str(name));
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(out)), str(d::s_dot_9d36e8));  // "."
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(out)), joan::orig::arg::str(*ext));
            if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(out)) != 0) return 1;  // 0x00590b80
        }
        FindFrame fr;
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(fr.spec)), joan::orig::arg::str(dir));
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(fr.spec)), str(d::s_bslash_star));  // "\\*"
        fr.handle = joan::orig::arg::as<i32>(joan::orig::libc::_findfirst(joan::orig::arg::str(fr.spec), &fr.fd));  // _findfirst
        if (fr.handle == -1) return 0;
        do {
            if ((fr.fd.attrib & 0x10) != 0 && std::strcmp(joan::orig::arg::str(fr.fd.name), str(d::s_dot_9d36e8)) != 0 &&
                std::strcmp(joan::orig::arg::str(fr.fd.name), str(d::s_dot_dot)) != 0) {
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(fr.sub)), joan::orig::arg::str(dir));
#if defined(__GNUC__) && !defined(__clang__)
// false positive: fr.sub is a separate buffer from `dir`/fr.fd.name; GCC cannot see that after inlining strcat
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrestrict"
#endif
                std::strcat(static_cast<char*>(joan::orig::arg::mptr(fr.sub)), str(d::s_bslash));
                std::strcat(static_cast<char*>(joan::orig::arg::mptr(fr.sub)), joan::orig::arg::str(fr.fd.name));
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
                if (joan::orig::arg::as<u8>(joan::orig::as_texture_manager(this)->find_file(joan::orig::arg::str(name), joan::orig::arg::str(static_cast<const char*>(fr.sub)), joan::orig::arg::str(out))) != 0) return 1;
            }
        } while (joan::orig::arg::as<i32>(joan::orig::libc::_findnext(joan::orig::arg::i32(fr.handle), &fr.fd)) == 0);  // _findnext
        joan::orig::libc::_findclose(joan::orig::arg::i32(fr.handle));                         // _findclose
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("find_file_609780", 0x00609780, &H::find_file_609780);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
//
//      0x0060a1c0 Texture2D::on_loaded / 0x0060a2b0 TextureCube::on_loaded / 0x0060a3a0
//                 TextureBump::on_loaded (cdecl(tex, data, size), file loader callbacks):
//                 D3DXCreate(Cube)TextureFromFileInMemory(Ex), attach, release, 0x0060a550.
//
//   - 0x0060a3a0: the du/dv texture pointer [ebp-0xc] stays uninitialised when 0x00607950's
//     D3DXCreateTexture fails without writing it; the original then AddRefs garbage in 0x00607110.
//     Initialised to nullptr here (crash either way; only on a D3DX failure).
namespace {
namespace game_b102_11 {

using joan::call::stdcall_call;
template <class R = i32, class... A>
R com_call(void* object, i32 index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

// IUnknown / IDirect3D(Cube)Texture8 vtable slots.
constexpr i32 kRelease = 2;

JOAN_COUNTER(c_on_loaded, "Texture::on_loaded");

// Shared body of the texture-load callbacks 0x0060a1c0 / 0x0060a2b0.
u32 on_loaded_common(void* tex, const void* data, i32 size, bool cube) {
    c_on_loaded.hit();
    if (size > 0 && joan::orig::arg::as<i32>(joan::orig::as_file_loader(jo::view<void*>(jo::as_texture(tex)->loader))->ok()) != 0) {  // 0x005b0a80
        void* d3d_tex = nullptr;  // [ebp-8], only read when the D3DX call succeeded
        i32 hr;
        void* const device = global<void*>(d::g_game_device);
        if (jo::as_texture(tex)->levels != 0) {
            if (cube)
                hr = joan::orig::arg::as<i32>(joan::orig::d3dx::create_cube_texture_from_file_in_memory_ex(joan::orig::arg::mptr(device), joan::orig::arg::mptr(data), joan::orig::arg::u32(size), 0xffffffffu, joan::orig::arg::u32(jo::as_texture(tex)->levels), 0u, 0u, 1u, 0xffffffffu, 0xffffffffu, 0u, joan::orig::arg::mptr(static_cast<void*>(nullptr)), joan::orig::arg::mptr(static_cast<void*>(nullptr)), &d3d_tex));  // D3DXCreateCubeTextureFromFileInMemoryEx
            else
                hr = stdcall_call<i32>(fn::fn_765258, device, data, size, 0xffffffffu, 0xffffffffu, jo::as_texture(tex)->levels, 0u,
                                       0u, 1u, 0xffffffffu, 0xffffffffu, 0u, static_cast<void*>(nullptr),
                                       static_cast<void*>(nullptr), &d3d_tex);  // D3DXCreateTextureFromFileInMemoryEx
        } else {
            if (cube)
                hr = joan::orig::arg::as<i32>(joan::orig::d3dx::create_cube_texture_from_file_in_memory(joan::orig::arg::mptr(device), joan::orig::arg::mptr(data), joan::orig::arg::u32(size), &d3d_tex));  // D3DXCreateCubeTextureFromFileInMemory
            else
                hr = joan::orig::arg::as<i32>(joan::orig::d3dx::create_texture_from_file_in_memory(joan::orig::arg::mptr(device), joan::orig::arg::mptr(data), joan::orig::arg::u32(size), &d3d_tex));  // D3DXCreateTextureFromFileInMemory
        }
        if (hr == 0) {
            if (cube)
                joan::orig::as_texture_cube(tex)->attach(joan::orig::arg::mptr(d3d_tex), 2u);  // 0x00606b90
            else
                joan::orig::as_texture2d(tex)->attach(joan::orig::arg::mptr(d3d_tex), 1u);  // 0x00605f00
            com_call(d3d_tex, kRelease);
        }
    }
    return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(jo::view<void*>(jo::as_texture(tex)->manager))->release(joan::orig::arg::mptr(tex)));  // 0x0060a550
}
u32 JOAN_CDECL texture2d_on_loaded(void* tex, const void* data, i32 size) { return on_loaded_common(tex, data, size, false); }
u32 JOAN_CDECL texture_cube_on_loaded(void* tex, const void* data, i32 size) { return on_loaded_common(tex, data, size, true); }

// 0x0060a3a0 (cdecl)
u32 JOAN_CDECL texture_bump_on_loaded(void* tex, const void* data, i32 size) {
    c_on_loaded.hit();
    if (size > 0 && joan::orig::arg::as<i32>(joan::orig::as_file_loader(jo::view<void*>(jo::as_texture(tex)->loader))->ok()) != 0) {  // 0x005b0a80
        void* height = nullptr;  // [ebp-8]
        void* du_dv = nullptr;   // [ebp-0xc] (uninitialised in the original, see header)
        i32 hr;
        void* const device = global<void*>(d::g_game_device);
        if (jo::as_texture(tex)->levels != 0)
            hr = stdcall_call<i32>(fn::fn_765258, device, data, size, 0xffffffffu, 0xffffffffu, jo::as_texture(tex)->levels, 0u, 0u,
                                   1u, 0xffffffffu, 0xffffffffu, 0u, static_cast<void*>(nullptr),
                                   static_cast<void*>(nullptr), &height);  // D3DXCreateTextureFromFileInMemoryEx
        else
            hr = joan::orig::arg::as<i32>(joan::orig::d3dx::create_texture_from_file_in_memory(joan::orig::arg::mptr(device), joan::orig::arg::mptr(data), joan::orig::arg::u32(size), &height));  // D3DXCreateTextureFromFileInMemory
        if (hr == 0) {
            joan::orig::as_texture_bump(tex)->make_du_dv(joan::orig::arg::mptr(height), &du_dv);  // 0x00607950
            joan::orig::as_texture_bump(tex)->attach(joan::orig::arg::mptr(du_dv), 2u);       // 0x00607110
            com_call(height, kRelease);
            com_call(du_dv, kRelease);
        }
    }
    return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(jo::view<void*>(jo::as_texture(tex)->manager))->release(joan::orig::arg::mptr(tex)));  // 0x0060a550
}

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Texture2D::on_loaded", 0x0060a1c0, &texture2d_on_loaded);
B11("TextureCube::on_loaded", 0x0060a2b0, &texture_cube_on_loaded);
B11("TextureBump::on_loaded", 0x0060a3a0, &texture_bump_on_loaded);
#undef B11

}  // namespace game_b102_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // --- texture manager (en3d/texturea.cpp) ------------------------------------------------------
    void* textures_find(const char* name) {  // 0x0060a4b0
        const std::int32_t i = as_index(joan::orig::as_texture_manager(self())->find(joan::orig::arg::mptr(name)));  // 0x0060ae70 index + 1
        if (i == 0) return nullptr;
        return jo::view<void**>(jo::as_texture_manager(self())->named)[i - 1];
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("TextureArray::get_texture", 0x0060a4b0, &H::textures_find);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

struct H {
    // 0x0060a500: texture manager - count a use of `texture` (+0xc) stamped with frame +0x38.
    std::uint32_t texture_use(void* texture) {
        if (texture == nullptr) return 0;
        jo::view<std::uint32_t>(jo::as_texture(texture)->ref_count) = jo::view<std::uint32_t>(jo::as_texture(texture)->ref_count) + 1;
        jo::view<std::uint32_t>(jo::as_texture(texture)->last_used) = jo::self<jo::TextureManager>(this)->stamp;
        return jo::view<std::uint32_t>(jo::as_texture(texture)->ref_count);
    }
    // 0x0060ae70: texture lookup by name in the hash table 0xa45fa8 (0x005911a0).
    std::int32_t texture_find(const void* name) {  // an index (the facade returns std::int32_t since Phase 2 engine-1)
        return joan::orig::as_misc(img::obj(d::g_misc))->bin_search_5911a0(joan::orig::arg::mptr(name), joan::orig::arg::str(jo::self<jo::TextureManager>(this)->named), jo::self<jo::TextureManager>(this)->named_count, static_cast<std::int32_t>(sizeof(jo::TexturePtr)), fn::code_value(fn::cmp_name_18_60ad10), joan::orig::arg::mptr(0));
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("TextureArray::add_ref_texture", 0x0060a500, &H::texture_use);
HELPER6("TextureArray::search_texture_by_name", 0x0060ae70, &H::texture_find);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (en3d/helpers33)
//
//    - TexturePaths::dir_index 0x006054c0, TextureManager::set_name 0x0060a670,
//      Game::flush_loader 0x00603af0.
namespace {
namespace game_leaves81 {

JOAN_COUNTER(c_tex_set_name, "TextureManager::set_name");

struct H {
    // 0x0060a670 TextureManager::set_name(texture, name)
    std::uint32_t tex_set_name(void* texture, const char* name) {
        c_tex_set_name.hit();
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::as_texture(texture)->name[0])) != 0) {
            joan::orig::as_texture_manager(this)->remove_a(joan::orig::arg::mptr(texture));  // 0x0060b0d0
        } else {
            if (name == nullptr || static_cast<std::int8_t>(*name) == 0)
                return joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(this)->find_sorted(joan::orig::arg::as<std::uintptr_t>(texture)));  // 0x0060b000 (x64: the pointer whole)
            joan::orig::as_texture_manager(this)->remove_b(joan::orig::arg::mptr(texture));  // 0x0060b140
        }
        if (name != nullptr && static_cast<std::int8_t>(*name) != 0)
            return joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(this)->insert_named(joan::orig::arg::as<std::uintptr_t>(texture), joan::orig::arg::as<std::uintptr_t>(name)));  // 0x0060ac20
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(this)->add_sorted(joan::orig::arg::as<std::uintptr_t>(texture)));  // 0x0060ad40
    }
};

#define T33(name, addr, fn) JOAN_REPLACE("en3d/helpers33", name, addr, fn)
T33("TextureArray::texture_set_name", 0x0060a670, &H::tex_set_name);
#undef T33

}  // namespace game_leaves81
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

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Textures::mode", 0x0060a720, &joan::orig::TextureManager::mode);
L("Textures::set_mode", 0x0060a740, &joan::orig::TextureManager::set_mode);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
//
//    - TextureLoader::flush 0x0060a770: waits (SleepEx(n * 10, TRUE)) while items busy.
namespace {
namespace game_b97_06 {

// KERNEL32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

constexpr img::Addr kSleepEx = d::iat_KERNEL32_SleepEx;

JOAN_COUNTER(c_flush, "TextureLoader::flush");

struct H {
    // 0x0060a770
    void flush() {
        c_flush.hit();
        std::int32_t n = 0;
        do {
            n = 0;
            for (std::int32_t i = 0; i < jo::self<jo::TextureManager>(this)->named_count; ++i)
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_texture(jo::view<void**>(jo::self<jo::TextureManager>(this)->named)[i])->is_loading()) != 0) ++n;  // 0x00605be0
            for (std::int32_t i = 0; i < jo::self<jo::TextureManager>(this)->anon_count; ++i)
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_texture(jo::view<void**>(jo::self<jo::TextureManager>(this)->anon)[i])->is_loading()) != 0) ++n;  // 0x00605be0
            if (n > 0) win_call<std::uint32_t>(kSleepEx, static_cast<std::uint32_t>(n * 10), 1);
            joan::orig::as_texture_manager(this)->trim_loaders(1);  // 0x0060a990
        } while (n > 0);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("TextureLoader::flush", 0x0060a770, &H::flush);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_leaves82a.cpp (en3d/helpers38)
//
//      0x0060a870 TextureManager::purge(age)           - drops textures older than age (0 = all)
//
//      0x0060a990 TextureManager::trim_loaders(all)    - finishes / frees background loaders
//
//      0x0060ac20 SortedArray::insert(value, key)      - sorted insert (+0 count, +4 cap, +8 data)
//
//      0x0060ad40 SortedArray::add(value)              - insert unless present (+0xc/+0x10/+0x14)
//
//      0x0060b000 SortedArray::find(value)             - binary search in +0xc/+0x14
namespace {
namespace game_leaves82a {

std::uint32_t timer() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(obj(d::g_misc))->get_time_591050()); }  // 0x00591050

JOAN_COUNTER(c_tm_purge, "TextureManager::purge");
JOAN_COUNTER(c_tm_trim, "TextureManager::trim_loaders");
JOAN_COUNTER(c_sa_insert, "SortedArray::insert");
JOAN_COUNTER(c_sa_add, "SortedArray::add");

struct H {
    // 0x0060a870 TextureManager::purge(age): +0x18 count, +0x20 textures (+0x10 age). With
    // age 0 every texture is released (0x0060a550), otherwise those older than age; the others
    // age by one and are compacted. +0x34 is cleared during the loop and restored.
    void tm_purge(std::int32_t age) {
        c_tm_purge.hit();
        const std::uint32_t saved = jo::self<jo::TextureManager>(this)->requeue;
        jo::self<jo::TextureManager>(this)->requeue = 0;
        std::int32_t kept = 0;
        for (std::int32_t i = 0; i < jo::self<jo::TextureManager>(this)->released_count; ++i) {
            if (age == 0 || jo::as_texture(jo::view<void**>(jo::self<jo::TextureManager>(this)->released)[i])->last_used > age) {
                joan::orig::as_texture_manager(this)->release(joan::orig::arg::mptr(jo::view<void**>(jo::self<jo::TextureManager>(this)->released)[i]));  // 0x0060a550
            } else {
                void* const texture = jo::view<void**>(jo::self<jo::TextureManager>(this)->released)[i];
                jo::as_texture(texture)->last_used = jo::as_texture(jo::view<void**>(jo::self<jo::TextureManager>(this)->released)[i])->last_used + 1;
                if (i != kept) jo::view<void**>(jo::self<jo::TextureManager>(this)->released)[kept] = jo::view<void**>(jo::self<jo::TextureManager>(this)->released)[i];
                ++kept;
            }
        }
        jo::self<jo::TextureManager>(this)->released_count = kept;
        jo::self<jo::TextureManager>(this)->requeue = saved;
    }

    // 0x0060a990 TextureManager::trim_loaders(all): +0x24 count, +0x2c pending textures (+0x50
    // loader, +0x54 priority countdown). Up to 2 (all: count + 10) finished loaders (0x00578250)
    // are completed through the loader's callback (+0x120 with +0x124 / +0x10c / +0x118), the
    // loader deleted (0x005afd50) and the texture dropped from the list; the others count down.
    void tm_trim(std::int32_t all) {
        c_tm_trim.hit();
        std::int32_t limit = 2;
        if (all != 0) limit = jo::self<jo::TextureManager>(this)->loading_count + 10;
        std::int32_t kept = 0, priority = 0, normal = 0;
        for (std::int32_t i = 0; i < jo::self<jo::TextureManager>(this)->loading_count; ++i) {
            if (jo::as_texture(jo::view<void**>(jo::self<jo::TextureManager>(this)->loading)[i])->priority != 0)
                ++priority;
            else
                ++normal;
        }
        if (priority >= limit) {
            priority = limit;
            normal = 0;
        } else if (priority + normal > limit) {
            normal = limit - priority;
        }
        for (std::int32_t i = 0; i < jo::self<jo::TextureManager>(this)->loading_count; ++i) {
            void* const e = jo::view<void**>(jo::self<jo::TextureManager>(this)->loading)[i];
            const std::int32_t budget = jo::as_texture(e)->priority != 0 ? priority : normal;
            if (budget > 0 && joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::view<void*>(jo::as_texture(e)->loader))->done()) != 0) {  // 0x00578250
                const std::uint32_t start = timer();
                void* const loader = jo::view<void*>(jo::as_texture(e)->loader);
                reinterpret_cast<void(JOAN_CDECL*)(void*, std::uint8_t*, std::uint32_t)>(jo::view<std::uintptr_t>(jo::as_file_loader(loader)->callback))(
                    jo::as_file_loader(loader)->context, jo::as_file_loader(loader)->buffer, jo::as_file_loader(loader)->bytes_read);
                const std::uint32_t elapsed = timer() - start;  // dead in the original
                (void)elapsed;
                void* const old = jo::view<void*>(jo::as_texture(e)->loader);
                if (old != nullptr) joan::orig::as_file_loader(old)->scalar_deleting_destructor(1u);  // 0x005afd50 delete
                jo::view<void*>(jo::as_texture(e)->loader) = nullptr;
                if (jo::as_texture(e)->priority != 0)
                    --priority;
                else
                    --normal;
            } else {
                if (jo::as_texture(e)->priority > 0) jo::as_texture(e)->priority = jo::as_texture(e)->priority - 1;
                if (i != kept) jo::view<void**>(jo::self<jo::TextureManager>(this)->loading)[kept] = jo::view<void**>(jo::self<jo::TextureManager>(this)->loading)[i];
                ++kept;
            }
        }
        jo::self<jo::TextureManager>(this)->loading_count = kept;
    }

    // 0x0060ac20 (value, key): +0 count, +4 capacity, +8 data, comparator 0x0060ad10; grows
    // by 64 (0x0058eb30, texturea.cpp line *0xa18a30 + 10) and inserts value at the position
    // found for key (0x005911a0 always writes it). Returns the position.
    std::int32_t sa_insert(std::uintptr_t value, std::uintptr_t key) {  // a Texture* and its name (x64: facade insert_named still u32)
        c_sa_insert.hit();
        std::int32_t position = 0;
        void* const sorter = obj(d::g_misc);
        joan::orig::as_misc(sorter)->bin_search_5911a0(joan::orig::arg::mptr(key), joan::orig::arg::str(jo::self<jo::TextureManager>(this)->named), joan::orig::arg::i32(jo::self<jo::TextureManager>(this)->named_count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), fn::code_value(fn::cmp_name_18_60ad10), &position);  // 0x005911a0
        if (jo::self<jo::TextureManager>(this)->named_count >= jo::self<jo::TextureManager>(this)->named_cap) {
            jo::self<jo::TextureManager>(this)->named_cap = jo::self<jo::TextureManager>(this)->named_count + 0x40;
            jo::self<jo::TextureManager>(this)->named = static_cast<jo::TexturePtr*>(joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::self<jo::TextureManager>(this)->named), joan::orig::arg::u32(jo::self<jo::TextureManager>(this)->named_cap * static_cast<std::int32_t>(sizeof(jo::TexturePtr))), joan::orig::arg::str(&img::global<const char>(d::s_D_ww_en3d_texturea_cpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a18a30)) + 0xa)));  // 0x0058eb30
        }
        joan::orig::as_misc(sorter)->insert_array_rec_5910f0(joan::orig::arg::str(jo::self<jo::TextureManager>(this)->named), joan::orig::arg::i32(jo::self<jo::TextureManager>(this)->named_count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), joan::orig::arg::i32(position), &value);  // 0x005910f0
        jo::self<jo::TextureManager>(this)->named_count = jo::self<jo::TextureManager>(this)->named_count + 1;
        return position;
    }

    // 0x0060ad40 (value): +0xc count, +0x10 capacity, +0x14 data, comparator 0x0060ae40.
    // Returns 0 when already present, else inserts and returns the position.
    std::int32_t sa_add(std::uintptr_t value) {  // a Texture* (x64: facade add_sorted still u32)
        c_sa_add.hit();
        std::int32_t position = 0;
        void* const sorter = obj(d::g_misc);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(sorter)->bin_search_5911a0(joan::orig::arg::mptr(value), joan::orig::arg::str(jo::self<jo::TextureManager>(this)->anon), joan::orig::arg::i32(jo::self<jo::TextureManager>(this)->anon_count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), fn::code_value(fn::cmp_diff_60ae40), &position)) != 0)  // 0x005911a0
            return 0;
        if (jo::self<jo::TextureManager>(this)->anon_count >= jo::self<jo::TextureManager>(this)->anon_cap) {
            jo::self<jo::TextureManager>(this)->anon_cap = jo::self<jo::TextureManager>(this)->anon_count + 0x40;
            jo::self<jo::TextureManager>(this)->anon = static_cast<jo::TexturePtr*>(joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::self<jo::TextureManager>(this)->anon), joan::orig::arg::u32(jo::self<jo::TextureManager>(this)->anon_cap * static_cast<std::int32_t>(sizeof(jo::TexturePtr))), joan::orig::arg::str(&img::global<const char>(d::s_D_ww_en3d_texturea_cpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a18a34)) + 0xc)));  // 0x0058eb30
        }
        joan::orig::as_misc(sorter)->insert_array_rec_5910f0(joan::orig::arg::str(jo::self<jo::TextureManager>(this)->anon), joan::orig::arg::i32(jo::self<jo::TextureManager>(this)->anon_count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), joan::orig::arg::i32(position), &value);  // 0x005910f0
        jo::self<jo::TextureManager>(this)->anon_count = jo::self<jo::TextureManager>(this)->anon_count + 1;
        return position;
    }

    // 0x0060b000 (value): 0x005911a0 result for value in +0x14 / +0xc (no position out).
    std::int32_t sa_find(std::uintptr_t value) {  // a Texture* (x64: facade find_sorted still u32)
        return joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(obj(d::g_misc))->bin_search_5911a0(joan::orig::arg::mptr(value), joan::orig::arg::str(jo::self<jo::TextureManager>(this)->anon), joan::orig::arg::i32(jo::self<jo::TextureManager>(this)->anon_count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), fn::code_value(fn::cmp_diff_60ae40), joan::orig::arg::mptr(static_cast<std::int32_t*>(nullptr))));  // 0x005911a0
    }
};

#define HELPER38(name, addr, fn) JOAN_REPLACE("en3d/helpers38", name, addr, fn)
HELPER38("TextureArray::purge_cached_texture", 0x0060a870, &H::tm_purge);
HELPER38("TextureArray::process_loading_texture", 0x0060a990, &H::tm_trim);
HELPER38("TextureArray::add_to_named_array", 0x0060ac20, &H::sa_insert);
HELPER38("TextureArray::add_to_unnamed_array", 0x0060ad40, &H::sa_add);
HELPER38("SortedArray::find", 0x0060b000, &H::sa_find);
#undef HELPER38

}  // namespace game_leaves82a
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
//
//    - cmp_name_18 0x0060ad10 (cdecl): stricmp(a, *b + 0x18).
//
//    - cmp_diff 0x0060ae40 / 0x0060b0a0 / 0x0060b2b0 (cdecl): a - *b.
namespace {
namespace game_b103_12 {

i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

// 0x0060ad10
static i32 JOAN_CDECL cmp_name_18(const char* a, char* const* b) { return crt_stricmp(a, jo::as_texture(*b)->name); }  // b: a Texture* of the sorted list
// 0x0060ae40 / 0x0060b0a0 / 0x0060b2b0
// a - *b on x86 (wraps, as the original); the elements are Texture* (8 bytes on x64, where the truncated
// difference would not be an order: compare instead).
static i32 JOAN_CDECL cmp_diff(std::uintptr_t a, const std::uintptr_t* b) {
    if constexpr (sizeof(std::uintptr_t) == 4) return static_cast<i32>(a - *b);
    else return a < *b ? -1 : (a > *b ? 1 : 0);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("cmp_name_18_60ad10", 0x0060ad10, &cmp_name_18);
B("cmp_diff_60ae40", 0x0060ae40, &cmp_diff);
B("cmp_diff_60b0a0", 0x0060b0a0, &cmp_diff);
B("cmp_diff_60b2b0", 0x0060b2b0, &cmp_diff);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
//
//    - TexList::find_near 0x0060aec0 (tex), ret 4: index from 0x60ae70(tex+0x18), then an
//      outward search for tex in the 1-based array +8 (count +0).
namespace {
namespace game_b98_06 {

JOAN_COUNTER(c_find_near, "TexList::find_near");

struct H {
    // ------------------------------------------------------------------ 0x0060aec0
    i32 find_near(const jo::Texture* tex) {
        c_find_near.hit();
        const i32 idx = as_index(joan::orig::as_texture_manager(this)->find(joan::orig::arg::mptr(tex->name)));  // 0x0060ae70
        if (idx == 0) return 0;
        if (jo::self<jo::TextureManager>(this)->named[idx - 1].p == tex) return idx;
        const i32 lim = (idx - 1 > jo::self<jo::TextureManager>(this)->named_count - idx) ? idx - 1 : jo::self<jo::TextureManager>(this)->named_count - idx;
        i32 lo = idx;
        i32 hi = idx;
        for (i32 j = 1; j < lim; ++j, --lo, ++hi) {
            if (lo > 0 && jo::self<jo::TextureManager>(this)->named[lo - 1].p == tex) return lo;
            if (hi <= jo::self<jo::TextureManager>(this)->named_count && jo::self<jo::TextureManager>(this)->named[hi - 1].p == tex) return hi;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("TexList::find_near", 0x0060aec0, &H::find_near);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

// Game memory manager (0xa48838) with file / line tracking.
constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kTextureaCpp(d::s_D_ww_en3d_texturea_cpp);  // "D:\ww\en3d\texturea.cpp"
std::int32_t line_base(img::Addr address) { return static_cast<std::int32_t>(global<std::int16_t>(address)); }
void* mem_realloc(void* p, std::uint32_t size, const char* file, std::int32_t line) {
    return joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(p), joan::orig::arg::u32(size), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058eb30
}
constexpr img::Addr kArrays(d::g_misc);  // sorted-array helpers

JOAN_COUNTER(c_tex_insert, "TextureList::insert");

struct H {

    // Sorted pointer lists of the texture manager: find, then remove (0x00591070).
    void* list_remove(std::uintptr_t find, std::int32_t& count, jo::TexturePtr* const& data, void* key) {
        void* const e = thiscall<void*>(find, this, key);
        if (e != nullptr) {
            joan::orig::as_misc(img::obj(kArrays))->del_array_rec_591070(joan::orig::arg::str(data), joan::orig::arg::i32(count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), as_index(e));
            count = count - 1;
        }
        return e;
    }
    std::int32_t list_remove_a(void* key) { return as_index(list_remove(fn::code_value(fn::TexList_find_near), jo::self<jo::TextureManager>(this)->named_count, jo::self<jo::TextureManager>(this)->named, key)); }    // 0x0060b0d0 (0x0060aec0)
    std::int32_t list_remove_b(void* key) { return as_index(list_remove(fn::code_value(fn::SortedArray_find), jo::self<jo::TextureManager>(this)->anon_count, jo::self<jo::TextureManager>(this)->anon, key)); }   // 0x0060b140 (0x0060b000)

    // 0x0060b1b0 TextureList::insert(key) -> slot, 0 when already present.
    std::int32_t texture_list_insert(void* key) {
        c_tex_insert.hit();
        std::int32_t pos;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(img::obj(kArrays))->bin_search_5911a0(joan::orig::arg::mptr(key), joan::orig::arg::str(jo::self<jo::TextureManager>(this)->released), joan::orig::arg::i32(jo::self<jo::TextureManager>(this)->released_count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), fn::code_value(fn::cmp_diff_60b2b0), &pos)) != 0)  // 0x005911a0 binary search
            return 0;
        if (jo::self<jo::TextureManager>(this)->released_count >= jo::self<jo::TextureManager>(this)->released_cap) {
            jo::self<jo::TextureManager>(this)->released_cap = jo::self<jo::TextureManager>(this)->released_count + 0x40;
            jo::self<jo::TextureManager>(this)->released = static_cast<jo::TexturePtr*>(mem_realloc(jo::self<jo::TextureManager>(this)->released, static_cast<std::uint32_t>(jo::self<jo::TextureManager>(this)->released_cap * static_cast<std::int32_t>(sizeof(jo::TexturePtr))), img::str(kTextureaCpp),
                                         line_base(d::g_a18a38) + 0xc));
        }
        joan::orig::as_misc(img::obj(kArrays))->insert_array_rec_5910f0(joan::orig::arg::str(jo::self<jo::TextureManager>(this)->released), joan::orig::arg::i32(jo::self<jo::TextureManager>(this)->released_count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), joan::orig::arg::i32(pos), &key);  // 0x005910f0
        jo::self<jo::TextureManager>(this)->released_count = jo::self<jo::TextureManager>(this)->released_count + 1;
        return pos;
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
#define HELPER17(name, addr, fn) JOAN_REPLACE("en3d/helpers17", name, addr, fn)
HELPER16("TextureArray::remove_from_unnamed_array", 0x0060b0d0, &H::list_remove_a);
HELPER16("TextureArray::search_unnamed_array", 0x0060b140, &H::list_remove_b);
HELPER17("TextureArray::add_to_cached_array", 0x0060b1b0, &H::texture_list_insert);
#undef HELPER16
#undef HELPER17

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/en3d_helpers29.cpp (en3d/helpers29)
//
//      0x0060b2e0 TextureManager::add(texture)          - sorted insert into the +0x2c / +0x24 / +0x28
//                 array (texturea.cpp).
namespace {
namespace en3d_helpers29 {

JOAN_COUNTER(c_tm_add, "TextureManager::add");

struct H {
    // 0x0060b2e0: +0x24 count, +0x28 capacity, +0x2c array of texture pointers, sorted with the
    // comparator 0x0060b0a0 (binary search 0x005911a0 / insert 0x005910f0 on 0xa45fa8). Returns the
    // insert position (0x005911a0 always writes it) when inserted, 0 when already present.
    void* tm_add(void* texture) {
        c_tm_add.hit();
        std::int32_t position = 0;
        void* const sorter = img::obj(d::g_misc);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(sorter)->bin_search_5911a0(joan::orig::arg::mptr(texture), joan::orig::arg::str(jo::self<jo::TextureManager>(this)->loading), joan::orig::arg::i32(jo::self<jo::TextureManager>(this)->loading_count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), fn::code_value(fn::cmp_diff_60b0a0), &position)) != 0)  // 0x005911a0
            return nullptr;
        if (jo::self<jo::TextureManager>(this)->loading_count >= jo::self<jo::TextureManager>(this)->loading_cap) {
            jo::self<jo::TextureManager>(this)->loading_cap = jo::self<jo::TextureManager>(this)->loading_count + 0x40;
            jo::self<jo::TextureManager>(this)->loading = static_cast<jo::TexturePtr*>(joan::orig::as_mem(img::obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::self<jo::TextureManager>(this)->loading), joan::orig::arg::u32(jo::self<jo::TextureManager>(this)->loading_cap * static_cast<std::int32_t>(sizeof(jo::TexturePtr))), joan::orig::arg::str(&img::global<const char>(d::s_D_ww_en3d_texturea_cpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a18a3c)) + 0xc)));  // 0x0058eb30 realloc
        }
        joan::orig::as_misc(sorter)->insert_array_rec_5910f0(joan::orig::arg::str(jo::self<jo::TextureManager>(this)->loading), joan::orig::arg::i32(jo::self<jo::TextureManager>(this)->loading_count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), joan::orig::arg::i32(position), &texture);  // 0x005910f0
        jo::self<jo::TextureManager>(this)->loading_count = jo::self<jo::TextureManager>(this)->loading_count + 1;
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(position));
    }
};

#define HELPER29(name, addr, fn) JOAN_REPLACE("en3d/helpers29", name, addr, fn)
HELPER29("TextureArray::add_to_loading_array", 0x0060b2e0, &H::tm_add);
#undef HELPER29

}  // namespace en3d_helpers29
}  // namespace

// ---------------------------------------- from modules/en3d_helpers29.cpp (en3d/helpers29)
//
//      0x00605f00 Texture2D::attach(d3d_texture, arg)   - stores / AddRefs the D3D texture (+0x58),
//
//      0x00606b90 TextureCube::attach(d3d_texture, arg)   reads level 0's size (GetLevelDesc, fatal
//
//      0x00607110 TextureBump::attach(d3d_texture, arg)   "Cannot read texture surface desc"), QIs
//                 the interface 0x9e1718 and hands it to 0x00605a40 (ptr, width, height, levels, arg).
//
//      0x00607950 TextureBump::make_du_dv(height_map, out) - D3DFMT_V8U8 du/dv map from the first
//                 byte of each 32-bit texel of a height map (wrapping neighbours).
//
//      0x006051c0 TexturePaths::resolve(name)           - name -> full path (0xa69b50) through the
//                 directory / file index (the object at 0xa59718).
namespace {
namespace en3d_helpers29_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::call::stdcall_call;
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

// IDirect3D(Cube)Texture8 vtable slots.
constexpr std::int32_t kQueryInterface = 0, kAddRef = 1, kRelease = 2, kGetLevelCount = 13, kGetLevelDesc = 14,
                       kLockRect = 16, kUnlockRect = 17;

struct SurfaceDesc {  // D3DSURFACE_DESC (D3D8), 32 bytes
    std::uint32_t format, type, usage, pool, size, multisample;
    std::uint32_t width, height;
};
static_assert(sizeof(SurfaceDesc) == 0x20);
struct LockedRect {  // D3DLOCKED_RECT
    std::int32_t pitch;
    std::uintptr_t bits;  // void* pBits (8 bytes at +0x8 on x64)
};

JOAN_COUNTER(c_attach2d, "Texture2D::attach");
JOAN_COUNTER(c_attach_cube, "TextureCube::attach");
JOAN_COUNTER(c_attach_bump, "TextureBump::attach");
JOAN_COUNTER(c_du_dv, "TextureBump::make_du_dv");
JOAN_COUNTER(c_resolve, "TexturePaths::resolve");

struct H {
    // 0x00605f00 / 0x00606b90 / 0x00607110 (identical bodies). If GetLevelDesc fails the original
    // calls the fatal error (0x00586cb0 -> exit(-2)); only when that is already running (re-entry
    // guard 0xa64528) would the uninitialised width / height reach 0x00605a40 - zeroed here.
    // A failed QueryInterface leaves the pointer uninitialised in the original (crash either way).
    void attach_common(void* d3d_texture, std::uint32_t arg) {
        jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture) = d3d_texture;
        com_call(jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture), kAddRef);
        SurfaceDesc desc{};
        std::int32_t hr = com_call(jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture), kGetLevelDesc, 0u, &desc);
        if (hr != 0)
            joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(d::g_err)), joan::orig::arg::str(&img::global<const char>(d::s_Cannot_read_texture_surface)));  // 0x00586cb0 "Cannot read texture surface desc"
        void* object = nullptr;
        hr = com_call(jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture), kQueryInterface, img::obj(d::blob_9e1718), &object);
        (void)hr;
        const std::uint32_t levels = com_call<std::uint32_t>(jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture), kGetLevelCount);
        joan::orig::as_texture(this)->attach(joan::orig::arg::mptr(object), joan::orig::arg::u32(desc.width), joan::orig::arg::u32(desc.height), joan::orig::arg::u32(levels), joan::orig::arg::u32(arg));  // 0x00605a40
        com_call(object, kRelease);
    }
    void attach_2d(void* d3d_texture, std::uint32_t arg) {
        c_attach2d.hit();
        attach_common(d3d_texture, arg);
    }
    void attach_cube(void* d3d_texture, std::uint32_t arg) {
        c_attach_cube.hit();
        attach_common(d3d_texture, arg);
    }
    void attach_bump(void* d3d_texture, std::uint32_t arg) {
        c_attach_bump.hit();
        attach_common(d3d_texture, arg);
    }

    // 0x00607950 (this unused). Desc / locked rect are single locals reused for every level and
    // both locks, as in the original (results of GetLevelDesc / LockRect are not checked; their
    // first-use values are zeroed here instead of stack garbage - only differs if level 0's
    // GetLevelDesc / the first LockRect fails).
    void make_du_dv(void* source, void** out) {
        c_du_dv.hit();
        if (source == nullptr) return;
        SurfaceDesc desc{};
        LockedRect rect{};
        com_call(source, kGetLevelDesc, 0u, &desc);
        const std::int32_t levels = com_call(source, kGetLevelCount);
        const std::int32_t hr = joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_texture(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::u32(desc.width), joan::orig::arg::u32(desc.height), joan::orig::arg::u32(levels), 0u, 0x3cu, 1u, joan::orig::arg::mptr(out)));  // D3DXCreateTexture V8U8 managed
        if (hr != 0 || *out == nullptr) return;
        for (std::int32_t level = 0; level < levels; ++level) {
            com_call(source, kGetLevelDesc, static_cast<std::uint32_t>(level), &desc);
            com_call(source, kLockRect, static_cast<std::uint32_t>(level), &rect, static_cast<void*>(nullptr), 0u);
            const std::uint32_t src_pitch = static_cast<std::uint32_t>(rect.pitch);
            const std::uintptr_t src_base = rect.bits;
            std::uintptr_t src_row = src_base;
            const std::uintptr_t src_last = src_base + src_pitch * (desc.height - 1);
            com_call(*out, kLockRect, static_cast<std::uint32_t>(level), &rect, static_cast<void*>(nullptr), 0u);
            const std::uint32_t dst_pitch = static_cast<std::uint32_t>(rect.pitch);
            std::uintptr_t dst_row = rect.bits;
            for (std::uint32_t y = 0; y < desc.height; ++y) {
                std::uintptr_t cur = src_row;
                std::uintptr_t next = y == desc.height - 1 ? src_base : src_row + src_pitch;
                std::uintptr_t prev = y == 0 ? src_last : src_row - src_pitch;
                std::uintptr_t dst = dst_row;
                for (std::uint32_t x = 0; x < desc.width; ++x) {
                    auto byte_at = [](std::uintptr_t address) { return static_cast<std::int32_t>(*reinterpret_cast<std::uint8_t*>(address)); };
                    const std::int32_t c = byte_at(cur);
                    const std::int32_t right = x == desc.width - 1 ? byte_at(src_row) : byte_at(cur + 4);
                    const std::int32_t left = x == 0 ? byte_at(src_row + desc.width * 4 - 4) : byte_at(cur - 4);
                    const std::int32_t down = byte_at(next);
                    const std::int32_t up = byte_at(prev);
                    const std::int32_t du = left - right;
                    const std::int32_t dv = up - down;
                    volatile std::int16_t unused = static_cast<std::int16_t>(c <= 1 ? 0x7f : 0x3f);  // dead store in the original
                    (void)unused;
                    *reinterpret_cast<std::uint8_t*>(dst) = static_cast<std::uint8_t>(du);
                    ++dst;
                    *reinterpret_cast<std::uint8_t*>(dst) = static_cast<std::uint8_t>(dv);
                    ++dst;
                    cur += 4;
                    next += 4;
                    prev += 4;
                }
                src_row += src_pitch;
                dst_row += dst_pitch;
            }
            com_call(*out, kUnlockRect, static_cast<std::uint32_t>(level));
            com_call(source, kUnlockRect, static_cast<std::uint32_t>(level));
        }
    }

    // 0x006051c0: splits name into directory / file name, strips one trailing '\', finds the
    // directory (0x006054c0, entries of 0x3d bytes at +0xc), then the file in that directory's
    // sorted list (+0x2d array of 0x28-byte records, +0x25 count, comparator 0x00605190) and makes
    // the path from the first extension (+0x18 table, +0x10 count) whose mask matches; otherwise
    // tries the directory's sub-locations (+0x39 / +0x31, 0x10c bytes each) through 0x00605540.
    const char* resolve(const char* path) {
        c_resolve.hit();
        char fname[0x100];
        char dir[0x100];
        joan::orig::libc::_splitpath(joan::orig::arg::mptr(path), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(dir), joan::orig::arg::mptr(fname), joan::orig::arg::mptr(static_cast<char*>(nullptr)));  // _splitpath
        const std::int32_t length = joan::orig::arg::as<std::int32_t>(std::strlen(joan::orig::arg::str(static_cast<const char*>(dir))));  // strlen
        if (length >= 2 && static_cast<std::int8_t>(dir[length - 1]) == 0x5c) dir[length - 1] = 0;
        const std::int32_t index = joan::orig::arg::as<std::int32_t>(joan::orig::as_tex_dir(this)->dir_index(joan::orig::arg::str(static_cast<char*>(dir))));  // 0x006054c0
        if (index < 0) return nullptr;
        std::uint8_t* const entry = jo::view<std::uint8_t*>(jo::self<jo::TexDir>(this)->dirs) + index * static_cast<std::int32_t>(sizeof(jo::TexDirEntry));
        const std::int32_t found = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(img::obj(d::g_misc))->bin_search_5911a0(joan::orig::arg::mptr(static_cast<char*>(fname)), joan::orig::arg::str(jo::as_tex_dir_entry(entry)->buf_2d), joan::orig::arg::i32(jo::as_tex_dir_entry(entry)->name_count), static_cast<std::int32_t>(sizeof(jo::TexName)),
                                                                                 fn::code_value(fn::cmp_stricmp_605190), joan::orig::arg::mptr(static_cast<void*>(nullptr))));  // 0x005911a0
        char* const result = &img::global<char>(d::g_a69b50);
        if (found != 0) {
            std::uint8_t* const record = jo::view<std::uint8_t*>(jo::as_tex_dir_entry(entry)->buf_2d) + (found - 1) * static_cast<std::int32_t>(sizeof(jo::TexName));
            std::int32_t i = 0;
            for (; i < jo::self<jo::TexDir>(this)->tag_count; ++i) {
                if ((static_cast<std::int32_t>(jo::view<std::int16_t>(jo::as_tex_name(record)->ext_mask)) &
                     static_cast<std::int32_t>(jo::view<std::int16_t>(jo::self<jo::TexDir>(this)->tags[i].bit))) != 0)
                    break;
            }
            joan::orig::libc::_makepath(joan::orig::arg::mptr(result), joan::orig::arg::mptr(static_cast<char*>(nullptr)), joan::orig::arg::mptr(jo::as_tex_dir_entry(entry)->files[jo::as_tex_name(record)->dir].path), joan::orig::arg::mptr(reinterpret_cast<char*>(record)), joan::orig::arg::mptr(reinterpret_cast<char*>(&jo::self<jo::TexDir>(this)->tags[i])));  // _makepath
            return result;
        }
        for (std::int32_t j = 0; j < jo::as_tex_dir_entry(entry)->file_count; ++j) {
            jo::TexFolder* const location = &jo::as_tex_dir_entry(entry)->files[j];
            if (static_cast<std::int8_t>(location->kind) == 0) continue;
            const char* const r = joan::orig::arg::as<const char*>(joan::orig::as_tex_dir(this)->find(joan::orig::arg::str(path), joan::orig::arg::str(static_cast<void*>(location)), joan::orig::arg::str(result), static_cast<std::int32_t>(static_cast<std::int8_t>(location->recurse))));  // 0x00605540
            if (r != nullptr) return r;
        }
        return nullptr;
    }
};

#define HELPER29(name, addr, fn) JOAN_REPLACE("en3d/helpers29", name, addr, fn)
HELPER29("Texture2D::attach", 0x00605f00, &H::attach_2d);
HELPER29("TextureCube::attach", 0x00606b90, &H::attach_cube);
HELPER29("TextureBump::attach", 0x00607110, &H::attach_bump);
HELPER29("TextureBump::make_du_dv", 0x00607950, &H::make_du_dv);
HELPER29("TextureDirectory::find_file", 0x006051c0, &H::resolve);
#undef HELPER29

}  // namespace en3d_helpers29_2
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
namespace game_leaves80a_2 {

using joan::call::thiscall;

struct H {
    void set_name(const char* name) {  // 0x00605ad0
        joan::orig::as_texture_manager(jo::view<void*>(jo::self<jo::Texture>(this)->manager))->set_name(joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::str(name));  // 0x0060a670
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Texture>(this)->name)), joan::orig::arg::str(name), 0x20u);  // strncpy
        jo::view<std::uint8_t>(jo::self<jo::Texture>(this)->name[32]) = 0;
    }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Texture::set_name", 0x00605ad0, &H::set_name);
#undef L

}  // namespace game_leaves80a_2
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - Obj::call_604150 0x006040e0 (ret 0), Obj::call_609260 0x00608de0 (ret 0).
namespace {
namespace game_b99_07_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00608de0
    u32 call_609260() { joan::orig::as_texture_manager(this)->clear(); return 0; }  // 0x00609260 (result unused: only term_5642b0, an exit handler)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::call_609260", 0x00608de0, &H::call_609260);
#undef B

}  // namespace game_b99_07_2
}  // namespace

// ---------------------------------------- from modules/en3d_helpers8.cpp (en3d/helpers8)
//
//  en3d leaf helpers, batch 7b: texture release (TextureManager::release, defined below).
namespace {
namespace en3d_helpers8 {

#define HELPER8(name, addr, fn) JOAN_REPLACE("en3d/helpers8", name, addr, fn)
HELPER8("TextureArray::release_texture", 0x0060a550, &joan::orig::TextureManager::release);
#undef HELPER8

}  // namespace en3d_helpers8
}  // namespace

// 0x0060a550: release one reference; the last one unloads (0x0060b0d0 for +0x18 set, else
// 0x0060b140), unlinks (0x0060b3e0) and deletes the texture. A texture at its last
// reference with +0x34 set is first touched and requeued (0x0060b1b0).
std::int32_t joan::orig::TextureManager::release(void* texture) {
    std::int32_t result = 0;
    if (texture == nullptr) return 0;
    if (jo::as_texture(texture)->ref_count == 1 && requeue != 0) {
        joan::orig::as_texture_manager(this)->use(joan::orig::arg::mptr(texture));  // 0x0060a500
        jo::view<std::uint32_t>(jo::as_texture(texture)->last_used) = stamp;
        joan::orig::as_texture_manager(this)->insert_unused(joan::orig::arg::mptr(texture));  // 0x0060b1b0
    }
    jo::as_texture(texture)->ref_count = jo::as_texture(texture)->ref_count - 1;
    if (jo::as_texture(texture)->ref_count > 0) return 0;
    if (jo::view<std::int8_t>(jo::as_texture(texture)->name[0]) != 0)
        result = as_index(joan::orig::as_texture_manager(this)->remove_a(joan::orig::arg::mptr(texture)));
    else
        result = as_index(joan::orig::as_texture_manager(this)->remove_b(joan::orig::arg::mptr(texture)));
    joan::orig::as_texture_manager(this)->remove_c(joan::orig::arg::mptr(texture));  // 0x0060b3e0
    using Dtor = void*(JOAN_THISCALL*)(void*, std::uint32_t);
    reinterpret_cast<Dtor>(joan::call::vtable_of(texture)[0])(texture, 1);  // scalar deleting dtor
    return result;
}

// ---------------------------------------- from modules/en3d_helpers28.cpp (en3d/helpers28)
//
//  * module en3d/helpers28: TextureManager::load (0x006099c0, defined below).
namespace {
namespace en3d_helpers28 {

#define HELPER28(name, addr, fn) JOAN_REPLACE("en3d/helpers28", name, addr, fn)
HELPER28("TextureArray::read_texture", 0x006099c0, &joan::orig::TextureManager::load);
#undef HELPER28

}  // namespace en3d_helpers28
}  // namespace

// Helpers of TextureManager::load (outside the module namespace: used by the joan::orig definition).
namespace {

using joan::call::stdcall_call;
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

void* game_device() { return global<void*>(d::g_game_device); }

JOAN_COUNTER(c_tex_load, "TextureManager::load");

// Fragment names in .rdata.
const char* name(img::Addr address) { return &img::global<const char>(address); }

}  // namespace

// 0x006099c0 TextureManager::load(name, kind, deferred): existing texture (0x0060a4b0) or a
// new one of kind 1 (2D, 0x006094c0), 2 (cube, 0x00609570) or 3 (0x00609620) read through
// D3DX from the resolved path (0x006051c0 on 0xa59718) - or, when deferred, handed to a
// background loader (0x164 bytes, 0x00577be0) with the kind's completion callback.
void* joan::orig::TextureManager::load(const char* texture_name, std::int32_t kind, std::uint32_t deferred) {
    joan::hitch::Span hitch_span("texture_load");  // [Debug] HitchLog
    c_tex_load.hit();
    if (texture_name == nullptr) return nullptr;
    void* texture = joan::orig::arg::as<void*>(joan::orig::as_textures(this)->find(joan::orig::arg::str(texture_name)));  // 0x0060a4b0 find
    if (texture != nullptr) {
        joan::orig::as_texture_manager(this)->use(joan::orig::arg::mptr(texture));  // 0x0060a500
        return texture;
    }
    bool found = false;
    char path[0x194];
    joan::orig::as_string(path)->construct();  // 0x005a1170
    void* const resolved = joan::orig::arg::as<void*>(joan::orig::as_tex_dir(img::obj(d::g_texture_dir))->resolve(joan::orig::arg::str(texture_name)));  // 0x006051c0
    if (resolved != nullptr) {
        found = true;
        joan::orig::as_string(path)->assign(joan::orig::arg::str(resolved));  // 0x005a1440 assign
    }
    if (!found) return texture;
    static const std::uintptr_t kCreate[4] = {0, fn::code_value(fn::TextureArray_create_Texture2DImage), fn::code_value(fn::TextureList_add_new), fn::code_value(fn::Obj_new_child_609620)};    // 0x006094c0 / 0x00609570 / 0x00609620
    static const std::uintptr_t kCallback[4] = {0, fn::code_value(fn::Texture2D_on_loaded), fn::code_value(fn::TextureCube_on_loaded), fn::code_value(fn::TextureBump_on_loaded)};
    if (kind < 1 || kind > 3) return texture;
    std::uint8_t* const object = thiscall<std::uint8_t*>(kCreate[kind], this);
    texture = object;
    joan::orig::as_texture(object)->set_name(joan::orig::arg::str(texture_name));  // 0x00605ad0 set name
    const char* const file = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());  // 0x00417d90 c_str
    if ((deferred & 0xff) == 0) {
        void* d3d_texture = nullptr;
        std::int32_t hr;
        if (kind == 2) {
            if (mode_ != 0)
                hr = joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_cube_texture_from_file_ex_a(joan::orig::arg::mptr(game_device()), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str())), 0xffffffffu, joan::orig::arg::u32(mode_), 0u, 0u, 1u, 0xffffffffu, 0xffffffffu, 0u, joan::orig::arg::mptr(0u), joan::orig::arg::mptr(0u), &d3d_texture));  // D3DXCreateCubeTextureFromFileExA
            else
                hr = joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_cube_texture_from_file_a(joan::orig::arg::mptr(game_device()), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str())), &d3d_texture));  // D3DXCreateCubeTextureFromFileA
        } else {
            if (mode_ != 0)
                hr = joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_texture_from_file_ex_a(joan::orig::arg::mptr(game_device()), joan::orig::arg::str(file), 0xffffffffu, 0xffffffffu, joan::orig::arg::u32(mode_), 0u, 0u, 1u, 0xffffffffu, 0xffffffffu, 0u, joan::orig::arg::mptr(0u), joan::orig::arg::mptr(0u), &d3d_texture));  // D3DXCreateTextureFromFileExA
            else
                hr = joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_texture_from_file_a(joan::orig::arg::mptr(game_device()), joan::orig::arg::str(file), &d3d_texture));  // D3DXCreateTextureFromFileA
        }
        if (hr != 0) joan::orig::error::fatal(joan::orig::arg::mptr(img::obj(d::g_err)), joan::orig::arg::str(name(d::s_Cannot_read_texture)));  // "Cannot read texture"
        if (kind == 1) {
            joan::orig::as_texture2d(object)->attach(joan::orig::arg::mptr(d3d_texture), 1u);  // 0x00605f00
            com_call(d3d_texture, 2);                     // Release
        } else if (kind == 2) {
            joan::orig::as_texture_cube(object)->attach(joan::orig::arg::mptr(d3d_texture), 2u);  // 0x00606b90
            com_call(d3d_texture, 2);
        } else {
            void* converted = nullptr;
            joan::orig::as_texture_bump(object)->make_du_dv(joan::orig::arg::mptr(d3d_texture), &converted);  // 0x00607950
            joan::orig::as_texture_bump(object)->attach(joan::orig::arg::mptr(converted), 2u);             // 0x00607110
            com_call(d3d_texture, 2);
            com_call(converted, 2);
        }
        return texture;
    }
    jo::as_texture(object)->levels = mode_;
    void* loader = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::FileLoader)));  // 0x164 on x86
    if (loader != nullptr) loader = joan::orig::as_file_loader(loader)->construct();  // 0x00577be0
    jo::view<void*>(jo::as_texture(object)->loader) = loader;
    joan::orig::as_file_loader(jo::view<void*>(jo::as_texture(object)->loader))->set_path(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str())));  // 0x00577c70
    joan::orig::as_texture_manager(this)->add(joan::orig::arg::mptr(static_cast<void*>(object)));                                  // 0x0060b2e0
    joan::orig::as_texture_manager(this)->use(joan::orig::arg::mptr(static_cast<void*>(object)));                                  // 0x0060a500
    joan::orig::as_file_loader(jo::view<void*>(jo::as_texture(object)->loader))->start(joan::orig::arg::mptr(0), 0u, joan::orig::arg::u32(-1), reinterpret_cast<void*>(kCallback[kind]), static_cast<void*>(object));  // 0x00577d10
    return texture;
}

// ---------------------------------------- from modules/en3d_b84_01_2.cpp (en3d/b84_01)
namespace {
namespace en3d_b84_01_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using joan::call::stdcall_call;
using joan::img::obj;
using joan::img::str;

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

void* const kLog = obj(d::g_err);        // error log

void game_memset(void* p, std::int32_t v, std::uint32_t n) { std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_rt_create, "RenderTarget::create");

struct H {
    // 0x00608080. QueryInterface writes the out pointer (null on failure per COM).
    std::int32_t rt_create(std::uint32_t w, std::uint32_t h, std::uint32_t levels) {
        c_rt_create.hit();
        const std::int32_t hr = joan::orig::arg::as<std::int32_t>(joan::orig::d3dx::create_texture(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::u32(w), joan::orig::arg::u32(h), joan::orig::arg::u32(levels), 1u, joan::orig::arg::u32(global<std::uint32_t>(d::g_a52444)), 0u, &jo::view<void*>(jo::self<jo::RenderTarget>(this)->d3d_texture)));  // D3DXCreateTexture
        if (hr != 0) {
            joan::orig::error::fatal(joan::orig::arg::mptr(kLog), str(d::s_Cannot_create_texture));  // 0x00586cb0 "Cannot create texture"
            return 0;
        }
        void* surface = nullptr;
        com_call(jo::view<void*>(jo::self<jo::RenderTarget>(this)->d3d_texture), 0, img::obj(d::blob_9e1718), &surface);  // QueryInterface
        joan::orig::as_texture(this)->attach(joan::orig::arg::mptr(surface), joan::orig::arg::u32(w), joan::orig::arg::u32(h), joan::orig::arg::u32(levels), 4u);                              // 0x00605a40
        com_call(surface, 2);                                                              // Release
        joan::orig::as_device_resource(&jo::self<jo::RenderTarget>(this)->device_resource)->set_callback(reinterpret_cast<void*>(fn::code_value(fn::Tex_release_608c60)), static_cast<void*>(this), 0u);                                       // 0x005e9010 (callback 0x00608c60)
        joan::orig::as_sys_en(obj(d::g_sys3d))->link_resource(&jo::self<jo::RenderTarget>(this)->device_resource);  // 0x005ff290
        return 1;
    }

    // 0x006058f0
    void* texture_ctor() {
        jo::view<std::uintptr_t>(jo::self<jo::Texture>(this)->vtable) = img::addr(d::vt_Obj_9d9a2c);
        game_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::Texture)));  // 0x58 (x86); clears the vtable just set, as the original
        jo::view<std::uint32_t>(jo::self<jo::Texture>(this)->ref_count) = 1;
        jo::self<jo::Texture>(this)->usage = 0;
        jo::view<std::uint32_t>(jo::self<jo::Texture>(this)->state) = 0xffffffffu;
        return this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b84_01", name, addr, fn)
E3D("RenderTarget::create", 0x00608080, &H::rt_create);
E3D("Texture::Texture", 0x006058f0, &H::texture_ctor);
#undef E3D

}  // namespace en3d_b84_01_2
}  // namespace

// ---------------------------------------- from modules/game_b103_12_2.cpp (game/b103_12)
namespace {
namespace game_b103_12_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;


using joan::orig::helpers::op_delete;

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    H* sdtor_605950(u32 flags) {
        joan::orig::as_texture(this)->destruct();  // 0x006059a0
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
    H* sdtor_605d30(u32 flags) {
        joan::orig::as_texture2d(this)->destruct();  // 0x00605d80
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
    H* sdtor_6069d0(u32 flags) {
        joan::orig::as_texture_cube(this)->destruct();  // 0x00606a20
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
    H* sdtor_606f40(u32 flags) {
        joan::orig::as_texture_bump(this)->destruct();  // 0x00606f90
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
    H* sdtor_607f60(u32 flags) {
        joan::orig::as_render_texture(this)->destruct();  // 0x00607fb0
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }

    u32 clear_48() {  // 0x00606c80
        jo::view<u32>(jo::self<jo::TextureCube>(this)->state) = 0;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::sdtor_605950", 0x00605950, &H::sdtor_605950);
B("Obj::sdtor_605d30", 0x00605d30, &H::sdtor_605d30);
B("Obj::sdtor_6069d0", 0x006069d0, &H::sdtor_6069d0);
B("Obj::clear_48", 0x00606c80, &H::clear_48);
B("Obj::sdtor_606f40", 0x00606f40, &H::sdtor_606f40);
B("Obj::sdtor_607f60", 0x00607f60, &H::sdtor_607f60);
#undef B

}  // namespace game_b103_12_2
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

using joan::call::thiscall;


JOAN_COUNTER(c_dtor6059a0, "Obj::dtor_6059a0");

struct H {
    // 0x006059a0
    // eax = this, or the Release result.
    std::uintptr_t dtor_6059a0() {
        c_dtor6059a0.hit();
        std::uintptr_t r = reinterpret_cast<std::uintptr_t>(this);
        jo::view<std::uintptr_t>(jo::self<jo::Texture>(this)->vtable) = img::addr(d::vt_Obj_9d9a2c);
        if (jo::view<void*>(jo::self<jo::Texture>(this)->loader) != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::Texture>(this)->loader);
            if (p != nullptr) joan::orig::as_file_loader(p)->scalar_deleting_destructor(1u);  // 0x005afd50 scalar deleting dtor
            jo::self<jo::Texture>(this)->loader = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Texture>(this)->d3d_base) != nullptr) {
            void* const o = jo::view<void*>(jo::self<jo::Texture>(this)->d3d_base);
            r = reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(void*)>((*static_cast<std::uintptr_t* const*>(o))[2])(o);  // IUnknown::Release
            jo::self<jo::Texture>(this)->d3d_base = nullptr;
        }
        return r;
    }

    // 0x00605be0
    std::int32_t idle() {
        if (jo::view<void*>(jo::self<jo::Texture>(this)->loader) == nullptr) return 0;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::view<void*>(jo::self<jo::Texture>(this)->loader))->done()) != 0) return 0;  // 0x00578250
        return 1;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("Obj::dtor_6059a0", 0x006059a0, &H::dtor_6059a0);
M("Obj::idle", 0x00605be0, &H::idle);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_leaves81_2.cpp (en3d/helpers33)
namespace {
namespace game_leaves81_2 {


struct H {
    // 0x00605a40 Texture::attach(d3d texture, width, height, levels, arg)
    void tex_attach(void* texture, std::uint32_t width, std::uint32_t height, std::uint32_t levels, std::uint32_t arg) {
        jo::view<void*>(jo::self<jo::Texture>(this)->d3d_base) = texture;
        void* const t = jo::view<void*>(jo::self<jo::Texture>(this)->d3d_base);
        using AddRef = std::uint32_t(JOAN_STDCALL*)(void*);
        reinterpret_cast<AddRef>(joan::call::vtable_of(jo::view<void*>(jo::self<jo::Texture>(this)->d3d_base))[1])(t);
        jo::self<jo::Texture>(this)->width = width;
        jo::self<jo::Texture>(this)->height = height;
        jo::self<jo::Texture>(this)->levels = levels;
        jo::self<jo::Texture>(this)->usage = arg;
        jo::self<jo::Texture>(this)->state = -1;
        jo::self<jo::Texture>(this)->alpha_bits = -1;
    }
};

#define T33(name, addr, fn) JOAN_REPLACE("en3d/helpers33", name, addr, fn)
T33("Texture::attach", 0x00605a40, &H::tex_attach);
#undef T33

}  // namespace game_leaves81_2
}  // namespace

// ---------------------------------------- from modules/game_b100_11_2.cpp (game/b100_11)
namespace {
namespace game_b100_11_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;


using joan::orig::helpers::crt_memset;

struct H {
    H* clear_3c() {  // 0x00608db0
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::TextureManager)));  // 0x3c (x86)
        return this;
    }

    // ------------------------------------------------------------------ 0x00605b30
    i32 has_605b70() { return joan::orig::arg::as<i32>(joan::orig::as_texture(this)->ready()) != 0 ? 1 : 0; }  // 0x00605b70

    // ------------------------------------------------------------------ 0x00605c30
    u32 play_605c30(u32 x) {
        if (jo::view<void*>(jo::self<jo::Texture>(this)->loader) == nullptr) return 0;
        jo::view<u32>(jo::self<jo::Texture>(this)->priority) = 2;
        return joan::orig::arg::as<u32>(joan::orig::as_sound(jo::view<void*>(jo::self<jo::Texture>(this)->loader))->set_11c(joan::orig::arg::u32(x)));  // 0x00578390
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::has_605b70", 0x00605b30, &H::has_605b70);
B("Obj::play_605c30", 0x00605c30, &H::play_605c30);
B("Obj::clear_3c", 0x00608db0, &H::clear_3c);
#undef B

}  // namespace game_b100_11_2
}  // namespace

// ---------------------------------------- from modules/game_ui8.cpp (game/ui8)
namespace {
namespace game_ui8 {

using joan::call::thiscall;


struct H {
    // 0x00605b70. ret 0.
    std::int32_t texture_ready() {
        if (jo::self<jo::Texture>(this)->d3d_base == nullptr) return 0;
        if (jo::self<jo::Texture>(this)->loader != nullptr) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::view<void*>(jo::self<jo::Texture>(this)->loader))->done()) == 0) return 0;  // 0x00578250
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_file_loader(jo::view<void*>(jo::self<jo::Texture>(this)->loader))->ok()) == 0) return 0;  // 0x005b0a80
        }
        return 1;
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("Texture::ready", 0x00605b70, &H::texture_ready);
#undef U8

}  // namespace game_ui8
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

using u32 = std::uint32_t;


struct H {
    u32 zero() { return 0; }
    u32 eight() { return 8; }
    u32 get_48() { return jo::view<u32>(jo::self<jo::Texture>(this)->state); }
};

JOAN_REPLACE("game/b105_17", "Obj::get_48", 0x00605c80, &H::get_48);
JOAN_REPLACE("game/b105_17", "Obj::eight_605ca0", 0x00605ca0, &H::eight);
JOAN_REPLACE("game/b105_17", "Obj::zero_605cc0", 0x00605cc0, &H::zero);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/en3d_b83_03.cpp (en3d/b83_03)
namespace {
namespace en3d_b83_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;


using joan::orig::helpers::memset_;

struct H {
    // 0x00605ce0 (eax = this)
    void* texture2d_ctor() {
        joan::orig::as_texture(this)->construct();  // 0x006058f0
        jo::view<std::uintptr_t>(jo::self<jo::Texture2D>(this)->vtable) = img::addr(d::vt_Texture2D);
        memset_(&jo::self<jo::Texture2D>(this)->d3d_texture, 0, static_cast<u32>(sizeof(jo::Texture2D::d3d_texture)));  // 4 on x86
        return this;
    }
};

#define B83_03(name, addr, fn) JOAN_REPLACE("en3d/b83_03", name, addr, fn)
B83_03("Texture2D::Texture2D", 0x00605ce0, &H::texture2d_ctor);
#undef B83_03

}  // namespace en3d_b83_03
}  // namespace

// ---------------------------------------- from modules/game_b99_07_3.cpp (game/b99_07)
namespace {
namespace game_b99_07_3 {

using u32 = std::uint32_t;

using joan::call::thiscall;

// COM method: vtable slot at byte offset `off`, stdcall with the interface as first argument.
template <class R = std::int32_t, class... A>
R com_call(void* iface, std::uint32_t off, A... args) {
    const std::uintptr_t* vt = *reinterpret_cast<std::uintptr_t* const*>(iface);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vt[joan::call::x86_vslot(off)])(iface, args...);
}

struct H {
    // ------------------------------------------------------------------ 0x00605d80
    u32 dtor_605d80() {
        jo::view<std::uintptr_t>(jo::self<jo::Texture2D>(this)->vtable) = img::addr(d::vt_Texture2D);
        if (jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture) != nullptr) {
            com_call<u32>(jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture), 8);  // Release
            jo::self<jo::Texture2D>(this)->d3d_texture = nullptr;
        }
        return joan::orig::arg::as<u32>(joan::orig::as_texture(this)->destruct());  // 0x006059a0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::dtor_605d80", 0x00605d80, &H::dtor_605d80);
#undef B

}  // namespace game_b99_07_3
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using joan::call::stdcall_call;
using joan::img::obj;
using joan::img::str;

using u8 = std::uint8_t;

// COM method (stdcall, interface pointer first) through the vtable slot (byte offset).
template <class R = u32, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    const std::uintptr_t fn = reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)]);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(fn)(object, args...);
}

JOAN_COUNTER(c_texture, "Texture::create");

struct H {
    // ------------------------------------------------------------------ 0x00605e20
    i32 create_texture(u32 width, u32 height, u32 levels, u32 format) {
        c_texture.hit();
        const u32 d3d_format = joan::orig::arg::as<u32>(joan::orig::d3d_format(joan::orig::arg::u32(format)));  // 0x00600080
        const u32 hr = joan::orig::arg::as<u32>(joan::orig::d3dx::create_texture(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::u32(width), joan::orig::arg::u32(height), joan::orig::arg::u32(levels), joan::orig::arg::u32(u32{0}), joan::orig::arg::u32(d3d_format), joan::orig::arg::u32(u32{1}), joan::orig::arg::mptr(&jo::self<jo::Texture2D>(this)->d3d_texture)));  // D3DXCreateTexture
        if (hr != 0) {
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Cannot_create_texture));  // 0x00586cb0 "Cannot create texture"
            return 0;
        }
        void* surface = nullptr;
        com_call(jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture), 0x0, obj(d::blob_9e1718), &surface);  // QueryInterface
        joan::orig::as_texture(this)->attach(joan::orig::arg::mptr(surface), joan::orig::arg::u32(width), joan::orig::arg::u32(height), joan::orig::arg::u32(levels), joan::orig::arg::u32(i32{1}));  // 0x00605a40
        com_call(surface, 0x8);                                                     // Release
        return 1;
    }
};

JOAN_REPLACE("game/b108_07", "Texture::create", 0x00605e20, &H::create_texture);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b104_10.cpp (game/b104_10)
namespace {
namespace game_b104_10 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;

using u16 = std::uint16_t;

// byte_offset: the x86 vtable byte offset (4-byte slots); the slot index is byte_offset / 4 on every architecture.
template <class R = i32, class... A>
R com_call(void* object, i32 byte_offset, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[joan::call::x86_vslot(byte_offset)])(object, args...);
}

JOAN_COUNTER(c_alpha, "Texture::alpha_state");

// ---------------------------------------------------------------------------------------
// Texture::alpha_state 0x00605ff0 / 0x00607200 / 0x006082a0
// ---------------------------------------------------------------------------------------
struct SurfaceDesc {  // D3DSURFACE_DESC (D3D8)
    u32 format, type, usage, pool, size, ms_type, width, height;
};
static_assert(sizeof(SurfaceDesc) == 32);
struct LockedRect {
    i32 pitch;
    u8* bits;
};

void* tex(void* self) { return jo::view<void*>(jo::as_texture2d(self)->d3d_texture); }
i32 tex_unlock_result(void* self, i32 result) {
    com_call<i32>(tex(self), 0x44, 0u);  // UnlockRect(0)
    jo::as_texture(self)->state = result;
    return result;
}

template <class Pixel, class Pred>
i32 alpha_scan(void* self, const SurfaceDesc& desc, Pred has_alpha) {
    LockedRect lr{};
    if (com_call<i32>(tex(self), 0x40, 0u, static_cast<void*>(&lr), static_cast<void*>(nullptr), 0x10u) != 0) {
        jo::as_texture(self)->state = -2;  // LockRect failed
        return -2;
    }
    u8* row = lr.bits;
    for (u32 y = 0; y < desc.height; ++y, row += lr.pitch) {
        const Pixel* p = reinterpret_cast<const Pixel*>(row);
        for (u32 x = 0; x < desc.width; ++x, ++p) {
            if (has_alpha(*p)) return tex_unlock_result(self, 1);
        }
    }
    return tex_unlock_result(self, 0);
}

i32 texture_alpha_state(void* self) {
    if (jo::as_texture(self)->state != -1) return jo::as_texture(self)->state;
    c_alpha.hit();
    if (joan::orig::arg::as<i32>(joan::orig::as_texture(self)->is_loading()) != 0) return jo::as_texture(self)->state;  // 0x00605be0
    if (joan::orig::arg::as<i32>(joan::orig::as_texture(self)->ready()) == 0) {                            // 0x00605b70
        jo::as_texture(self)->state = -2;
        return -2;
    }
    SurfaceDesc desc;
    std::memset(&desc, 0, sizeof desc);
    if (com_call<i32>(tex(self), 0x38, 0u, static_cast<void*>(&desc)) != 0) {  // GetLevelDesc(0)
        jo::as_texture(self)->state = -2;
        return -2;
    }
    switch (desc.format) {
        case 0x14: case 0x16: case 0x17: case 0x18: case 0x1b: case 0x1e: case 0x29:
            jo::as_texture(self)->state = 0;
            return 0;
        case 0x15:  // A8R8G8B8
            return alpha_scan<u32>(self, desc, [](u32 v) { return (v & 0xff000000u) < 0xfc000000u; });
        case 0x19:  // A1R5G5B5
            return alpha_scan<u16>(self, desc, [](u16 v) { return (v & 0x8000) < 0x8000; });
        case 0x1a:  // A4R4G4B4
            return alpha_scan<u16>(self, desc, [](u16 v) { return (v & 0xf000) < 0xf000; });
        case 0x1c:  // A8
            return alpha_scan<u8>(self, desc, [](u8 v) { return v < 0xfc; });
        case 0x1d:  // A8R3G3B2
        case 0x28:  // A8P8
            return alpha_scan<u16>(self, desc, [](u16 v) { return (v & 0xff00) < 0xfc00; });
        default:
            jo::as_texture(self)->state = -2;
            return -2;
    }
}

struct H {
    i32 alpha_state() { return texture_alpha_state(this); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_10", name, addr, fn)
B("Texture::alpha_state_605ff0", 0x00605ff0, &H::alpha_state);
B("Texture::alpha_state_607200", 0x00607200, &H::alpha_state);
B("Texture::alpha_state_6082a0", 0x006082a0, &H::alpha_state);
#undef B

}  // namespace game_b104_10
}  // namespace

// ---------------------------------------- from modules/game_b102_11_2.cpp (game/b102_11)
namespace {
namespace game_b102_11_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::img::obj;
using joan::img::str;

using u8 = std::uint8_t;

template <class R = i32, class... A>
R com_call(void* object, i32 index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

// IUnknown / IDirect3D(Cube)Texture8 vtable slots.
constexpr i32 kQueryInterface = 0, kAddRef = 1, kRelease = 2, kGetLevelCount = 13, kGetLevelDesc = 14;

struct SurfaceDesc {  // D3DSURFACE_DESC (D3D8), 32 bytes
    u32 format, type, usage, pool, size, multisample;
    u32 width, height;
};
static_assert(sizeof(SurfaceDesc) == 0x20);

JOAN_COUNTER(c_alpha_bits, "Texture::alpha_bits");
JOAN_COUNTER(c_mem_size, "Texture::memory_size");
JOAN_COUNTER(c_attach, "TextureX::attach");

struct H {
    // 0x00606740 / 0x00606cb0 / 0x00607cd0 / 0x006089f0 (ret), vtable slots
    i32 alpha_bits() {
        c_alpha_bits.hit();
        if (jo::self<jo::Texture2D>(this)->alpha_bits != -1) return jo::self<jo::Texture2D>(this)->alpha_bits;
        if (joan::orig::arg::as<i32>(joan::orig::as_texture(this)->is_loading()) != 0) return jo::self<jo::Texture2D>(this)->alpha_bits;  // 0x00605be0
        if (joan::orig::arg::as<i32>(joan::orig::as_texture(this)->ready()) == 0) {                       // 0x00605b70
            jo::self<jo::Texture2D>(this)->alpha_bits = -2;
            return -2;
        }
        SurfaceDesc desc;
        if (com_call(jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture), kGetLevelDesc, 0u, &desc) != 0) {
            jo::self<jo::Texture2D>(this)->alpha_bits = -2;
            return -2;
        }
        const i32 fmt = static_cast<i32>(desc.format);
        switch (fmt) {
            case 20:  // R8G8B8
            case 22:  // X8R8G8B8
            case 23:  // R5G6B5
            case 24:  // X1R5G5B5
            case 27:  // R3G3B2
            case 30:  // X4R4G4B4
            case 41:  // P8
                jo::self<jo::Texture2D>(this)->alpha_bits = 0;
                break;
            case 21:  // A8R8G8B8
            case 28:  // A8
            case 29:  // A8R3G3B2
            case 40:  // A8P8
                jo::self<jo::Texture2D>(this)->alpha_bits = 8;
                break;
            case 25:          // A1R5G5B5
            case 0x31545844:  // DXT1
                jo::self<jo::Texture2D>(this)->alpha_bits = 1;
                break;
            case 26:          // A4R4G4B4
            case 0x32545844:  // DXT2
            case 0x33545844:  // DXT3
                jo::self<jo::Texture2D>(this)->alpha_bits = 4;
                break;
            default: break;
        }
        return jo::self<jo::Texture2D>(this)->alpha_bits;
    }

    u32 memory_size_common(u32 factor) {
        c_mem_size.hit();
        u32 total = 0;
        if (jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture) == nullptr) return total;
        const u32 levels = com_call<u32>(jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture), kGetLevelCount);
        SurfaceDesc desc;
        for (u32 i = 0; i < levels; ++i) {
            if (com_call(jo::view<void*>(jo::self<jo::Texture2D>(this)->d3d_texture), kGetLevelDesc, i, &desc) == 0) total += desc.size * factor;  // add / imul 6, 32-bit wrap
        }
        return total;
    }
    // 0x006068d0 / 0x00607e60 / 0x00608d00 (ret), vtable slots
    u32 memory_size() { return memory_size_common(1); }
    // 0x00606e40 (ret), vtable slot
    u32 memory_size_cube() { return memory_size_common(6); }

    // 0x00608180 (ret 8)
    u32 attach(void* d3d_texture, u32 arg) {
        c_attach.hit();
        jo::view<void*>(jo::self<jo::RenderTexture>(this)->d3d_texture) = d3d_texture;
        com_call(jo::view<void*>(jo::self<jo::RenderTexture>(this)->d3d_texture), kAddRef);
        SurfaceDesc desc{};
        i32 hr = com_call(jo::view<void*>(jo::self<jo::RenderTexture>(this)->d3d_texture), kGetLevelDesc, 0u, &desc);
        if (hr != 0)
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Cannot_read_texture_surface));  // 0x00586cb0 "Cannot read texture surface desc"
        void* object = nullptr;
        hr = com_call(jo::view<void*>(jo::self<jo::RenderTexture>(this)->d3d_texture), kQueryInterface, obj(d::blob_9e1718), &object);
        (void)hr;
        const u32 levels = com_call<u32>(jo::view<void*>(jo::self<jo::RenderTexture>(this)->d3d_texture), kGetLevelCount);
        joan::orig::as_texture(this)->attach(joan::orig::arg::mptr(object), joan::orig::arg::u32(desc.width), joan::orig::arg::u32(desc.height), joan::orig::arg::u32(levels), joan::orig::arg::u32(arg));  // 0x00605a40
        com_call(object, kRelease);
        joan::orig::as_device_resource(&jo::self<jo::RenderTexture>(this)->device_resource)->set_callback(reinterpret_cast<void*>(fn::code_value(fn::Tex_release_608c60)), this, 0u);       // 0x005e9010 (callback 0x00608c60)
        joan::orig::as_sys_en(obj(d::g_sys3d))->link_resource(joan::orig::arg::mptr(&jo::self<jo::RenderTexture>(this)->device_resource));  // 0x005ff290
        return 0;  // no callers (0x00608180 is never called)
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Texture2D::alpha_bits", 0x00606740, &H::alpha_bits);
B11("Texture2D::memory_size", 0x006068d0, &H::memory_size);
B11("TextureCube::alpha_bits", 0x00606cb0, &H::alpha_bits);
B11("TextureCube::memory_size", 0x00606e40, &H::memory_size_cube);
B11("TextureBump::alpha_bits", 0x00607cd0, &H::alpha_bits);
B11("TextureBump::memory_size", 0x00607e60, &H::memory_size);
B11("TextureX::attach", 0x00608180, &H::attach);
B11("TextureX::alpha_bits", 0x006089f0, &H::alpha_bits);
B11("TextureX::memory_size", 0x00608d00, &H::memory_size);
#undef B11

}  // namespace game_b102_11_2
}  // namespace

// ---------------------------------------- from modules/game_b97_06_2.cpp (game/b97_06)
namespace {
namespace game_b97_06_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;


void* crt_memset(void* d, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x00606980
    void* ctor_606980() {
        joan::orig::as_texture(this)->construct();  // 0x006058f0
        jo::view<std::uintptr_t>(jo::self<jo::TextureCube>(this)->vtable) = img::addr(d::vt_Obj_9d9a98);
        crt_memset(&jo::self<jo::TextureCube>(this)->d3d_texture, 0, static_cast<u32>(sizeof(void*)));
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("Obj::ctor_606980", 0x00606980, &H::ctor_606980);
#undef B

}  // namespace game_b97_06_2
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

using joan::call::thiscall;

// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    return reinterpret_cast<Fn>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

struct H {
    // 0x00606a20 CubeTexture::dtor
    void cube_dtor() {
        jo::view<std::uintptr_t>(jo::self<jo::TextureCube>(this)->vtable) = static_cast<std::uint32_t>(img::addr(d::vt_Obj_9d9a98));
        if (jo::view<void*>(jo::self<jo::TextureCube>(this)->d3d_texture) != nullptr) {
            com_call(jo::view<void*>(jo::self<jo::TextureCube>(this)->d3d_texture), 8);  // Release
            jo::view<void*>(jo::self<jo::TextureCube>(this)->d3d_texture) = nullptr;
        }
        joan::orig::as_texture(this)->destruct();  // 0x006059a0
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("CubeTexture::dtor", 0x00606a20, &H::cube_dtor);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using joan::call::stdcall_call;
using joan::img::obj;
using joan::img::str;

using u8 = std::uint8_t;


JOAN_COUNTER(c_texture_606ac0, "Texture::create_606ac0");

struct H {
    // -------------------------------------------------------------- 0x00606ac0 (dead code)
    u32 texture_create(u32 w, u32 h) {
        c_texture_606ac0.hit();
        const i32 hr = joan::orig::arg::as<i32>(joan::orig::d3dx::create_cube_texture(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::u32(w), joan::orig::arg::u32(h), joan::orig::arg::u32(u32{0}), joan::orig::arg::u32(u32{0x14}), joan::orig::arg::u32(u32{1}), joan::orig::arg::mptr(&jo::self<jo::TextureCube>(this)->d3d_texture)));
        if (hr != 0) {
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Cannot_create_texture));  // 0x00586cb0 "Cannot create texture"
            return 0;
        }
        void* out = nullptr;
        void* const tex = jo::view<void*>(jo::self<jo::TextureCube>(this)->d3d_texture);
        using QueryFn = i32(JOAN_STDCALL*)(void*, const void*, void**);
        reinterpret_cast<QueryFn>((*static_cast<std::uintptr_t* const*>(tex))[0])(tex, obj(d::blob_9e1718), &out);  // QueryInterface
        joan::orig::as_texture(this)->attach(joan::orig::arg::mptr(out), joan::orig::arg::u32(w), joan::orig::arg::u32(w), joan::orig::arg::u32(h), joan::orig::arg::u32(i32{2}));  // 0x00605a40
        using ReleaseFn = u32(JOAN_STDCALL*)(void*);
        reinterpret_cast<ReleaseFn>((*static_cast<std::uintptr_t* const*>(out))[2])(out);  // Release
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Texture::create_606ac0", 0x00606ac0, &H::texture_create);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b98_06_2.cpp (game/b98_06)
namespace {
namespace game_b98_06_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;


using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x00606ef0
    H* ctor_606ef0() {
        joan::orig::as_texture(this)->construct();  // 0x006058f0
        jo::view<std::uintptr_t>(jo::self<jo::TextureBump>(this)->vtable) = img::addr(d::vt_TextureBump);
        crt_memset(&jo::self<jo::TextureBump>(this)->d3d_texture, 0, static_cast<u32>(sizeof(void*)));
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Obj::ctor_606ef0", 0x00606ef0, &H::ctor_606ef0);
#undef B

}  // namespace game_b98_06_2
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;

// COM (stdcall, this pushed) call through vtable byte offset.
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

struct H {
    // ------------------------------------------------------------------ 0x00606f90
    void dtor_606f90() {
        jo::view<std::uintptr_t>(jo::self<jo::TextureBump>(this)->vtable) = img::addr(d::vt_TextureBump);
        if (jo::self<jo::TextureBump>(this)->d3d_texture != nullptr) {
            com_call<u32>(jo::view<void*>(jo::self<jo::TextureBump>(this)->d3d_texture), 8);  // Release
            jo::self<jo::TextureBump>(this)->d3d_texture = nullptr;
        }
        joan::orig::as_texture(this)->destruct();  // 0x006059a0
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::~Obj_606f90", 0x00606f90, &H::dtor_606f90);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_tex, "TexTarget::create_607030");

struct H {
    // ------------------------------------------------------------------ 0x00607030
    u32 create_607030(u32 width, u32 height, u32 levels, u32 kind) {
        c_tex.hit();
        const u32 format = joan::orig::arg::as<u32>(joan::orig::d3d_format(joan::orig::arg::u32(kind)));  // 0x00600080
        using Create = i32(JOAN_STDCALL*)(void*, u32, u32, u32, u32, u32, u32, void*);
        const i32 hr = reinterpret_cast<Create>(fn::code_value(fn::D3DXCreateTexture))(global<void*>(d::g_game_device), width, height, levels, 0u, format, 1u,
                                                           &jo::self<jo::TextureBump>(this)->d3d_texture);
        if (hr != 0) {
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Cannot_create_texture));  // 0x00586cb0 'Cannot create texture'
            return 0;
        }
        void* const tex = jo::view<void*>(jo::self<jo::TexTarget>(this)->d3d_texture);
        void* base = nullptr;
        using Query = i32(JOAN_STDCALL*)(void*, const void*, void**);
        reinterpret_cast<Query>(joan::call::vtable_of(tex)[0])(tex, obj(d::blob_9e1718), &base);
        joan::orig::as_texture(this)->attach(joan::orig::arg::mptr(base), joan::orig::arg::u32(width), joan::orig::arg::u32(height), joan::orig::arg::u32(levels), joan::orig::arg::u32(i32{3}));  // 0x00605a40
        using Release = u32(JOAN_STDCALL*)(void*);
        reinterpret_cast<Release>(joan::call::vtable_of(base)[2])(base);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("TexTarget::create_607030", 0x00607030, &H::create_607030);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;


void* game_memset(void* p, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    // 0x00607f10
    H* render_texture_ctor() {
        joan::orig::as_texture(this)->construct();  // 0x006058f0
        jo::view<std::uintptr_t>(jo::self<jo::RenderTexture>(this)->vtable) = img::addr(d::vt_TextureX);
        game_memset(&jo::self<jo::RenderTexture>(this)->d3d_texture, 0, static_cast<std::uint32_t>(sizeof(jo::RenderTexture) - sizeof(jo::Texture)));  // +0x58, 0x1c (x86)
        return this;
    }
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("RenderTexture::RenderTexture", 0x00607f10, &H::render_texture_ctor);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::img::obj;

using u8 = std::uint8_t;


JOAN_COUNTER(c_texobj_dtor, "TexObj::~TexObj");

struct H {
    // ------------------------------------------------------------------------ 0x00607fb0
    u32 texobj_dtor() {
        c_texobj_dtor.hit();
        jo::view<std::uintptr_t>(jo::self<jo::RenderTexture>(this)->vtable) = img::addr(d::vt_TextureX);
        if (jo::view<void*>(jo::self<jo::RenderTexture>(this)->d3d_texture) != nullptr) {
            void* const com = jo::view<void*>(jo::self<jo::RenderTexture>(this)->d3d_texture);
            using Release = u32(JOAN_STDCALL*)(void*);
            (*reinterpret_cast<Release**>(com))[2](com);  // IUnknown::Release
            jo::view<void*>(jo::self<jo::RenderTexture>(this)->d3d_texture) = nullptr;
        }
        if (joan::orig::arg::as<u8>(joan::orig::as_device_resource(&jo::self<jo::RenderTexture>(this)->device_resource)->is_linked()) != 0)                // 0x005d2b10
            joan::orig::as_sys_en(obj(d::g_sys3d))->unlink_resource(joan::orig::arg::mptr(&jo::self<jo::RenderTexture>(this)->device_resource));         // 0x005ff380
        joan::orig::as_device_resource(&jo::self<jo::RenderTexture>(this)->device_resource)->clear_link();                              // 0x005e9060
        return joan::orig::arg::as<u32>(joan::orig::as_texture(this)->destruct());                           // 0x006059a0
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("TexObj::~TexObj", 0x00607fb0, &H::texobj_dtor);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;

using joan::call::stdcall_call;
using joan::img::obj;
using joan::img::str;

using u8 = std::uint8_t;


JOAN_COUNTER(c_create_tex, "RenderTexture::create_608b80");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x00608b80
    u32 create_608b80() {
        c_create_tex.hit();
        const i32 hr = joan::orig::arg::as<i32>(joan::orig::d3dx::create_texture(joan::orig::arg::mptr(global<void*>(d::g_game_device)), joan::orig::arg::u32(jo::self<jo::RenderTexture>(this)->width), joan::orig::arg::u32(jo::self<jo::RenderTexture>(this)->height), joan::orig::arg::u32(jo::self<jo::RenderTexture>(this)->levels), 1u, joan::orig::arg::u32(global<u32>(d::g_a52444)), 0u, joan::orig::arg::mptr(reinterpret_cast<void**>(&jo::self<jo::RenderTexture>(this)->d3d_texture))));  // D3DXCreateTexture
        if (hr != 0) joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Cannot_create_texture));  // 0x00586cb0
        void* surface = nullptr;
        void* const tex = jo::view<void*>(jo::self<jo::RenderTexture>(this)->d3d_texture);
        void* const* const vt = joan::call::vtable_of(tex);
        reinterpret_cast<i32(JOAN_STDCALL*)(void*, const void*, void**)>(vt[0])(tex, obj(d::blob_9e1718), &surface);
        joan::orig::as_texture(this)->attach(joan::orig::arg::mptr(surface), joan::orig::arg::u32(jo::self<jo::RenderTexture>(this)->width), joan::orig::arg::u32(jo::self<jo::RenderTexture>(this)->height), joan::orig::arg::u32(jo::self<jo::RenderTexture>(this)->levels), 4u);  // 0x00605a40
        void* const* const svt = joan::call::vtable_of(surface);
        return reinterpret_cast<u32(JOAN_STDCALL*)(void*)>(svt[2])(surface);  // Release
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("RenderTexture::create_608b80", 0x00608b80, &H::create_608b80);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;


JOAN_COUNTER(c_release_608c60, "Tex::release_608c60");

// ------------------------------------------------------------------------- 0x00608c60
void com_release(void* p) {
    using Release = u32(JOAN_STDCALL*)(void*);
    const Release fn = reinterpret_cast<Release>(joan::call::vtable_of(p)[2]);
    fn(p);
}
void JOAN_CDECL release_608c60(u8* p, i32 mode) {
    c_release_608c60.hit();
    if (mode == 0) {
        if (jo::view<void*>(jo::as_render_target(p)->d3d_texture) != nullptr) {
            com_release(jo::view<void*>(jo::as_render_target(p)->d3d_texture));
            jo::as_render_target(p)->d3d_texture = nullptr;
        }
        if (jo::view<void*>(jo::as_render_target(p)->d3d_base) != nullptr) {
            com_release(jo::view<void*>(jo::as_render_target(p)->d3d_base));
            jo::as_render_target(p)->d3d_base = nullptr;
        }
    } else if (mode == 1) {
        joan::orig::as_render_texture(p)->create();  // 0x00608b80
    }
}

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Tex::release_608c60", 0x00608c60, &release_608c60);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16_2 {

using joan::call::thiscall;

using joan::img::obj;


constexpr img::Addr kArrays(d::g_misc);  // sorted-array helpers

struct H {

    // Sorted pointer lists of the texture manager: find, then remove (0x00591070).
    void* list_remove(std::uintptr_t find, std::int32_t& count, jo::TexturePtr* const& data, void* key) {
        void* const e = thiscall<void*>(find, this, key);
        if (e != nullptr) {
            joan::orig::as_misc(img::obj(kArrays))->del_array_rec_591070(joan::orig::arg::str(data), joan::orig::arg::i32(count), static_cast<std::int32_t>(sizeof(jo::TexturePtr)), as_index(e));
            count = count - 1;
        }
        return e;
    }
    void* list_remove_c(void* key) { return list_remove(fn::code_value(fn::TextureArray_search_loading_array_60b050), jo::self<jo::TextureManager>(this)->loading_count, jo::self<jo::TextureManager>(this)->loading, key); }  // 0x0060b3e0 (0x0060b050)
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
HELPER16("TextureArray::remove_from_loading_array", 0x0060b3e0, &H::list_remove_c);
#undef HELPER16

}  // namespace en3d_helpers16_2
}  // namespace

// ---------------------------------------- from modules/game_b111_11.cpp (fix/uninit)
namespace {
namespace game_b111_11 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

JOAN_COUNTER(c_dds, "Dds::has_alpha_60b450");

struct H {
    // ------------------------------------------------------------------ 0x0060b450 (fix/uninit)
    u8 dds_has_alpha(const char* name) {
        c_dds.hit();
        char ext[4];  // [ebp-0x14]
        const u32 len = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(name)));  // strlen
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(ext)), joan::orig::arg::str(name + len - 3), joan::orig::arg::u32(i32{3}));  // strncpy
        ext[3] = 0;
        if (joan::crt::stricmp(joan::orig::arg::str(ext), str(d::s_TGA_9d9ad8)) == 0) return 1;  // _stricmp "TGA"
        if (joan::crt::stricmp(joan::orig::arg::str(ext), str(d::s_DDS_9d9adc)) != 0) return 0;  // "DDS"
        alignas(alignof(void*)) u8 file[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x1a0]; a File: 0x10e on x86, 0x112 on x64
        joan::orig::as_file(file)->construct();                                       // 0x004179c0
        joan::orig::as_file(file)->file_open(joan::orig::arg::str(name), i32{1}, static_cast<char>(i32{0}));  // 0x00586da0 open
        joan::orig::as_file(file)->file_seek(i32{4}, i32{-1});                           // 0x00587ea0 seek
        u32 hdr[0x1f];  // [ebp-0x90]; fix/uninit: zeroed (a short read leaves it uninitialised)
        std::memset(hdr, 0, sizeof hdr);
        joan::orig::as_file(file)->file_read(static_cast<void*>(hdr), joan::orig::arg::u32(i32{0x7c}));                         // 0x005873a0 read
        joan::orig::as_file(file)->file_close();                                       // 0x00587260 close
        const u32* const pf = &hdr[0x12];  // DDPIXELFORMAT at header + 0x48
        auto table = [pf](img::Addr t) {
            for (i32 i = 0; i < 8; ++i)
                if (pf[i] != global<u32>(t + 4u * static_cast<u32>(i))) return false;
            return true;
        };
        const bool four = (pf[1] & 4u) != 0;
        u8 r = 0;
        if (four && pf[2] == 0x31545844u)  // DXT1
            r = 0;
        else if (four && pf[2] == 0x33545844u)  // DXT3
            r = 1;
        else if (table(d::s_sp_9d9ae0))
            r = 1;
        else if (four && pf[2] == 0x32545844u)  // DXT2
            r = 1;
        else if (four && pf[2] == 0x34545844u)  // DXT4
            r = 1;
        else if (four && pf[2] == 0x35545844u)  // DXT5
            r = 1;
        else if (table(d::s_sp_9d9b00))
            r = 0;
        else if (table(d::s_sp_9d9b20))
            r = 1;
        else
            r = 0;  // 0x009d9b40, 0x009d9b60 and no match
        joan::orig::as_file(file)->destruct();  // 0x005872b0 dtor
        return r;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: zeroed DDS header instead of stack garbage after a short read (dead code).
FIX("TextureArray::is_transparent", 0x0060b450, &H::dds_has_alpha);
#undef FIX

}  // namespace game_b111_11
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
namespace {
namespace game_b102_11_3 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;


JOAN_COUNTER(c_sum_virtual, "Lists::sum_virtual");

struct H {
    // 0x0060ba00 (ret)
    i32 sum_virtual() {
        c_sum_virtual.hit();
        i32 sum = 0;
        for (i32 i = 0; i < jo::self<jo::TextureManager>(this)->named_count; ++i) {
            void* const e = jo::view<void**>(jo::self<jo::TextureManager>(this)->named)[i];
            sum = static_cast<i32>(static_cast<u32>(sum) + thiscall<u32>(reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(e)[1]), e));
        }
        for (i32 i = 0; i < jo::self<jo::TextureManager>(this)->anon_count; ++i) {
            void* const e = jo::view<void**>(jo::self<jo::TextureManager>(this)->anon)[i];
            sum = static_cast<i32>(static_cast<u32>(sum) + thiscall<u32>(reinterpret_cast<std::uintptr_t>(joan::call::vtable_of(e)[1]), e));
        }
        return sum;
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Lists::sum_virtual", 0x0060ba00, &H::sum_virtual);
#undef B11

}  // namespace game_b102_11_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::TexDir::resolve(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::en3d_helpers29_2::H::resolve>(this, a0)); }  // 0x6051c0
auto joan::orig::Texture::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01_2::H::texture_ctor>(this)); }  // 0x6058f0
auto joan::orig::Texture::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_04::H::dtor_6059a0>(this)); }  // 0x6059a0
auto joan::orig::Texture::attach(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> void { joan::bind::member<&::game_leaves81_2::H::tex_attach>(this, a0, a1, a2, a3, a4); }  // 0x605a40
auto joan::orig::Texture::set_name(const char* a0) -> void { joan::bind::member<&::game_leaves80a_2::H::set_name>(this, a0); }  // 0x605ad0
auto joan::orig::Texture::ready() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_ui8::H::texture_ready>(this)); }  // 0x605b70
auto joan::orig::Texture::is_loading() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_04::H::idle>(this)); }  // 0x605be0
auto joan::orig::Texture2D::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b83_03::H::texture2d_ctor>(this)); }  // 0x605ce0
auto joan::orig::Texture2D::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_07_3::H::dtor_605d80>(this)); }  // 0x605d80
auto joan::orig::Texture::create(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_07::H::create_texture>(this, a0, a1, a2, a3)); }  // 0x605e20
auto joan::orig::Texture2D::attach(void* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_helpers29_2::H::attach_2d>(this, a0, a1); }  // 0x605f00
auto joan::orig::TextureCube::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_06_2::H::ctor_606980>(this)); }  // 0x606980
auto joan::orig::TextureCube::destruct() -> void { joan::bind::member<&::game_b94_06::H::cube_dtor>(this); }  // 0x606a20
auto joan::orig::TextureCube::attach(void* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_helpers29_2::H::attach_cube>(this, a0, a1); }  // 0x606b90
auto joan::orig::TextureBump::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_06_2::H::ctor_606ef0>(this)); }  // 0x606ef0
auto joan::orig::TextureBump::destruct() -> void { joan::bind::member<&::game_b96_06::H::dtor_606f90>(this); }  // 0x606f90
auto joan::orig::TextureBump::attach(void* a0, std::uint32_t a1) -> void { joan::bind::member<&::en3d_helpers29_2::H::attach_bump>(this, a0, a1); }  // 0x607110
auto joan::orig::TextureBump::make_du_dv(void* a0, void* a1) -> void { joan::bind::member<&::en3d_helpers29_2::H::make_du_dv>(this, a0, a1); }  // 0x607950
auto joan::orig::RenderTexture::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04::H::render_texture_ctor>(this)); }  // 0x607f10
auto joan::orig::RenderTexture::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_06::H::texobj_dtor>(this)); }  // 0x607fb0
auto joan::orig::RenderTarget::create(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b84_01_2::H::rt_create>(this, a0, a1, a2)); }  // 0x608080
auto joan::orig::RenderTexture::create() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_08::H::create_608b80>(this)); }  // 0x608b80
auto joan::orig::TextureManager::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_11_2::H::clear_3c>(this)); }  // 0x608db0
auto joan::orig::TextureManager::clear() -> void { joan::bind::member<&::game_b92_05::H::texarray_clear>(this); }  // 0x609260
auto joan::orig::TextureManager::create_2d() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::create_2d>(this)); }  // 0x6094c0
auto joan::orig::TextureManager::new_render_target() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b84_01::H::new_render_target>(this)); }  // 0x6096d0
auto joan::orig::TextureManager::find_file(const char* a0, const char* a1, const char* a2) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_02::H::find_file_609780>(this, a0, a1, a2)); }  // 0x609780
auto joan::orig::Textures::find(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::textures_find>(this, a0)); }  // 0x60a4b0
auto joan::orig::TextureManager::use(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers6::H::texture_use>(this, a0)); }  // 0x60a500
auto joan::orig::TextureManager::set_name(void* a0, const char* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_leaves81::H::tex_set_name>(this, a0, a1)); }  // 0x60a670
auto joan::orig::TextureManager::flush_loaders() -> void { joan::bind::member<&::game_b97_06::H::flush>(this); }  // 0x60a770
auto joan::orig::TextureManager::purge(std::int32_t a0) -> void { joan::bind::member<&::game_leaves82a::H::tm_purge>(this, a0); }  // 0x60a870
auto joan::orig::TextureManager::trim_loaders(std::int32_t a0) -> void { joan::bind::member<&::game_leaves82a::H::tm_trim>(this, a0); }  // 0x60a990
// x64: needs facade TextureManager::insert_named / add_sorted / find_sorted taking std::uintptr_t (a Texture* and
// its name; texture_manager_methods.inc) - callers in tex_set_name still pass arg::as<std::uint32_t>.
auto joan::orig::TextureManager::insert_named(std::uintptr_t a0, std::uintptr_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82a::H::sa_insert>(this, a0, a1)); }  // 0x60ac20
auto joan::orig::TextureManager::add_sorted(std::uintptr_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82a::H::sa_add>(this, a0)); }  // 0x60ad40
auto joan::orig::TextureManager::find(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers6::H::texture_find>(this, a0)); }  // 0x60ae70
auto joan::orig::TextureManager::find_sorted(std::uintptr_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_leaves82a::H::sa_find>(this, a0)); }  // 0x60b000
auto joan::orig::TextureManager::remove_a(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers16::H::list_remove_a>(this, a0)); }  // 0x60b0d0
auto joan::orig::TextureManager::remove_b(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers16::H::list_remove_b>(this, a0)); }  // 0x60b140
auto joan::orig::TextureManager::insert_unused(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers16::H::texture_list_insert>(this, a0)); }  // 0x60b1b0
auto joan::orig::TextureManager::add(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers29::H::tm_add>(this, a0)); }  // 0x60b2e0
auto joan::orig::TextureManager::remove_c(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16_2::H::list_remove_c>(this, a0)); }  // 0x60b3e0
