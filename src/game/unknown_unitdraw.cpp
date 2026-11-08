// game/unknown_unitdraw.cpp: functions whose original source file is unknown (formerly game/_unplaced/unitdraw.cpp;
// re/attribution.csv: between ww/unit.cpp and ww/unitres.cpp). They move to their original file when evidence names it
// (Phase 3).
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
}  // namespace

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

using joan::call::cdecl_call;
using joan::img::str;

constexpr img::Addr kGameUnits(d::g_units);

std::uint8_t* world() { return global<std::uint8_t*>(d::g_world); }

}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (game/b82_17)
namespace {
namespace en3d_b82_17 {

JOAN_COUNTER(c_set_unit, "Object53::set_unit");

struct H {
    // 0x00520170
    void* object53_ctor() {
#if UINTPTR_MAX == 0xffffffffu
        static_assert(sizeof(jo::UnitDraw) == 0x53, "UnitDraw ends with u8_52");
#endif
        std::memset(static_cast<void*>(this), 0, sizeof(jo::UnitDraw));  // memset (0x53 on x86)
        return this;
    }

    // 0x005201d0 (eax: the 0x00428b70 result, or this)
    std::uintptr_t set_unit(std::int16_t id) {
        c_set_unit.hit();
        void* const kUnits = obj(d::g_units);
        const std::int32_t missing = joan::orig::as_dynarray(kUnits)->empty_at(static_cast<std::int32_t>(id));  // 0x00428b70
        if (missing != 0) return static_cast<std::uint32_t>(missing);
        jo::self<jo::UnitDraw>(this)->unit_id = id;
        // `mov ax, [0xa5b9e8]; push eax` with eax = 0 from the test above.
        const std::uint32_t side = global<std::uint16_t>(d::g_player_unit_recno);
        void* const unit = joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<std::int32_t>(id));  // 0x0041b3c0
        jo::self<jo::UnitDraw>(this)->i16_46 = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(side))));  // 0x005094d0
        joan::orig::as_unit_draw(this)->create_plate();  // 0x00520270
        joan::orig::as_unit_draw(this)->create_label();  // 0x00520e80
        jo::self<jo::UnitDraw>(this)->plate_attached = 0;
        jo::self<jo::UnitDraw>(this)->label_attached = 0;
        jo::self<jo::UnitDraw>(this)->u8_52 = 1;
        return reinterpret_cast<std::uintptr_t>(this);
    }
};

#define B82_17G(name, addr, fn) JOAN_REPLACE("game/b82_17", name, addr, fn)
B82_17G("Object53::Object53", 0x00520170, &H::object53_ctor);
B82_17G("Object53::set_unit", 0x005201d0, &H::set_unit);
#undef B82_17G

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
namespace {
namespace game_b84_02 {

struct H {
    // 0x005201a0 dtor body: 0x00521340.
    void dtor_5201a0() { joan::orig::as_unit_draw(this)->release(); }  // 0x00521340
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("Obj5201a0::dtor", 0x005201a0, &H::dtor_5201a0);
#undef B84

}  // namespace game_b84_02
}  // namespace

// ---------------------------------------- from modules/game_overlays.cpp (game/units3)
namespace {
namespace game_overlays {

using i16 = std::int16_t;

constexpr img::Addr kTypes(d::g_types);
constexpr img::Addr kTextures(d::g_texture_array);
constexpr img::Addr kFont(d::g_font_a5cdc0);

using joan::orig::helpers::operator_new;
void* unit_type(const void* unit) { return joan::orig::arg::as<void*>(joan::orig::as_records_203(img::obj(kTypes))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id))); }  // 0x0042d0a0
std::uint32_t colour(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
}
// Height of the overlay above the unit by type kind (+0x118), as in UnitDraw::plate.
float overlay_height(float z, std::int16_t kind) {
    switch (kind) {
    case 4: return z + 95.0f;
    case 5: return z + 85.0f;
    case 6: return z + 77.0f;
    case 8: return z + 825.0f;
    case 10: return z + 625.0f;
    case 11: return z + 360.0f;
    default: return z + 95.0f;
    }
}

// the font object passed by value (a bitwise copy, destroyed by the callee): a Font2d, 0x40 bytes on x86 (larger on
// x64, where the callee reads the whole object through the hidden reference)
using Font64 = jo::Font2d;

JOAN_COUNTER(c_plate_create, "UnitDraw::create_plate");
JOAN_COUNTER(c_label_create, "UnitDraw::create_label");

struct H {

