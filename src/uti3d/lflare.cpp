// uti3d/lflare.cpp: functions of the original d:\ww\uti3d\lflare.cpp (re/attribution.csv).
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
#include "../x87.hpp"
#include <cmath>
#include <cstdarg>
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../display.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include "../original/cxx_eh.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_light1.cpp (game/light1)
//
//  * module game/light1:
//      LightSys::load_lights 0x004a5bf0 (ret 4): MAP\<name>.lgt and MAP\<name>.sca. The .lgt
//        gives the light count per set (+0x5b0) and the set count (+0x0); the sets (0x18c
//        bytes each, +0x2) get per light (0x1a bytes from +0x42) an enabled byte (+0x43), and
//        when enabled the set name (strncpy 0x40 into the set), a byte (+0x42), four ints as
//        floats (+0x44..+0x50) and two angles in degrees * pi/180 (+0x58, +0x54); then the
//        set's +0x12c block is initialised (0x00617ee0) and the .sca supplies 14 floats
//        (+0x178..+0x188, +0x168..+0x170, +0x174 = 100, +0x15c..+0x164 normalized).
//      LightSys::load_special 0x004a6840 (ret 0): same for MAP\special.lgt / .sca into the
//        +0x6 / +0x8 sets; the light count is a local and the enabled byte is not tested.
//      LightSys::load_day 0x004a7450 (ret 4): MAP\<name>.day: +0x4c4 count, the two flicker
//        chances / durations (+0x58c, +0x590 / 100, +0x59c, +0x5a0 / 100), three names
//        (+0x54c, +0x4cc, +0x50c) and the 10-byte day table (+0x4c8: id from 0x004a9120,
//        two floats).
//      LensFlares::init 0x00662640 (ret 0x14), LensFlares::clear 0x006626a0,
//      LensFlares::add 0x00662710 (ret 0x10; 0x19b-byte entries, realloc'd array).
//      TerrainBlock::normal 0x006645b0 (ret 0xc; height-map rotation of the up vector, or the
//        normal of the plane of the block's three corner heights; returns 1).
//      P2::sub 0x006639e0 (cdecl, explicit out pointer), PfdData::PfdData 0x00664fd0
//      (memset 0x38), PfdData::~ 0x00665000 (helper calls + member deletes),
//      P2::length_sq 0x0066a440, P2::copy_to 0x0066a4f0 (ret 4).
namespace {
namespace game_light1 {

using joan::hook::global;

using joan::call::thiscall;
constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kLflareCpp(d::s_d_ww_uti3d_lflare_cpp);    // "d:\ww\uti3d\lflare.cpp"

JOAN_COUNTER(c_flare_add, "LensFlares::add");

struct H {
    // 0x006626a0 LensFlares::clear() (ret 0). Returns eax as the original leaves it (this or 0); uintptr_t: the full
    // pointer on x64 (the only caller discards it).
    std::uintptr_t flare_clear() {
        joan::orig::as_lens_flares(this)->release_texture();  // 0x00662db0
        if (jo::view<void*>(jo::self<jo::LensFlares>(this)->entries) == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        joan::orig::as_mem(img::obj(kMemory))->del(jo::view<void*>(jo::self<jo::LensFlares>(this)->entries), img::str(kLflareCpp),
                                                   static_cast<std::int32_t>(global<std::int16_t>(d::g_a21670)) + 5);  // 0x0058ed70
        jo::view<void*>(jo::self<jo::LensFlares>(this)->entries) = nullptr;
        jo::self<jo::LensFlares>(this)->count = 0;
        return 0;  // (the only caller discards the result)
    }

    // 0x00662710 LensFlares::add(colour, w, scale, texture) (ret 0x10); returns the new entry.
    void* flare_add(std::uint32_t colour, std::int16_t w, std::uint32_t scale, const char* texture) {
        c_flare_add.hit();
        alignas(8) std::uint8_t entry[((sizeof(jo::FlareEntry) > 0x1a0 ? sizeof(jo::FlareEntry) : 0x1a0) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // ebp-0x1a0 (0x19b-byte FlareEntry on x86)
        joan::orig::as_flare_entry(entry)->construct();   // 0x00662800
        jo::as_flare_entry(entry)->colour = colour;
        jo::as_flare_entry(entry)->size = w;
        jo::view<std::uint32_t>(jo::as_flare_entry(entry)->scale) = scale;
        joan::orig::as_string(&jo::as_flare_entry(entry)->texture)->assign(joan::orig::arg::str(texture));  // 0x005a1440
        jo::self<jo::LensFlares>(this)->count = static_cast<std::int16_t>(jo::self<jo::LensFlares>(this)->count + 1);
        jo::view<void*>(jo::self<jo::LensFlares>(this)->entries) = joan::orig::as_mem(img::obj(kMemory))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::LensFlares>(this)->entries)), joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::LensFlares>(this)->count) * static_cast<std::int32_t>(sizeof(jo::FlareEntry))), joan::orig::arg::str(img::str(kLflareCpp)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a21674)) + 0xa));  // 0x0058eb30
        std::uint8_t* const slot = reinterpret_cast<std::uint8_t*>(&jo::self<jo::LensFlares>(this)->entries[static_cast<std::int32_t>(jo::self<jo::LensFlares>(this)->count) - 1]);
        return joan::orig::arg::as<void*>(joan::orig::as_flare_entry(slot)->assign(joan::orig::arg::mptr(static_cast<void*>(entry))));  // 0x00662830
    }
};

