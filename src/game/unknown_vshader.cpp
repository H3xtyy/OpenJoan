// game/unknown_vshader.cpp: functions whose original source file is unknown (formerly game/_unplaced/vshader.cpp;
// re/attribution.csv: between ww/en3d/vshader2.cpp and ww/en3d/vshader2.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../d3d8/d3d8_types.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
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

// The vertex-shader cache key (0x00618200 builds it on the stack, find 0x006182c0 / create 0x00618590 read it): the
// first three members of VsEntry. format: the vertex format descriptor of vertex_format 0x00600200 (a pointer).
struct VsKey {
    std::uint8_t* format;
    std::uint32_t flags;
    std::uint32_t extra;
};
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::img::obj;

using joan::call::cdecl_call;

using joan::hook::global;

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::orig::helpers::op_delete;

using Device = joan::d3d8::IDirect3DDevice8;

using joan::orig::helpers::game_delete;  // operator delete

using joan::orig::helpers::game_new;
void out_of_memory() { joan::orig::as_error_log(img::obj(d::g_err))->mem_error(); }  // 0x00586af0

using joan::call::import_call;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers17)
namespace {
namespace en3d_helpers16 {

struct H {
    // 0x00617d80: vertex-shader constant defaults (raw float bits as in the original).
    H* shader_constants_ctor() {
        joan::orig::as_vector4(&jo::self<jo::ShaderConstants>(this)->c[15])->construct();  // 0x005bc990 (+0x3c)
        static const std::uint32_t kValues[] = {
            0x3c621965, 0x3c3923a3, 0x3c03126f, 0x36e9340c, 0x3745f89d, 0x37ccae9a, 0x37a7c5ac, 0x37fba882,
            0x3827c5ac, 0x381b3073, 0x38388ca4, 0x3888509c, 0x00000000, 0x3d809d49, 0xbf7f7cee, 0x3f605bc0,
            0x3f553261, 0x3f3ccccd, 0x42c80000, 0x3e4ccccd, 0x3c23d70a, 0x3e99999a, 0x3f4ccccd, 0x3f800000,
        };
        for (std::int32_t i = 0; i < 24; ++i) jo::view<std::uint32_t>(jo::self<jo::ShaderConstants>(this)->c[i]) = kValues[i];
        return this;
    }
};

#define HELPER17(name, addr, fn) JOAN_REPLACE("en3d/helpers17", name, addr, fn)
HELPER17("ShaderConstants::ShaderConstants", 0x00617d80, &H::shader_constants_ctor);
#undef HELPER17

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (en3d/b82_17)
namespace {
namespace en3d_b82_17 {

struct H {
    // 0x00617ee0 (eax = this)
    void* light_params_ctor() {
        static const std::uint32_t kValues[24] = {
            0x3c621965, 0x3c3923a3, 0x3c03126f, 0x36e9340c, 0x3745f89d, 0x37ccae9a, 0x37a7c5ac, 0x37fba882,
            0x3827c5ac, 0x381b3073, 0x38388ca4, 0x3888509c, 0x00000000, 0x3d809d49, 0xbf7f7cee, 0x3f605bc0,
            0x3f553261, 0x3f3ccccd, 0x42c80000, 0x3e4ccccd, 0x3c23d70a, 0x3e99999a, 0x3f4ccccd, 0x3f800000,
        };
        for (std::int32_t i = 0; i < 24; ++i) reinterpret_cast<std::uint32_t*>(jo::self<jo::LightParams>(this))[i] = kValues[i];  // the 24 floats as raw bits
        return this;
    }
};

#define B82_17E(name, addr, fn) JOAN_REPLACE("en3d/b82_17", name, addr, fn)
B82_17E("LightParams::LightParams", 0x00617ee0, &H::light_params_ctor);
#undef B82_17E

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

constexpr img::Addr kSysEn(d::g_sys3d);

struct H {
    // 0x00618030: vertex-shader cache entry ctor.
    H* vs_entry_construct() {
        joan::orig::as_shader_constants(&jo::self<jo::VsEntry>(this)->constants)->construct();  // 0x00617d80
        jo::self<jo::VsEntry>(this)->key_extra = 0;
        jo::self<jo::VsEntry>(this)->constant_table = nullptr;
        jo::self<jo::VsEntry>(this)->u32_10 = 0;
        jo::view<std::uint32_t>(jo::self<jo::VsEntry>(this)->spot_lights) = 0;
        return this;
    }
    // The same original function 0x00618030 as seen by VsCache: the original builds the cache with the entry's ctor, the
    // x86 offsets of the two classes coincide (+8 entries, +0xc capacity, +0x10 count, +0x1c linker, +0x20 constants).
    // On x64 they do not: the entry's offsets left `entries` uninitialised and released garbage at exit.
    H* vs_cache_construct() {
        joan::orig::as_shader_constants(jo::self<jo::VsCache>(this)->gap_20)->construct();  // 0x00617d80
        jo::self<jo::VsCache>(this)->entries = nullptr;
        jo::self<jo::VsCache>(this)->i32_c = 0;
        jo::self<jo::VsCache>(this)->i32_10 = 0;
        jo::self<jo::VsCache>(this)->ptr_1c = nullptr;
        return this;
    }
    // 0x00618950: does (vertex type, ..., flags) need a vertex shader? (flags & 0x4d, or type 7)
    bool vs_needed(std::int32_t vertex_type, std::uint32_t, std::uint32_t, std::uint32_t flags) {
        if ((flags & 0x4d) != 0) return true;
        return static_cast<std::int8_t>(vertex_type) == 7;
    }
    // 0x00618200: vertex-shader cache lookup / create for (vertex type, blend, texture, flags):
    // key {FVF, flags, SysEn current texture}; remembered in +0x14.
    void* vs_lookup(std::int32_t vertex_type, std::uint32_t blend, std::uint32_t texture, std::uint32_t flags) {
        std::uint8_t* const format = static_cast<std::uint8_t*>(joan::orig::vertex_format(static_cast<char>(joan::orig::arg::i32(static_cast<std::int8_t>(vertex_type)))));  // 0x00600200
        if (!joan::orig::arg::as<bool>(joan::orig::as_vs_cache(this)->needed(joan::orig::arg::i32(static_cast<std::int8_t>(vertex_type)), joan::orig::arg::u32(blend), joan::orig::arg::u32(texture), joan::orig::arg::u32(flags)))) return nullptr;
        VsKey key;
        key.format = format;
        key.flags = flags;
        key.extra = joan::orig::arg::as<std::uint32_t>(joan::orig::as_sys_en(img::obj(kSysEn))->overlay_stage());  // 0x00601470
        void* entry = joan::orig::arg::as<void*>(joan::orig::as_vs_cache(this)->find(&key));   // 0x006182c0 find
        if (entry == nullptr) entry = joan::orig::arg::as<void*>(joan::orig::as_vs_cache(this)->create(&key));  // 0x00618590 create
        jo::view<void*>(jo::self<jo::VsCache>(this)->current) = entry;
        return entry;
    }
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("VsEntry::VsEntry", 0x00618030, &H::vs_entry_construct);
HELPER6("VsCache::needed", 0x00618950, &H::vs_needed);
HELPER6("VsCache::lookup", 0x00618200, &H::vs_lookup);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

// COM method: vtable slot at byte offset `off`, stdcall with the interface as first argument.
template <class R = std::int32_t, class... A>
R com_call(void* iface, std::uint32_t off, A... args) {
    const std::uintptr_t* vt = *reinterpret_cast<std::uintptr_t* const*>(iface);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vt[joan::call::x86_vslot(off)])(iface, args...);
}
JOAN_COUNTER(c_release_all, "Effects::release_all");

struct H {
    // ------------------------------------------------------------------ 0x00618090
    void release_all() {
        c_release_all.hit();
        if (jo::view<void*>(jo::self<jo::VsCache>(this)->entries) != nullptr) {
            if (jo::self<jo::VsCache>(this)->i32_10 > 0) {
                for (i32 i = 0; i < jo::self<jo::VsCache>(this)->i32_10; ++i) {
                    const u32 val = jo::self<jo::VsCache>(this)->entries[i].entry->u32_10;
                    void* const device = global<void*>(d::g_game_device);
                    void* const iface = jo::self<jo::VsCache>(this)->entries[i].entry->constant_table;
                    com_call<i32>(iface, 0x34, device, val);
                    void* const iface2 = jo::self<jo::VsCache>(this)->entries[i].entry->constant_table;
                    com_call<u32>(iface2, 0x8);  // Release
                    op_delete(jo::view<void*>(jo::self<jo::VsCache>(this)->entries[i].entry));
                    jo::self<jo::VsCache>(this)->entries[i].entry = nullptr;
                }
            }
            op_delete(jo::view<void*>(jo::self<jo::VsCache>(this)->entries));
            jo::self<jo::VsCache>(this)->entries = nullptr;
        }
        // The original then FreeLibrary's nvlinker.dll (+0x1c, loaded by VsCache::create_linker). Since port round P2
        // that DLL is never loaded (create_linker below always uses our linker): +0x1c stays 0 from the constructor
        // (0x00618030 clears it), so the call is gone.
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Effects::release_all", 0x00618090, &H::release_all);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers8.cpp (en3d/helpers8)
namespace {
namespace en3d_helpers8 {

Device* game_device() { return global<Device*>(d::g_game_device); }

struct H {
    // --- Vertex-shader cache (this = { ..., +0x8 entries, +0x10 count }) -----------------------
    // 0x006182c0: find an entry for key {format*, flags, texture} with at least as many lights
    // of each kind as are enabled now (unless the format has no normals (+3) or flags & 0x40):
    // +0x14 point, +0x18 directional, +0x1c spot. Type-7 formats clamp the counts to 1 / 1 / 0;
    // flags & 1 rewrites the format's type byte to 5 (as the original).
    void* vs_find(VsKey* key) {
        if (jo::self<jo::VsCache>(this)->i32_10 <= 0) return nullptr;
        std::int32_t point = 0, directional = 0, spot = 0;
        std::uint8_t* const format = key->format;
        if (static_cast<std::int8_t>(format[3]) == 0 && (key->flags & 0x40) == 0) {
            for (std::uint32_t i = 0; i < global<std::uint32_t>(d::g_a5259f); ++i) {
                joan::d3d8::BOOL enabled = 0;
                if (game_device()->GetLightEnable(i, &enabled) != 0 || enabled == 0) continue;
                std::uint8_t light[0x68];
                game_device()->GetLight(i, light);
                switch (jo::object<joan::d3d8::Light>(light)->type) {
                    case 1: ++point; break;
                    case 2: ++spot; break;
                    case 3: ++directional; break;
                    default: break;
                }
            }
            if (static_cast<std::int8_t>(format[0]) == 7) {
                if (point > 1) point = 1;
                if (directional > 1) directional = 1;
                if (spot > 0) spot = 0;
            }
            if ((key->flags & 1) != 0) format[0] = 5;
        }
        for (std::int32_t i = 0; i < jo::self<jo::VsCache>(this)->i32_10; ++i) {
            std::uint32_t* const e = jo::view<std::uint32_t**>(jo::self<jo::VsCache>(this)->entries)[i];
            jo::VsEntry* const v = jo::as_vs_entry(e);
            if (v->key_format != key->format || v->key_flags != key->flags || v->key_extra != key->extra)
                continue;
            if (v->dir_lights < directional) continue;
            if (v->point_lights < point) continue;
            if (v->spot_lights < spot) continue;
            return e;
        }
        return nullptr;
    }
};

#define HELPER8(name, addr, fn) JOAN_REPLACE("en3d/helpers8", name, addr, fn)
HELPER8("VsCache::find", 0x006182c0, &H::vs_find);
#undef HELPER8

}  // namespace en3d_helpers8
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers22)
namespace {
namespace en3d_helpers22 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

void* game_device() { return global<void*>(d::g_game_device); }

// Interface methods by vtable index.
constexpr std::int32_t kSetVertexShaderConstant = 79;

JOAN_COUNTER(c_vs_constant, "VsConstants::set");
JOAN_COUNTER(c_linker, "VsCache::linker");

struct H {
    // 0x0061c080 VsConstants::set(entry, index, value): store the constant (0xa19280 + i*32),
    // look its register up by name (0xa19274 + i*32) in the entry's (or current +0x14) linker
    // shader and upload it.
    std::int32_t vs_constant_set(std::uint8_t* entry, std::int32_t index, const void* value) {
        c_vs_constant.hit();
        std::uint8_t* const slot = &img::global<std::uint8_t>(d::g_a19280) + (index << 5);
        std::memmove(slot, value, 16);
        if (entry == nullptr) entry = jo::view<std::uint8_t*>(jo::self<jo::VsCache>(this)->current);
        if (entry == nullptr) return 0;
        std::uint32_t id = 0;
        // the 0xa19274 constant table (0x20-byte records): the name pointer at +0x0, read as a pointer as the other
        // readers of the table do (GetConstantByName takes a const char*)
        com_call(jo::as_vs_entry(entry)->constant_table, 5, global<const char*>(d::g_a19274 + (index << 5)), &id);
        std::int32_t reg = 0;
        const std::int32_t hr = com_call(jo::as_vs_entry(entry)->constant_table, 16, id, 0u, &reg);
        if (reg == -1) return hr;
        return com_call(game_device(), kSetVertexShaderConstant, static_cast<std::uint32_t>(reg), static_cast<const void*>(slot), 1u);
    }
    // 0x00618990 VsCache::create_linker: nvlinker.dll (+0x1c) CreateLinker(100, 1) and the
    // shader fragment files. Our own linker (render/nvlink_com.cpp) replaces nvlinker.dll. Port round P2: always -
    // the nvlinker.dll fallback ([Hooks] Disable=render/nvlink: LoadLibraryA / GetProcAddress "CreateLinker") is
    // deleted (a closed-source DLL; +0x1c stays 0).
    void* vs_create_linker() {
        c_linker.hit();
        void* const linker = joan::hook::nvlink::create_linker(1);
        joan::hook::nvlink_log::wrap_linker(linker);  // ShaderLog=1 diagnostics (NVLINK.md), no-op otherwise
        static const img::Addr kFiles[] = {d::s_SHADER_default_shader_nvo, d::s_SHADER_transform_nvo, d::s_SHADER_lighting_nvo, d::s_SHADER_fog_nvo, d::s_SHADER_scatter_nvo, d::s_SHADER_hardware_skin_nvo, d::s_SHADER_clip_plane_nvo};
        for (img::Addr file : kFiles) joan::orig::as_vs_cache(this)->load_fragment(joan::orig::arg::mptr(linker), joan::orig::arg::str(str(file)));  // 0x00618ab0
        return linker;
    }
    // 0x00618ab0 VsCache::load_fragment(linker, file): whole file into memory, linker method 3.
    std::int32_t vs_load_fragment(void* linker, const char* file) {
        void* const fp = joan::orig::arg::as<void*>(joan::orig::libc::fopen(joan::orig::arg::str(file), joan::orig::arg::str(&img::global<const char>(d::s_rb_9d90dc))));  // fopen
        joan::orig::libc::fseek(joan::orig::arg::mptr(fp), 0, 2);                                                 // fseek end
        const std::int32_t size = joan::orig::arg::as<std::int32_t>(joan::orig::libc::ftell(joan::orig::arg::mptr(fp)));                             // ftell
        joan::orig::libc::fseek(joan::orig::arg::mptr(fp), 0, 0);
        void* const data = game_new(static_cast<std::uint32_t>(size));
        if (data == nullptr) out_of_memory();
        joan::orig::libc::fread(joan::orig::arg::mptr(data), 1u, joan::orig::arg::u32(size), joan::orig::arg::mptr(fp));  // fread
        joan::orig::libc::fclose(joan::orig::arg::mptr(fp));                  // fclose
        const std::int32_t hr = com_call(linker, 3, data, size);
        game_delete(data);
        return hr;
    }
};

#define HELPER22(name, addr, fn) JOAN_REPLACE("en3d/helpers22", name, addr, fn)
HELPER22("VsConstants::set", 0x0061c080, &H::vs_constant_set);
HELPER22("VsCache::create_linker", 0x00618990, &H::vs_create_linker);
HELPER22("VsCache::load_fragment", 0x00618ab0, &H::vs_load_fragment);
#undef HELPER22

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/game_b115_13.cpp (fix/uninit)
namespace {
namespace game_b115_13 {

using i8 = std::int8_t;

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {  // COM method by vtable index
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}
Device* game_device() { return global<Device*>(d::g_game_device); }

constexpr u32 kNoConstant = 0;       // 0x0061ae30 ids never looked up

JOAN_COUNTER(c_lights, "VsCache::set_light_constants");

// D3DLIGHT8 offsets (0x68 bytes at [ebp-0xe0]).
constexpr i32 kDiffuse = 0x04, kSpecular = 0x14, kAmbient = 0x24, kPosition = 0x34, kDirection = 0x40,
              kFalloff = 0x50;

struct H {
    // ---------------------------------------------------------------------------------------
    // 0x0061ae30 VsCache::set_light_constants(entry)
    void set_light_constants(u8* entry) {
        if (static_cast<i8>(jo::view<u8*>(jo::as_vs_entry(entry)->key_format)[3]) != 0 || (jo::as_vs_entry(entry)->key_flags & 0x40) != 0) return;
        c_lights.hit();
        i32 points = 0, directionals = 0, spots = 0;  // [ebp-0x8] / [ebp-0xc] / [ebp-0x10]
        auto linker = [entry]() { return jo::as_vs_entry(entry)->constant_table; };
        i32 slot = -1;  // [ebp-0x5c]
        auto find_slot = [&](u32 id, u32 instance) { com_call(linker(), 16, id, instance, &slot); };  // GetConstantSlot
        auto upload = [&](const void* data) { game_device()->SetVertexShaderConstant(static_cast<joan::d3d8::DWORD>(slot), data, 1); };
        auto upload_found = [&](const void* data) {
            if (slot != -1) upload(data);
        };

        joan::d3d8::DWORD ambient_colour = 0;  // [ebp-0x60]
        game_device()->GetRenderState(0x8b, &ambient_colour);
        float ambient[4] = {0.0f, 0.0f, 0.0f, 0.0f};  // [ebp-0x70]
        joan::orig::as_colour(&ambient_colour)->to_float_5e9260(joan::orig::arg::mptr(static_cast<void*>(ambient)));  // 0x005e9260
        game_device()->SetVertexShaderConstant(9, ambient, 1);
        joan::d3d8::DWORD lighting = 0;  // [ebp-0x74]
        game_device()->GetRenderState(0x89, &lighting);

        // Constant ids [ebp-0x14] .. [ebp-0x50]. fix/uninit: the three *_ambient ids are never
        // looked up, and none of them when the light pass is skipped -> kNoConstant (header).
        u32 point_position = kNoConstant, point_attenuation = kNoConstant, point_ambient = kNoConstant,
            point_diffuse = kNoConstant, point_specular = kNoConstant, dir_direction = kNoConstant,
            dir_ambient = kNoConstant, dir_diffuse = kNoConstant, dir_specular = kNoConstant, spot_position = kNoConstant,
            spot_direction = kNoConstant, spot_attenuation = kNoConstant, spot_ambient = kNoConstant,
            spot_diffuse = kNoConstant, spot_specular = kNoConstant, spot_falloff = kNoConstant;

        bool lights = lighting != 0;
        if (!lights && (jo::as_vs_entry(entry)->key_flags & 1) != 0) {
            jo::view<u8*>(jo::as_vs_entry(entry)->key_format)[0] = 5;
            lights = true;
        }
        if (lights) {
            auto id_of = [&](img::Addr name, u32* id) {  // GetConstantID
                com_call(linker(), 5, global<const char*>(name), id);
            };
            id_of(d::g_a192b4, &point_position);
            id_of(d::g_a192d4, &point_attenuation);
            id_of(d::g_a19314, &point_diffuse);
            id_of(d::strs_a19334, &point_specular);
            id_of(d::g_a19354, &dir_direction);
            id_of(d::g_a19394, &dir_diffuse);
            id_of(d::strs_a193b4, &dir_specular);
            id_of(d::g_a193d4, &spot_position);
            id_of(d::g_a193f4, &spot_direction);
            id_of(d::strs_a19414, &spot_attenuation);
            id_of(d::g_a19454, &spot_diffuse);
            id_of(d::g_a19474, &spot_specular);
            id_of(d::strs_a19494, &spot_falloff);

            u8 light[0x68];  // [ebp-0xe0] D3DLIGHT8
            std::memset(light, 0, sizeof(light));
            u32 dot3[4] = {0, 0, 0, 0};  // [ebp-0xf4]; dot3[3] is the BOOL [ebp-0xe8]
            u32 dot3_id = 0;              // [ebp-0x54]
            for (u32 i = 0; i < global<u32>(d::g_a5259f); ++i) {
                joan::d3d8::BOOL enabled = 0;
                const auto hr = game_device()->GetLightEnable(i, &enabled);
                std::memcpy(&dot3[3], &enabled, 4);
                if (hr != 0 || enabled == 0) continue;
                game_device()->GetLight(i, light);
                const u32 type = jo::object<joan::d3d8::Light>(light)->type;
                if (type == 1) {  // point
                    find_slot(point_position, static_cast<u32>(points));
                    upload(light + kPosition);
                    find_slot(point_attenuation, static_cast<u32>(points));
                    u8 vec[16];  // [ebp-0x178]
                    upload(joan::orig::arg::as<const void*>(joan::orig::as_vector4(static_cast<void*>(vec))->construct_xyzw(joan::orig::arg::u32(jo::view<u32>(jo::object<joan::d3d8::Light>(light)->attenuation0)), joan::orig::arg::u32(jo::view<u32>(jo::object<joan::d3d8::Light>(light)->attenuation1)), joan::orig::arg::u32(jo::view<u32>(jo::object<joan::d3d8::Light>(light)->attenuation2)), joan::orig::arg::u32(jo::view<u32>(jo::object<joan::d3d8::Light>(light)->range)))));  // 0x005bd220
                    find_slot(point_ambient, static_cast<u32>(points));
                    upload_found(light + kAmbient);
                    find_slot(point_diffuse, static_cast<u32>(points));
                    upload(light + kDiffuse);
                    find_slot(point_specular, static_cast<u32>(points));
                    upload_found(light + kSpecular);
                    ++points;
                } else if (type == 2) {  // spot
                    find_slot(spot_position, static_cast<u32>(spots));
                    upload(light + kPosition);
                    find_slot(spot_direction, static_cast<u32>(spots));
                    upload(light + kDirection);
                    find_slot(spot_attenuation, static_cast<u32>(spots));
                    u8 vec[16];  // [ebp-0x1a0]
                    upload(joan::orig::arg::as<const void*>(joan::orig::as_vector4(static_cast<void*>(vec))->construct_xyzw(joan::orig::arg::u32(jo::view<u32>(jo::object<joan::d3d8::Light>(light)->attenuation0)), joan::orig::arg::u32(jo::view<u32>(jo::object<joan::d3d8::Light>(light)->attenuation1)), joan::orig::arg::u32(jo::view<u32>(jo::object<joan::d3d8::Light>(light)->attenuation2)), joan::orig::arg::u32(jo::view<u32>(jo::object<joan::d3d8::Light>(light)->range)))));  // 0x005bd220
                    find_slot(spot_ambient, static_cast<u32>(points));  // instance = point count (original)
                    upload_found(light + kAmbient);
                    find_slot(spot_diffuse, static_cast<u32>(spots));
                    upload(light + kDiffuse);
                    find_slot(spot_specular, static_cast<u32>(spots));
                    upload_found(light + kSpecular);
                    float falloff[4];  // [ebp-0x104]
                    falloff[0] = joan::orig::arg::as<float>(joan::orig::libc::cos_f(joan::orig::arg::f32(jo::object<joan::d3d8::Light>(light)->theta / 2.0f)));  // 0x00425670 cos
                    falloff[1] = joan::orig::arg::as<float>(joan::orig::libc::cos_f(joan::orig::arg::f32(jo::object<joan::d3d8::Light>(light)->phi / 2.0f)));
                    std::memcpy(&falloff[2], light + kFalloff, 4);
                    falloff[3] = 1.0f / (falloff[0] - falloff[1]);
                    find_slot(spot_falloff, static_cast<u32>(spots));
                    upload(falloff);
                    ++spots;
                } else if (type == 3) {  // directional
                    if (jo::self<jo::VsCache>(this)->u32_18 == i && (jo::as_vs_entry(entry)->key_flags & 1) != 0) {
                        jo::view<u8*>(jo::as_vs_entry(entry)->key_format)[0] = 5;
                        u8 negated[12];  // [ebp-0x190]
                        const void* const v = joan::orig::arg::as<const void*>(joan::orig::vec_negate(static_cast<void*>(negated), static_cast<void*>(light + kDirection)));  // 0x0060cbc0
                        std::memcpy(dot3, v, 12);
                        com_call(linker(), 5, global<const char*>(d::g_a194b4), &dot3_id);  // c_dot3_light_direction
                        find_slot(dot3_id, 0);
                        upload(dot3);
                    }
                    find_slot(dir_direction, static_cast<u32>(directionals));
                    upload(light + kDirection);
                    find_slot(dir_ambient, static_cast<u32>(directionals));
                    upload_found(light + kAmbient);
                    find_slot(dir_diffuse, static_cast<u32>(directionals));
                    upload(light + kDiffuse);
                    find_slot(dir_specular, static_cast<u32>(directionals));
                    upload_found(light + kSpecular);
                    ++directionals;
                }
            }
        }

        // Shader light slots without an enabled light.
        for (i32 i = points; i < jo::as_vs_entry(entry)->point_lights; ++i) {
            const u32 zero[4] = {0, 0, 0, 0};                // [ebp-0x118]
            const u32 one[4] = {0x3f800000u, 0, 0, 0};  // [ebp-0x128]
            find_slot(point_position, static_cast<u32>(i));
            upload(zero);
            find_slot(point_attenuation, static_cast<u32>(i));
            upload(one);
            find_slot(point_ambient, static_cast<u32>(i));
            upload_found(zero);
            find_slot(point_diffuse, static_cast<u32>(i));
            upload(zero);
            find_slot(point_specular, static_cast<u32>(i));
            upload_found(zero);
        }
        for (i32 i = directionals; i < jo::as_vs_entry(entry)->dir_lights; ++i) {
            const u32 zero[4] = {0, 0, 0, 0};                // [ebp-0x138]
            const u32 one[4] = {0x3f800000u, 0, 0, 0};  // [ebp-0x148]
            find_slot(dir_direction, static_cast<u32>(i));
            upload(one);
            find_slot(dir_ambient, static_cast<u32>(i));
            upload_found(zero);
            find_slot(dir_diffuse, static_cast<u32>(i));
            upload(zero);
            find_slot(dir_specular, static_cast<u32>(i));
            upload_found(zero);
        }
        for (i32 i = spots; i < jo::as_vs_entry(entry)->spot_lights; ++i) {
            const u32 zero[4] = {0, 0, 0, 0};                // [ebp-0x158]
            const u32 one[4] = {0x3f800000u, 0, 0, 0};  // [ebp-0x168]
            find_slot(spot_position, static_cast<u32>(i));
            upload(zero);
            find_slot(spot_direction, static_cast<u32>(i));
            upload(one);
            find_slot(spot_attenuation, static_cast<u32>(i));
            upload(one);
            find_slot(spot_ambient, static_cast<u32>(i));
            upload_found(zero);
            find_slot(spot_diffuse, static_cast<u32>(i));
            upload(zero);
            find_slot(spot_specular, static_cast<u32>(i));
            upload(zero);
            find_slot(spot_falloff, static_cast<u32>(i));
            upload_found(zero);
        }
    }
};

// fix/uninit: the never-looked-up constant ids (*_ambient always; all 13 when LIGHTING is off without
// DOT3) = 0, an id no constant has -> GetConstantSlot gives -1, guarded uploads skipped, unguarded ones
// fail on register 0xffffffff; see the header.
JOAN_REPLACE("fix/uninit", "VsCache::set_light_constants", 0x0061ae30, &H::set_light_constants);

}  // namespace game_b115_13
}  // namespace

// ---------------------------------------- from modules/game_b111_10.cpp (fix/uninit)
namespace {
namespace game_b111_10 {

// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = i32, class... A>
R com_call(void* o, std::int32_t offset, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(o)[joan::call::x86_vslot(offset)])(o, args...);
}
struct H {
    // ------------------------------------------------------------------ 0x0061c160
    // fix/uninit (dead code): for constant slots 0 (c_fog_constant) and 18 (c_dot3_light_direction)
    // asks the linker ctx+0xc for the constant id (GetConstantID, +0x14) and register
    // (GetConstantSlot, +0x40) without checking either HRESULT; on failure the id [ebp-0xc] /
    // register [ebp-0x10] are uninitialised and the register decides the SetVertexShaderConstant
    // upload. They start at 0 / -1 (-1 = the linker's own "no slot" answer, skipped by the code);
    // one pair for the whole loop like the original.
    void upload_vs_constants(u8* ctx) {
        u32 id = 0;
        i32 reg = -1;
        for (i32 i = 0; i < 0x20; ++i) {
            if (i != 0 && i != 0x12) continue;
            com_call(jo::as_vs_entry(ctx)->constant_table, 0x14, global<const char*>(d::g_a19274 + static_cast<u32>(i) * 0x20), &id);
            com_call(jo::as_vs_entry(ctx)->constant_table, 0x40, id, u32{0}, &reg);
            if (reg != -1)
                com_call(global<void*>(d::g_game_device), 0x13c, reg, img::addr(d::g_a19280 + static_cast<u32>(i) * 0x20), i32{1});
        }
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// [ebp-0xc] / [ebp-0x10] constant id / register unwritten on linker failure -> 0 / -1 (no upload).
FIX("VsConstants::upload_61c160", 0x0061c160, &H::upload_vs_constants);
#undef FIX

}  // namespace game_b111_10
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

// Every helper shares one class so `this` is just the object's address.
struct H {
    // 0x0061c250 VsCache::set_tint_source(id): the light id / index the directional branch of the light upload compares
    // with its loop counter (an integer on every call site; the Tint view read it as a pointer - r8 round)
    H* tint_set(std::int32_t source) {
        jo::self<jo::VsCache>(this)->u32_18 = static_cast<u32>(source);
        return this;
    }
    // 0x0061d200 VsCache::set_constants: gap_20 + slider_factors (+0x20..+0x80, no pointer in between, same on x64)
    void copy_60(const void* src) {
        static_assert(offsetof(jo::VsCache, slider_factors) == offsetof(jo::VsCache, gap_20) + 0x30, "gap_20, slider_factors adjacent");
        std::memmove(jo::self<jo::VsCache>(this)->gap_20, src, 0x60);
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Tint::set", 0x0061c250, &H::tint_set);
HELPER("Object::copy_60", 0x0061d200, &H::copy_60);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

// COM method: vtable slot at byte offset `off`, stdcall with the interface as first argument.
template <class R = std::int32_t, class... A>
R com_call(void* iface, std::uint32_t off, A... args) {
    const std::uintptr_t* vt = *reinterpret_cast<std::uintptr_t* const*>(iface);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vt[joan::call::x86_vslot(off)])(iface, args...);
}
JOAN_COUNTER(c_release_61c280, "Effects::release_61c280");

struct H {
    // ------------------------------------------------------------------ 0x0061c280
    H* release_61c280() {
        c_release_61c280.hit();
        if (jo::view<void*>(jo::self<jo::VsCache>(this)->entries) != nullptr && jo::self<jo::VsCache>(this)->i32_10 > 0) {
            for (i32 i = 0; i < jo::self<jo::VsCache>(this)->i32_10; ++i) {
                u8* e = jo::view<u8*>(jo::self<jo::VsCache>(this)->entries[i].entry);
                const u32 arg = jo::self<jo::VsEntry>(e)->u32_10;
                void* const dev = global<void*>(d::g_game_device);
                void* iface = jo::self<jo::VsCache>(this)->entries[i].entry->constant_table;
                com_call<u32>(iface, 0x34, dev, arg);
                iface = jo::self<jo::VsCache>(this)->entries[i].entry->constant_table;
                com_call<u32>(iface, 8);  // Release
                e = jo::view<u8*>(jo::self<jo::VsCache>(this)->entries[i].entry);
                op_delete(e);
            }
        }
        jo::view<u32>(jo::self<jo::VsCache>(this)->i32_10) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Effects::release_61c280", 0x0061c280, &H::release_61c280);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ShaderConstants::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::shader_constants_ctor>(this)); }  // 0x617d80
auto joan::orig::LightParams::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::light_params_ctor>(this)); }  // 0x617ee0
auto joan::orig::VsCache::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers6::H::vs_cache_construct>(this)); }  // 0x618030
auto joan::orig::VsCache::release_all() -> void { joan::bind::member<&::game_b98_06::H::release_all>(this); }  // 0x618090
auto joan::orig::VsCache::lookup(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers6::H::vs_lookup>(this, a0, a1, a2, a3)); }  // 0x618200
auto joan::orig::VsCache::find(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers8::H::vs_find>(this, a0)); }  // 0x6182c0
auto joan::orig::VsCache::needed(std::int32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3) -> bool { return joan::bind::conv<bool>(joan::bind::member<&::en3d_helpers6::H::vs_needed>(this, a0, a1, a2, a3)); }  // 0x618950
auto joan::orig::VsCache::create_linker() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers22::H::vs_create_linker>(this)); }  // 0x618990
auto joan::orig::VsCache::load_fragment(void* a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers22::H::vs_load_fragment>(this, a0, a1)); }  // 0x618ab0
auto joan::orig::VsCache::set_light_constants(void* a0) -> void { joan::bind::member<&::game_b115_13::H::set_light_constants>(this, a0); }  // 0x61ae30
auto joan::orig::VsCache::set_constant(void* a0, std::int32_t a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers22::H::vs_constant_set>(this, a0, a1, a2)); }  // 0x61c080
auto joan::orig::VsCache::set_tint_source(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::tint_set>(this, a0)); }  // 0x61c250
auto joan::orig::VsCache::release_shaders() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07::H::release_61c280>(this)); }  // 0x61c280
auto joan::orig::VsCache::set_constants(void* a0) -> void { joan::bind::member<&::en3d_helpers::H::copy_60>(this, a0); }  // 0x61d200