    void set_plane(void* plane) { jo::self<jo::Billboard>(this)->plane_ = static_cast<jo::Plane*>(plane); }  // 0x00522940

    // 0x00520270 UnitDraw::create_plate: the health plate billboard (+0x4e) for units whose
    // type flags (+0x11c) show one; two 4-vertex bar quads added to the quad, the bar
    // texture by side ("bar_english" / "bar_french").
    void create_plate() {
        if (joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->unit_id)) != 0) return;  // 0x00428b70
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->unit_id)));  // 0x0041b3c0
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_) != nullptr) return;
        if (jo::as_unit(unit)->layer == 0 && jo::self<jo::UnitDraw>(this)->unit_id != global<std::int16_t>(d::g_a47c36)) return;
        const std::uint32_t masks[6] = {0x1, 0x2, 0x40, 0x400, 0x2000, 0x4000};
        bool shown = false;
        for (const std::uint32_t m : masks)
            if ((jo::self<jo::UnitType>(unit_type(unit))->trigger_flags & m) != 0) {
                shown = true;
                break;
            }
        if (!shown) return;
        c_plate_create.hit();
        void* const mem = operator_new(static_cast<std::uint32_t>(sizeof(jo::Billboard)));  // 0x37 on x86
        jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_) = mem != nullptr ? joan::orig::as_billboard(mem)->construct() : nullptr;  // 0x006351a0
        float pos[3];
        std::memcpy(pos, jo::as_unit(unit)->position_, sizeof pos);
        pos[2] = overlay_height(pos[2], jo::self<jo::UnitType>(unit_type(unit))->kind);
        const std::uint32_t white = colour(0xff, 0xff, 0xff, 0xff);
        std::uint32_t p[3];
        std::memcpy(p, pos, sizeof p);
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_))->create(joan::orig::arg::u32(p[0]), joan::orig::arg::u32(p[1]), joan::orig::arg::u32(p[2]), 0x41800000u, 0x40000000u, joan::orig::arg::str(&img::global<const char>(d::s_LifeBarre)), joan::orig::arg::i16(// "LifeBarre"
                 static_cast<std::int32_t>(0)), joan::orig::arg::u32(white), 0u, 0u, 0u, 0u, 6u, 0x20000u, 1u);  // 0x00635260
        std::uint8_t* const plane = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_billboard(jo::view<void*>(jo::self<jo::UnitDraw>(this)->plate_))->plane());  // 0x00635e80
        jo::as_plane(plane)->materials->raster = 0x10;
        const std::uint32_t saved = joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(img::obj(kTextures))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(img::obj(kTextures))->set_mode(1u);                                         // 0x0060a740
        void* const tex = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(global<std::int16_t>(d::g_player_unit_recno)))) != 0  // 0x005094d0
                              ? joan::orig::arg::as<void*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(&img::global<const char>(d::s_bar_english)), 1, 0u))  // "bar_english"
                              : joan::orig::as_texture_manager(img::obj(kTextures))->load(joan::orig::arg::str(&img::global<const char>(d::s_bar_french)), 1, 0u);  // "bar_french"
        joan::orig::as_texture_manager(img::obj(kTextures))->set_mode(joan::orig::arg::u32(saved));
        const std::int32_t mat = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_materials(1));  // 0x005cd470 add_materials
        joan::orig::as_material(&jo::as_plane(plane)->materials[mat])->assign(joan::orig::arg::mptr(jo::view<void*>(jo::as_plane(plane)->materials)));  // 0x005d53e0 copy
        joan::orig::as_material(&jo::as_plane(plane)->materials[mat])->set_texture(0, joan::orig::arg::mptr(tex));                  // 0x005d5650
        jo::as_plane(plane)->materials[mat].raster = 0x20;
        joan::orig::as_texture_manager(img::obj(kTextures))->release(joan::orig::arg::mptr(tex));  // 0x0060a550 release
        for (std::int32_t quad = 0; quad < 2; ++quad) {
            const std::int32_t base = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_vertices(4));  // 0x005cd7f0 add_vertices
            for (std::int32_t k = 0; k < 4; ++k) {
                std::uint8_t* const v = jo::view<std::uint8_t*>(jo::as_plane(plane)->vertices_f4) + (base + k) * 0x18;
                std::memmove(v, jo::view<std::uint8_t*>(jo::as_plane(plane)->vertices_f4) + k * 0x18, 0x18);
                if (k < 2)
                    jo::as_vertex_f4(v)->pos[1] = jo::as_vertex_f4(v)->pos[1] + 0.046875f;
                else
                    jo::as_vertex_f4(v)->pos[1] = jo::as_vertex_f4(v)->pos[1] - 0.171875f;
                if (k == 0 || k == 2)
                    jo::as_vertex_f4(v)->pos[2] = jo::as_vertex_f4(v)->pos[2] - 0.1875f;
                else
                    jo::as_vertex_f4(v)->pos[2] = jo::as_vertex_f4(v)->pos[2] + 0.46875f;
                jo::view<std::uint32_t>(jo::as_vertex_f4(v)->attr[1]) = k < 2 ? 0u : 0x3f480000u;              // 0.78125
                jo::view<std::uint32_t>(jo::as_vertex_f4(v)->attr[2]) = (k == 0 || k == 2) ? 0u : 0x3f200000u;  // 0.625
            }
            const std::int32_t tri = joan::orig::arg::as<std::int32_t>(joan::orig::as_plane(plane)->add_triangles(2));  // 0x005ce6f0 add_triangles
            joan::orig::as_plane(plane)->set_triangle(joan::orig::arg::i32(tri), joan::orig::arg::u32(mat), joan::orig::arg::u32(base), joan::orig::arg::u32(base + 1), joan::orig::arg::u32(base + 2));        // 0x005ce800
            joan::orig::as_plane(plane)->set_triangle(joan::orig::arg::i32(tri + 1), joan::orig::arg::u32(mat), joan::orig::arg::u32(base + 2), joan::orig::arg::u32(base + 1), joan::orig::arg::u32(base + 3));
        }
        joan::orig::as_plane(plane)->sort_by_material(1);  // 0x005cf1d0 sort_by_material
    }

    // 0x00520e80 UnitDraw::create_label: "<name> (<player>)" text billboard (+0x4a), name
    // kept at +6, player at +2.
    void create_label() {
        if (joan::orig::as_dynarray(img::obj(kGameUnits))->empty_at(static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->unit_id)) != 0) return;
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(img::obj(kGameUnits))->ptr_at(static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->unit_id)));
        std::uint8_t* const lead = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_unit(unit)->leader());  // 0x004216d0
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_) != nullptr) return;
        if (jo::as_unit(unit)->layer == 0 && jo::self<jo::UnitDraw>(this)->unit_id != global<std::int16_t>(d::g_a47c36)) return;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->leads()) == 0 && lead == nullptr) return;  // 0x0044a660
        c_label_create.hit();
        void* const mem = operator_new(static_cast<std::uint32_t>(sizeof(jo::Billboard)));  // 0x37 on x86
        jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_) = mem != nullptr ? joan::orig::as_billboard(mem)->construct() : nullptr;
        char* const name = jo::self<jo::UnitDraw>(this)->label_text;  // the label text (0x40 bytes at +0x6)
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(name)), joan::orig::arg::str(&img::global<const char>(d::kEmpty)));  // strcpy ""
        char text[0x44];
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->leads()) != 0)
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(text)), joan::orig::arg::str(static_cast<const char*>(jo::as_unit(unit)->name_)));
        else if (lead != nullptr)
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(text)), joan::orig::arg::str(static_cast<const char*>(jo::as_unit(lead)->name_b)));
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(name)), joan::orig::arg::str(static_cast<const char*>(text)));
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(text)), joan::orig::arg::str(&img::global<const char>(d::s_sp_lparen)));  // strcat " ("
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(text)), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_misc(img::obj(d::g_misc))->num_to_str_590130(static_cast<std::int32_t>(jo::as_unit(unit)->level)))));  // 0x00590130 number
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(text)), joan::orig::arg::str(&img::global<const char>(d::s_rparen)));  // ")"
        jo::self<jo::UnitDraw>(this)->i32_2 = jo::as_unit(unit)->level;
        float pos[3];
        std::memcpy(pos, jo::as_unit(unit)->position_, sizeof pos);
        pos[2] = pos[2] + 96.0f;
        std::uint32_t p[3];
        std::memcpy(p, pos, sizeof p);
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_))->set_position(joan::orig::arg::u32(p[0]), joan::orig::arg::u32(p[1]), joan::orig::arg::u32(p[2]));  // 0x00635d40
        (void)joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(global<std::int16_t>(d::g_player_unit_recno))));  // (both sides use the same font)
        const std::int16_t w = joan::orig::as_font(img::obj(kFont))->measure(joan::orig::arg::str(static_cast<const char*>(text)), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_))->set_15(joan::orig::arg::bits(static_cast<float>(w)));  // 0x00635d80
        const std::int16_t h = joan::orig::arg::as<std::int16_t>(joan::orig::as_font(img::obj(kFont))->text_height(joan::orig::arg::i16(2), 1.0f));  // 0x006253f0
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_))->set_19(joan::orig::arg::bits(static_cast<float>(h)));  // 0x00635db0
        jo::self<jo::UnitDraw>(this)->label_->i16_1d = 0;
        jo::self<jo::UnitDraw>(this)->label_->owned = 1;
        void* const pmem = operator_new(static_cast<std::uint32_t>(sizeof(jo::Plane)));  // 0x11c on x86
        std::uint8_t* const plane = pmem != nullptr ? joan::orig::arg::as<std::uint8_t*>(joan::orig::as_plane(pmem)->construct(4u, 0u)) : nullptr;  // 0x005cbaf0
        jo::view<std::uint32_t>(jo::as_plane(plane)->list_kind) = 5;
        joan::orig::as_plane(plane)->init(0, 0, 0, 1u);       // 0x005cc7a0
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_))->set_plane(joan::orig::arg::mptr(plane));  // 0x00522940
        const std::uint32_t c = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(global<std::int16_t>(d::g_player_unit_recno)))) != 0 ? colour(0xf5, 0x41, 0x21, 0xff)
                                                                                                            : colour(0x4c, 0x7f, 0xff, 0xff);
        Font64 font;
        std::memcpy(&font, img::obj(kFont), sizeof font);
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_))->draw_text(font, joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::str(static_cast<const char*>(text)), joan::orig::arg::u32(c), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)), joan::orig::arg::i16(static_cast<std::int16_t>(0)));  // 0x00636040
        joan::orig::as_billboard(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_))->normalise();  // 0x00636aa0
        joan::orig::as_plane(plane)->invalidate(joan::orig::arg::u32(-1));       // 0x005cf920
        joan::orig::as_plane(plane)->sort_by_material(1);        // 0x005cf1d0
    }
};

