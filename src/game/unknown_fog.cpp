// game/unknown_fog.cpp: functions whose original source file is unknown (formerly game/_unplaced/fog.cpp;
// re/attribution.csv: between ww/en3d/bonetree.cpp and ww/en3d/geodef.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

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
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using joan::hook::global;

using joan::orig::helpers::at;

using joan::orig::helpers::differs;   // x87 C3 clear

}  // namespace

// ---------------------------------------- from modules/game_b105_16.cpp (game/b105_16)
namespace {
namespace game_b105_16 {

// ---- free functions ----
void JOAN_CDECL empty_cdecl() {}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_16", name, addr, fn)
B("empty_5bfd60", 0x005bfd60, &empty_cdecl);
#undef B

}  // namespace game_b105_16
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x005bfd80
    void* zero_50() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::Fog)));  // 0x50 on x86 (the TexCache members hold pointers)
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Obj50::zero", 0x005bfd80, &H::zero_50);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

struct H {
    // 0x005bfdb0
    void dtor_5bfdb0() { joan::orig::as_fog(this)->release_textures(); }  // 0x005bfed0
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj5bf::~Obj5bf", 0x005bfdb0, &H::dtor_5bfdb0);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

// COM method: vtable slot at byte offset `off`, stdcall with the interface as first argument.
template <class R = std::int32_t, class... A>
R com_call(void* iface, std::uint32_t off, A... args) {
    const std::uintptr_t* vt = *reinterpret_cast<std::uintptr_t* const*>(iface);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vt[joan::call::x86_vslot(off)])(iface, args...);
}

JOAN_COUNTER(c_texcache_init, "TexCache::init");

struct H {
    // 0x005bfde0
    std::int32_t texcache_init() {
        c_texcache_init.hit();
        jo::self<jo::Fog>(this)->range_tex.texture = nullptr;
        jo::self<jo::Fog>(this)->range_tex.filled = 0;
        jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->range_tex.width) = 0x80;
        jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->range_tex.height) = 1;
        jo::self<jo::Fog>(this)->height_tex.texture = nullptr;
        jo::self<jo::Fog>(this)->height_tex.filled = 0;
        jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->height_tex.width) = 0x100;
        jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->height_tex.height) = 0x100;
        joan::orig::as_fog(this)->range_texture(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->range_tex.width)), joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->range_tex.height)));  // 0x005c0cf0
        joan::orig::as_fog(this)->height_texture(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->height_tex.width)), joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->height_tex.height)));  // 0x005c0da0
        const std::int32_t hr = com_call<std::int32_t>(global<void*>(d::g_game_device), 0xc8, 0x1cu, 0u);  // SetRenderState
        jo::self<jo::Fog>(this)->active = 0;
        jo::self<jo::Fog>(this)->dirty = 1;
        return hr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("TexCache::init", 0x005bfde0, &H::texcache_init);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

std::uint32_t u32(const void* p) { return joan::call::register_low_bits(p); }  // eax residue of Device::release (0x005bfed0): the result is not used as an address

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x005bfed0: COM-style Release (vtable slot 2, stdcall) of +0x30 then +0.
    std::uint32_t release_all() {
        using Release = std::uint32_t(JOAN_STDCALL*)(void*);
        std::uint32_t eax = u32(self());
        if (jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture) != nullptr) {
            void* const o = jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture);
            eax = reinterpret_cast<Release>(static_cast<void**>(jo::self<jo::IDirect3DTexture8>(o)->vtable)[2])(o);
            jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture) != nullptr) {
            void* const o = jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture);
            reinterpret_cast<Release>(static_cast<void**>(jo::self<jo::IDirect3DTexture8>(o)->vtable)[2])(o);
            jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture) = nullptr;
            eax = u32(self());
        }
        return eax;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Device::release", 0x005bfed0, &H::release_all);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers25)