#define L1(name, addr, fn) JOAN_REPLACE("game/light1", name, addr, fn)
L1("LensFlares::clear", 0x006626a0, &H::flare_clear);
L1("LensFlares::add", 0x00662710, &H::flare_add);
#undef L1

}  // namespace game_light1
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
//
//  * module game/b82_14:
//      Video::read_sample 0x00685dd0 (ret 0xc): IWMSyncReader::GetNextSample on +0x24; a video
//        sample (stream +0xc) is copied bottom-up into the +4 texture (+0x58, LockRect /
//        UnlockRect), an audio sample (stream +0xe) is wrapped in a sound buffer (0x0048d120)
//        and queued on the +8 stream; *time = sample time / 10000. Returns 0 (no sample),
//        1 video, 2 audio, -1 other stream.
//      Video::has_audio_stream 0x00685990, Video::has_video_stream 0x00685940,
//      Video::has_audio 0x00685a30, Video::has_video 0x006859e0.
//      Zone::set_points 0x00663cb0 (ret 0xc; copies the point pointer list, bounding box),
//      Zone::link 0x00664130 (ret 0xc), PathZones::group 0x00675660 (connected components of
//      the zone links into +0x70), PathSpace::zone_count 0x0066b9f0,
//      PfdData::clear_grid 0x00665290, PfdData::free_10 0x00666d80,
//      PfdItem::scalar_deleting_destructor 0x006651e0 (ret 4),
//      Zone::vector_deleting_destructor 0x00675d70 (ret 4), Zones::~Zones 0x00675930,
//      Container::clear 0x00677e00.
//      Collisions::add 0x0068dbf0 (ret 0x14), Collisions::remove 0x0068dd80 (ret 8).
//      LensFlares::acquire_texture 0x00662d20, LensFlares::release_texture 0x00662db0,
//      FlareEntry::FlareEntry 0x00662800, FlareEntry::operator= 0x00662830 (ret 4).
//      List::~List 0x006893a0, Entry::~Entry 0x0068c800 (empty), Entry::flag 0x0068c940.
namespace {
namespace game_b82_14 {

using joan::hook::global;

struct H {
    // 0x00662800 FlareEntry::FlareEntry() (ret 0).
    void* entry_ctor() {
        joan::orig::as_string(&jo::self<jo::FlareEntry>(this)->texture)->construct();  // +0xa  // 0x005a1170
        return this;
    }