#define U3(name, addr, fn) JOAN_REPLACE("game/units3", name, addr, fn)
U3("Billboard::set_plane", 0x00522940, &H::set_plane);
U3("UnitDraw::create_plate", 0x00520270, &H::create_plate);
U3("UnitDraw::create_label", 0x00520e80, &H::create_label);
#undef U3

}  // namespace game_overlays
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

struct H {
    std::uint32_t dtor_521340() {  // 0x00521340
        jo::view<std::uint16_t>(jo::self<jo::UnitDraw>(this)->unit_id) = 0;
        joan::orig::as_unit_draw(this)->plate_delete();                          // 0x004287b0
        joan::orig::as_unit_draw(this)->label_delete(); // 0x005213b0
        return 0;  // void callee; the caller drops the eax
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj521340::dtor", 0x00521340, &H::dtor_521340);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_units.cpp (game/units1)
namespace {
namespace game_units {

struct Obj {};

template <class R>
R call_virtual(void* self, std::int32_t index, std::uint32_t arg) {
    using Fn = R (Obj::*)(std::uint32_t);
    JOAN_MEMBER_POINTER_CHECK(Fn);
    Fn method{};
    std::memcpy(&method, &joan::call::vtable_of(self)[index], sizeof(void*));
    return (static_cast<Obj*>(self)->*method)(arg);
}

struct H {
    void label_delete() {  // 0x005213b0
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_) == nullptr) return;
        joan::orig::as_unit_draw(this)->detach_b();  // 0x005228e0
        void* const p = jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_);
        if (p != nullptr) call_virtual<void*>(p, 0, 1u);
        jo::self<jo::UnitDraw>(this)->label_ = nullptr;
    }
};

#define U1(name, addr, fn) JOAN_REPLACE("game/units1", name, addr, fn)
U1("UnitDraw::label_delete", 0x005213b0, &H::label_delete);
#undef U1

}  // namespace game_units
}  // namespace