namespace {
namespace en3d_helpers25 {

struct H {
    std::int32_t fog_texture_stub(void*, std::int32_t, std::int32_t) { return 0; }  // 0x005bff40
};

#define HELPER25(name, addr, fn) JOAN_REPLACE("en3d/helpers25", name, addr, fn)
HELPER25("Fog::create_texture", 0x005bff40, &H::fog_texture_stub);
#undef HELPER25

}  // namespace en3d_helpers25
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers22)
namespace {
namespace en3d_helpers22 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

std::int32_t ftol(float v) { return joan::hook::x87::ftol(static_cast<double>(v)); }

constexpr std::int32_t kTextureLockRect = 16, kTextureUnlockRect = 17;

JOAN_COUNTER(c_fog_kinds, "Fog::kinds");
JOAN_COUNTER(c_fog_tex, "fog texture build");

struct H {
    // --- en3d/helpers22 ---------------------------------------------------------------------
    // 0x005c09f0 Fog::kinds: 2 = vertex / table fog caps, 4 | 8 = texture fog (VS version >= 1).
    std::uint32_t fog_kinds() {
        c_fog_kinds.hit();
        std::uint32_t kinds = 0;
        if ((global<std::uint32_t>(d::g_a52494) & 1) != 0) kinds |= 2;
        if (((global<std::uint32_t>(d::g_a52534) >> 8) & 0xff) >= 1 && (global<std::uint32_t>(d::g_a524ac) & 1) != 0) kinds |= 0xc;
        return kinds;
    }
    // 0x005c0cf0 / 0x005c0da0: (re)create the range / height fog texture (+0x30 / +0) of size
    // (w, h); 0x005bff40 always fails in this build, so the texture stays null.
    std::int32_t fog_range_texture(std::int32_t w, std::int32_t h) {
        if (w == jo::self<jo::Fog>(this)->range_tex.width && h == jo::self<jo::Fog>(this)->range_tex.height && jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture) != nullptr) return 1;
        void* texture = nullptr;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_fog(this)->create_texture(&texture, joan::orig::arg::i32(w), joan::orig::arg::i32(h))) == 0) return 0;  // 0x005bff40
        jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture) = texture;
        jo::self<jo::Fog>(this)->range_tex.filled = 0;
        jo::self<jo::Fog>(this)->range_tex.width = w;
        jo::self<jo::Fog>(this)->range_tex.height = h;
        return 1;
    }
    std::int32_t fog_height_texture(std::int32_t w, std::int32_t h) {
        if (w == jo::self<jo::Fog>(this)->height_tex.width && h == jo::self<jo::Fog>(this)->height_tex.height && jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture) != nullptr) return 1;
        void* texture = nullptr;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_fog(this)->create_texture(&texture, joan::orig::arg::i32(w), joan::orig::arg::i32(h))) == 0) return 0;
        jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture) = texture;
        jo::self<jo::Fog>(this)->height_tex.filled = 0;
        jo::self<jo::Fog>(this)->height_tex.width = w;
        jo::self<jo::Fog>(this)->height_tex.height = h;
        return 1;
    }
    // 0x005bff60: fill the range fog texture (+0x30, w +0x38, h +0x34) - colour +0x40 with alpha
    // rising 0..255 across the columns.
    std::int32_t fog_fill_range_texture() {
        c_fog_tex.hit();
        struct {
            std::int32_t pitch;
            std::uint8_t* bits;
        } lr{};
        const std::int32_t hr = com_call(jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture), kTextureLockRect, 0u, &lr, static_cast<void*>(nullptr), 0u);
        std::uint32_t colour = jo::self<jo::Fog>(this)->colour;
        float t = 0.0f;
        const float step = 1.0f / static_cast<float>(jo::self<jo::Fog>(this)->range_tex.width);
        if (hr == 0) {
            for (std::int32_t x = 0; x < jo::self<jo::Fog>(this)->range_tex.width; ++x) {
                colour = (colour & 0x00ffffffu) | (static_cast<std::uint32_t>(static_cast<std::uint8_t>(ftol(t * 255.0f))) << 24);
                std::uint8_t* p = lr.bits + x * 4;
                for (std::int32_t y = 0; y < jo::self<jo::Fog>(this)->range_tex.height; ++y) {
                    std::memcpy(p, &colour, sizeof colour);  // one ARGB texel in the locked bits
                    p += lr.pitch;
                }
                t = t + step;
            }
            jo::self<jo::Fog>(this)->range_tex.filled = 1;
        } else {
            jo::self<jo::Fog>(this)->range_tex.filled = 0;
        }
        com_call(jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture), kTextureUnlockRect, 0u);
        return hr == 0 ? 1 : 0;
    }
    // 0x005c00b0: fill the height fog texture (+0, w +8, h +4) for the eye height `eye`:
    // per row (world height from +0x20 to +0x24) a start alpha and per-texel step between the
    // fog bottom +0x14 and top +0x1c, density +0x18 * +0x28 / w.
    std::int32_t fog_fill_height_texture(float eye) {
        c_fog_tex.hit();
        struct {
            std::int32_t pitch;
            std::uint8_t* bits;
        } lr{};
        const std::int32_t hr = com_call(jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture), kTextureLockRect, 0u, &lr, static_cast<void*>(nullptr), 0u);
        std::uint32_t colour = jo::self<jo::Fog>(this)->height_colour;
        const float range = jo::self<jo::Fog>(this)->far_z - jo::self<jo::Fog>(this)->near_z;
        float y = jo::self<jo::Fog>(this)->near_z;
        const float dy = range / static_cast<float>(jo::self<jo::Fog>(this)->height_tex.height);
        const float k = jo::self<jo::Fog>(this)->height_a * jo::self<jo::Fog>(this)->length / static_cast<float>(jo::self<jo::Fog>(this)->height_tex.width);
        if (hr == 0) {
            for (std::int32_t r = 0; r < jo::self<jo::Fog>(this)->height_tex.height; ++r) {
                std::uint8_t* p = lr.bits + r * lr.pitch;
                float step, a;
                if (eye > jo::self<jo::Fog>(this)->height_c) {
                    if (y > jo::self<jo::Fog>(this)->height_c) {
                        step = 0.0f;
                        a = 0.0f;
                    } else {
                        if (!(y < jo::self<jo::Fog>(this)->height_b))
                            step = (jo::self<jo::Fog>(this)->height_c - y) * k / (eye - y);
                        else
                            step = (jo::self<jo::Fog>(this)->height_c - jo::self<jo::Fog>(this)->height_b) * k / (eye - y);
                        a = (eye - jo::self<jo::Fog>(this)->height_c) * -k / (eye - y);
                    }
                } else if (!(eye < jo::self<jo::Fog>(this)->height_b)) {
                    if (y > jo::self<jo::Fog>(this)->height_c)
                        step = (jo::self<jo::Fog>(this)->height_c - eye) * k / (y - eye);
                    else if (!(y < jo::self<jo::Fog>(this)->height_b))
                        step = k;
                    else
                        step = (eye - jo::self<jo::Fog>(this)->height_b) * k / (eye - y);
                    a = 0.0f;
                } else if (!(y < jo::self<jo::Fog>(this)->height_b)) {
                    if (y > jo::self<jo::Fog>(this)->height_c)
                        step = (jo::self<jo::Fog>(this)->height_c - jo::self<jo::Fog>(this)->height_b) * k / (y - eye);
                    else
                        step = (y - jo::self<jo::Fog>(this)->height_b) * k / (y - eye);
                    a = (jo::self<jo::Fog>(this)->height_b - eye) * -k / (y - eye);
                } else {
                    step = 0.0f;
                    a = 0.0f;
                }
                for (std::int32_t c = 0; c < jo::self<jo::Fog>(this)->height_tex.width; ++c) {
                    std::uint8_t alpha;
                    if (a >= 1.0f)
                        alpha = 0xff;
                    else if (!(a > 0.0f))
                        alpha = 0;
                    else
                        alpha = static_cast<std::uint8_t>(ftol(a * 255.0f));
                    colour = (colour & 0x00ffffffu) | (static_cast<std::uint32_t>(alpha) << 24);
                    std::memcpy(p, &colour, sizeof colour);  // one ARGB texel in the locked bits
                    a = a + step;
                    p += 4;
                }
                y = y + dy;
            }
            jo::self<jo::Fog>(this)->f32_2c = eye;
            jo::self<jo::Fog>(this)->height_tex.filled = 1;
        } else {
            jo::self<jo::Fog>(this)->height_tex.filled = 0;
        }
        com_call(jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture), kTextureUnlockRect, 0u);
        return hr == 0 ? 1 : 0;
    }
};

