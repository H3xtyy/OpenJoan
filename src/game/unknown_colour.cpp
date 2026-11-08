// game/unknown_colour.cpp: functions whose original source file is unknown (formerly game/_unplaced/colour.cpp;
// re/attribution.csv: between ww/en3d/rgb.cpp and ww/en3d/scene.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_10.cpp (game/b102_10)
namespace {
namespace game_b102_10 {

struct H {
    // 0x005e9090 bytes * (1/255) -> float4
    float* to_float4(float* out) {
        const float k = global<float>(d::flt_0_003921569);
        out[0] = static_cast<float>(static_cast<i32>(jo::self<jo::Colour>(this)->r)) * k;
        out[1] = static_cast<float>(static_cast<i32>(jo::self<jo::Colour>(this)->g)) * k;
        out[2] = static_cast<float>(static_cast<i32>(jo::self<jo::Colour>(this)->b)) * k;
        out[3] = static_cast<float>(static_cast<i32>(jo::self<jo::Colour>(this)->a)) * k;
        return out;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_10", name, addr, fn)
B("Color::to_float4", 0x005e9090, &H::to_float4);
#undef B

}  // namespace game_b102_10
}  // namespace

// ---------------------------------------- from modules/en3d_helpers22.cpp (en3d/helpers22)
namespace {
namespace en3d_helpers22 {

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // 0x005e9130 Colour::to_float(out, scale): rgb * scale / 255, alpha / 255.
    float* colour_to_float(float* out, float scale) {
        const float s = scale * 0.003921568859368563f;  // 0x9d97d8
        out[0] = static_cast<float>(self()[0]) * s;
        out[1] = static_cast<float>(self()[1]) * s;
        out[2] = static_cast<float>(self()[2]) * s;
        out[3] = static_cast<float>(self()[3]) * 0.003921568859368563f;
        return out;
    }
};

#define HELPER22(name, addr, fn) JOAN_REPLACE("en3d/helpers22", name, addr, fn)
HELPER22("Colour::to_float", 0x005e9130, &H::colour_to_float);
#undef HELPER22

}  // namespace en3d_helpers22
}  // namespace

// ---------------------------------------- from modules/en3d_helpers16.cpp (en3d/helpers16)
namespace {
namespace en3d_helpers16 {

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    H* colour_from_rgba(const std::uint8_t* s) {  // 0x005e95b0
        self()[2] = s[0];
        self()[1] = s[1];
        self()[0] = s[2];
        self()[3] = s[3];
        return this;
    }
};

#define HELPER16(name, addr, fn) JOAN_REPLACE("en3d/helpers16", name, addr, fn)
HELPER16("Colour::from_rgba", 0x005e95b0, &H::colour_from_rgba);
#undef HELPER16

}  // namespace en3d_helpers16
}  // namespace