// ---------------------------------------- from modules/game_units_3.cpp (game/units2)
namespace {
namespace game_units_3 {

using joan::hook::x87::Fpu;

using joan::call::import_call;

void* camera() { return global<void*>(d::g_camera); }

JOAN_COUNTER(c_label, "UnitDraw::label");

// The scale of an overlay at pos that keeps it a constant size on screen: the camera eye
// from its orbit frame (0x006140a0: +4 target, +0x10 pitch, +0x14 yaw, +0x1c distance),
// the ground plane (0x00617150), l1 = |eye - pos|, l2 = |eye - ground hit|.
struct Scale {
    float l1, l2;
};
Scale screen_scale(const float pos[3]) {
    const joan::hook::x87::Fpu fpu{true};
    std::uint32_t frame[10];
    joan::orig::as_camera(camera())->frame(joan::orig::arg::mptr(static_cast<void*>(frame)));  // 0x006140a0
    float f[10];
    std::memcpy(f, frame, sizeof f);
    float eye[3] = {f[1], f[2], f[3]};
    eye[2] = static_cast<float>(fpu.add(fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(f[5])))), f[7]), eye[2]));
    const float t = static_cast<float>(fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(f[5])))), f[7]));
    eye[0] = static_cast<float>(fpu.sub(eye[0], fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::cos(joan::orig::arg::f64(static_cast<double>(f[4])))), t)));
    eye[1] = static_cast<float>(fpu.sub(eye[1], fpu.mul(joan::orig::arg::as<double>(joan::orig::libc::sin(joan::orig::arg::f64(static_cast<double>(f[4])))), t)));
    float plane[8] = {};
    joan::orig::as_plane_equation(static_cast<void*>(plane))->init_425700();                          // 0x00425700 Plane4::Plane4
    joan::orig::as_camera(camera())->plane_617150(0, joan::orig::arg::mptr(static_cast<void*>(plane)));             // 0x00617150 ground plane
    const float seg[6] = {eye[0], eye[1], eye[2], pos[0], pos[1], pos[2]};
    float k = 0.0f;  // (the original leaves it unset when the ray misses the plane)
    joan::orig::line_plane(&k, joan::orig::arg::mptr(seg), joan::orig::arg::mptr(static_cast<const float*>(plane)));  // 0x0060e590
    const float hit[3] = {(seg[3] - seg[0]) * k + seg[0], (seg[4] - seg[1]) * k + seg[1], (seg[5] - seg[2]) * k + seg[2]};
    float a[3] = {eye[0] - pos[0], eye[1] - pos[1], eye[2] - pos[2]};
    float b[3] = {eye[0] - hit[0], eye[1] - hit[1], eye[2] - hit[2]};
    Scale s;
    s.l1 = v3d_float(d::iat_v3d_87_length, static_cast<void*>(a));  // length
    s.l2 = v3d_float(d::iat_v3d_87_length, static_cast<void*>(b));
    return s;
}