#define HELPER22(name, addr, fn) JOAN_REPLACE("en3d/helpers22", name, addr, fn)
HELPER22("Fog::kinds", 0x005c09f0, &H::fog_kinds);
HELPER22("Fog::range_texture", 0x005c0cf0, &H::fog_range_texture);
HELPER22("Fog::height_texture", 0x005c0da0, &H::fog_height_texture);
HELPER22("Fog::fill_range_texture", 0x005bff60, &H::fog_fill_range_texture);
HELPER22("Fog::fill_height_texture", 0x005c00b0, &H::fog_fill_height_texture);
#undef HELPER22

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers21)
namespace {
namespace en3d_helpers20 {

using joan::img::obj;

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

void* game_device() { return global<void*>(d::g_game_device); }

// Device methods (IDirect3DDevice8 vtable indices).
constexpr std::int32_t kSetRenderState = 50, kGetRenderState = 51, kSetTexture = 61, kSetTextureStageState = 63;

JOAN_COUNTER(c_fog_apply, "Fog::apply");

struct H {
    // 0x005c0460 Fog::apply(kind, stage, x, y, z): kind 2 = fixed-function fog states, 4 = fog
    // texture by distance range, 8 = height fog texture; kinds this fog lacks (0x005c09f0)
    // disable the stage. The callers ignore the return value.
    std::uint32_t fog_apply(std::int32_t kind, std::int32_t stage, float, float, float z) {
        c_fog_apply.hit();
        void* const vs_constants = global<void*>(d::g_vs_cache);
        const std::uint32_t kinds = joan::orig::arg::as<std::uint32_t>(joan::orig::as_fog(this)->kinds());  // 0x005c09f0
        if ((kinds & static_cast<std::uint32_t>(kind)) == 0) {
            if (kind == 2 || stage == -1) return 0;
            com_call(game_device(), kSetTextureStageState, static_cast<std::uint32_t>(stage), 1u, 1u);  // COLOROP = DISABLE
            return static_cast<std::uint32_t>(com_call(game_device(), kSetTextureStageState, static_cast<std::uint32_t>(stage), 4u, 1u));
        }
        if (kind == 2) {
            if (jo::self<jo::Fog>(this)->active == 0) {
                com_call(game_device(), kSetRenderState, 0x1cu, 1u);  // FOGENABLE
                jo::self<jo::Fog>(this)->active = 1;
            }
            std::uint32_t previous;
            com_call(game_device(), kGetRenderState, 0x1cu, &previous);
            com_call(game_device(), kSetRenderState, 0x22u, joan::orig::arg::as<std::uint32_t>(joan::orig::as_color(&jo::self<jo::Fog>(this)->colour)->get()));  // FOGCOLOR
            if ((global<std::uint32_t>(d::g_a52494) & 1) != 0) {
                com_call(game_device(), kSetRenderState, 0x8cu, global<std::uint8_t>(d::g_a52569) != 0 ? 0u : 3u);  // FOGVERTEXMODE
                com_call(game_device(), kSetRenderState, 0x23u, 0u);  // FOGTABLEMODE
            } else if ((global<std::uint32_t>(d::g_a52494) & 1) != 0) {
                com_call(game_device(), kSetRenderState, 0x8cu, 0u);
                com_call(game_device(), kSetRenderState, 0x23u, 3u);
            }
            com_call(game_device(), kSetRenderState, 0x24u, jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->start));  // FOGSTART
            com_call(game_device(), kSetRenderState, 0x25u, jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->end));  // FOGEND
            if ((global<std::uint32_t>(d::g_a52494) & 0x10000) == 0 ||
                joan::orig::arg::as<std::int32_t>(joan::orig::as_sys_en(img::obj(d::g_sys3d))->vs_version()) == 0x14)  // 0x005f8220
                com_call(game_device(), kSetRenderState, 0x30u, 0u);  // RANGEFOGENABLE
            else
                com_call(game_device(), kSetRenderState, 0x30u, 1u);
            jo::self<jo::Fog>(this)->dirty = 0;
            return joan::call::register_low_bits(this);  // eax = this (low half on x64); the callers ignore it
        }
        if (kind == 4 && stage != -1) {
            if (jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture) == nullptr) joan::orig::as_fog(this)->range_texture(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->range_tex.width)), joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->range_tex.height)));  // 0x005c0cf0
            if (jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture) != nullptr && jo::self<jo::Fog>(this)->range_tex.filled == 0) joan::orig::as_fog(this)->fill_range_texture();                   // 0x005bff60
            if (jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture) == nullptr || jo::self<jo::Fog>(this)->range_tex.filled == 0) return joan::call::register_low_bits(this);  // eax = this, ignored
            com_call(game_device(), kSetTexture, static_cast<std::uint32_t>(stage), jo::view<void*>(jo::self<jo::Fog>(this)->range_tex.texture));
            float c[4];
            void* v;
            if (jo::self<jo::Fog>(this)->start < jo::self<jo::Fog>(this)->end) {
                const float scale = 1.0f / (jo::self<jo::Fog>(this)->end - jo::self<jo::Fog>(this)->start);
                v = joan::orig::arg::as<void*>(joan::orig::as_vector4(c)->construct_xyzw(joan::orig::arg::bits(jo::self<jo::Fog>(this)->start), joan::orig::arg::bits(jo::self<jo::Fog>(this)->end), joan::orig::arg::bits(scale), joan::orig::arg::bits(0.0f)));  // 0x005bd220 Vec4
            } else {
                v = joan::orig::arg::as<void*>(joan::orig::as_vector4(c)->construct_xyzw(joan::orig::arg::bits(jo::self<jo::Fog>(this)->start), joan::orig::arg::bits(jo::self<jo::Fog>(this)->end), joan::orig::arg::bits(0.0f), joan::orig::arg::bits(0.0f)));
            }
            return joan::orig::arg::as<std::uint32_t>(joan::orig::as_vs_cache(vs_constants)->set_constant(joan::orig::arg::mptr(0), 0, joan::orig::arg::mptr(v)));  // 0x0061c080
        }
        if (kind == 8 && stage != -1) {
            if (jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture) == nullptr) joan::orig::as_fog(this)->height_texture(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->height_tex.width)), joan::orig::arg::i32(jo::view<std::uint32_t>(jo::self<jo::Fog>(this)->height_tex.height)));  // 0x005c0da0
            if ((jo::self<jo::Fog>(this)->f32_2c < z || jo::self<jo::Fog>(this)->f32_2c > z) || (jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture) != nullptr && jo::self<jo::Fog>(this)->height_tex.filled == 0))
                joan::orig::as_fog(this)->fill_height_texture(joan::orig::arg::f32(z));  // 0x005c00b0
            if (jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture) == nullptr || jo::self<jo::Fog>(this)->height_tex.filled == 0) return 0;
            com_call(game_device(), kSetTexture, static_cast<std::uint32_t>(stage), jo::view<void*>(jo::self<jo::Fog>(this)->height_tex.texture));
            const float inv_height = 1.0f / jo::self<jo::Fog>(this)->length;
            const float inv_range = 1.0f / (jo::self<jo::Fog>(this)->far_z - jo::self<jo::Fog>(this)->near_z);
            float c[4];
            void* const v = joan::orig::arg::as<void*>(joan::orig::as_vector4(c)->construct_xyzw(joan::orig::arg::bits(-jo::self<jo::Fog>(this)->near_z), joan::orig::arg::bits(inv_range), joan::orig::arg::bits(inv_height), joan::orig::arg::bits(0.0f)));
            return joan::orig::arg::as<std::uint32_t>(joan::orig::as_vs_cache(vs_constants)->set_constant(joan::orig::arg::mptr(0), 0, joan::orig::arg::mptr(v)));
        }
        return kinds & static_cast<std::uint32_t>(kind);
    }
};