// ---------------------------------------- from modules/game_b102_11.cpp (game/b102_11)
namespace {
namespace game_b102_11 {

using joan::hook::x87::Fpu;
using i16 = std::int16_t;

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

JOAN_COUNTER(c_color_ops, "Color::operators");

// Color ctor 0x0042a260 (thiscall(r, g, b, a) bytes passed as dwords, ret 0x10) on a local, then
// the 4 bytes are copied to the hidden return slot.
u8* make_color(u8* out, u32 r, u32 g, u32 b, u32 a) {
    u8 local[4];
    joan::orig::as_colour(local)->construct(r, g, b, a);  // 0x0042a260
    std::memcpy(out, local, 4);
    return out;
}

// fild (byte) * fmul dword at PC24, then _ftol.
i32 scale_channel(u8 c, float s) {
    const Fpu fpu{true};
    return ftol(fpu.mul(static_cast<double>(static_cast<i32>(c)), s));
}

// 0x005e96a0 (cdecl, hidden return)
u8* JOAN_CDECL color_add(u8* out, const u8* a, const u8* b) {
    c_color_ops.hit();
    // Pushed a, b, g, r: all reads are pure.
    const u32 ca = static_cast<u32>(a[3]) + b[3];
    const u32 cb = static_cast<u32>(a[2]) + b[2];
    const u32 cg = static_cast<u32>(a[1]) + b[1];
    const u32 cr = static_cast<u32>(a[0]) + b[0];
    return make_color(out, cr, cg, cb, ca);
}

// 0x005e9730 (cdecl, hidden return)
u8* JOAN_CDECL color_sub(u8* out, const u8* a, const u8* b) {
    c_color_ops.hit();
    const u32 ca = static_cast<u32>(a[3]) - b[3];
    const u32 cb = static_cast<u32>(a[2]) - b[2];
    const u32 cg = static_cast<u32>(a[1]) - b[1];
    const u32 cr = static_cast<u32>(a[0]) - b[0];
    return make_color(out, cr, cg, cb, ca);
}

// 0x005e97c0 (cdecl, hidden return): movsx word * byte (imul)
u8* JOAN_CDECL color_short_mul(u8* out, i32 k_raw, const u8* c) {
    c_color_ops.hit();
    const i32 k = static_cast<i16>(k_raw);
    return make_color(out, static_cast<u32>(k * static_cast<i32>(c[0])), static_cast<u32>(k * static_cast<i32>(c[1])),
                      static_cast<u32>(k * static_cast<i32>(c[2])), static_cast<u32>(k * static_cast<i32>(c[3])));
}

// 0x005e9840 (cdecl, hidden return)
u8* JOAN_CDECL color_float_mul(u8* out, float s, const u8* c) {
    c_color_ops.hit();
    const i32 ca = scale_channel(c[3], s);
    const i32 cb = scale_channel(c[2], s);
    const i32 cg = scale_channel(c[1], s);
    const i32 cr = scale_channel(c[0], s);
    return make_color(out, static_cast<u32>(cr), static_cast<u32>(cg), static_cast<u32>(cb), static_cast<u32>(ca));
}

// 0x005e98e0 (cdecl, hidden return)
u8* JOAN_CDECL color_mul_short(u8* out, const u8* c, i32 k_raw) {
    c_color_ops.hit();
    const i32 k = static_cast<i16>(k_raw);
    return make_color(out, static_cast<u32>(static_cast<i32>(c[0]) * k), static_cast<u32>(static_cast<i32>(c[1]) * k),
                      static_cast<u32>(static_cast<i32>(c[2]) * k), static_cast<u32>(static_cast<i32>(c[3]) * k));
}

// 0x005e9960 (cdecl, hidden return)
u8* JOAN_CDECL color_mul_float(u8* out, const u8* c, float s) {
    c_color_ops.hit();
    const i32 ca = scale_channel(c[3], s);
    const i32 cb = scale_channel(c[2], s);
    const i32 cg = scale_channel(c[1], s);
    const i32 cr = scale_channel(c[0], s);
    return make_color(out, static_cast<u32>(cr), static_cast<u32>(cg), static_cast<u32>(cb), static_cast<u32>(ca));
}

struct H {
    // 0x005e9300 (ret 8)
    float* color_to_vector(float* out, float scale) {
        c_color_ops.hit();
        const Fpu fpu{true};
        const float s = scale * global<float>(d::flt_0_003921569);  // [ebp-8], 1/255
        const u8* const c = reinterpret_cast<const u8*>(this);
        out[2] = static_cast<float>(fpu.mul(static_cast<double>(static_cast<i32>(c[0])), s));
        out[1] = static_cast<float>(fpu.mul(static_cast<double>(static_cast<i32>(c[1])), s));
        out[0] = static_cast<float>(fpu.mul(static_cast<double>(static_cast<i32>(c[2])), s));
        out[3] = static_cast<float>(fpu.mul(static_cast<double>(static_cast<i32>(c[3])), global<float>(d::flt_0_003921569)));
        return out;
    }
};

#define B11(name, addr, fn) JOAN_REPLACE("game/b102_11", name, addr, fn)
B11("Color::to_vector", 0x005e9300, &H::color_to_vector);
B11("Color::operator+", 0x005e96a0, &color_add);
B11("Color::operator-", 0x005e9730, &color_sub);
B11("Color::operator*(short,Color)", 0x005e97c0, &color_short_mul);
B11("Color::operator*(float,Color)", 0x005e9840, &color_float_mul);
B11("Color::operator*(Color,short)", 0x005e98e0, &color_mul_short);
B11("Color::operator*(Color,float)", 0x005e9960, &color_mul_float);
#undef B11

}  // namespace game_b102_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers.cpp (en3d/helpers)
namespace {
namespace en3d_helpers {

// Every helper shares one class so `this` is just the object's address.
struct H {
    std::uint32_t* colour_to_d3d(std::uint32_t* out) {  // 0x005e93b0: bytes r g b a -> D3DCOLOR
        *out = static_cast<std::uint32_t>(jo::self<jo::Colour>(this)->a) << 24 | static_cast<std::uint32_t>(jo::self<jo::Colour>(this)->b) << 16 |
               static_cast<std::uint32_t>(jo::self<jo::Colour>(this)->g) << 8 | jo::self<jo::Colour>(this)->r;
        return out;
    }
};

#define HELPER(name, addr, fn) JOAN_REPLACE("en3d/helpers", name, addr, fn)
HELPER("Colour::to_d3d", 0x005e93b0, &H::colour_to_d3d);
#undef HELPER

}  // namespace en3d_helpers
}  // namespace

// ---------------------------------------- from modules/en3d_helpers36.cpp (en3d/helpers36)
namespace {
namespace en3d_helpers36 {

struct H {
    // 0x005e9430 Color::set_bgr(src). ret 4, eax = this.
    void* color_set(const std::uint8_t* src) {
        jo::self<jo::Colour>(this)->b = src[0];
        jo::self<jo::Colour>(this)->g = src[1];
        jo::self<jo::Colour>(this)->r = src[2];
        jo::self<jo::Colour>(this)->a = src[3];
        return this;
    }
};

#define HELPER36(name, addr, fn) JOAN_REPLACE("en3d/helpers36", name, addr, fn)
HELPER36("Color::set_bgr", 0x005e9430, &H::color_set);
#undef HELPER36

}  // namespace en3d_helpers36
}  // namespace

// ---------------------------------------- from modules/game_b103_11.cpp (game/b103_11)
namespace {
namespace game_b103_11 {

struct H {
    // 0x005e9490
    H* from_rgb(const u8* p) {
        jo::self<jo::Colour>(this)->r = p[0];
        jo::self<jo::Colour>(this)->g = p[1];
        jo::self<jo::Colour>(this)->b = p[2];
        jo::self<jo::Colour>(this)->a = 0xff;
        return this;
    }
    // 0x005e9550
    H* from_bgr(const u8* p) {
        jo::self<jo::Colour>(this)->b = p[0];
        jo::self<jo::Colour>(this)->g = p[1];
        jo::self<jo::Colour>(this)->r = p[2];
        jo::self<jo::Colour>(this)->a = 0xff;
        return this;
    }
};

#define B103(name, addr, fn) JOAN_REPLACE("game/b103_11", name, addr, fn)
B103("Color::from_rgb", 0x005e9490, &H::from_rgb);
B103("Color::from_bgr", 0x005e9550, &H::from_bgr);
#undef B103

}  // namespace game_b103_11
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

struct H {
    // ------------------------------------------------------------------ 0x005e94f0
    H* set_argb(u32 c) {
        jo::self<jo::Colour>(this)->b = static_cast<u8>(c);
        jo::self<jo::Colour>(this)->g = static_cast<u8>(c >> 8);
        jo::self<jo::Colour>(this)->r = static_cast<u8>(c >> 16);
        jo::self<jo::Colour>(this)->a = static_cast<u8>(c >> 24);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Colour::set_argb", 0x005e94f0, &H::set_argb);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/en3d_helpers2.cpp (en3d/helpers2)
namespace {
namespace en3d_helpers2 {

// Every thiscall helper shares one class so `this` is just the object's address.
struct H {
    H* colour_set(std::uint32_t colour) {  // 0x005e9610
        jo::view<std::uint32_t>(*jo::self<jo::Colour>(this)) = colour;
        return this;
    }
};

#define HELPER2(name, addr, fn) JOAN_REPLACE("en3d/helpers2", name, addr, fn)
HELPER2("Colour::set", 0x005e9610, &H::colour_set);
#undef HELPER2

}  // namespace en3d_helpers2
}  // namespace

// ---------------------------------------- from modules/en3d_b82_09.cpp (en3d/b82_09)
namespace {
namespace en3d_b82_09 {

struct H {
    // 0x005e9640 Color::Color(bgra). ret 4, eax = this.
    void* color_ctor(std::uint32_t v) {
        jo::self<jo::Colour>(this)->b = static_cast<std::uint8_t>(v);
        jo::self<jo::Colour>(this)->g = static_cast<std::uint8_t>(v >> 8);
        jo::self<jo::Colour>(this)->r = static_cast<std::uint8_t>(v >> 16);
        jo::self<jo::Colour>(this)->a = static_cast<std::uint8_t>(v >> 24);
        return this;
    }
};

#define B82_09(name, addr, fn) JOAN_REPLACE("en3d/b82_09", name, addr, fn)
B82_09("Color::Color(dword)", 0x005e9640, &H::color_ctor);
#undef B82_09

}  // namespace en3d_b82_09
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Colour::to_float_5e9130(void* a0, float a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers22::H::colour_to_float>(this, a0, a1)); }  // 0x5e9130
auto joan::orig::Colour::to_d3d(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers::H::colour_to_d3d>(this, a0)); }  // 0x5e93b0
auto joan::orig::Color::set_bgr(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers36::H::color_set>(this, a0)); }  // 0x5e9430
auto joan::orig::Colour::set_argb(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_11::H::set_argb>(this, a0)); }  // 0x5e94f0
auto joan::orig::Colour::from_rgba(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers16::H::colour_from_rgba>(this, a0)); }  // 0x5e95b0
auto joan::orig::Colour::set(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers2::H::colour_set>(this, a0)); }  // 0x5e9610
auto joan::orig::Color::Color_dword(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_09::H::color_ctor>(this, a0)); }  // 0x5e9640