// Billboard (0x635xxx) setters through their thunks.
void bb_position(void* bb, const float p[3]) {
    std::uint32_t v[3];
    std::memcpy(v, p, sizeof v);
    joan::orig::as_billboard(bb)->set_position(joan::orig::arg::u32(v[0]), joan::orig::arg::u32(v[1]), joan::orig::arg::u32(v[2]));  // 0x00635d40
}
void bb_scale_y(void* bb, float v) { joan::orig::as_billboard(bb)->set_19(joan::orig::arg::bits(v)); }  // 0x00635db0
void bb_scale_x(void* bb, float v) { joan::orig::as_billboard(bb)->set_15(joan::orig::arg::bits(v)); }  // 0x00635d80
constexpr float kTiny = 0.01f;                                        // 0x3c23d70a

struct H {

    // 0x005215f0 UnitDraw::label: the name billboard (+0x4a, text +6) above the unit.
    void label() {
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(img::obj(kGameUnits))->empty_at_b(static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->unit_id))) != 0) return;
        if (global<std::int32_t>(d::g_control_mode) == 4 &&
            static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_player(img::obj(d::g_player_unit_recno))->group_contains(static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->unit_id)))) == 0) {  // 0x00453590
            joan::orig::as_unit_draw(this)->detach_b();  // 0x005228e0
            return;
        }
        c_label.hit();
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(img::obj(kGameUnits))->get_b(static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->unit_id)));
        char* const text = jo::self<jo::UnitDraw>(this)->label_text;  // the label text (0x40 bytes at +0x6)
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_) == nullptr) {
            void* const lead = joan::orig::arg::as<void*>(joan::orig::as_unit(unit)->leader());  // 0x004216d0
            if (lead != nullptr && joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(static_cast<const char*>(text)))) == 0) joan::orig::as_unit_draw(this)->create_label();  // strlen / 0x00520e80
        }
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_) == nullptr || global<std::uint32_t>(d::g_camera_lock_flag_a48ceb) == 0) return;
        const std::int16_t player = global<std::int16_t>(d::g_player_unit_recno);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(player))) != static_cast<std::int32_t>(jo::self<jo::UnitDraw>(this)->i16_46)) {
            jo::self<jo::UnitDraw>(this)->i16_46 = static_cast<std::int16_t>(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->hostile_to(joan::orig::arg::i16(player))));
            joan::orig::as_unit_draw(this)->label_delete();
            joan::orig::as_unit_draw(this)->create_label();
        }
        if (static_cast<std::int32_t>(jo::as_unit(unit)->level) != jo::self<jo::UnitDraw>(this)->i32_2) {
            joan::orig::as_unit_draw(this)->label_delete();
            joan::orig::as_unit_draw(this)->create_label();
        }
        std::uint8_t* const lead = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_unit(unit)->leader());
        if (lead == nullptr && joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(unit)->leads()) == 0) {
            joan::orig::as_unit_draw(this)->label_delete();
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(text)), joan::orig::arg::str(&img::global<const char>(d::kEmpty)));  // strcpy ""
        }
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_) == nullptr) return;
        if (lead != unit && joan::crt::stricmp(joan::orig::arg::str(static_cast<const char*>(text)), joan::orig::arg::str(static_cast<const char*>(jo::as_unit(lead)->name_b))) != 0) {  // _stricmp
            joan::orig::as_unit_draw(this)->label_delete();
            joan::orig::as_unit_draw(this)->create_label();
        }
        float pos[3];
        std::memcpy(pos, jo::as_unit(unit)->position_, sizeof pos);
        pos[2] = pos[2] + 96.0f;
        if (jo::view<std::uint32_t>(jo::as_unit(unit)->mount_id) != 0) pos[2] = pos[2] + 30.0f;
        bb_position(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_), pos);
        const Scale s = screen_scale(pos);
        const joan::hook::x87::Fpu fpu{true};
        const float q = s.l1 / s.l2;
        std::uint8_t* const bb = jo::view<std::uint8_t*>(jo::self<jo::UnitDraw>(this)->label_);
        bb_scale_y(bb, static_cast<float>(fpu.mul(fpu.mul(q, 0.0044), jo::as_billboard(bb)->height)));
        bb_scale_x(bb, static_cast<float>(fpu.mul(fpu.mul(q, 0.0044), jo::self<jo::UnitDraw>(this)->label_->width)));
        pos[2] = static_cast<float>(fpu.add(pos[2], fpu.mul(q, 0.064)));
        bb_position(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_), pos);
        if (s.l1 > 1000.0f && global<std::int32_t>(d::g_control_mode) != 4) {
            bb_scale_y(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_), kTiny);
            bb_scale_x(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_), kTiny);
        }
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/units2", name, addr, fn)
U2("UnitDraw::label", 0x005215f0, &H::label);
#undef U2

}  // namespace game_units_3
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    void unit_hide() {                                                                   // 0x00522850
        joan::orig::as_unit_draw(this)->detach_a();  // 0x00423540
        joan::orig::as_unit_draw(this)->detach_b();  // 0x005228e0
    }
    void unit_detach_b() {                                                               // 0x005228e0
        if (static_cast<std::int8_t>(jo::self<jo::UnitDraw>(this)->plate_attached) == 0) return;
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_) == nullptr) return;
        joan::orig::as_scene(jo::as_world(world())->scene)->remove_sorted(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_)));  // 0x005eded0
        jo::self<jo::UnitDraw>(this)->plate_attached = 0;
    }
    void unit_show() {                                                                   // 0x005221c0
        joan::orig::as_unit(this)->attach_a();  // 0x00421960
        joan::orig::as_unit(this)->attach_b();  // 0x005227f0
    }
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("UnitDraw::hide", 0x00522850, &H::unit_hide);
W1("UnitDraw::detach_b", 0x005228e0, &H::unit_detach_b);
W1("UnitDraw::show", 0x005221c0, &H::unit_show);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {

struct H {
    void attach_b() {  // 0x005227f0
        if (static_cast<std::int8_t>(jo::self<jo::UnitDraw>(this)->plate_attached) != 0) return;
        if (jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_) == nullptr) return;
        joan::orig::as_scene(jo::as_world(world())->scene)->register_material(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::UnitDraw>(this)->label_)));  // 0x005eddc0
        jo::self<jo::UnitDraw>(this)->plate_attached = 1;
    }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Unit::attach_b", 0x005227f0, &H::attach_b);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::UnitDraw::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b82_17::H::object53_ctor>(this)); }  // 0x520170