#define HELPER21(name, addr, fn) JOAN_REPLACE("en3d/helpers21", name, addr, fn)
HELPER21("Fog::apply", 0x005c0460, &H::fog_apply);
#undef HELPER21

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/en3d_helpers6.cpp (en3d/helpers6)
namespace {
namespace en3d_helpers6 {

struct H {
    std::uint32_t first_dword() { return jo::view<std::uint32_t>(*jo::self<jo::Colour>(this)); }                    // 0x005c0980 Color::get
};

#define HELPER6(name, addr, fn) JOAN_REPLACE("en3d/helpers6", name, addr, fn)
HELPER6("Color::get", 0x005c0980, &H::first_dword);
#undef HELPER6

}  // namespace en3d_helpers6
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

using Device = joan::d3d8::IDirect3DDevice8;

Device* game_device() { return global<Device*>(d::g_game_device); }

// Every helper shares one class so `this` is just the object's address.
struct H {
    // --- light / fog objects (between bonetree.cpp and morph.cpp) ---------------------------------
    void fog_off() {  // 0x005c09a0
        if (jo::self<jo::Fog>(this)->active == 0) return;
        game_device()->SetRenderState(28, 0);  // FOGENABLE
        jo::self<jo::Fog>(this)->active = 0;
    }
    void fog_begin() {  // 0x005c0e50
        jo::self<jo::Fog>(this)->dirty = 1;
        game_device()->SetRenderState(28, jo::self<jo::Fog>(this)->active != 0 ? 1 : 0);
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Fog::off", 0x005c09a0, &H::fog_off);
HELPER("Fog::begin", 0x005c0e50, &H::fog_begin);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_helpers3.cpp (en3d/helpers3)
namespace {
namespace en3d_helpers3 {

using joan::call::cdecl_call;

bool below_or_nan(float a, float b) { return !(a >= b); }
bool below_or_nan(double a, double b) { return !(a >= b); }

struct Vec3 {
    float x, y, z;
};

struct H {
    // --- Fog (0x005c0xxx) ------------------------------------------------------------------------
    // 0x005c0a70: colour (+0x10) and three values (+0x18 / +0x14 / +0x1c); when any changed, the
    // span from `near` to `far` is compared with the stored one (+0x20 / +0x24 z, +0x28 length)
    // and re-stored (+0x0c reset) when it moved by more than the 0.75 tolerance.
    void fog_set_range(std::uint32_t colour, float a, float b, float c, Vec3 near, Vec3 far) {
        std::uint32_t col = colour;
        if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_color(&col)->get()) == joan::orig::arg::as<std::uint32_t>(joan::orig::as_color(&jo::self<jo::Fog>(this)->height_colour)->get()) &&
            !differs(a, jo::self<jo::Fog>(this)->height_a) && !differs(b, jo::self<jo::Fog>(this)->height_b) && !differs(c, jo::self<jo::Fog>(this)->height_c))
            return;
        jo::self<jo::Fog>(this)->height_tex.filled = 0;
        jo::self<jo::Fog>(this)->height_colour = colour;
        jo::self<jo::Fog>(this)->height_a = a;
        jo::self<jo::Fog>(this)->height_b = b;
        jo::self<jo::Fog>(this)->height_c = c;
        const float dx = far.x - near.x, dy = far.y - near.y, dz = far.z - near.z;
        const float sum = dx * dx + dy * dy + dz * dz;
        const float length = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::sqrt(joan::orig::arg::f64(static_cast<double>(sum)))));  // CRT sqrt
        if (below_or_nan(static_cast<double>(length / jo::self<jo::Fog>(this)->length), 0.75) ||
            below_or_nan(static_cast<double>(jo::self<jo::Fog>(this)->length / length), 0.75) || below_or_nan(near.z, jo::self<jo::Fog>(this)->near_z) ||
            far.z > jo::self<jo::Fog>(this)->far_z ||
            below_or_nan(static_cast<double>((far.z - near.z) / (jo::self<jo::Fog>(this)->far_z - jo::self<jo::Fog>(this)->near_z)), 0.75)) {
            jo::self<jo::Fog>(this)->height_tex.filled = 0;
            jo::self<jo::Fog>(this)->far_z = far.z;
            jo::self<jo::Fog>(this)->near_z = near.z;
            jo::self<jo::Fog>(this)->length = length;
        }
    }
};

#define HELPER3(name, addr, fn) JOAN_REPLACE("en3d/helpers3", name, addr, fn)
HELPER3("Fog::set_range", 0x005c0a70, &H::fog_set_range);
#undef HELPER3

}  // namespace en3d_helpers3
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {

// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    // --- Fog (0x005c0xxx) ---------------------------------------------------------------------
    // 0x005c0c30: colour (+0x40), start / end (+0x48 / +0x4c); +0x44 dirty, +0x3c reset on a new colour.
    void fog_set(std::uint32_t colour, float start, float end) {
        std::uint32_t c = colour;
        if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_color(&c)->get()) != joan::orig::arg::as<std::uint32_t>(joan::orig::as_color(&jo::self<jo::Fog>(this)->colour)->get()) ||
            differs(jo::self<jo::Fog>(this)->start, start) || differs(jo::self<jo::Fog>(this)->end, end))
            jo::self<jo::Fog>(this)->dirty = 1;
        if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_color(&c)->get()) != joan::orig::arg::as<std::uint32_t>(joan::orig::as_color(&jo::self<jo::Fog>(this)->colour)->get())) {
            jo::self<jo::Fog>(this)->range_tex.filled = 0;
            jo::self<jo::Fog>(this)->colour = colour;
        }
        jo::self<jo::Fog>(this)->start = start;
        jo::self<jo::Fog>(this)->end = end;
    }
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Fog::set", 0x005c0c30, &H::fog_set);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Fog::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_06::H::zero_50>(this)); }  // 0x5bfd80
auto joan::orig::Fog::destruct() -> void { joan::bind::member<&::game_b88_06::H::dtor_5bfdb0>(this); }  // 0x5bfdb0
auto joan::orig::Fog::init() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_06::H::texcache_init>(this)); }  // 0x5bfde0
auto joan::orig::Fog::release_textures() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b89_06::H::release_all>(this)); }  // 0x5bfed0
auto joan::orig::Fog::create_texture(void* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers25::H::fog_texture_stub>(this, a0, a1, a2)); }  // 0x5bff40
auto joan::orig::Fog::fill_range_texture() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers22::H::fog_fill_range_texture>(this)); }  // 0x5bff60
auto joan::orig::Fog::fill_height_texture(float a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers22::H::fog_fill_height_texture>(this, a0)); }  // 0x5c00b0
auto joan::orig::Fog::apply(std::int32_t a0, std::int32_t a1, float a2, float a3, float a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers20::H::fog_apply>(this, a0, a1, a2, a3, a4)); }  // 0x5c0460
auto joan::orig::Color::get() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers6::H::first_dword>(this)); }  // 0x5c0980
auto joan::orig::Fog::off() -> void { joan::bind::member<&::en3d_helpers::H::fog_off>(this); }  // 0x5c09a0
auto joan::orig::Fog::kinds() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_helpers22::H::fog_kinds>(this)); }  // 0x5c09f0
auto joan::orig::Fog::set(std::uint32_t a0, float a1, float a2) -> void { joan::bind::member<&::en3d_helpers2::H::fog_set>(this, a0, a1, a2); }  // 0x5c0c30
auto joan::orig::Fog::range_texture(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers22::H::fog_range_texture>(this, a0, a1)); }  // 0x5c0cf0
auto joan::orig::Fog::height_texture(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_helpers22::H::fog_height_texture>(this, a0, a1)); }  // 0x5c0da0
auto joan::orig::Fog::begin() -> void { joan::bind::member<&::en3d_helpers::H::fog_begin>(this); }  // 0x5c0e50