    // 0x00662830 FlareEntry::operator=(src) (ret 4).
    void* entry_assign(const std::uint8_t* src) {
        jo::self<jo::FlareEntry>(this)->colour = jo::as_flare_entry(src)->colour;
        jo::view<std::uint16_t>(jo::self<jo::FlareEntry>(this)->size) = jo::view<std::uint16_t>(jo::as_flare_entry(src)->size);
        jo::view<std::uint32_t>(jo::self<jo::FlareEntry>(this)->scale) = jo::view<std::uint32_t>(jo::as_flare_entry(src)->scale);
        joan::orig::as_string(&jo::self<jo::FlareEntry>(this)->texture)->assign(joan::orig::as_string(static_cast<const void*>(&jo::as_flare_entry(src)->texture)));  // 0x005a1400
        return this;
    }
};

// 0x00662db0 LensFlares::release_texture() (ret 0).
std::int32_t JOAN_CDECL release_texture() {
    const std::int32_t r = static_cast<std::int32_t>(global<std::int16_t>(d::g_lens_flare_shared_refcount));
    if (r == 0) return 0;
    global<std::int16_t>(d::g_lens_flare_shared_refcount) = static_cast<std::int16_t>(global<std::int16_t>(d::g_lens_flare_shared_refcount) - 1);
    if (global<std::int16_t>(d::g_lens_flare_shared_refcount) != 0) return r;
    const std::int32_t c = joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(img::obj(d::g_texture_array))->release(global<void*>(d::g_a6a26c)));  // 0x0060a550
    global<void*>(d::g_a6a26c) = nullptr;
    return c;
}

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("LensFlares::release_texture", 0x00662db0, &release_texture);
B82_14("FlareEntry::FlareEntry", 0x00662800, &H::entry_ctor);
B82_14("FlareEntry::operator=", 0x00662830, &H::entry_assign);
#undef B82_14

}  // namespace game_b82_14
}  // namespace

// ---------------------------------------- from modules/game_b99_09.cpp (game/b99_09)
namespace {
namespace game_b99_09 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    std::uintptr_t zero_14() {  // 0x006625f0 (returns this)
        std::memset(static_cast<void*>(this), i32{0}, joan::orig::arg::u32(static_cast<u32>(sizeof(jo::LensFlares))));  // memset 0x14 on x86
        return reinterpret_cast<std::uintptr_t>(this);
    }
};