auto joan::orig::UnitDraw::destruct() -> void { joan::bind::member<&::game_b84_02::H::dtor_5201a0>(this); }  // 0x5201a0
auto joan::orig::UnitDraw::set_unit(std::int16_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b82_17::H::set_unit>(this, a0)); }  // 0x5201d0
auto joan::orig::UnitDraw::create_plate() -> void { joan::bind::member<&::game_overlays::H::create_plate>(this); }  // 0x520270
auto joan::orig::UnitDraw::create_label() -> void { joan::bind::member<&::game_overlays::H::create_label>(this); }  // 0x520e80
auto joan::orig::UnitDraw::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b85_06::H::dtor_521340>(this)); }  // 0x521340
auto joan::orig::UnitDraw::label_delete() -> void { joan::bind::member<&::game_units::H::label_delete>(this); }  // 0x5213b0
auto joan::orig::UnitDraw::label() -> void { joan::bind::member<&::game_units_3::H::label>(this); }  // 0x5215f0
auto joan::orig::UnitDraw::show() -> void { joan::bind::member<&::game_world::H::unit_show>(this); }  // 0x5221c0
auto joan::orig::Unit::attach_b() -> void { joan::bind::member<&::game_leaves80a::H::attach_b>(this); }  // 0x5227f0
auto joan::orig::UnitDraw::hide() -> void { joan::bind::member<&::game_world::H::unit_hide>(this); }  // 0x522850
auto joan::orig::UnitDraw::detach_b() -> void { joan::bind::member<&::game_world::H::unit_detach_b>(this); }  // 0x5228e0
auto joan::orig::Billboard::set_plane(void* a0) -> void { joan::bind::member<&::game_overlays::H::set_plane>(this, a0); }  // 0x522940