#define B99(name, addr_, fn) JOAN_REPLACE("game/b99_09", name, addr_, fn)
B99("Struct14::zero", 0x006625f0, &H::zero_14);
#undef B99

}  // namespace game_b99_09
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07 {

struct H {
    // ------------------------------------------------------------------ 0x00662620
    void nop() {}
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("Obj::nop_662620", 0x00662620, &H::nop);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- from modules/game_light1.cpp (game/light1)
namespace {
namespace game_light1_2 {

using joan::call::thiscall;

struct H {
    // 0x00662640 LensFlares::init(max, a, b, cx, cy) (ret 0x14).
    std::uintptr_t flare_init(std::int16_t a6, std::uint32_t a8, std::uint32_t ac, std::int16_t a10, std::int16_t a12) {
        joan::orig::as_lens_flares(this)->acquire_texture();  // 0x00662d20
        jo::self<jo::LensFlares>(this)->steps = a6;
        jo::view<std::uint32_t>(jo::self<jo::LensFlares>(this)->fade_min) = a8;
        jo::view<std::uint32_t>(jo::self<jo::LensFlares>(this)->fade_max) = ac;
        jo::self<jo::LensFlares>(this)->centre_x = a10;
        jo::self<jo::LensFlares>(this)->centre_y = a12;
        // eax: this with ax = the last argument (uintptr_t: the whole register on x64).
        return (reinterpret_cast<std::uintptr_t>(this) & ~std::uintptr_t{0xffff}) | static_cast<std::uint16_t>(a12);
    }
};

#define L1(name, addr, fn) JOAN_REPLACE("game/light1", name, addr, fn)
L1("LensFlares::init", 0x00662640, &H::flare_init);
#undef L1

}  // namespace game_light1_2
}  // namespace

// ---------------------------------------- from modules/game_b111_13.cpp (game/b111_13)
namespace {
namespace game_b111_13 {

using joan::hook::global;
using joan::hook::x87::Fpu;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::img::obj;

using joan::img::str;

using joan::hook::x87::ftol;

using u32 = std::uint32_t;

i16 low16(i32 v) { return static_cast<i16>(static_cast<u16>(static_cast<u32>(v) & 0xffffu)); }

constexpr img::Addr kSprites(d::g_sprites_a46128);
constexpr img::Addr kScreenW = d::g_screen_w, kScreenH = d::g_screen_h;

JOAN_COUNTER(c_flare, "LensFlare::draw");

struct H {
    // ------------------------------------------------------------------------ 0x006628a0
    static i16 overshoot(i16 v, i32 limit) {
        i32 d = 0;
        if (static_cast<i32>(v) < 0) {
            d = -static_cast<i32>(v);
        } else if (static_cast<i32>(v) > limit) {
            d = static_cast<i32>(v) - limit;
        }
        return low16(d);
    }
    // centre + _ftol((p - centre) * scale), x87 fild int * float at PC24.
    static i16 scale_about(i16 p, i16 centre, float scale) {
        const i32 diff = static_cast<i32>(p) - static_cast<i32>(centre);
        const i32 t = joan::hook::x87::ftol(Fpu{true}.mul(static_cast<double>(diff), static_cast<double>(scale)));
        return low16(static_cast<i32>(low16(t)) + static_cast<i32>(centre));
    }

    void flare_draw(i16 x, i16 y, float k) {
        c_flare.hit();
        const i16 ox = overshoot(x, global<i32>(kScreenW));
        const i16 oy = overshoot(y, global<i32>(kScreenH));
        const i16 m = static_cast<i32>(ox) > static_cast<i32>(oy) ? ox : oy;
        const Fpu fpu{true};
        const float span = jo::self<jo::LensFlares>(this)->fade_max - jo::self<jo::LensFlares>(this)->fade_min;
        double t = fpu.div(static_cast<double>(span), static_cast<double>(static_cast<i32>(jo::self<jo::LensFlares>(this)->steps)));
        t = fpu.mul(t, static_cast<double>(static_cast<i32>(m)));
        t = fpu.add(t, static_cast<double>(jo::self<jo::LensFlares>(this)->fade_min));
        float v = static_cast<float>(t);  // fstp dword [ebp-0x14]
        if (v > jo::self<jo::LensFlares>(this)->fade_max) v = jo::self<jo::LensFlares>(this)->fade_max;
        const float fade = 1.0f - v;  // 0x009cf784
        alignas(4) u8 elem[((sizeof(jo::FlareEntry) > 0x19c ? sizeof(jo::FlareEntry) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // 0x19c on x86
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::LensFlares>(this)->count); ++i) {
            const u8* const src = reinterpret_cast<const u8*>(&jo::self<jo::LensFlares>(this)->entries[i]);  // stride 0x19b
            joan::orig::as_flare_entry(elem)->copy_construct(joan::orig::arg::mptr(src));  // 0x00662cb0 copy
            u32 colour = jo::as_flare_entry(elem)->colour;       // [ebp-0x1bc]
            const float alpha = static_cast<float>(static_cast<i32>(colour >> 24));
            const float scaled = alpha * fade * k;
            const i32 na = joan::hook::x87::ftol(static_cast<double>(scaled));
            colour = (colour & 0x00ffffffu) | (static_cast<u32>(na & 0xff) << 24);
            const float sc = jo::as_flare_entry(elem)->scale;
            const i16 cx = scale_about(x, jo::self<jo::LensFlares>(this)->centre_x, sc);
            const i16 cy = scale_about(y, jo::self<jo::LensFlares>(this)->centre_y, sc);
            // The flares' sizes are pixels of a 1024x768 screen: scaled like the rest of the mission's 2D by
            // [Display] UiScale (not part of the original game; positions are the sun's on screen and stay as they are).
            const float ui = joan::display::ui_scale();
            const i32 size = static_cast<i32>(std::lround(static_cast<float>(jo::as_flare_entry(elem)->size) * ui));
            const i16 x1 = low16(static_cast<i32>(cx) - size / 2);
            const i16 y1 = low16(static_cast<i32>(cy) - size / 2);
            const i16 x2 = low16(static_cast<i32>(x1) + size - 1);
            const i16 y2 = low16(static_cast<i32>(y1) + size - 1);
            if (static_cast<i32>(x1) < global<i32>(kScreenW) && static_cast<i32>(y1) < global<i32>(kScreenH) &&
                static_cast<i32>(x2) >= 0 && static_cast<i32>(y2) >= 0) {
                const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_string(&jo::as_flare_entry(elem)->texture)->c_str());  // 0x00417d90
                const joan::display::SelfScaled self_scaled;  // sized above: not "unscaled 2D"
                joan::orig::as_sprite_queue(img::obj(kSprites))->draw_named(joan::orig::arg::i16(static_cast<i32>(x1)), joan::orig::arg::i16(static_cast<i32>(y1)), 9u, joan::orig::arg::str(name), img::obj(d::g_bmp_res_lflare), joan::orig::arg::u32(colour), 4, 3, 3, 1, -1, -1, -1, -1, 0, joan::orig::arg::i16(static_cast<i32>(x2)), joan::orig::arg::i16(static_cast<i32>(y2)));  // 0x00633500
            }
        }
    }
};

#define B111(name, addr, fn) JOAN_REPLACE("game/b111_13", name, addr, fn)
B111("LensFlare::draw", 0x006628a0, &H::flare_draw);
#undef B111

}  // namespace game_b111_13
}  // namespace

// ---------------------------------------- from modules/game_b98_07.cpp (game/b98_07)
namespace {
namespace game_b98_07 {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------ 0x00662cb0
    void* rec_assign(u8* src) {
        jo::view<i32>(jo::self<jo::FlareEntry>(this)->colour) = jo::view<i32>(jo::as_flare_entry(src)->colour);
        jo::self<jo::FlareEntry>(this)->size = jo::as_flare_entry(src)->size;
        jo::view<i32>(jo::self<jo::FlareEntry>(this)->scale) = jo::view<i32>(jo::as_flare_entry(src)->scale);
        joan::orig::as_string(&jo::self<jo::FlareEntry>(this)->texture)->construct_copy(joan::orig::as_string(static_cast<void*>(&jo::as_flare_entry(src)->texture)));  // +0xa  // 0x005a11f0
        return this;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_07", name, addr, fn)
B98("Rec::assign", 0x00662cb0, &H::rec_assign);
#undef B98

}  // namespace game_b98_07
}  // namespace

// ---------------------------------------- from modules/game_b82_14.cpp (game/b82_14)
namespace {
namespace game_b82_14_2 {

using joan::hook::global;

using joan::call::thiscall;

using joan::img::obj;

using joan::img::str;

// 0x00662d20 LensFlares::acquire_texture() (cdecl / no args, ret 0).
std::int32_t JOAN_CDECL acquire_texture() {
    std::int32_t r = static_cast<std::int32_t>(global<std::int16_t>(d::g_lens_flare_shared_refcount));
    if (r == 0) {
        void* const tm = img::obj(d::g_texture_array);
        const std::uint32_t saved = joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(tm)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(tm)->set_mode(1u);                                         // 0x0060a740
        global<void*>(d::g_a6a26c) =
            joan::orig::as_texture_manager(tm)->load(joan::orig::arg::str(&img::global<const char>(d::s_LFlare)), 1, 0u);  // 0x006099c0 "LFlare"
        joan::orig::as_texture_manager(tm)->set_mode(joan::orig::arg::u32(saved));  // 0x0060a740
        r = 0;  // (the only caller discards the result)
    }
    global<std::int16_t>(d::g_lens_flare_shared_refcount) = static_cast<std::int16_t>(global<std::int16_t>(d::g_lens_flare_shared_refcount) + 1);
    return r;
}

#define B82_14(name, addr, fn) JOAN_REPLACE("game/b82_14", name, addr, fn)
B82_14("LensFlares::acquire_texture", 0x00662d20, &acquire_texture);
#undef B82_14

}  // namespace game_b82_14_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::LensFlares::construct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_09::H::zero_14>(this)); }  // 0x6625f0
auto joan::orig::LensFlares::destruct() -> void { joan::bind::member<&::game_b94_07::H::nop>(this); }  // 0x662620
auto joan::orig::LensFlares::init(std::int16_t a0, std::uint32_t a1, std::uint32_t a2, std::int16_t a3, std::int16_t a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_light1_2::H::flare_init>(this, a0, a1, a2, a3, a4)); }  // 0x662640
auto joan::orig::LensFlares::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_light1::H::flare_clear>(this)); }  // 0x6626a0
auto joan::orig::LensFlares::add(std::uint32_t a0, std::int16_t a1, std::uint32_t a2, const char* a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_light1::H::flare_add>(this, a0, a1, a2, a3)); }  // 0x662710
auto joan::orig::FlareEntry::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_14::H::entry_ctor>(this)); }  // 0x662800
auto joan::orig::FlareEntry::assign(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_14::H::entry_assign>(this, a0)); }  // 0x662830
auto joan::orig::LensFlares::draw(std::int16_t a0, std::int16_t a1, float a2) -> void { joan::bind::member<&::game_b111_13::H::flare_draw>(this, a0, a1, a2); }  // 0x6628a0
auto joan::orig::FlareEntry::copy_construct(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_07::H::rec_assign>(this, a0)); }  // 0x662cb0
