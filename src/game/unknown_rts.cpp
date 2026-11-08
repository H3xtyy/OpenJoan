// game/unknown_rts.cpp: functions whose original source file is unknown (formerly game/_unplaced/rts.cpp;
// re/attribution.csv: between ww/camres.cpp and ww/cpuusage.cpp). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include <cmath>

#include "../registry.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../interfac/view_range.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::hook::global;
using joan::call::cdecl_call;
using joan::call::import_call;

using joan::hook::x87::Fpu;

using i32 = std::int32_t;


struct Vec3 {
    float x, y, z;
};

struct Obj {};

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;
using i16 = std::int16_t;

void* const kCamMan = obj(d::g_camera_man);

using joan::hook::x87::ftol;
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01 {

using joan::call::cdecl_call;

void* const kArrays = obj(d::g_misc);     // 0x00591070 remove, 0x005911a0 search, 0x00590660 random

struct alignas(4) Path {  // 0x194 bytes in the frame (ebp-0x1a4 .. ebp-0x10)
    std::uint8_t bytes[0x194];
};

JOAN_COUNTER(c_cursor, "Panel::load_cursor");

struct H {
    // ------------------------------------------------------------ 0x0043f7c0
    void load_cursor() {
        c_cursor.hit();
        if (jo::view<void*>(jo::self<jo::Player>(this)->cam_zones) != nullptr) return;
        void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::CamZones)));  // operator new (0x4c on x86)
        void* made = nullptr;
        if (mem != nullptr) made = joan::orig::as_camera_zones(mem)->construct();  // 0x00425dc0
        jo::view<void*>(jo::self<jo::Player>(this)->cam_zones) = made;
        joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones))->init();  // 0x00425e20
        Path path;                                            // ebp-0x1a4
        joan::orig::as_string(&path)->construct();            // 0x005a1170
        joan::orig::as_string(&path)->assign(str(d::s_RESOURCE));            // 0x005a1440 "RESOURCE\"
        joan::orig::as_string(&path)->append(str(d::s_rts_cm));                // 0x005a1550 "rts.cm"
        const char* name = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
        if (joan::orig::as_misc(kArrays)->is_file_exist(joan::orig::arg::str(name)) == 0) return;  // 0x00590b80
        jo::view<std::uint32_t>(jo::self<jo::Player>(this)->cam_zones->origin[0]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Player>(this)->cam_zones->origin[1]) = 0;
        jo::view<std::uint32_t>(jo::self<jo::Player>(this)->cam_zones->origin[2]) = 0;
        name = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
        joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones))->load(name);  // 0x00426330

    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Panel::load_cursor", 0x0043f7c0, &H::load_cursor);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using joan::hook::global;

using i32 = std::int32_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;
using i16 = std::int16_t;

// CRT (called directly as in the original)
void crt_strcpy(void* d, const void* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

using u32 = std::uint32_t;


JOAN_COUNTER(c_reset43f, "Obj::reset_43f940");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x0043f940
    std::uintptr_t reset_43f940() {  // EAX = this (pointer-wide on x64)
        c_reset43f.hit();
        void* const units = obj(d::g_units);
        joan::orig::as_parser(obj(d::g_mouse))->drain();  // 0x005959c0
        jo::self<jo::Player>(this)->rtsicon_res = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_rtsicon));  // the BmpRes 0x00a48f50
        crt_strcpy(jo::self<jo::Player>(this)->point_name, str(d::s_POINT00));  // "POINT00"
        jo::self<jo::Player>(this)->point_kind = 0;
        if (jo::self<jo::Player>(this)->rts_icon == nullptr) {
            void* const res = obj(d::g_texture_array);
            const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(res)->mode());  // 0x0060a720
            joan::orig::as_texture_manager(res)->set_mode(u32{1});            // 0x0060a740
            jo::self<jo::Player>(this)->rts_icon = joan::orig::as_texture_manager(res)->load(str(d::s_RTSICON), i32{1}, joan::orig::arg::u32(i32{0}));  // 0x006099c0
            joan::orig::as_texture_manager(res)->set_mode(joan::orig::arg::u32(saved));
        }
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) == 0) {
            void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));
            joan::orig::as_unit(u)->play_idle(i32{0}, i32{1});  // 0x00508b90
        }
        joan::orig::as_camera(global<void*>(d::g_camera))->set_light(joan::orig::arg::mptr(i32{0}));  // 0x006169d0
        global<u32>(d::g_far_clip) = 0x45fa0000;                          // 8000.0f
        joan::orig::as_marker(obj(d::g_mini_map))->set_word_f(joan::orig::arg::u16(i32{0}));            // 0x0044cf50
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) == 0) {
            void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));
            jo::view<u16>(jo::as_unit(u)->layer) = 1;
            jo::as_unit(u)->combo_time = 0;
            if (jo::as_unit(u)->actor != nullptr) {
                void* p = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(u)->actor)->record)));  // 0x006af310
                if (p != nullptr) {
                    jo::view<u32>(jo::as_shot(p)->type) = 1;
                    const i16 w = static_cast<i16>(jo::as_shot(p)->type);
                    const u32 v27a = jo::as_shot(p)->u32_27a;
                    joan::orig::as_anim_table(obj(d::g_physic_sys))->restart_6af370(joan::orig::arg::u32(jo::view<u32>(jo::as_actor(jo::as_unit(u)->actor)->record)), joan::orig::arg::u32(v27a), joan::orig::arg::u32(w), joan::orig::arg::u32(i16{0}), joan::orig::arg::u32(i16{2}), joan::orig::arg::u32(i16{1}));  // 0x006af370
                }
            }
        }
        u8 snapshot[0x2c];
        std::memset(snapshot, 0, sizeof(snapshot));
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(snapshot));  // 0x006140a0 (writes 40 bytes)
        jo::self<jo::Player>(this)->rts_active = 1;
        jo::self<jo::Player>(this)->prev_rts_active = jo::self<jo::Player>(this)->rts_active;
        for (i32 i = 0;; ++i) {
            const i32 team = jo::self<jo::Player>(this)->group;
            if (!(i < jo::view<i8>(jo::self<jo::Player>(this)->group_counts[team]))) break;
            auto id_at = [&]() { return jo::self<jo::Player>(this)->group_units[jo::self<jo::Player>(this)->group * 100 + i]; };
            if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(id_at())) != 0) continue;
            void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(id_at()));
            if (joan::orig::arg::as<u8>(joan::orig::as_unit(u)->dead()) != 0) continue;  // 0x0042d0e0
            void* u2 = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(id_at()));
            joan::orig::as_unit(u2)->attach_weapon_model(i32{1});  // 0x00519250
        }
        return reinterpret_cast<std::uintptr_t>(this);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Obj::reset_43f940", 0x0043f940, &H::reset_43f940);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

struct H {
    // ------------------------------------------------------------------ 0x0043fbe0
    void call_508b90(void* unit) { joan::orig::as_unit(unit)->play_idle(0, 1); }  // 0x00508b90
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::call_508b90", 0x0043fbe0, &H::call_508b90);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b109_02.cpp (game/b109_02)
namespace {
namespace game_b109_02 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i32 = std::int32_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;
using i16 = std::int16_t;

// CRT (called directly as in the original)
void crt_strcpy(void* d, const void* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

using u32 = std::uint32_t;

float as_float(u32 bits) {
    float f;
    std::memcpy(&f, &bits, sizeof(f));
    return f;
}
u32 as_bits(float f) {
    u32 b;
    std::memcpy(&b, &f, sizeof(b));
    return b;
}
// fld m32 / fstp m32 round trip (an SNaN gets quieted): src/fp/bits.hpp.
using joan::fp::quiet;

// 40-byte camera snapshot passed by value to 0x00453930.
struct Snap {
    u32 d[10];
};
static_assert(sizeof(Snap) == 40, "snap");

JOAN_COUNTER(c_rts, "Obj::rts_init_43fc90");

struct H {
    // 0x0043fc90
    std::uintptr_t rts_init() {  // EAX = this / the dead count
        c_rts.hit();
        void* const units = obj(d::g_units);
        joan::orig::as_parser(obj(d::g_mouse))->drain();  // 0x005959c0
        jo::self<jo::Player>(this)->rtsicon_res = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_rtsicon));  // the BmpRes 0x00a48f50
        crt_strcpy(jo::self<jo::Player>(this)->point_name, str(d::s_POINT00));  // "POINT00"
        jo::self<jo::Player>(this)->point_kind = 0;
        if (jo::self<jo::Player>(this)->rts_icon == nullptr) {
            void* const res = obj(d::g_texture_array);
            const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(res)->mode());  // 0x0060a720
            joan::orig::as_texture_manager(res)->set_mode(u32{1});            // 0x0060a740
            jo::self<jo::Player>(this)->rts_icon = joan::orig::as_texture_manager(res)->load(str(d::s_RTSICON), i32{1}, joan::orig::arg::u32(i32{0}));  // 0x006099c0 RTSICON
            joan::orig::as_texture_manager(res)->set_mode(joan::orig::arg::u32(saved));
        }
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) == 0) {  // 0x00428b70
            void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));  // 0x0041b3c0
            joan::orig::as_unit(u)->play_idle(i32{0}, i32{1});             // 0x00508b90
        }
        joan::orig::as_camera(global<void*>(d::g_camera))->set_light(joan::orig::arg::mptr(i32{0}));  // 0x006169d0
        global<u32>(d::g_far_clip) = 0x45fa0000;                          // 8000.0f
        joan::orig::as_marker(obj(d::g_mini_map))->set_word_f(joan::orig::arg::u16(i32{0}));            // 0x0044cf50
        if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_))) == 0) {
            void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));
            jo::view<u16>(jo::as_unit(u)->layer) = 1;
            jo::as_unit(u)->combo_time = 0;
            if (jo::as_unit(u)->actor != nullptr) {
                void* p = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::as_unit(u)->actor)->record)));  // 0x006af310
                if (p != nullptr) jo::view<u32>(jo::as_shot(p)->type) = 1;
            }
        }
        Snap s38{}, s60{};
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(&s38);  // 0x006140a0 (writes 40 bytes)
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(&s60);
        void* const paths = jo::self<jo::Player>(this)->cam_zones->tracks;
        const i32 init_id = joan::orig::arg::as<i32>(joan::orig::as_cam_tracks(paths)->find(str(d::s_rts_init)));  // 0x00429c70 "rts_init"
        const i32 play_id =
            joan::orig::arg::as<i32>(joan::orig::as_cam_tracks(jo::self<jo::Player>(this)->cam_zones->tracks)->find(str(d::s_rts_play)));  // "rts_play"
        void* const play = joan::orig::arg::as<void*>(joan::orig::as_tracks(jo::self<jo::Player>(this)->cam_zones->tracks)->get(joan::orig::arg::i16(static_cast<i16>(play_id))));  // 0x00428b30
        void* const init = joan::orig::arg::as<void*>(joan::orig::as_tracks(jo::self<jo::Player>(this)->cam_zones->tracks)->get(joan::orig::arg::i16(static_cast<i16>(init_id))));
        u32 v = jo::view<u32>(jo::self<jo::Player>(this)->zoom_dist);
        while (as_float(v) > global<float>(d::g_a0bbf0)) {
            if (joan::orig::arg::as<i8>(joan::orig::as_player(this)->corners_on_map(s60)) != 0) break;  // 0x00453930
            v = as_bits(as_float(v) - global<float>(d::flt_200));
        }
        const float k = global<float>(d::flt_0_017453292);
        for (i32 i = 0; i < jo::as_cam_track(play)->key_count; ++i) {
            jo::CamKey& pk = jo::as_cam_track(play)->keys[i];
            const jo::CamKey& ik = jo::as_cam_track(init)->keys[i];
            jo::view<u32>(pk.time) = jo::view<u32>(ik.time);
            jo::view<u32>(pk.point[0]) = s38.d[1];
            jo::view<u32>(pk.point[1]) = s38.d[2];
            pk.point[2] = as_float(s38.d[3]) + ik.point[2];
            pk.yaw = as_float(s38.d[4]) / k;
            jo::view<u32>(pk.pitch) = jo::view<u32>(ik.pitch);
            pk.roll = as_float(s38.d[6]) / k;
            // fld a; fcomp v; test ah,1: a < v OR unordered -> a (copied through fld/fstp, which
            // quiets a signalling NaN), else v (raw mov).
            const u32 a = jo::view<u32>(ik.zoom);
            jo::view<u32>(pk.zoom) = !(as_float(a) >= as_float(v)) ? quiet(a) : v;
        }
        jo::view<u32>(jo::as_cam_track(play)->keys[0].zoom) = s38.d[7];
        jo::as_cam_track(play)->keys[0].pitch = as_float(s38.d[5]) / k;
        jo::view<u32>(jo::as_cam_track(play)->keys[jo::as_cam_track(play)->key_count - 1].zoom) = v;
        joan::orig::as_cam_tracks(jo::self<jo::Player>(this)->cam_zones->tracks)->prepare(joan::orig::arg::i32(play_id));  // 0x00429d00
        void* const cam = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(i32{4}));  // 0x00563c80
        joan::orig::as_camera_motion(cam)->start(&s60, &s60);                            // 0x00423ec0
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(i32{2});                    // 0x00429860
        joan::orig::as_camera_zones(jo::self<jo::Player>(this)->cam_zones)->start_track(str(d::s_rts_play), joan::orig::arg::u32(global<u32>(d::g_now)));  // 0x00428850
        jo::self<jo::Player>(this)->rts_active = 0;
        jo::self<jo::Player>(this)->prev_rts_active = jo::self<jo::Player>(this)->rts_active;
        joan::orig::as_group(this)->release_all();  // 0x00451430
        jo::self<jo::Player>(this)->group_counts[0] = 0;
        jo::view<u8>(jo::self<jo::Player>(this)->group) = 0;
        jo::self<jo::Player>(this)->selected_hero = 0;
        global<u32>(d::g_a5704d) = 0;
        const i32 dead = joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));
        if (dead != 0) return static_cast<u32>(dead);
        void* const me = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)));
        auto list = [&]() { return &jo::self<jo::Player>(this)->group_units[jo::self<jo::Player>(this)->group * 100]; };  // 100 units per group
        auto count = [&]() -> u8& { return jo::self<jo::Player>(this)->group_counts[jo::self<jo::Player>(this)->group]; };
        list()[static_cast<i8>(count())] = jo::view<u32>(jo::self<jo::Player>(this)->unit_id_);
        count() = static_cast<u8>(count() + 1);
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(me)->leads()) != 0) {  // 0x0044a660
            for (i32 k2 = 0; k2 < jo::as_group(jo::as_unit(me)->group)->count; ++k2) {
                if (!(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0]) < 100)) break;
                const i32 id = jo::as_group(jo::as_unit(me)->group)->members[k2];
                if (joan::orig::arg::as<i8>(joan::orig::as_unit(this)->can_target_453620(joan::orig::arg::i32(id))) == 0) continue;  // 0x00453620
                jo::view<i32>(list()[static_cast<i8>(count())]) = jo::as_group(jo::as_unit(me)->group)->members[k2];
                count() = static_cast<u8>(count() + 1);
            }
        }
        for (i32 i = 0; i < static_cast<i8>(count()); ++i) {
            if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(list()[i])) != 0) continue;
            void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(list()[i]));
            if (joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0) continue;  // 0x0042d0e0
            void* u2 = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(list()[i]));
            joan::orig::as_unit(u2)->attach_weapon_model(i32{1});  // 0x00519250
        }
        jo::self<jo::Player>(this)->selected_commander_id = jo::view<u32>(jo::self<jo::Player>(this)->unit_id_);
        return reinterpret_cast<std::uintptr_t>(this);
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_02", name, addr, fn)
B("Obj::rts_init_43fc90", 0x0043fc90, &H::rts_init);
#undef B

}  // namespace game_b109_02
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

void* const kCamera = obj(d::g_camera_man);

void* const kGameUnits = obj(d::g_units);  // unit manager
void* const kScene = obj(d::g_texture_array);

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

static_assert(sizeof(Vec3) == 12);
struct Track {  // 0x28-byte camera state (0x006140a0 copies +0xa0 .. +0xc8 of the camera)
    std::uint8_t bytes[0x28];
};
static_assert(sizeof(Track) == 0x28);

JOAN_COUNTER(c_start_rts, "Game::start_rts");

// Unit reset shared by 0x004404c0's player unit and its group members.
void release_carrier(std::uint8_t* unit) {
    if (jo::as_unit(unit)->orders != nullptr) {
        joan::orig::as_action(jo::as_unit(unit)->orders)->dispatch(joan::orig::arg::mptr(static_cast<void*>(unit)));  // 0x004bb660
        joan::orig::dynarray(jo::as_unit(unit)->orders)->zap(1);                  // 0x00584580
    }
}
void order_clear(std::uint8_t* unit) {
    joan::orig::as_unit_task(&jo::as_unit(unit)->task)->set(0u, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 0u, 0u, 0u, 0u);  // 0x004e4520
}

struct H {
    // ---- 0x004404c0 Game::start_rts --------------------------------------------------------
    void start_rts(std::int8_t play) {
        c_start_rts.hit();
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_world(this)->finish_level()) != 0) return;  // 0x00447860
        if (jo::self<jo::Player>(this)->rts_icon != nullptr) {
            joan::orig::as_texture_manager(kScene)->release(jo::self<jo::Player>(this)->rts_icon);  // 0x0060a550
            jo::self<jo::Player>(this)->rts_icon = nullptr;
        }
        joan::optmenu::clamp_view_ranges();  // widened draw distance range (interfac/view_range.hpp)
        global<float>(d::g_far_clip) = static_cast<float>(global<std::int32_t>(d::g_cfg_far_limit));
        if (global<void*>(d::g_camera) == nullptr || jo::self<jo::Player>(this)->cam_zones == nullptr) {
            joan::orig::as_camera_man(kCamera)->set_mode(0);  // 0x00429860
            jo::self<jo::Player>(this)->rts_active = 4;
            return;
        }
        if (play == 0) {
            joan::orig::as_camera(global<void*>(d::g_camera))->set_light(joan::orig::arg::mptr(1));  // 0x006169d0
            joan::orig::as_camera_man(kCamera)->set_mode(0);                    // 0x00429860
            jo::self<jo::Player>(this)->rts_active = 4;
            return;
        }
        Track cam;  // ebp-0x2c
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(static_cast<void*>(&cam)));  // 0x006140a0
        if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->selected_hero) != 0) {
            std::uint32_t hero = 0;
            const char* name = nullptr;
            switch (static_cast<std::uint32_t>(static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->selected_hero)) - 1)) {
                case 0: name = str(d::s_JOAN_OF_ARC); break;  // "JOAN_OF_ARC"
                case 1: name = str(d::s_JEAN_DE_METZ); break;  // "JEAN_DE_METZ"
                case 2: name = str(d::s_LAHIRE_9cfb3c); break;  // "LAHIRE"
                case 3: name = str(d::s_ALENCON_9cfb30); break;  // "ALENCON"
                case 4: name = str(d::s_ORLEAN); break;  // "ORLEAN"
                case 5: name = str(d::s_charlie7); break;  // "charlie7"
                case 6: name = str(d::s_arthur_de_richemont); break;  // "arthur_de_richemont"
                case 7: name = str(d::s_FLAVY); break;  // "FLAVY"
                default: break;
            }
            if (name != nullptr) {
                const std::uint32_t key = joan::orig::arg::as<std::uint32_t>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(name)));  // 0x005270b0
                hero = joan::orig::arg::as<std::uint32_t>(joan::orig::as_units(kGameUnits)->find_by_3b(joan::orig::arg::i32(key)));                           // 0x0051fd50
            }
            if (joan::orig::arg::as<std::int32_t>(joan::orig::unit::is_hero(joan::orig::arg::i32(hero))) != 0) {                                 // 0x0042ff90
                joan::orig::as_player(this)->select_hero(static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::Player>(this)->selected_hero)));  // 0x00432300
            }
        }
        if (unit_missing(jo::self<jo::Player>(this)->unit_id_) != 0) {
            jo::self<jo::Player>(this)->rts_active = 2;
            return;
        }
        std::uint8_t* const unit = unit_get(jo::self<jo::Player>(this)->unit_id_);
        jo::view<std::uint16_t>(jo::as_unit(unit)->layer) = 0;
        jo::as_unit(unit)->combo_time = 0;
        release_carrier(unit);
        if (jo::as_unit(unit)->task.kind != 0 || jo::as_unit(unit)->task.target == 0) {
            std::memcpy(&jo::as_unit(unit)->task_next, &jo::as_unit(unit)->task, sizeof(jo::Unit::task));  // rep movsd x6 + movsb
            order_clear(unit);
        }
        if (jo::as_unit(unit)->group != nullptr) {
            for (std::int32_t j = static_cast<std::int32_t>(jo::as_group(jo::as_unit(unit)->group)->count) - 1; j >= 0; --j) {
                const std::int32_t id = jo::as_group(jo::as_unit(unit)->group)->members[j];
                if (unit_missing(id) != 0) continue;
                std::uint8_t* const member = unit_get(static_cast<std::int32_t>(jo::as_group(jo::as_unit(unit)->group)->members[j]));
                release_carrier(member);
                order_clear(member);
            }
        }
        if (jo::as_unit(unit)->actor != nullptr) {
            void* const r = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_actor(jo::as_unit(unit)->actor)->record)));  // 0x006af310
            if (r != nullptr) jo::view<std::uint32_t>(jo::as_shot(r)->type) = 0;
        }
        Vec3 pos;  // ebp-0x4c
        joan::orig::as_unit(unit)->position(joan::orig::arg::mptr(static_cast<void*>(&pos)));  // 0x004fedb0
        void* const tracks = jo::view<void*>(jo::self<jo::Player>(this)->cam_zones->tracks);
        const std::uint32_t aim = joan::orig::arg::as<std::uint32_t>(joan::orig::as_cam_tracks(tracks)->find(str(d::s_rts_aim)));   // 0x00429c70 "rts_aim"
        const std::uint32_t play_id = joan::orig::arg::as<std::uint32_t>(joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones->tracks))->find(str(d::s_rts_play)));  // "rts_play"
        std::uint8_t* const dst = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones->tracks))->get(joan::orig::arg::i16(static_cast<std::int16_t>(play_id))));  // 0x00428b30
        std::uint8_t* const src = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_tracks(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones->tracks))->get(joan::orig::arg::i16(static_cast<std::int16_t>(aim))));
        const float* const c = reinterpret_cast<const float*>(cam.bytes);  // ebp-0x2c
        const joan::hook::x87::Fpu fpu{true};
        for (std::int32_t i = 0; i < jo::as_cam_track(dst)->key_count; ++i) {
            jo::CamKey& d = jo::as_cam_track(dst)->keys[i];
            const jo::CamKey& s = jo::as_cam_track(src)->keys[i];
            jo::view<std::uint32_t>(d.time) = jo::view<std::uint32_t>(s.time);
            const float t1 = static_cast<float>(fpu.div(static_cast<double>(i), static_cast<double>(jo::as_cam_track(dst)->key_count - 1)));
            d.point[0] = joan::orig::arg::as<float>(joan::orig::as_world(this)->lerp(joan::orig::arg::f32(c[1]), joan::orig::arg::f32(pos.x), joan::orig::arg::f32(t1)));  // 0x00447930
            const float t2 = static_cast<float>(fpu.div(static_cast<double>(i), static_cast<double>(jo::as_cam_track(dst)->key_count - 1)));
            d.point[1] = joan::orig::arg::as<float>(joan::orig::as_world(this)->lerp(joan::orig::arg::f32(c[2]), joan::orig::arg::f32(pos.y), joan::orig::arg::f32(t2)));
            d.point[2] = c[3] + s.point[2];
            d.yaw = c[4] / global<float>(d::flt_0_017453292);
            jo::view<std::uint32_t>(d.pitch) = jo::view<std::uint32_t>(s.pitch);
            d.roll = c[6] / global<float>(d::flt_0_017453292);
            d.zoom = c[7] + s.zoom;
        }
        std::memcpy(&jo::as_cam_track(dst)->keys[0].zoom, &c[7], 4);
        joan::orig::as_cam_tracks(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones->tracks))->prepare(joan::orig::arg::i32(play_id));  // 0x00429d00
        Vec3 delta;  // ebp-0x6c
        delta.x = pos.x - c[1];
        delta.y = pos.y - c[2];
        delta.z = 0.0f;
        if (static_cast<float>(import_call<double>(d::iat_v3d_87_length, static_cast<const void*>(&delta))) > global<float>(d::flt_200)) {  // length > 200
            joan::orig::as_camera_man(kCamera)->set_mode(2);                                     // 0x00429860
            void* const motion = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamera)->motion(2));         // 0x00563c80
            joan::orig::as_camera_motion(motion)->start(joan::orig::arg::mptr(static_cast<void*>(&cam)), joan::orig::arg::mptr(static_cast<void*>(&cam)));  // 0x00423ec0
            joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones))->start_track(str(d::s_rts_play), joan::orig::arg::u32(global<std::uint32_t>(d::g_now)));  // 0x00428850
        }
        jo::self<jo::Player>(this)->rts_active = 2;
        global<std::uint32_t>(d::g_a5704d) = 0;
        joan::orig::as_group(this)->release_all();  // 0x00451430
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Game::start_rts", 0x004404c0, &H::start_rts);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b110_15.cpp (game/b110_15)
namespace {
namespace game_b110_15 {

void* const kKinds = obj(d::g_types);

void* const kRandom = obj(d::g_misc);

void* const kPlayer = obj(d::g_player_unit_recno);

void* const kKeys = obj(d::g_keys_a48820);

struct alignas(4) Text {  // 0x005a11a0 / 0x005a1170 path string (0x194 bytes on the stack)
    u8 b[0x194];
};

using u32 = std::uint32_t;

template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}
static_assert(sizeof(Vec3) == 12, "Vec3");
struct CamState {  // 0x006140a0 output, [ebp-0x21c] .. [ebp-0x1f4] (10 dwords)
    u32 d[10];
};

void* const kUnits = obj(d::g_units);
void* const kSfx = obj(d::g_sfx_player);
void* const kInput = obj(d::g_mouse);
void* const kMinimap = obj(d::g_mini_map);
void* const kCastle = obj(d::g_popup_a52df8);
void* const kPlaces = obj(d::g_places);

JOAN_COUNTER(c_input, "Player::handle_input");

// ---------------------------------------------------------------------------- shared helpers
bool dead(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0; }  // 0x00428b70
u8* unit(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
bool busy(const void* u) { return joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0; }      // 0x0042d0e0 (al)
void text_set(Text* t, const char* s) { joan::orig::as_string(t)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void text_add(Text* t, const char* s) { joan::orig::as_string(t)->append(joan::orig::arg::str(s)); }  // 0x005a1550
void text_num(Text* t, i32 n) { joan::orig::as_string(t)->append(joan::orig::arg::i32(n)); }  // 0x005a15d0
const char* c_str(Text* t) { return joan::orig::arg::as<const char*>(joan::orig::as_string(t)->c_str()); }  // 0x00417d90
i32 rnd(i32 a, i32 b) { return joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->random_range(joan::orig::arg::i32(a), joan::orig::arg::i32(b))); }   // 0x00590690
char* strncpy_(char* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
bool ne_ordered(double a, double b) { return a < b || a > b; }  // fcomp + test ah,0x40 je
u32 flag3() { return (global<u16>(d::g_key_modifiers) & 3u) == 0 ? 1u : 0u; }  // neg / sbb / inc

struct H {
    // ======================================================================== 0x00440b30
    i32 grp() { return static_cast<i32>(jo::self<jo::Player>(this)->group); }
    i32 grp_count() { return static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[grp()])); }
    i32 grp_id(i32 i) { return jo::view<i32>(jo::self<jo::Player>(this)->group_units[grp() * 100 + i]); }
    // Live group member with +0x104 >= 4 (the loop filter of the speech blocks).
    bool speaker(i32 i) {
        if (dead(grp_id(i))) return false;
        if (busy(unit(grp_id(i)))) return false;
        return jo::as_unit(unit(grp_id(i)))->rank >= 4;
    }
    const char* name_of(i32 id) { return joan::orig::as_player(this)->name_tag(joan::orig::arg::u32(id)); }  // 0x00453da0

    // "CASSTO00" order (+0x1254 = 0xb) and its "RTS<name>015" / "RTS_LTINF_0<n>" speech.
    void cassto_speech() {
        jo::self<jo::Player>(this)->point_kind = 0xb;
        strncpy_(&jo::self<jo::Player>(this)->point_name[0], str(d::s_CASSTO00), 8u);  // "CASSTO00"
        Text s;
        joan::orig::as_string(&s)->construct(str(d::s_RTS));      // 0x005a11a0 "RTS"
        u8 count = 0;
        for (i32 i = 0; i < grp_count(); ++i) {
            if (!speaker(i)) continue;
            if (count == 0) {
                text_add(&s, name_of(grp_id(i)));
                text_add(&s, str(d::s_015));  // "015"
            }
            count = static_cast<u8>(count + 1);
        }
        if (count != 1) {
            text_set(&s, str(d::s_RTS_LTINF_0));  // "RTS_LTINF_0"
            text_num(&s, rnd(0xb, 0xd));
        }
        if (grp_count() == 1 && !dead(grp_id(0))) {
            text_set(&s, str(d::s_RTS));  // "RTS"
            text_add(&s, name_of(grp_id(0)));
            if (jo::as_unit(unit(grp_id(0)))->rank >= 4) {
                text_add(&s, str(d::s_015));  // "015"
            } else {
                text_add(&s, str(d::s_0));  // "0"
                text_num(&s, rnd(0xb, 0xd));
            }
        }
        joan::orig::as_sound(kSfx)->play_kind(c_str(&s));  // 0x00492c40
    }

    // End of a drag (0x0044193d..0x00441cb3): "RTS<name>002" / "RTS<name>008" speech.
    void release_speech() {
        Text s1;  // [ebp-0x3b4]
        Text s2;  // [ebp-0x548]
        joan::orig::as_string(&s1)->construct(str(d::s_RTS));      // 0x005a11a0 "RTS"
        joan::orig::as_string(&s2)->construct(str(d::s_RTS));
        u8 count = 0;
        for (i32 i = 0; i < grp_count(); ++i) {
            if (!speaker(i)) continue;
            if (count == 0) {
                text_add(&s1, name_of(grp_id(i)));
                text_add(&s1, str(d::s_002));  // "002"
                text_add(&s2, name_of(grp_id(i)));
                text_add(&s2, str(d::s_008));  // "008"
            }
            count = static_cast<u8>(count + 1);
        }
        if (count != 1) {
            text_set(&s1, str(d::s_RTS_LTINF_00));  // "RTS_LTINF_00"
            text_num(&s1, rnd(2, 4));
        }
        if (grp_count() == 1) {
            text_set(&s1, str(d::s_RTS));  // "RTS"
            text_add(&s1, name_of(grp_id(0)));
            if (jo::as_unit(unit(grp_id(0)))->rank >= 4) {
                text_add(&s1, str(d::s_008));  // "008"
            } else {
                u8* const u = unit(grp_id(0));
                u8* const k = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->type_id))));  // 0x0042d0a0
                if (jo::as_unit_type(k)->unit_class == 8)
                    text_set(&s1, str(d::s_RTS_LTINF_001));  // "RTS_LTINF_001"
                else
                    text_add(&s1, str(d::s_001));  // "001"
            }
        }
        if (joan::orig::arg::as<i8>(joan::orig::as_sound(kSfx)->is_playing(c_str(&s2))) != 0) return;  // 0x00473140
        if (joan::orig::arg::as<i8>(joan::orig::as_sound(kSfx)->is_playing(c_str(&s1))) != 0) return;
        joan::orig::as_sound(kSfx)->play_kind(c_str(&s1));  // 0x00492c40
    }

    // Jump-table case 10 (0x00441eb7). Returns false for the `xor ax,ax` exit (no trailing calls).
    bool click_place() {
        const i32 id = jo::view<i32>(jo::self<jo::Player>(this)->pick_id);  // [ebp-0x560]
        if (joan::orig::arg::as<i32>(joan::orig::as_list(kPlaces)->empty_at_e(joan::orig::arg::i32(id))) != 0) return true;  // 0x0044cf80
        u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_list(kPlaces)->get_e(joan::orig::arg::i32(id)));           // 0x0043c450
        const float lim512 = global<float>(d::flt_512);
        if (jo::as_facility(o)->kind == 3) {
            u8* const r = vcall<u8*>(o, 0x24);
            if (joan::orig::arg::as<i32>(joan::orig::as_list(kPlaces)->empty_at_e(static_cast<i32>(jo::as_flag_facility(r)->i16_91))) != 0) return true;
            if (jo::as_flag_facility(r)->list_79 == nullptr) return true;
            const i32 u = joan::orig::arg::as<i32>(joan::orig::as_group(this)->first_ready_member());  // 0x00452570
            if (dead(u)) {
                u8* const e = unit(jo::self<jo::Player>(this)->unit_id_);
                if (static_cast<i32>(jo::as_unit(e)->side) == jo::as_flag_facility(r)->owner->nation && jo::view<i8>(jo::as_flag_facility(r)->flag_97) == 1) {
                    jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 0xa;
                    global<u32>(d::g_a48959) = 0;
                    global<i32>(d::g_a48955) = id;
                    joan::orig::as_barracks_screen(obj(d::g_barracks_screen))->set_open(1);  // 0x006c0310
                }
                return true;
            }
            if (static_cast<i32>(jo::as_unit(unit(u))->side) != jo::as_flag_facility(r)->owner->nation) return true;
            if (jo::view<i8>(jo::as_flag_facility(r)->flag_97) != 1) return true;
            return place_boat(r, u, id, lim512);
        }
        if (jo::as_facility(o)->kind != 0x10) return true;
        u8* const r = vcall<u8*>(o, 0x1c);
        const i32 u = joan::orig::arg::as<i32>(joan::orig::as_group(this)->first_ready_member());  // 0x00452570
        if (dead(u)) return false;
        if (static_cast<i32>(jo::as_unit(unit(u))->side) != jo::as_flag_facility(r)->owner->nation) return true;
        {
            const float* const target = jo::as_flag_facility(r)->owner->position_;
            Vec3 tmp{0.0f, 0.0f, 0.0f};
            const Vec3* const d = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::as_unit(unit(u))->position_), joan::orig::arg::ptr(target)));  // 0x0060cc90
            const Vec3 v = *d;
            if (import_call<double>(d::iat_v3d_87_length, &v) > static_cast<double>(lim512)) return true;
        }
        if (jo::as_unit(unit(u))->rank < 4) return true;
        if (jo::as_flag_facility(r)->list_79 != nullptr) {
            const u32 a = joan::orig::arg::as<u32>(joan::orig::as_unit_types(kKinds)->find(str(d::s_ALENCON_9fa6e0)));  // 0x005270b0 "ALENCON"
            const u32 b = joan::orig::arg::as<u32>(joan::orig::as_units(kUnits)->find_by_3b(joan::orig::arg::i32(a)));                 // 0x0051fd50
            i16* const p = joan::orig::arg::as<i16*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(b)));  // 0x0041b3f0
            if (p == nullptr) return true;
            if (static_cast<i32>(*p) != u) return true;
            const float* const target = jo::as_flag_facility(r)->owner->position_;
            Vec3 tmp{0.0f, 0.0f, 0.0f};
            const Vec3* const d = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::as_unit(p)->position_), joan::orig::arg::ptr(target)));  // 0x0060cc90
            const Vec3 v = *d;
            const double len = import_call<double>(d::iat_v3d_87_length, &v);
            if (len >= static_cast<double>(global<float>(d::flt_1024))) return true;  // 1024 (test ah,1 je; NaN goes on)
        }
        jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 0xa;
        global<i32>(d::g_a4856d) = u;
        global<i32>(d::g_a48569) = id;
        joan::orig::as_workshop_screen(obj(d::g_workshop_screen))->set_open(1u);  // 0x006ed3e0
        return true;
    }
    bool place_boat(const u8* r, i32 u, i32 id, float lim512) {
        const float* const target = jo::as_flag_facility(r)->owner->position_;
        Vec3 tmp{0.0f, 0.0f, 0.0f};
        const Vec3* const d = joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, joan::orig::arg::ptr(jo::as_unit(unit(u))->position_), joan::orig::arg::ptr(target)));  // 0x0060cc90
        const Vec3 v = *d;
        if (import_call<double>(d::iat_v3d_87_length, &v) > static_cast<double>(lim512)) return true;
        if (jo::as_unit(unit(u))->rank < 4) return true;
        jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 0xa;
        global<i32>(d::g_a48959) = u;
        global<i32>(d::g_a48955) = id;
        joan::orig::as_barracks_screen(obj(d::g_barracks_screen))->set_open(1);  // 0x006c0310
        return true;
    }

    i16 handle_input() {
        c_input.hit();
        if (jo::view<i32>(jo::self<jo::Player>(this)->rts_active) != 1) return 0;
        if (jo::view<i32>(jo::self<jo::Player>(this)->pick_kind) == 7 && jo::view<i8>(jo::self<jo::Player>(this)->castle_popup_open) != 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_popup(kCastle)->poll_buttons()) != 0) {  // 0x006d5120
                if (joan::orig::arg::as<i32>(joan::orig::as_popup(kCastle)->result()) == 1)  // 0x006d5370
                    joan::orig::as_player(this)->assign_troops(joan::orig::arg::i32(jo::self<jo::Player>(this)->pick_id), 1);  // 0x0044f750
                else
                    cassto_speech();
                jo::self<jo::Player>(this)->castle_popup_open = 0;
                joan::orig::as_popup(kCastle)->free_gpopup();  // 0x006d3c60
                joan::orig::as_unit(kPlayer)->state_sound();  // 0x0042fef0
                if (global<u8>(d::g_game_running) == 0) {
                    global<u8>(d::g_game_running) = 1;
                    const float z = static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)));
                    joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(z));  // 0x0042f930
                }
            }
            return 1;
        }
        if (jo::self<jo::Player>(this)->cursor_mode == 0xa) return 0;

        u8 scroll_y = 0;  // [ebp-0x1a4]
        u8 keys = 0;      // [ebp-0x1a8]
        u8 scroll_x = 0;  // [ebp-0x1ac]
        if (global<u16>(d::g_rts_scroll_bits) & 4u) keys = 1;
        if (global<u16>(d::g_rts_scroll_bits) & 8u) keys = static_cast<u8>(keys | 2u);
        if (global<u16>(d::g_rts_scroll_bits) & 1u) keys = static_cast<u8>(keys | 4u);
        if (global<u16>(d::g_rts_scroll_bits) & 2u) keys = static_cast<u8>(keys | 8u);
        if ((global<i32>(d::g_a516d8) != 0 && global<i32>(d::g_mouse_delta_x) > 0) ||
            joan::orig::arg::as<i32>(joan::orig::as_bindings(kKeys)->state(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->prev_mode)), 0x32u)) != 0)  // 0x004cb6d0
            scroll_y = 2;
        if ((global<i32>(d::g_a516d8) != 0 && global<i32>(d::g_mouse_delta_x) < 0) ||
            joan::orig::arg::as<i32>(joan::orig::as_bindings(kKeys)->state(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->prev_mode)), 0x33u)) != 0)
            scroll_y = 1;
        if (global<i32>(d::g_mouse_wheel) > 0 || joan::orig::arg::as<i32>(joan::orig::as_bindings(kKeys)->state(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->prev_mode)), 0x30u)) != 0) scroll_x = 1;
        if (global<i32>(d::g_mouse_wheel) < 0 || joan::orig::arg::as<i32>(joan::orig::as_bindings(kKeys)->state(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->prev_mode)), 0x31u)) != 0) scroll_x = 2;

        u8 dragging = 0;   // [ebp-0x1bc]
        u8 ctrl = 0;       // [ebp-0x1c0]
        u8 shift = 0;      // [ebp-0x1c4]
        u8 pick = 0;       // [ebp-0x1c8]
        i16 digit = 0;     // [ebp-0x1cc]
        i16 key_sel = 0;   // [ebp-0x1d0]
        i32 picked = 0;    // [ebp-0x1d4]
        u8 picked_r = 0;   // [ebp-0x1d8]
        const u8* cam = global<u8*>(d::g_camera);
        const bool left_free = joan::orig::arg::as<i32>(joan::orig::as_parser(kInput)->rect_value(joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_x1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_y1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_x2)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_y2)), 0)) != 0 &&
                               joan::orig::arg::as<i8>(joan::orig::as_marker(kMinimap)->visible()) == 0;  // 0x005945d0, 0x00717650
        const u8 click_l = left_free ? 1 : 0;  // [ebp-0x1b0]
        cam = global<u8*>(d::g_camera);
        const u8 click_any = static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_parser(kInput)->rect_value(joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_x1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_y1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_x2)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_y2)), 1)));  // [ebp-0x1b4]
        cam = global<u8*>(d::g_camera);
        const bool right_free = joan::orig::arg::as<i32>(joan::orig::as_joystick(kInput)->in_rect(joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_x1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_y1)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_x2)), joan::orig::arg::i32(jo::view<u32>(jo::as_camera(cam)->view_y2)), 0)) != 0 &&
                                joan::orig::arg::as<i8>(joan::orig::as_marker(kMinimap)->visible()) == 0;  // 0x00594360
        const u8 click_r = right_free ? 1 : 0;  // [ebp-0x1b8]
        if (global<i32>(d::g_mouse_left_down) != 0) dragging = 1;
        if (global<u16>(d::g_key_modifiers) & 0xcu) ctrl = 1;
        if (global<u16>(d::g_key_modifiers) & 0x30u) shift = 1;
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(kKeys)->test(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->prev_mode)), 0x13)) != 0) joan::orig::as_group(this)->apply_units_453790(1u);  // 0x004cb820, 0x00453790
        if (joan::orig::arg::as<i32>(joan::orig::as_race_res(kKeys)->test(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->prev_mode)), 0x14)) != 0 && (global<u16>(d::g_key_modifiers) & 0x30u) == 0)
            joan::orig::as_group(this)->apply_units_453790(0u);
        if (ctrl != 0) {
            for (i32 k = 1; k <= 9; ++k)
                if (joan::orig::arg::as<i32>(joan::orig::keyboard::translate(joan::orig::arg::u32(global<u32>(d::g_last_key_scancode)), joan::orig::arg::u32(static_cast<u32>(global<u16>(d::g_a51d1a))), joan::orig::arg::u32(0x30 + k), 4u)) != 0)  // 0x00599240
                    digit = static_cast<i16>(k);
        }
        if (shift != 0) {
            for (i32 k = 1; k <= 9; ++k)
                if (joan::orig::arg::as<i32>(joan::orig::keyboard::translate(joan::orig::arg::u32(global<u32>(d::g_last_key_scancode)), joan::orig::arg::u32(static_cast<u32>(global<u16>(d::g_a51d1a))), joan::orig::arg::u32(0x30 + k), 8u)) != 0)
                    digit = static_cast<i16>(k);
        }
        if (shift == 0 && ctrl == 0) {
            if (joan::orig::arg::as<i32>(joan::orig::as_parser(kInput)->rect_mode(joan::orig::arg::i32(global<u32>(d::g_mouse_limit_x1)), joan::orig::arg::i32(global<u32>(d::g_mouse_limit_y1)), joan::orig::arg::i32(global<u32>(d::g_mouse_limit_x2)), joan::orig::arg::i32(global<u32>(d::g_mouse_limit_y2)), 0)) != 0) {  // 0x005940f0
                const u32 id = joan::orig::arg::as<u32>(joan::orig::as_hud(obj(d::g_player_hud))->hit_portrait(joan::orig::arg::i32(global<u32>(d::g_mouse_x)), joan::orig::arg::i32(global<u32>(d::g_mouse_y))));  // 0x006ffd90
                i16* const p = joan::orig::arg::as<i16*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3f0
                if (p != nullptr) picked = static_cast<i32>(*p);
            } else if (joan::orig::arg::as<i32>(joan::orig::as_joystick(kInput)->in_rect(joan::orig::arg::i32(global<u32>(d::g_mouse_limit_x1)), joan::orig::arg::i32(global<u32>(d::g_mouse_limit_y1)), joan::orig::arg::i32(global<u32>(d::g_mouse_limit_x2)), joan::orig::arg::i32(global<u32>(d::g_mouse_limit_y2)), 0)) != 0) {  // 0x00594360
                const u32 id = joan::orig::arg::as<u32>(joan::orig::as_hud(obj(d::g_player_hud))->hit_portrait(joan::orig::arg::i32(global<u32>(d::g_mouse_x)), joan::orig::arg::i32(global<u32>(d::g_mouse_y))));
                i16* const p = joan::orig::arg::as<i16*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)));
                if (p != nullptr) {
                    picked = static_cast<i32>(*p);
                    picked_r = 1;
                }
            } else {
                static const i32 kSelKeys[8] = {0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1f, 0x1e, 0x20};
                for (i32 k = 0; k < 8; ++k)
                    if (joan::orig::arg::as<i32>(joan::orig::as_race_res(kKeys)->test(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Player>(this)->prev_mode)), joan::orig::arg::i32(kSelKeys[k]))) != 0) key_sel = static_cast<i16>(k + 1);
                if (global<i32>(d::g_key) == 0x39) key_sel = 9;
            }
            if (key_sel != 0 || picked != 0) pick = 1;
        }
        if (global<u8>(d::g_game_running) == 0) return 0;
        CamState view{};  // [ebp-0x21c]
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(&view);  // 0x006140a0
        if (ctrl != 0 && digit != 0)
            return static_cast<i16>(joan::orig::arg::as<i8>(joan::orig::as_group(this)->copy_slot(static_cast<i32>(digit))));  // 0x00418310
        if (shift != 0 && digit != 0)
            return static_cast<i16>(joan::orig::arg::as<i8>(joan::orig::as_group(this)->select_slot(static_cast<i32>(digit))));  // 0x0044aad0
        if (pick != 0)
            return static_cast<i16>(joan::orig::arg::as<i8>(joan::orig::as_army(this)->select_hero_group(joan::orig::arg::i16(static_cast<i32>(key_sel)), joan::orig::arg::u32(picked), static_cast<i32>(static_cast<i8>(picked_r)), joan::orig::arg::i32(flag3()))));  // 0x00451530
        if (click_r != 0) {
            if (joan::orig::arg::as<i8>(joan::orig::as_world(this)->siege_release_44a690()) != 0) return 1;  // 0x0044a690
            if (joan::orig::arg::as<i8>(joan::orig::as_player(this)->rts_pick_use()) != 0) return 1;  // 0x0044a430
        }
        if (click_l != 0 && jo::view<i8>(jo::self<jo::Player>(this)->dragging) == 0) {
            jo::self<jo::Player>(this)->point_kind = 1;
            strncpy_(&jo::self<jo::Player>(this)->point_name[0], str(d::s_POINT00), 8u);  // "POINT00"
            jo::self<jo::Player>(this)->click_time = global<u32>(d::g_now);
            return static_cast<i16>(joan::orig::arg::as<i8>(joan::orig::as_group(this)->select_449b30(joan::orig::arg::u32(flag3()))));  // 0x00449b30
        }
        if (dragging != 0 && jo::view<i8>(jo::self<jo::Player>(this)->dragging) == 0 && joan::orig::arg::as<i8>(joan::orig::as_marker(kMinimap)->visible()) == 0) {
            jo::self<jo::Player>(this)->drag_x = static_cast<float>(joan::orig::arg::as<i32>(joan::orig::as_table(kInput)->at_672(0)));  // 0x0043c3c0
            jo::self<jo::Player>(this)->drag_y = static_cast<float>(joan::orig::arg::as<i32>(joan::orig::as_table(kInput)->at_676(0)));  // 0x0043c3f0
            jo::self<jo::Player>(this)->dragging = 1;
            jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 3;
            return 1;
        }
        if (dragging == 0 && jo::view<i8>(jo::self<jo::Player>(this)->dragging) != 0) {
            jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 0;
            jo::self<jo::Player>(this)->dragging = 0;
            if (grp_count() != 0) release_speech();
        }
        if (jo::view<i8>(jo::self<jo::Player>(this)->dragging) != 0) {
            if (ne_ordered(static_cast<double>(global<i32>(d::g_mouse_x)), static_cast<double>(jo::self<jo::Player>(this)->drag_x)) ||
                ne_ordered(static_cast<double>(global<i32>(d::g_mouse_y)), static_cast<double>(jo::self<jo::Player>(this)->drag_y)))
                return static_cast<i16>(joan::orig::arg::as<i8>(joan::orig::as_player(this)->cull_visible(joan::orig::arg::i32(flag3()))));  // 0x004489c0
        }
        jo::self<jo::Player>(this)->group_counts[11] = jo::self<jo::Player>(this)->group_counts[0];
        if (joan::orig::arg::as<i8>(joan::orig::as_marker(kMinimap)->visible()) != 0 && click_any != 0) {
            Vec3 v{0.0f, 0.0f, 0.0f};  // [ebp-0x1f4]
            if (joan::orig::arg::as<i8>(joan::orig::as_mini_map(kMinimap)->to_screen(&v)) != 0)  // 0x00717700
                joan::orig::as_player(this)->move_formation(v, joan::orig::arg::i32(static_cast<u32>(global<u16>(d::g_key_modifiers) & 0xcu)));  // 0x0044cfb0
        } else {
            Vec3 dest{0.0f, 0.0f, 0.0f};  // [ebp-0x55c]
            if (joan::orig::arg::as<i8>(joan::orig::as_player(this)->rts_pick(reinterpret_cast<u8*>(&jo::self<jo::Player>(this)->pick_kind), reinterpret_cast<u8*>(&jo::self<jo::Player>(this)->pick_id), &dest)) != 0 && click_any != 0) {  // 0x0044ac60
                switch (jo::self<jo::Player>(this)->pick_kind) {
                    case 0: {
                        Vec3 out{0.0f, 0.0f, 0.0f};  // [ebp-0x1f4]
                        Vec3 eye;
                        std::memcpy(&eye, &view.d[1], sizeof eye);  // [ebp-0x218]
                        const i8 r = joan::orig::arg::as<i8>(joan::orig::as_player(this)->ray_query(global<u32>(d::g_mouse_x), global<u32>(d::g_mouse_y), eye.x, eye.y,
                                                  eye.z, &out));  // 0x00452830
                        if (r != 0)
                            joan::orig::as_player(this)->move_formation(out, joan::orig::arg::i32(static_cast<u32>(global<u16>(d::g_key_modifiers) & 0xcu)));
                        break;
                    }
                    case 1:
                        if (jo::self<jo::Player>(this)->pick_id != 0) joan::orig::as_world(this)->siege_board_44eb80(joan::orig::arg::i32(jo::self<jo::Player>(this)->pick_id));  // 0x0044eb80
                        break;
                    case 2:
                        if (jo::self<jo::Player>(this)->pick_id != 0) joan::orig::as_world(this)->siege_attack_44f1f0(joan::orig::arg::i32(jo::self<jo::Player>(this)->pick_id));  // 0x0044f1f0
                        break;
                    case 4:
                        joan::orig::as_player(this)->move_formation(dest, joan::orig::arg::i32(static_cast<u32>(global<u16>(d::g_key_modifiers) & 0xcu)));
                        break;
                    case 6:
                    case 7:
                    case 8:
                        joan::orig::as_player(this)->assign_troops(joan::orig::arg::i32(jo::self<jo::Player>(this)->pick_id), 0);  // 0x0044f750
                        break;
                    case 9:
                        cassto_speech();
                        break;
                    case 10:
                        if (!click_place()) return 0;
                        break;
                    case 11:
                        if (jo::self<jo::Player>(this)->pick_id != 0) joan::orig::as_player(this)->spawn_effect_200(joan::orig::arg::i32(jo::self<jo::Player>(this)->pick_id));  // 0x00452720
                        break;
                    default:
                        break;
                }
            }
        }
        joan::orig::as_camera(this)->update_follow_445990(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<u32>(keys))));      // 0x00445990
        joan::orig::as_player(this)->rts_turn(joan::orig::arg::u32(static_cast<u32>(scroll_y)));  // 0x00447740
        joan::orig::as_unit(this)->zoom_step(static_cast<char>(joan::orig::arg::i32(static_cast<u32>(scroll_x))));  // 0x00447960
        return 0;
    }
};

#define B110(name, addr, fn) JOAN_REPLACE("game/b110_15", name, addr, fn)
B110("Player::handle_input", 0x00440b30, &H::handle_input);
#undef B110

}  // namespace game_b110_15
}  // namespace

// ---------------------------------------- from modules/game_world4.cpp (game/world4)
namespace {
namespace game_world4 {

JOAN_COUNTER(c_unload, "World::unload");

struct H {
    // 0x004426a0 World::unload (+1 loaded, +0x72 / +0x7a cells, +0x6a / +0x6e trees, +0x61).
    void world_unload() {
        if (static_cast<std::int8_t>(jo::self<jo::World>(this)->loaded) == 0) return;
        c_unload.hit();
        joan::orig::as_light_sys(img::obj(d::g_light_sys))->clear();        // 0x004a5970
        joan::orig::as_sound3d(img::obj(d::g_weather_system))->release(static_cast<char>(joan::orig::arg::i32(0)));     // 0x00689d00
        joan::orig::as_markers(img::obj(d::g_slots_a3ce90))->delete_all();        // 0x004876d0
        joan::orig::as_missile_list(img::obj(d::g_orders))->delete_all();        // 0x004e9f10
        void* const grass = img::obj(d::g_dyn_grass);
        joan::orig::as_grass(grass)->set_enabled(static_cast<char>(joan::orig::arg::i32(static_cast<char>(0))));                               // 0x00464600
        joan::orig::as_grass(grass)->set_enabled(static_cast<char>(joan::orig::arg::i32(static_cast<char>(global<std::uint8_t>(d::g_cfg_grass_enable)))));
        for (std::int32_t i = jo::self<jo::World>(this)->cell_count; i > 0; --i) joan::orig::as_cell(jo::view<void**>(jo::self<jo::World>(this)->cells)[i - 1])->unload();  // 0x0042ac40
        global<void*>(d::g_main_light) = nullptr;
        joan::orig::as_actor_list(img::obj(d::g_actor_list))->delete_all();  // 0x0041ba00
        void* const a = jo::view<void*>(jo::self<jo::World>(this)->path_space);
        if (a != nullptr) joan::orig::as_pfd_data(a)->destroy(1u);  // 0x004201a0 delete
        jo::view<void*>(jo::self<jo::World>(this)->path_space) = nullptr;
        void* const b = jo::view<void*>(jo::self<jo::World>(this)->scene);
        if (b != nullptr) joan::orig::as_scene(b)->destroy(1u);  // 0x004201f0 delete
        jo::view<void*>(jo::self<jo::World>(this)->scene) = nullptr;
        jo::self<jo::World>(this)->loaded = 0;
        joan::orig::as_bmp_res(&jo::self<jo::World>(this)->bitmaps)->free();  // 0x00620ab0
    }
};

#define W4(name, addr, fn) JOAN_REPLACE("game/world4", name, addr, fn)
W4("World::unload", 0x004426a0, &H::world_unload);
#undef W4

}  // namespace game_world4
}  // namespace

// ---------------------------------------- from modules/game_world5.cpp (game/world5)
namespace {
namespace game_world5 {

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
constexpr img::Addr kUnits(d::g_items);
constexpr img::Addr kSounds(d::g_physic_sys);
constexpr img::Addr kMap(d::s_MAP_9cf818);        // "MAP\"
constexpr img::Addr kPfd(d::s_PFD);        // ".PFD"

JOAN_COUNTER(c_attach, "CellObject::attach");
JOAN_COUNTER(c_load_pfd, "World::load_pfd");

// Path string object (0x194 bytes, no destructor).
void str_init(void* s) { joan::orig::as_string(s)->construct(); }                       // 0x005a1170
void str_assign(void* s, const void* text) { joan::orig::as_string(s)->assign(joan::orig::arg::str(text)); }  // 0x005a1440
void str_append(void* s, const void* text) { joan::orig::as_string(s)->append(joan::orig::arg::str(text)); }  // 0x005a1550
const char* str_c(void* s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(s)->c_str()); }          // 0x00417d90
// new(0x38) + 0x00664fd0 (path-finding data).
void* new_pfd() {
    void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::PathSpace)));  // 0x38 on x86; operator new
    return mem != nullptr ? joan::orig::as_path_space(mem)->construct() : nullptr;  // 0x00664fd0
}
std::int32_t units_missing(std::int16_t id) { return joan::orig::as_units(img::obj(kUnits))->list.empty_at(static_cast<std::int32_t>(id)); }  // 0x00425d50
void* units_get(std::int16_t id) { return joan::orig::as_units(img::obj(kUnits))->list.ptr_at(static_cast<std::int32_t>(id)); }  // 0x00425d20

struct H {
    void attach_unit(const std::int16_t& id) {
        if (units_missing(id) != 0) return;
        virtual_call(units_get(id), 0x18, 1);
        virtual_call(units_get(id), 0x1c);
        void* const unit = units_get(id);
        joan::orig::as_cell(jo::self<jo::Unit>(this)->cell)->attach_unit(joan::orig::arg::mptr(unit));  // 0x0043b190
    }
    void add_unit_meshes(const std::int16_t& id) {
        if (units_missing(id) != 0) return;
        std::uint8_t* const unit = static_cast<std::uint8_t*>(units_get(id));
        if (jo::view<void*>(jo::as_map_object(unit)->actor) == nullptr) return;
        joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::view<void*>(jo::as_map_object(unit)->actor)));  // 0x005efc50
        if (jo::view<void*>(jo::as_map_object(unit)->actor_b) == nullptr) return;
        joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::view<void*>(jo::as_map_object(unit)->actor_b)));
    }

    // 0x00442820 CellObject::attach.
    void attach() {
        if (static_cast<std::int8_t>(jo::self<jo::Unit>(this)->in_scene) != 0) return;
        c_attach.hit();
        jo::self<jo::Unit>(this)->in_scene = 1;
        jo::self<jo::Unit>(this)->instantiated = 1;
        joan::orig::as_cell_object(this)->create_scene_objects();  // 0x0052b970
        attach_unit(jo::self<jo::Unit>(this)->carried_a);
        attach_unit(jo::self<jo::Unit>(this)->carried_b);
        void* cell = jo::self<jo::Unit>(this)->cell;   // ebp-8
        void* scene = jo::self<jo::Unit>(this)->scene;  // ebp-0xc
        if (jo::self<jo::Unit>(this)->world != nullptr)
            joan::orig::as_map(jo::self<jo::Unit>(this)->world)->find_cell(joan::orig::arg::mptr(static_cast<void*>(&cell)), joan::orig::arg::mptr(static_cast<void*>(&scene)), joan::orig::arg::mptr(static_cast<void*>(jo::self<jo::Unit>(this)->position_)));  // 0x0054a2c0
        if (scene != nullptr) {
            jo::self<jo::Unit>(this)->scene = static_cast<jo::Scene*>(scene);
            if (jo::self<jo::Unit>(this)->actor != nullptr) joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor));  // 0x005efc50
            if (jo::self<jo::Unit>(this)->model != nullptr) joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->model));
            if (jo::self<jo::Unit>(this)->weapon_model != nullptr) joan::orig::as_scene(jo::self<jo::Unit>(this)->scene)->attach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->weapon_model));
            add_unit_meshes(jo::self<jo::Unit>(this)->carried_a);
            if (jo::self<jo::Unit>(this)->trail_a != nullptr) joan::orig::as_trail(jo::self<jo::Unit>(this)->trail_a)->set_scene(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->scene));  // 0x006891d0
            add_unit_meshes(jo::self<jo::Unit>(this)->carried_b);
            if (jo::self<jo::Unit>(this)->trail_b != nullptr) joan::orig::as_trail(jo::self<jo::Unit>(this)->trail_b)->set_scene(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->scene));
        }
        if (jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->record) == 0) {
            const std::int32_t sound = joan::orig::arg::as<std::int32_t>(joan::orig::as_army(img::obj(kSounds))->fire_shot(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor_c), joan::orig::arg::bits(60.0f), joan::orig::arg::u32(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->layer)), 0u, 2u, 0u));  // 0x006adbf0
            jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->record) = sound;
        }
        std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(img::obj(d::g_types))->get(joan::orig::arg::i16(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->type_id))));  // 0x0042d0a0
        std::uint8_t* const voice = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_players(img::obj(kSounds))->list.ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_actor(jo::self<jo::Unit>(this)->actor_c)->record))));  // 0x006af310
        switch (static_cast<std::int32_t>(jo::as_unit_type(def)->kind)) {
            case 2: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 6; break;
            case 3: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 8; break;
            case 4: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 9; break;
            case 5: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 0xa; break;
            case 6: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 0xb; break;
            case 7: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 0xc; break;
            case 8: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 0xd; break;
            case 9: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 0xe; break;
            case 10: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 0xf; break;
            case 11: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 0x10; break;
            case 12: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 0x11; break;
            case 13: jo::view<std::uint32_t>(jo::as_shot(voice)->type) = 0x13; break;
            default: break;
        }
        jo::self<jo::Unit>(this)->instantiated = 0;
        if (jo::view<std::int32_t>(jo::self<jo::Unit>(this)->alt_mesh_state) == 1) joan::orig::as_cell_object(this)->create_alt_mesh(1);  // 0x0050cc70
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_world(this)->flag_1fd()) != 0 ||                       // 0x0042d690
            static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_game(this)->byte_2e1())) != 0) {  // 0x00432b90
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
            joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(0));
        }
    }

    // 0x00442d30 World::load_pfd -> 1.
    std::int32_t load_pfd() {
        c_load_pfd.hit();
        jo::view<void*>(jo::self<jo::World>(this)->path_space) = new_pfd();
        alignas(8) std::uint8_t path[0x194];  // ebp-0x1a4
        str_init(path);
        str_assign(path, img::str(kMap));
        str_append(path, jo::self<jo::World>(this)->name);
        str_append(path, img::str(kPfd));
        {
            const char* const p = str_c(path);
            joan::orig::as_path_space(jo::view<void*>(jo::self<jo::World>(this)->path_space))->load(joan::orig::arg::str(p));  // 0x00670980
        }
        jo::self<jo::World>(this)->path_space->map_id = 0;
        if (static_cast<std::int8_t>(jo::self<jo::World>(this)->u8_60) == 0) {
            str_assign(path, img::str(kMap));
            str_append(path, jo::self<jo::World>(this)->name);
            str_append(path, &img::global<const char>(d::s_WOR));  // ".WOR"
            alignas(8) std::uint8_t reader[((sizeof(jo::FileTxt) > 0x1a0 ? sizeof(jo::FileTxt) : 0x1a0) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // ebp-0x340 (x64: the larger FileTxt)
            joan::orig::as_file_txt(static_cast<void*>(reader))->construct(joan::orig::arg::str(str_c(path)));  // 0x00589550
            joan::orig::as_world(this)->read_nodes(joan::orig::arg::mptr(static_cast<void*>(reader)));  // 0x0054a6c0
            joan::orig::as_file_txt(static_cast<void*>(reader))->destruct();                       // 0x00589890
        }
        return 1;
    }
};

#define W5(name, addr, fn) JOAN_REPLACE("game/world5", name, addr, fn)
W5("CellObject::attach", 0x00442820, &H::attach);
W5("World::load_pfd", 0x00442d30, &H::load_pfd);
#undef W5

}  // namespace game_world5
}  // namespace

// ---------------------------------------- from modules/game_b110_14.cpp (game/b110_14)
namespace {
namespace game_b110_14 {

using u32 = std::uint32_t;

struct V3 {
    float x, y, z;
};
static_assert(sizeof(V3) == 12, "vec3");
struct Frame40 {  // 0x28-byte camera frame passed by value (rep movs 10 dwords)
    u32 d[10];
};
static_assert(sizeof(Frame40) == 40, "frame");

void* const kUnits = obj(d::g_units);
void* const kScratch = obj(d::g_misc);

u32 now() { return global<u32>(d::g_now); }
u8* unit_rec(u32 k) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(k))); }  // 0x0041b3c0
i32 unit_missing(u32 k) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(k)); }  // 0x00428b70
void* cam_motion(i32 idx) { return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamMan)->motion(joan::orig::arg::i32(idx))); }  // 0x00563c80

// fld dword / fstp dword round trip (an SNaN gets quieted): src/fp/bits.hpp.
using joan::fp::quiet;
// fcomp + `test ah,0x40` taken: equal or unordered.
bool fcom_eq(float a, float b) { return !(a < b) && !(a > b); }
u32 bits(float f) {
    u32 b = 0;
    std::memcpy(&b, &f, 4);
    return b;
}

// CRT (called at their addresses, as the original does).
i32 crt_strcmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
#if defined(__GNUC__) && !defined(__clang__)
// callers copy 8 bytes of a longer string into the 8-byte name field without terminator, as the original strncpy does
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
char* crt_strncpy(char* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
char* crt_strchr(const char* s, i32 c) { return joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(s), joan::orig::arg::i32(c))); }
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }

JOAN_COUNTER(c_rts, "Game::rts_mode_442ef0");

// One animation-name case of the +0x1254 switch. `last`: final frame name; on a match the
// animation stops (+0x1254 = 0) or, with `reset`, restarts there. Otherwise the frame number after
// strchr(name, ch) + skip is advanced and the name rebuilt from `prefix` (string ctor) or from
// `below10` / `above` (default ctor + assign).
void anim_case(void* self, img::Addr last, i32 ch, u32 skip, img::Addr prefix, img::Addr below10, img::Addr above, img::Addr reset,
               u32 t) {
    char* const name = jo::as_player(self)->point_name;
    if (crt_strcmp(name, str(last)) == 0) {
        if (jo::as_player(self)->click_time + 50u < t) {
            if (reset == img::kNoAddr) {
                jo::as_player(self)->point_kind = 0;
            } else {
                crt_strncpy(name, str(reset), 8);
                jo::as_player(self)->click_time = t;
            }
        }
        return;
    }
    if (!(jo::as_player(self)->click_time + 50u < t)) return;
    alignas(4) char s1[0x194];
    alignas(4) char s2[0x194];
    joan::orig::as_string(s1)->construct(joan::orig::arg::str(static_cast<const char*>(name)));  // 0x005a11a0
    char* p = crt_strchr(joan::orig::as_string(s1)->c_str(), ch);  // 0x00417d90
    p = reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(p) + skip);
    const u32 len = crt_strlen(p);
    i16 n = static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_misc(kScratch)->atoi_field_590270(joan::orig::arg::str(static_cast<const char*>(p)), joan::orig::arg::u32(len))));  // 0x00590270
    n = static_cast<i16>(n + 1);
    if (prefix != img::kNoAddr) {
        joan::orig::as_string(s2)->construct(joan::orig::arg::str(str(prefix)));  // 0x005a11a0
    } else {
        joan::orig::as_string(s2)->construct();  // 0x005a1170
        if (n < 10) {
            joan::orig::as_string(s2)->assign(joan::orig::arg::str(str(below10)));  // 0x005a1440
        } else {
            joan::orig::as_string(s2)->assign(joan::orig::arg::str(str(above)));
        }
    }
    joan::orig::as_string(s2)->append(static_cast<i32>(n));           // 0x005a15d0
    crt_strncpy(name, joan::orig::as_string(s2)->c_str(), 8);
    jo::as_player(self)->click_time = t;
}

// ------------------------------------------------------------------ thiscall functions
struct H {
    i16 zone_busy() { return joan::orig::arg::as<i16>(joan::orig::as_track(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones))->is_type1()); }  // 0x00428aa0
    void* zones() { return jo::self<jo::Player>(this)->cam_zones->tracks; }

    // rts_play set-up (+0xf5 == 2). Returns false on the early return (unit gone).
    bool rts_start() {
        const float k_deg = global<float>(d::flt_0_017453292);
        alignas(4) u8 cam_a[0x28];  // [ebp-0x2c]
        alignas(4) u8 cam_b[0x28];  // [ebp-0x54]
        joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(static_cast<void*>(cam_a)));  // 0x006140a0
        joan::orig::as_camera_motion(cam_motion(0))->end(joan::orig::arg::mptr(static_cast<void*>(cam_b)));              // 0x00423fe0
        if (unit_missing(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)) != 0) return false;
        u8* const rec = unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_));
        u32 pos[3] = {0, 0, 0};
        joan::orig::as_unit(rec)->position(joan::orig::arg::mptr(static_cast<void*>(pos)));  // 0x004fedb0
        const i32 h_deinit = joan::orig::arg::as<i32>(joan::orig::as_cam_tracks(zones())->find(str(d::s_rts_deinit)));  // 0x00429c70 "rts_deinit"
        const i32 h_play = joan::orig::arg::as<i32>(joan::orig::as_cam_tracks(zones())->find(str(d::s_rts_play)));    // "rts_play"
        u8* const p70 = joan::orig::arg::as<u8*>(joan::orig::as_tracks(zones())->get(joan::orig::arg::i16(h_play)));                // 0x00428b30 (word)
        u8* const p74 = joan::orig::arg::as<u8*>(joan::orig::as_tracks(zones())->get(joan::orig::arg::i16(h_deinit)));
        jo::as_cam_params(cam_b)->orbit[0] = joan::orig::arg::as<float>(joan::orig::as_ground_object(rec)->heading());  // 0x004ff850 -> [ebp-0x44]
        const Fpu fpu{true};
        for (i32 i = 0; i < jo::as_cam_track(p70)->key_count; ++i) {
            jo::CamKey& pk = jo::as_cam_track(p70)->keys[i];
            const jo::CamKey& dk = jo::as_cam_track(p74)->keys[i];
            jo::view<u32>(pk.time) = jo::view<u32>(dk.time);
            jo::view<u32>(pk.point[0]) = pos[0];
            jo::view<u32>(pk.point[1]) = pos[1];
            float z = 0.0f;
            std::memcpy(&z, &pos[2], 4);
            pk.point[2] = (z + dk.point[2]) + global<float>(d::flt_50);
            const i32 last = static_cast<i32>(jo::view<u32>(jo::as_cam_track(p70)->key_count) - 1u);
            const float t = static_cast<float>(fpu.div(static_cast<double>(i), static_cast<double>(last)));
            const float b = jo::as_cam_params(cam_b)->orbit[0] / k_deg;
            const float a = jo::as_cam_params(cam_a)->orbit[0] / k_deg;
            pk.yaw = joan::orig::arg::as<float>(joan::orig::as_world(this)->lerp(joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(t)));  // 0x00447930 (lerp)
            jo::view<u32>(pk.pitch) = jo::view<u32>(dk.pitch);
            pk.roll = jo::as_cam_params(cam_a)->orbit[2] / k_deg;
            u32 v = 0;
            if (!(dk.zoom >= jo::as_cam_params(cam_a)->orbit[3])) {
                v = quiet(jo::view<u32>(dk.zoom));
            } else {
                v = jo::view<u32>(jo::as_cam_params(cam_a)->orbit[3]);
            }
            jo::view<u32>(pk.zoom) = v;
        }
        jo::view<u32>(jo::as_cam_track(p70)->keys[0].point[0]) = jo::view<u32>(jo::as_cam_params(cam_a)->position[0]);
        jo::view<u32>(jo::as_cam_track(p70)->keys[0].point[1]) = jo::view<u32>(jo::as_cam_params(cam_a)->position[1]);
        jo::view<u32>(jo::as_cam_track(p70)->keys[0].point[2]) = jo::view<u32>(jo::as_cam_params(cam_a)->position[2]);
        jo::as_cam_track(p70)->keys[0].pitch = jo::as_cam_params(cam_a)->orbit[1] / k_deg;
        jo::as_cam_track(p70)->keys[0].yaw = jo::as_cam_params(cam_a)->orbit[0] / k_deg;
        jo::as_cam_track(p70)->keys[0].roll = jo::as_cam_params(cam_a)->orbit[2] / k_deg;
        jo::view<u32>(jo::as_cam_track(p70)->keys[0].zoom) = jo::view<u32>(jo::as_cam_params(cam_a)->orbit[3]);
        jo::view<u32>(jo::as_cam_track(p70)->keys[jo::view<u32>(jo::as_cam_track(p70)->key_count) - 1u].zoom) = global<u32>(d::g_a0bbf0);
        const float lastp = jo::as_cam_params(cam_b)->orbit[1] / k_deg;
        jo::as_cam_track(p70)->keys[jo::view<u32>(jo::as_cam_track(p70)->key_count) - 1u].pitch = lastp;
        joan::orig::as_cam_tracks(zones())->prepare(joan::orig::arg::i32(h_play));  // 0x00429d00
        joan::orig::as_camera_man(kCamMan)->set_mode(i32{2});  // 0x00429860
        joan::orig::as_camera_motion(cam_motion(2))->start(joan::orig::arg::mptr(static_cast<void*>(cam_a)), joan::orig::arg::mptr(static_cast<void*>(cam_a)));  // 0x00423ec0
        joan::orig::as_camera_motion(cam_motion(0))->start(joan::orig::arg::mptr(static_cast<void*>(cam_b)), joan::orig::arg::mptr(static_cast<void*>(cam_b)));
        const u32 t_now = now();
        joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones))->start_track(str(d::s_rts_play), joan::orig::arg::u32(t_now));  // 0x00428850
        jo::self<jo::Player>(this)->rts_active = 3;
        return true;
    }

    void anim_switch(u32 t) {
        switch (jo::self<jo::Player>(this)->point_kind) {
            case 0: crt_strncpy(jo::self<jo::Player>(this)->point_name, str(d::s_POINT00), 8); break;  // "POINT00"
            case 1: anim_case(this, d::s_POINT05, 'T', 1, d::s_POINT0, img::kNoAddr, img::kNoAddr, img::kNoAddr, t); break;  // POINT05 / POINT0
            case 2: anim_case(this, d::s_HVYATT08, 'T', 1, d::s_HVYATT0, img::kNoAddr, img::kNoAddr, img::kNoAddr, t); break;  // HVYATT08 / HVYATT0
            case 3: anim_case(this, d::s_INFATT08, 'T', 1, d::s_INFATT0, img::kNoAddr, img::kNoAddr, img::kNoAddr, t); break;  // INFATT08 / INFATT0
            case 4: anim_case(this, d::s_ARCATT08, 'T', 1, d::s_ARCATT0, img::kNoAddr, img::kNoAddr, img::kNoAddr, t); break;  // ARCATT08 / ARCATT0
            case 6:
            case 14: anim_case(this, d::s_MOVTO10, 'O', 1, img::kNoAddr, d::s_MOVTO0, d::s_MOVTO, img::kNoAddr, t); break;  // MOVTO10
            case 7: anim_case(this, d::s_CMOVTO10, 'O', 1, img::kNoAddr, d::s_CMOVTO0, d::s_CMOVTO, img::kNoAddr, t); break;   // CMOVTO10
            case 8: anim_case(this, d::s_ASSTO_10, '_', 1, img::kNoAddr, d::s_ASSTO_0, d::s_ASSTO, img::kNoAddr, t); break;   // ASSTO_10
            case 9: anim_case(this, d::s_ASSIGN15, 'N', 1, img::kNoAddr, d::s_ASSIGN0, d::s_ASSIGN, img::kNoAddr, t); break;   // ASSIGN15
            case 10: anim_case(this, d::s_FOLLOW17, 'W', 1, img::kNoAddr, d::s_FOLLOW0, d::s_FOLLOW, img::kNoAddr, t); break;  // FOLLOW17
            case 11: anim_case(this, d::s_CASSTO10, 'O', 1, img::kNoAddr, d::s_CASSTO0, d::s_CASSTO, img::kNoAddr, t); break;  // CASSTO10
            case 15:
                if (jo::self<jo::Player>(this)->pick_kind != 10) jo::self<jo::Player>(this)->point_kind = 0;
                anim_case(this, d::s_BARR18, 'R', 2, img::kNoAddr, d::s_BARR0, d::s_BARR, d::s_BARR01, t);  // BARR18 / BARR01
                break;
            case 16:
                if (jo::self<jo::Player>(this)->pick_kind != 10) jo::self<jo::Player>(this)->point_kind = 0;
                anim_case(this, d::s_WEAPON09, 'N', 1, img::kNoAddr, d::s_WEAPON0, d::s_WEAPON, d::s_WEAPON01, t);  // WEAPON09 / 01
                break;
            case 17:
                if (jo::self<jo::Player>(this)->pick_kind != 10) jo::self<jo::Player>(this)->point_kind = 0;
                anim_case(this, d::s_IRON09, 'N', 1, img::kNoAddr, d::s_IRON0, d::s_IRON, d::s_IRON01, t);  // IRON09 / 01
                break;
            case 18:
                if (jo::self<jo::Player>(this)->pick_kind != 10) jo::self<jo::Player>(this)->point_kind = 0;
                anim_case(this, d::s_WOOD17, 'D', 1, img::kNoAddr, d::s_WOOD0, d::s_WOOD, d::s_WOOD01, t);  // WOOD17 / 01
                break;
            default: break;  // 5, 12, 13, > 18
        }
    }

    // Camera pull-back until 0x00453930 accepts the frame. Returns false on the early returns.
    bool pull_back() {
        if (joan::orig::arg::as<i16>(joan::orig::as_marker(obj(d::g_mini_map))->word_8()) == 0) return false;  // 0x0044cf10
        alignas(4) Frame40 cam{};                                             // [ebp-0x2db4]
        void* const m = cam_motion(joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamMan)->target()));       // 0x00563dc0
        joan::orig::as_camera_motion(m)->end(joan::orig::arg::mptr(static_cast<void*>(&cam)));             // 0x00423fe0
        if (unit_missing(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)) != 0) return false;
        if ((joan::orig::arg::as<u32>(joan::orig::as_marker(obj(d::g_mini_map))->byte_e()) & 0xff) == 0) return true;  // 0x0044cf30
        i32 count = 0;
        for (;;) {
            if (joan::orig::arg::as<i8>(joan::orig::as_player(this)->corners_on_map(cam)) != 0) break;  // 0x00453930 (frame by value)
            ++count;
            V3 pos{};
            joan::orig::as_unit(unit_rec(jo::view<u32>(jo::self<jo::Player>(this)->unit_id_)))->position(&pos);  // 0x004fedb0
            V3 tmp{};
            const V3* const r = joan::orig::arg::as<V3*>(joan::orig::vec_sub(&tmp, &pos, static_cast<void*>(&cam.d[1])));  // 0x0060cc90
            V3 dir = *r;
            import_call<void*>(d::iat_v3d_87_vector_normalize_1, &dir);  // normalize in place
            if ((joan::orig::arg::as<u32>(joan::orig::vec_equal(&dir, joan::orig::arg::ptr(obj(d::g_pos)))) & 0xff) != 0) break;  // 0x0060ced0
            if (count >= 15) {
                float dist = 0.0f;
                std::memcpy(&dist, &cam.d[7], 4);
                const float floor = global<float>(d::g_a0bbf0);
                const float d = dist - global<float>(d::flt_100);
                cam.d[7] = d > floor ? bits(dist - global<float>(d::flt_100)) : global<u32>(d::g_a0bbf0);
                if (fcom_eq(jo::self<jo::Player>(this)->zoom_dist, global<float>(d::g_a0bbf0))) break;
                if (count > 1000) break;
            }
            V3 step_out{};
            const V3* const s = joan::orig::arg::as<V3*>(joan::orig::vec_scale(&step_out, &dir, joan::orig::arg::f32(u32{0x40a00000})));  // 0x0060cd00 (* 5)
            V3 step = *s;
            joan::orig::vec_add_in_place(static_cast<void*>(&cam.d[1]), &step);  // 0x0060cdc0 (+=)
            cam.d[5] = 0x3f490fdb;                                              // pi / 4
            void* const m2 = cam_motion(joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamMan)->target()));
            joan::orig::as_camera_motion(m2)->start(joan::orig::arg::mptr(static_cast<void*>(&cam)), joan::orig::arg::mptr(static_cast<void*>(&cam)));  // 0x00423ec0
        }
        jo::view<u32>(jo::self<jo::Player>(this)->zoom_dist) = cam.d[7];
        return true;
    }

    // ------------------------------------------------------------------ 0x00442ef0
    void rts_mode() {
        c_rts.hit();
        jo::self<jo::Player>(this)->prev_rts_active = jo::self<jo::Player>(this)->rts_active;
        if (jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) == 10) {
            if (joan::orig::arg::as<i8>(joan::orig::as_barracks_screen(obj(d::g_barracks_screen))->is_open()) != 0 ||  // 0x006c03e0
                joan::orig::arg::as<i8>(joan::orig::as_workshop_screen(obj(d::g_workshop_screen))->is_open()) != 0) {  // 0x006ed4c0
                jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 0;
            }
        }
        joan::orig::as_camera_zones(jo::view<void*>(jo::self<jo::Player>(this)->cam_zones))->update();  // 0x00425fc0
        if (zone_busy() == 0 && jo::self<jo::Player>(this)->rts_active == 0) {
            joan::orig::as_camera_man(kCamMan)->set_mode(i32{4});  // 0x00429860
            jo::self<jo::Player>(this)->rts_active = 1;
        }
        if (zone_busy() == 0 && jo::self<jo::Player>(this)->rts_active == 2) {
            if (!rts_start()) return;
        }
        if (zone_busy() == 0 && jo::self<jo::Player>(this)->rts_active == 3) {
            joan::orig::as_camera_man(kCamMan)->set_mode(i32{0});
            jo::self<jo::Player>(this)->rts_active = 4;
        }
        if (zone_busy() == 0) {
            alignas(4) u8 view[0x28];  // [ebp-0xa0], unused afterwards
            joan::orig::as_camera(global<void*>(d::g_camera))->frame(joan::orig::arg::mptr(static_cast<void*>(view)));  // 0x006140a0
            float quad[12] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
            quad[0] = 0.0f;
            quad[1] = 0.0f;
            quad[3] = 0.0f;
            quad[4] = static_cast<float>(static_cast<i32>(global<u32>(d::g_screen_h) - 1u));
            quad[6] = static_cast<float>(static_cast<i32>(global<u32>(d::g_screen_w) - 1u));
            quad[7] = 0.0f;
            quad[9] = static_cast<float>(static_cast<i32>(global<u32>(d::g_screen_w) - 1u));
            quad[10] = static_cast<float>(static_cast<i32>(global<u32>(d::g_screen_h) - 1u));
            joan::orig::as_player(this)->map_quad(joan::orig::arg::mptr(static_cast<void*>(quad)), joan::orig::arg::mptr(jo::self<jo::Player>(this)->view_quad));  // 0x00452320
        }
        anim_switch(now());
        if (zone_busy() == 0 && jo::self<jo::Player>(this)->rts_active == 1) {
            if (!pull_back()) return;
        }
        if (jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) == 0xb) {
            if (joan::orig::arg::as<i8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) == 0) {  // 0x004301c0
                u8* const u = unit_rec(joan::orig::arg::as<u32>(joan::orig::as_group(this)->first_ready_member()));  // 0x00452570
                if (jo::as_unit(u)->task.kind == 7) joan::orig::as_unit(u)->release_slot();  // 0x00528300
                jo::view<u32>(jo::self<jo::Player>(this)->cursor_mode) = 0;
            }
        }
        joan::orig::as_world(this)->cull_lights_452d20();  // 0x00452d20
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_14", name, addr, fn)
B("Game::rts_mode_442ef0", 0x00442ef0, &H::rts_mode);
#undef B

}  // namespace game_b110_14
}  // namespace

// ---------------------------------------- from modules/game_b104_01.cpp (game/b104_01)
namespace {
namespace game_b104_01 {

void* const kItemTypes = obj(d::g_item_res);

using u32 = std::uint32_t;

struct FVec {
    float x, y, z;
};
static_assert(sizeof(FVec) == 12, "by-value 12-byte argument");

JOAN_COUNTER(c_rect_444a20, "Map::rect_hit_444a20");

u8* item_type(u32 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kItemTypes)->get(joan::orig::arg::i16(w))); }  // 0x00425ce0

struct H {
    // ------------------------------------------------------------ 0x00444a20
    u32 rect_hit_444a20(FVec a, FVec b) {
        c_rect_444a20.hit();
        u8* r = item_type(static_cast<u32>(jo::view<u16>(jo::self<jo::MapObject>(this)->type_id)));  // x64: the call takes the low word only (arg::i16)
        // signs of (A.x, A.y, B.x, B.y) half extents per pass (true = negated)
        static const bool neg[4][4] = {
            {true, true, true, false}, {true, true, false, true}, {false, false, false, true}, {false, false, true, false}};
        for (int k = 0; k < 4; ++k) {
            auto half = [&](const std::uint16_t& extent, bool n) -> float {
                i32 v = static_cast<i16>(extent);
                if (n) v = -v;
                return static_cast<float>(v / 2);
            };
            const float ax = half(jo::as_item_def(r)->stats[0], neg[k][0]);
            const float ay = half(jo::as_item_def(r)->stats[1], neg[k][1]);
            const float bx = half(jo::as_item_def(r)->stats[0], neg[k][2]);
            const float by = half(jo::as_item_def(r)->stats[1], neg[k][3]);
            const FVec A{ax, ay, 0.0f};  // z: uninitialised in the original, never read
            const FVec B{bx, by, 0.0f};
            float f24, f28, f2c;
            if (joan::orig::tri_normal_2d(a.x, a.y, a.z, b.x, b.y, b.z, A.x, A.y, A.z, B.x, B.y, B.z, &f24, &f28, &f2c) != 0) {  // 0x00664c60
                if (f28 >= 0.0f && !(f28 > f24) && f2c >= 0.0f && !(f2c > f24)) return 1;
            }
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_01", name, addr, fn)
B("Map::rect_hit_444a20", 0x00444a20, &H::rect_hit_444a20);
#undef B

}  // namespace game_b104_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

struct alignas(4) Path {  // 0x005a1170 string (NUL at +0x190), no destructor
    std::uint8_t b[0x194];
};
struct alignas(8) Script {  // 0x00589550 / 0x00589890 text reader (0x19c in the frame)
    std::uint8_t b[((sizeof(jo::FileTxt) > 0x1a0 ? sizeof(jo::FileTxt) : 0x1a0) + 8 - 1) / 8 * 8];  // 0x1a0 on x86; FileTxt is 0x1a6 on x64
};

JOAN_COUNTER(c_water, "World::load_water");

using joan::orig::helpers::stricmp_;

struct H {
    // ------------------------------------------------------------------ 0x00444e70
    void load_water() {
        c_water.hit();
        Path path;
        joan::orig::as_string(&path)->construct();                  // 0x005a1170
        joan::orig::as_string(&path)->assign(str(d::s_MAP_9cf818));                  // 0x005a1440 "MAP\\"
        joan::orig::as_string(&path)->append(joan::orig::arg::str(jo::self<jo::World>(this)->name));  // 0x005a1550
        joan::orig::as_string(&path)->append(str(d::s_WOR));                      // 0x005a1550 ".WOR"
        const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
        Script r;
        joan::orig::as_file_txt(&r)->construct(joan::orig::arg::str(name));  // 0x00589550
        alignas(4) char buf[0x100];
        joan::orig::as_file_txt(&r)->get_token(1);    // 0x00589c70
        joan::orig::as_file_txt(&r)->get_token(1);
        joan::orig::as_file_txt(&r)->get_token(1);
        joan::orig::as_file_txt(&r)->read_line(static_cast<char*>(static_cast<void*>(buf)), 0xff);                         // 0x00589f10
        joan::orig::as_file_txt(&r)->get_token(1);
        joan::orig::as_file_txt(&r)->read_line(static_cast<char*>(static_cast<void*>(buf)), 0xff);
        joan::orig::as_file_txt(&r)->get_token(1);
        for (;;) {
            const char* const tok = joan::orig::arg::as<const char*>(joan::orig::as_file_txt(&r)->get_token(1));
            if (tok == nullptr) break;
            if (stricmp_(tok, str(d::s_CreateWater)) == 0) joan::orig::as_world(this)->create_water(joan::orig::arg::mptr(static_cast<void*>(&r)));  // "CreateWater" 0x00553aa0
        }
        joan::orig::as_file_txt(&r)->destruct();  // 0x00589890
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("World::load_water", 0x00444e70, &H::load_water);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using u32 = std::uint32_t;

void* const kItems = obj(d::g_items);

JOAN_COUNTER(c_check_height, "Unit::check_height");

struct H {
    // ------------------------------------------------------------------ 0x00445000
    void check_height() {
        c_check_height.hit();
        if (joan::orig::as_units(kItems)->list.empty_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->held_item_id)) != 0) return;  // 0x00425d50
        void* const it = joan::orig::as_units(kItems)->list.ptr_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->held_item_id));  // 0x00425d20
        float ipos[3];  // ebp-0x14
        float upos[3];  // ebp-0x20
        joan::orig::as_map_object(it)->position(joan::orig::arg::mptr(static_cast<void*>(ipos)));     // 0x00499930
        joan::orig::as_unit(this)->position(joan::orig::arg::mptr(static_cast<void*>(upos)));   // 0x004fedb0
        std::memcpy(jo::self<jo::Unit>(this)->height_point, upos, 12);
        if (ipos[2] + global<float>(d::flt_9d5360) >= upos[2]) {        // + 0.0001f (NaN -> skip)
            joan::orig::as_unit(this)->set_action(0x5c, 0, 1);              // 0x00501190
            return;
        }
        if (static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 0x59) {
            joan::orig::as_unit(this)->set_action(0x5b, 0, 1);
            return;
        }
        if (static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind) == 0x5a) {
            joan::orig::as_unit(this)->set_action(0x5d, 0, 1);
        }
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Unit::check_height", 0x00445000, &H::check_height);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
namespace {
namespace game_b82_16 {

// 4-byte colour built by 0x0042a260.
struct alignas(4) Rgba {
    std::uint8_t b[4];
};

JOAN_COUNTER(c_cursor, "Cursor::draw");

char* strncpy_(char* d, const char* s, std::uint32_t n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
std::uint32_t make_rgba(Rgba* c, std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260
}

struct H {
    // ------------------------------------------------------------------ 0x004450f0
    void set_cursor(img::Addr name) { strncpy_(jo::self<jo::Player>(this)->point_name, str(name), 8u); }

    // Attack cursor by the selected unit's kind (shared by order cases 1 and 2).
    void attack_cursor() {
        const std::int32_t idx = jo::self<jo::Player>(this)->group;
        if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->group_counts[idx]) == 1 &&
            joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Player>(this)->group_units[jo::self<jo::Player>(this)->group * 100]))) == 0) {  // 0x00428b70
            void* const unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Player>(this)->group_units[jo::self<jo::Player>(this)->group * 100])));  // 0x0041b3c0
            std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
            switch (jo::as_unit_type(def)->unit_class) {
                case 1: set_cursor(d::s_INFATT00); break;  // INFATT00
                case 2:
                case 3:
                case 4: set_cursor(d::s_HVYATT00); break;  // HVYATT00
                case 5:
                case 6: set_cursor(d::s_ARCATT00); break;  // ARCATT00
                case 7: break;
                default: set_cursor(d::s_INFATT00); break;
            }
        } else {
            set_cursor(d::s_INFATT00);
        }
    }

    void cursor_draw() {
        c_cursor.hit();
        if (global<std::uint8_t>(d::g_game_running) == 0) {
            if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->castle_popup_open) != 0) {
                joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
                joan::orig::as_dialog(obj(d::g_popup_a52df8))->init();  // 0x006d5080
            }
            return;
        }
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) != 0) return;  // 0x004301c0
        if (global<std::int8_t>(d::g_flag_a59298) == 2) return;
        if (jo::self<jo::Player>(this)->rtsicon_res == nullptr) return;
        joan::orig::as_cursor(obj(d::g_mouse))->set_pos();  // 0x00592bd0
        Rgba c8;
        joan::orig::as_colour(&c8)->construct(0xffu, 0xffu, 0xffu, 0xffu);  // 0x0042a260 (unused)
        if (jo::view<std::int8_t>(jo::self<jo::Player>(this)->dragging) != 0) {
            const std::int32_t mx = global<std::int32_t>(d::g_mouse_x), my = global<std::int32_t>(d::g_mouse_y);
            const float v1 = static_cast<double>(mx) > static_cast<double>(jo::self<jo::Player>(this)->drag_x) ? jo::self<jo::Player>(this)->drag_x
                                                                                                : static_cast<float>(mx);
            const std::int16_t x0 = static_cast<std::int16_t>(joan::hook::x87::ftol(v1));
            const float v2 = static_cast<double>(my) > static_cast<double>(jo::self<jo::Player>(this)->drag_y) ? jo::self<jo::Player>(this)->drag_y
                                                                                                : static_cast<float>(my);
            const std::int16_t y0 = static_cast<std::int16_t>(joan::hook::x87::ftol(v2));
            const float v3 = !(static_cast<double>(mx) >= static_cast<double>(jo::self<jo::Player>(this)->drag_x)) ? jo::self<jo::Player>(this)->drag_x
                                                                                                   : static_cast<float>(mx);
            const std::int16_t x1 = static_cast<std::int16_t>(joan::hook::x87::ftol(v3));
            const float v4 = !(static_cast<double>(my) >= static_cast<double>(jo::self<jo::Player>(this)->drag_y)) ? jo::self<jo::Player>(this)->drag_y
                                                                                                   : static_cast<float>(my);
            const std::int16_t y1 = static_cast<std::int16_t>(joan::hook::x87::ftol(v4));
            const std::uint32_t colour = global<std::uint32_t>(d::g_a33724);
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue_frame(joan::orig::arg::i16(x0), joan::orig::arg::i16(y0), joan::orig::arg::i16(x1), joan::orig::arg::i16(y1), 1u, joan::orig::arg::u32(colour), joan::orig::arg::i16(1), nullptr, joan::orig::arg::i16(0), joan::orig::arg::i16(0), joan::orig::arg::i16(0), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(-1), joan::orig::arg::i16(0));  // 0x00633070
        }
        if (jo::view<std::int32_t>(jo::self<jo::Player>(this)->point_kind) == 0) {
            switch (jo::self<jo::Player>(this)->pick_kind) {
                case 0: set_cursor(d::s_POINT00); break;  // POINT00
                case 1:
                    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Player>(this)->pick_id))) != 0 ||  // 0x00428b70
                        jo::view<std::int8_t>(jo::self<jo::Player>(this)->group_counts[jo::self<jo::Player>(this)->group]) == 0)
                        set_cursor(d::s_POINT00);
                    attack_cursor();
                    joan::orig::as_hud(obj(d::g_player_hud))->set_item_portrait(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Player>(this)->pick_id)));  // 0x006fe280
                    break;
                case 2:
                    if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Player>(this)->pick_id))) != 0 ||  // 0x00425d50
                        jo::view<std::int8_t>(jo::self<jo::Player>(this)->group_counts[jo::self<jo::Player>(this)->group]) == 0)
                        set_cursor(d::s_POINT00);
                    attack_cursor();
                    joan::orig::as_hud(obj(d::g_player_hud))->set_unit_portrait(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Player>(this)->pick_id)));  // 0x006fe340
                    break;
                case 6: set_cursor(d::s_ASSTO_00); break;  // ASSTO_00
                case 7: set_cursor(d::s_ASSIGN01); break;  // ASSIGN01
                case 8: set_cursor(d::s_FOLLOW01); break;  // FOLLOW01
                default: break;
            }
            jo::view<std::int32_t>(jo::self<jo::Player>(this)->click_time) = global<std::int32_t>(d::g_now);
        }
        jo::view<std::uint8_t>(jo::self<jo::Player>(this)->point_name[8]) = 0;
        std::int32_t x, y;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::libc::strncmp(joan::orig::arg::str(jo::self<jo::Player>(this)->point_name), str(d::s_POINT), 5u)) == 0) {
            x = global<std::int32_t>(d::g_mouse_x);  // strncmp "POINT"
            y = global<std::int32_t>(d::g_mouse_y);
        } else {
            std::uint8_t* const img = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_bmp_res(obj(d::g_bmp_res_rtsicon))->find(joan::orig::arg::str(jo::self<jo::Player>(this)->point_name)));  // 0x00620b40
            x = global<std::int32_t>(d::g_mouse_x) - static_cast<std::int16_t>(jo::as_bmp_entry(img)->width) / 2;
            y = global<std::int32_t>(d::g_mouse_y) - static_cast<std::int16_t>(jo::as_bmp_entry(img)->height) / 2;
        }
        Rgba c38;
        const std::uint32_t colour = make_rgba(&c38, 0xffu, 0xffu, 0xffu, 0xffu);
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 1u, joan::orig::arg::str(jo::self<jo::Player>(this)->point_name), joan::orig::arg::mptr(jo::self<jo::Player>(this)->rtsicon_res), joan::orig::arg::u32(colour), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("Cursor::draw", 0x004450f0, &H::cursor_draw);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world2)
namespace {
namespace game_world {

bool equal_or_nan(float a, float b) { return !(a < b || a > b); }

JOAN_COUNTER(c_free_cells, "World::free_far_cells");

struct H {
    // World (0xa45f3e): +0x6e scene, +0x72 cell count, +0x7a cells. Cell: +8 box (min, max),
    // +0x43 state (0 unloaded, 1 loaded, 2 to load, 3 keep).
    std::uint8_t* cell(std::int32_t i) { return jo::view<std::uint8_t**>(jo::self<jo::World>(this)->cells)[i]; }
    // 0x004457e0 World::free_far_cells(boxes, count): loaded cells not overlapping (as the
    // original tests it) any box release their textures (0x0043f4e0).
    void free_far_cells(const std::uint8_t* boxes, std::int32_t count) {
        c_free_cells.hit();
        for (std::int32_t i = 0; i < jo::self<jo::World>(this)->cell_count; ++i) {
            std::uint8_t* const c = cell(i);
            if (static_cast<std::int8_t>(jo::as_cell(c)->loaded) == 0) continue;
            float r[6];
            std::memcpy(r, &jo::as_cell(c)->box, sizeof r);
            bool hit = false;
            for (std::int32_t k = 0; k < count; ++k) {
                const float* const b = reinterpret_cast<const float*>(boxes + k * 0x18);
                if (b[0] > r[0]) continue;
                if (equal_or_nan(r[3], 0.0f)) continue;
                if (equal_or_nan(b[3], 0.0f)) continue;
                if (b[1] > r[1]) continue;
                if (equal_or_nan(r[4], 0.0f)) continue;
                if (equal_or_nan(b[4], 0.0f)) continue;
                if (b[2] > r[2]) continue;
                if (equal_or_nan(r[5], 0.0f)) continue;
                if (equal_or_nan(b[5], 0.0f)) continue;
                hit = true;
                break;
            }
            if (!hit) joan::orig::as_cell(c)->free_textures(joan::orig::arg::mptr(static_cast<const void*>(boxes)), joan::orig::arg::i32(count));  // 0x0043f4e0
        }
    }
};

#define W2(name, addr, fn) JOAN_REPLACE("game/world2", name, addr, fn)
W2("World::free_far_cells", 0x004457e0, &H::free_far_cells);
#undef W2

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b110_12.cpp (game/b110_12)
namespace {
namespace game_b110_12 {

void* const kCams = obj(d::g_camera_man);

// v3d_87.dll exports are cdecl, called through their IAT slot.
using joan::orig::helpers::v3d_call;

using u32 = std::uint32_t;

using joan::orig::helpers::at;

JOAN_COUNTER(c_cam, "Camera::update_follow_445990");

// ---------------------------------------------------------------- 0x00445990
struct Vf {  // D3DVECTOR
    float x, y, z;
};
static_assert(sizeof(Vf) == 12, "Vf");
struct Sphere {  // 0x005acc80 output (centre + radius)
    float x, y, z, r;
};
static_assert(sizeof(Sphere) == 16, "Sphere");
struct Box {
    Vf a, b;
};
static_assert(sizeof(Box) == 24, "Box");
struct Mat {
    float m[16];
};
static_assert(sizeof(Mat) == 64, "Mat");
// Camera slot copy (0x00423fe0 copies 10 dwords from slot + 0x28).
struct Cam {
    u32 id;
    float x, y, z;
    u32 f10;
    u32 f14;  // angle (pi/4 written as raw bits)
    u32 f18;
    u32 f1c;  // look-ahead scale, passed raw to 0x0060cd00
    u32 f20, f24;
};
static_assert(sizeof(Cam) == 40, "Cam");

u8* cam_slot() {
    const u32 index = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(kCams)->target());  // 0x00563dc0
    return joan::orig::arg::as<u8*>(joan::orig::as_camera_follow(kCams)->motion(joan::orig::arg::i32(index)));      // 0x00563c80
}
// The player camera bone's world matrix (Bone::matrix_, x86 +0x58): row 0 = side, row 2 (+0x20) = dir.
const u8* player_matrix() { return jo::as_camera(global<u8*>(d::g_camera))->bone_->matrix_; }  // **0xa48ccc
Vf load3(const void* p) {
    Vf v;
    std::memcpy(&v, p, sizeof v);
    return v;
}

struct Ground {
    bool found;  // 0x00429090 out flag
    bool have;   // [ebp-0x78]
    float h;     // [ebp-0x7c]
};

// [ebp-0x50] probe position (also the box centre), wpos = 0x00469d10's position argument.
// 0x00429090 / 0x005ef610 read a box (min, max): the degenerate box {pos, pos} (the caller 0x00445990 keeps a copy
// of [ebp-0x50] at [ebp-0x44]).
Ground probe(Vf* pos, Vf* wpos) {
    Ground g{false, false, 0.0f};
    Vf pos_box[2] = {*pos, *pos};
    i32 found = 0;
    u8** const cell = joan::orig::arg::as<u8**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(joan::orig::arg::mptr(pos_box), &found));  // 0x00429090
    if (found == 0) return g;
    g.found = true;
    u8* const tile = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(*cell)->grid(joan::orig::arg::i16(0)));  // 0x005600c0
    if (tile != nullptr) {
        const Fpu fpu{true};
        const float fx = static_cast<float>(fpu.div(fpu.sub(pos->x, static_cast<double>(jo::as_cell(*cell)->origin_x)),
                                                    static_cast<double>(jo::as_path_space(tile)->cell_w)));
        const float fy = static_cast<float>(fpu.div(fpu.sub(pos->y, static_cast<double>(jo::as_cell(*cell)->origin_y)),
                                                    static_cast<double>(jo::as_path_space(tile)->cell_h)));
        const i32 ix = ftol(static_cast<double>(fx));
        const i32 iy = ftol(static_cast<double>(fy));
        g.have = true;
        const i32 even = ((static_cast<u32>(ix) + static_cast<u32>(iy)) & 1u) == 0 ? 1 : 0;
        g.h = joan::orig::arg::as<float>(joan::orig::as_terrain(jo::as_path_space(tile)->grid)->height_4c7b70(joan::orig::arg::f32(fx), joan::orig::arg::f32(fy), static_cast<char>(joan::orig::arg::i32(even))));  // 0x004c7b70
    }
    u8* const wall = joan::orig::arg::as<u8*>(joan::orig::as_waters(obj(d::g_water_array))->find_at(joan::orig::arg::mptr(wpos), 0));  // 0x00469d10
    if (wall != nullptr) {
        if (g.have) {
            if (jo::as_water(wall)->box[2] > g.h) g.h = jo::as_water(wall)->box[2];  // the region box z
        } else {
            g.h = jo::as_water(wall)->box[2];
            g.have = true;
        }
    }
    i32 count = 0;  // always written by 0x005ef610
    u8** const list =
        joan::orig::arg::as<u8**>(joan::orig::as_scene(jo::as_world(global<u8*>(d::g_world))->scene)->nodes_in_box(joan::orig::arg::mptr(pos_box), &count));  // 0x005ef610
    Box box{*pos, *pos};
    const float k1000 = global<float>(d::flt_1000);
    box.a.z = box.a.z + k1000;
    box.b.z = box.b.z - k1000;
    float best = 0.0f;
    u8* best_obj = nullptr;
    for (i32 i = count - 1; i >= 0; --i) {
        u8* const entry = list[i];
        if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(&box, joan::orig::arg::ptr(jo::as_sector(entry)->desc))) == 0) continue;  // 0x0060dfd0
        for (u8* o = joan::orig::arg::as<u8*>(joan::orig::as_scene_node(entry)->attachments()); o != nullptr; o = reinterpret_cast<u8*>(jo::as_actor(o)->scene_next)) {  // 0x005efff0
            u8* const model = reinterpret_cast<u8*>(jo::as_actor(o)->plane);
            if (jo::as_actor(o)->kind != 1) continue;
            if (model != nullptr && jo::as_plane(model)->vertex_format == 7) continue;
            u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(jo::as_map_object(jo::as_actor(o)->owner)->type_id)));  // 0x00425ce0
            if (jo::view<i8>(jo::as_item_def(unit)->has_path) != 2) continue;
            if (model == nullptr) continue;
            Sphere s;
            joan::orig::as_actor(o)->bounding_sphere(&s, 1u);                            // 0x005acc80
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&box, &s)) == 0) continue;  // 0x0060e3f0
            float d = 0.0f;  // written whenever 0x005d4a80 returns nonzero
            if (joan::orig::arg::as<i32>(joan::orig::as_plane(jo::as_actor(o)->plane)->segment_hit(&d, &box)) != 0) {  // 0x005d4a80
                if (best_obj == nullptr || best > d) {
                    best = d;
                    best_obj = o;
                }
            }
        }
    }
    if (best_obj != nullptr) {
        Vf out;
        joan::orig::segment_point_at(&out, &box, joan::orig::arg::f32(best));  // 0x0060de40
        if (g.have) {
            if (out.z > g.h) g.h = out.z;
        } else {
            g.h = out.z;
            g.have = true;
        }
    }
    return g;
}

// cam.xy += [0xa500b6] * dir.xy / 10 (fild qword of the unsigned dword).
void accumulate(Cam& c, const Vf& d) {
    const Fpu fpu{true};
    const double k10 = global<float>(d::flt_10);
    c.x = static_cast<float>(
        fpu.add(fpu.div(fpu.mul(static_cast<double>(global<u32>(d::g_frame_elapsed_ms)), d.x), k10), c.x));
    c.y = static_cast<float>(
        fpu.add(fpu.div(fpu.mul(static_cast<double>(global<u32>(d::g_frame_elapsed_ms)), d.y), k10), c.y));
}

// Key move: dir.z = 0, normalize, scale to 5 (0x0060ce80), accumulate.
void key_move(Cam& c, Vf d) {
    d.z = 0.0f;
    v3d_call<Vf*>(d::iat_v3d_87_vector_normalize_1, &d);                  // vector_normalize
    joan::orig::vec_scale_in_place(&d, joan::orig::arg::f32(0x40a00000u));  // 0x0060ce80 (5.0f)
    accumulate(c, d);
}

// Mouse edge move: rotate the flattened direction, update / clamp the speed +0xe9, accumulate.
// perp: the direction is 0x0060cbc0(src), computed after the rotation matrix as in the original.
void edge_move(u8* self, Cam& c, const Vf& src, bool perp, float angle) {
    Mat m;
    joan::orig::matrix_rotation(&m, joan::orig::arg::f32(angle));  // 0x0060bc50
    Vf d = src;
    if (perp) {
        Vf tmp;
        d = *joan::orig::arg::as<Vf*>(joan::orig::vec_negate(&tmp, &src));  // 0x0060cbc0
    }
    d.z = 0.0f;
    v3d_call<Vf*>(d::iat_v3d_87_vector_normalize_1, &d);      // vector_normalize
    v3d_call<Vf*>(d::iat_v3d_87_vector_transform_2, &d, &m);  // vector_transform
    if (global<i32>(d::g_mouse_delta_x) != 0 || global<i32>(d::g_mouse_delta_y) != 0) {
        const float k0 = global<float>(d::k_9cf5c4);
        if (!(jo::as_player(self)->pan_speed < k0) && !(jo::as_player(self)->pan_speed > k0)) {  // equal or unordered
            const Fpu fpu{true};
            Vf delta{static_cast<float>(global<i32>(d::g_mouse_delta_x)), static_cast<float>(global<i32>(d::g_mouse_delta_y)), 0.0f};
            const double len = v3d_call<double>(d::iat_v3d_87_length, &delta);  // length
            jo::as_player(self)->pan_speed = static_cast<float>(fpu.mul(len, global<float>(d::flt_0_3)));
            const double r = fpu.add(fpu.mul(fpu.div(jo::as_player(self)->zoom_dist, global<float>(d::flt_2500)),
                                             global<float>(d::flt_35)),
                                     global<float>(d::flt_5));
            const i32 limit = ftol(r);
            const double dl = static_cast<double>(limit);
            if (dl > static_cast<double>(jo::as_player(self)->pan_speed)) jo::as_player(self)->pan_speed = static_cast<float>(limit);
            if (!(dl >= static_cast<double>(jo::as_player(self)->pan_speed))) jo::as_player(self)->pan_speed = static_cast<float>(limit);
        }
    }
    joan::orig::vec_scale_in_place(&d, joan::orig::arg::f32(jo::view<u32>(jo::as_player(self)->pan_speed)));  // 0x0060ce80
    accumulate(c, d);
}

// `call 0x0040ab14` (crt_atan2 0x00421020, its CRT atan2 st0), `add esp, 8; fadd dword [k]` under PC24: the
// wrapper's result as the double its facade returns, plus k rounded once to 24 bits (Fpu::add; |sum| is far from
// the float denormal range). The original CRT atan2 st0 has a 64-bit significand, which the double rounds first:
// over all integer (y, x) in [-2048, 2048]^2 and the three k this differs once (y = -1555, x = -724, k =
// 2.1816616f; none for |x|, |y| <= 1024), more often for x tiny against y (re/p1/gamefloat).
float atan2_fadd(float y, float x, const float* k) {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan2_f(joan::orig::arg::f32(y), joan::orig::arg::f32(x)));
    return static_cast<float>(Fpu{true}.add(a, static_cast<double>(*k)));
}
const float* cfp(img::Addr a) { return &img::global<const float>(a); }

// Hand the camera to 0x00453930 (by value) and write it back when accepted.
void commit(u8* self, Cam& c) {
    if (joan::orig::arg::as<i8>(joan::orig::as_player(self)->corners_on_map(c)) != 0) {  // 0x00453930
        u8* const slot = cam_slot();
        joan::orig::as_camera_motion(slot)->start(&c, &c);  // 0x00423ec0
    }
}

struct H {
    // 0x00445990
    void update_follow(i8 flags) {
        c_cam.hit();
        u8* const self = reinterpret_cast<u8*>(this);
        Cam c;
        joan::orig::as_camera_motion(cam_slot())->end(&c);  // 0x00423fe0
        const Vf dir = load3(player_matrix() + 0x20);

        // Lift the target.
        Vf p50{c.x, c.y, c.z};
        {
            const Ground g = probe(&p50, reinterpret_cast<Vf*>(&c.x));
            if (g.found && g.have) {
                if (g.h > c.z) {
                    c.z = g.h;
                } else {  // [ebp-0x64] is still 0 here
                    const Fpu fpu{true};
                    double v = fpu.sub(g.h, c.z);
                    v = fpu.mul(v, static_cast<double>(global<i32>(d::g_frame_step_ms)));
                    v = fpu.div(v, global<float>(d::flt_1000));
                    c.z = static_cast<float>(fpu.add(v, c.z));
                }
            }
        }
        // Lift for the look-ahead point.
        {
            Vf t1;
            Vf* const r1 = joan::orig::arg::as<Vf*>(joan::orig::vec_scale(&t1, &dir, joan::orig::arg::f32(c.f1c)));  // 0x0060cd00
            Vf ahead = *r1;
            Vf t2;
            Vf* const r2 = joan::orig::arg::as<Vf*>(joan::orig::vec_sub(&t2, &c.x, &ahead));  // 0x0060cc90
            Vf p60 = *r2;
            p50 = p60;
            const Ground g = probe(&p50, &p60);
            if (g.found && g.have) {
                const float k40 = global<float>(d::flt_40);
                if (g.h + k40 > p60.z) c.z = ((g.h + k40) - p60.z) + c.z;  // [ebp-0x64] = 1 (unused)
            }
        }
        const Vf side = load3(player_matrix());
        if ((flags & 1) != 0) key_move(c, dir);
        if ((flags & 2) != 0) {
            Vf t;
            key_move(c, *joan::orig::arg::as<Vf*>(joan::orig::vec_negate(&t, &dir)));  // 0x0060cbc0
        }
        if ((flags & 4) != 0) {
            Vf t;
            key_move(c, *joan::orig::arg::as<Vf*>(joan::orig::vec_negate(&t, &side)));
        }
        if ((flags & 8) != 0) key_move(c, side);

        // Mouse at the screen edges.
        void* const mouse = obj(d::g_mouse);
        const i32 w = global<i32>(d::g_screen_w), h = global<i32>(d::g_screen_h);
        if (joan::orig::arg::as<i32>(joan::orig::as_point(mouse)->inside(i32{10}, i32{10}, joan::orig::arg::i32(w - 10), joan::orig::arg::i32(h - 10))) != 0) {  // 0x00593980
            jo::view<u32>(jo::as_player(self)->pan_speed) = 0;
            c.f14 = 0x3f490fdbu;
            commit(self, c);
            return;
        }
        const Fpu fpu{true};
        auto mx = [] { return global<u32>(d::g_mouse_x); };
        auto my = [] { return global<u32>(d::g_mouse_y); };
        // fx = mx - W/2, fy = -my - H/2 (fild of the int results).
        auto fx_a = [&] {
            return static_cast<float>(static_cast<i32>(mx() - static_cast<u32>(global<i32>(d::g_screen_w) / 2)));
        };
        auto fy_a = [&] {
            return static_cast<float>(
                static_cast<i32>(0u - my() - static_cast<u32>(global<i32>(d::g_screen_h) / 2)));
        };
        if (joan::orig::arg::as<i32>(joan::orig::as_point(mouse)->inside(i32{0}, i32{0}, joan::orig::arg::i32(global<i32>(d::g_screen_w)), i32{10})) != 0) {
            const float fx = fx_a(), fy = fy_a();
            const float t = atan2_fadd(fy, fx, cfp(d::flt_1_5707964));  // 0x00421020 + fadd
            edge_move(self, c, dir, false, -t);
        } else if (joan::orig::arg::as<i32>(joan::orig::as_point(mouse)->inside(i32{0}, joan::orig::arg::i32(global<i32>(d::g_screen_h) - 10), joan::orig::arg::i32(global<i32>(d::g_screen_w)), joan::orig::arg::i32(global<i32>(d::g_screen_h)))) != 0) {
            const float fx = static_cast<float>(
                static_cast<i32>(static_cast<u32>(global<i32>(d::g_screen_w) / 2) - mx()));
            const i32 nh = static_cast<i32>(0u - static_cast<u32>(global<i32>(d::g_screen_h)));
            const float fy = static_cast<float>(static_cast<i32>(static_cast<u32>(nh / 2) + my()));
            const float t = atan2_fadd(fy, fx, cfp(d::flt_1_5707964));
            edge_move(self, c, dir, false, t);
        } else if (joan::orig::arg::as<i32>(joan::orig::as_point(mouse)->inside(i32{0}, i32{10}, i32{10}, joan::orig::arg::i32(global<i32>(d::g_screen_h) - 10))) != 0) {
            const float fx = fx_a(), fy = fy_a();
            const float t = atan2_fadd(fy, fx, cfp(d::flt_2_1816616));
            const float angle = static_cast<float>(fpu.mul(t, global<float>(d::flt_3)));
            edge_move(self, c, side, true, angle);
        } else if (joan::orig::arg::as<i32>(joan::orig::as_point(mouse)->inside(joan::orig::arg::i32(global<i32>(d::g_screen_w) - 10), i32{10}, joan::orig::arg::i32(global<i32>(d::g_screen_w)), joan::orig::arg::i32(global<i32>(d::g_screen_h) - 10))) != 0) {
            const float fx = fx_a(), fy = fy_a();
            const float t = atan2_fadd(fy, fx, cfp(d::flt_0_9599311));
            const float angle = static_cast<float>(fpu.mul(t, global<float>(d::flt_3)));
            edge_move(self, c, side, false, angle);
        }
        commit(self, c);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b110_12", name, addr, fn)
B("Camera::update_follow_445990", 0x00445990, &H::update_follow);
#undef B

}  // namespace game_b110_12
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

using u32 = std::uint32_t;

// KERNEL32 / USER32 (stdcall) import through its IAT slot.
using joan::orig::helpers::win_call;

constexpr img::Addr kMessageBoxA = d::iat_USER32_MessageBoxA;

JOAN_COUNTER(c_check_v3d, "check_v3d_module_447150");

// ------------------------------------------------------------------ 0x00447150
// v3d_87.dll is ours (no DLL file is involved any more); see the module comment at the top.
static i32 JOAN_CDECL check_v3d_module() {
    c_check_v3d.hit();
    if (import_call<i32>(d::iat_v3d_87_vertexf_check_cpu) == 0) {  // v3d vertexf_check_cpu (ours)
        win_call(kMessageBoxA, static_cast<void*>(nullptr), str(d::s_V3d_87_invalid), str(d::s_Wars_Warriors_Joan_of_Arc),
                 u32{0});  // "V3d_87 invalid"
        return 1;
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("check_v3d_module_447150", 0x00447150, &check_v3d_module);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

void* const kSlots = obj(d::g_nations);
void* const kLevel = obj(d::g_player_unit_recno);

JOAN_COUNTER(c_sync_shadow, "WorldObj::sync_shadow");
JOAN_COUNTER(c_finish_level, "World::finish_level");

struct H {
    // ---- 0x00447600 WorldObj::sync_shadow() (world.cpp) -----------------------------------
    void sync_shadow() {
        c_sync_shadow.hit();
        if (jo::self<jo::Unit>(this)->group == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<std::uint32_t>(sizeof(jo::Group)));  // operator new (0x58)
            void* rec = nullptr;
            if (mem != nullptr) rec = joan::orig::as_group(mem)->construct();  // 0x00528660
            jo::self<jo::Unit>(this)->group = static_cast<jo::Group*>(rec);
        }
        jo::as_group(jo::self<jo::Unit>(this)->group)->leader = jo::self<jo::Unit>(this)->id;
        jo::as_group(jo::self<jo::Unit>(this)->group)->formation = 1;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(kSlots)->is_null(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side))) == 0) {  // 0x00484870
            void* const slot = joan::orig::as_dynarray(kSlots)->ptr_at(static_cast<std::int32_t>(jo::self<jo::Unit>(this)->side));  // 0x0047b600
            joan::orig::as_nation(slot)->update_unit(joan::orig::arg::mptr(static_cast<void*>(this)));  // 0x004b5e40
        }
        std::memcpy(&jo::as_group(jo::self<jo::Unit>(this)->group)->x, jo::self<jo::Unit>(this)->position_, 12);  // x, y, z
        jo::view<std::uint32_t>(jo::as_group(jo::self<jo::Unit>(this)->group)->angle) = jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->heading);
        std::memcpy(&jo::as_group(jo::self<jo::Unit>(this)->group)->front_x, jo::self<jo::Unit>(this)->position_, 12);  // front_x, front_y, +0x50
        jo::as_group(jo::self<jo::Unit>(this)->group)->u32_54 = jo::view<std::uint32_t>(jo::self<jo::Unit>(this)->heading);
    }

    // ---- 0x00447860 World::finish_level() (world.cpp) --------------------------------------
    std::int8_t finish_level() {
        c_finish_level.hit();
        if (static_cast<std::int32_t>(joan::orig::arg::as<std::int8_t>(joan::orig::as_world(this)->state())) != 4) return 0;  // 0x00447900
        joan::orig::as_player(kLevel)->set_follow(1);  // 0x00436190
        joan::orig::as_player(kLevel)->set_follow(0);
        while (global<std::int32_t>(d::g_a48cef) == 0 && global<std::int8_t>(d::g_a5ba5c) != 0) {
            joan::orig::as_player(kLevel)->update_camera();  // 0x004384e0
        }
        if (global<void*>(d::g_camera) != nullptr) joan::orig::as_camera(global<void*>(d::g_camera))->set_light(joan::orig::arg::mptr(1));  // 0x006169d0
        return 1;
    }

    // ---- 0x00447930 World::lerp(a, b, t) ret 0xc: fld b, fsub a, fmul t, fadd a ------------
    float lerp(float a, float b, float t) { return (b - a) * t + a; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("WorldObj::sync_shadow", 0x00447600, &H::sync_shadow);
B("World::finish_level", 0x00447860, &H::finish_level);
B("World::lerp", 0x00447930, &H::lerp);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using u32 = std::uint32_t;

struct State40 {
    u32 d[10];
};

JOAN_COUNTER(c_turn, "Obj::turn_447740");

struct H {
    // ------------------------------------------------------------------ 0x00447740
    void turn_447740(u32 dir_arg) {
        c_turn.hit();
        const i32 dir = static_cast<i8>(dir_arg & 0xff);
        if (dir == 0) return;
        State40 s{};
        {
            const u32 slot = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(obj(d::g_camera_man))->target());         // 0x00563dc0
            void* const src = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(joan::orig::arg::i32(slot)));  // 0x00563c80
            joan::orig::as_camera_motion(src)->end(joan::orig::arg::mptr(static_cast<void*>(&s)));                // 0x00423fe0
        }
        const Fpu fpu{true};
        double t = static_cast<double>(global<u32>(d::g_frame_elapsed_ms));
        t = fpu.mul(t, 0.15000000596046448);   // 0x9cff3c
        t = fpu.mul(t, 0.01745329238474369);   // 0x9cf884
        float a;
        std::memcpy(&a, &s.d[4], 4);
        const double r = dir == 1 ? fpu.add(t, a) : fpu.sub(a, t);
        const float rf = static_cast<float>(r);
        std::memcpy(&s.d[4], &rf, 4);
        s.d[5] = 0x3f490fdb;
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_player(this)->corners_on_map(s))) == 0) return;  // 0x00453930
        const u32 slot = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(obj(d::g_camera_man))->target());
        void* const dst = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(obj(d::g_camera_man))->motion(joan::orig::arg::i32(slot)));
        joan::orig::as_camera_motion(dst)->start(joan::orig::arg::mptr(static_cast<void*>(&s)), joan::orig::arg::mptr(static_cast<void*>(&s)));  // 0x00423ec0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Obj::turn_447740", 0x00447740, &H::turn_447740);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b87_05.cpp (game/b87_05)
namespace {
namespace game_b87_05 {

struct H {
    // ------------------------------------------------------------ 0x00447900
    // `this` is the Player (its only caller, World::finish_level 0x00447860, runs on the Player): the low byte of
    // Player::rts_active (+0xf5; 4 = leaving RTS mode), not a World field.
    std::uintptr_t world_state() {
        return (reinterpret_cast<std::uintptr_t>(this) & 0xffffff00u) | static_cast<std::uint8_t>(jo::self<jo::Player>(this)->rts_active);  // mov eax,this; mov al
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_05", name, addr, fn)
B("World::state", 0x00447900, &H::world_state);
#undef B

}  // namespace game_b87_05
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

void* const kCameras = obj(d::g_camera_man);

using u32 = std::uint32_t;

u32 bits(float v) {
    u32 b;
    std::memcpy(&b, &v, 4);
    return b;
}
float as_float(u32 b) {
    float v;
    std::memcpy(&v, &b, 4);
    return v;
}

struct Box {  // 40-byte camera box (0x00423fe0 copies +0x28..+0x50 of the camera)
    u32 d[10];
};
static_assert(sizeof(Box) == 40, "camera box");

JOAN_COUNTER(c_zoom, "Unit::zoom_step");

// Camera of the active view: 0x00563c80(0x00563dc0()) on 0xa48cc0.
void* active_camera() {
    const u32 id = joan::orig::arg::as<u32>(joan::orig::as_camera_follow(kCameras)->target());  // 0x00563dc0
    return joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCameras)->motion(joan::orig::arg::i32(id)));    // 0x00563c80
}

struct H {
    // ------------------------------------------------------------------ 0x00447960
    std::uintptr_t zoom_step(char direction) {  // EAX = this / 0
        c_zoom.hit();
        if (static_cast<i32>(direction) == 0) return 0;
        Box box{};
        joan::orig::as_camera_motion(active_camera())->end(&box);  // 0x00423fe0
        double step = static_cast<double>(global<u32>(d::g_frame_elapsed_ms) << 1);
        // A wheel event lasts one frame, so the original's step shrinks with the frame time: 66 units a notch (120 counts) at
        // the 30 frames per second it was made for, almost nothing at hundreds. A notch keeps those 66 units at any rate.
        if (const i32 wheel = global<i32>(d::g_mouse_wheel); wheel != 0 && joan::hook::settings().fix_wheel_zoom)
            step = std::abs(static_cast<double>(wheel)) * 66.0 / 120.0;
        float dist = as_float(box.d[7]);
        if (static_cast<i32>(direction) == 1)
            dist = static_cast<float>(Fpu{true}.sub(dist, step));
        else
            dist = static_cast<float>(Fpu{true}.add(step, dist));
        box.d[7] = bits(dist);
        if (!(as_float(box.d[7]) >= global<float>(d::g_a0bbf0))) box.d[7] = global<u32>(d::g_a0bbf0);
        if (as_float(box.d[7]) > global<float>(d::flt_2500)) box.d[7] = 0x451c4000u;
        box.d[5] = 0x3f490fdbu;
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_player(this)->corners_on_map(box))) != 0) {  // 0x00453930
            joan::orig::as_camera_motion(active_camera())->start(&box, &box);       // 0x00423ec0 (pushes kept: &box first)
        }
        jo::view<u32>(jo::self<jo::Player>(this)->zoom_dist) = box.d[7];
        return reinterpret_cast<std::uintptr_t>(this);
    }
};

JOAN_REPLACE("game/b108_07", "Unit::zoom_step", 0x00447960, &H::zoom_step);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using u32 = std::uint32_t;

static_assert(sizeof(Vec3) == 12, "Vec3");
struct Ray {
    Vec3 o, d;
};
static_assert(sizeof(Ray) == 0x18, "Ray");
struct Blob24 {
    u32 d[6];
};
struct Blob20 {
    u32 d[5];
};
struct alignas(4) CamState {  // 0x00423fe0 copies 10 dwords
    u8 b[0x28];
};

JOAN_COUNTER(c_pick_object, "World::pick_object_447ab0");

struct H {
    // ------------------------------------------------------------------ 0x00447ab0
    void* pick_object_447ab0(i32 x, i32 y, Vec3* out) {
        c_pick_object.hit();
        CamState cam;
        {
            const i32 idx = joan::orig::arg::as<i32>(joan::orig::as_camera_follow(kCamMan)->target());                     // 0x00563dc0
            void* c = joan::orig::arg::as<void*>(joan::orig::as_camera_follow(kCamMan)->motion(joan::orig::arg::i32(idx)));                    // 0x00563c80
            joan::orig::as_camera_motion(c)->end(&cam);                                    // 0x00423fe0
        }
        float box[6];
        const float* const cpos = jo::as_cam_params(&cam)->position;
        box[0] = cpos[0] - global<float>(d::flt_1000);
        box[1] = cpos[1] - global<float>(d::flt_1000);
        box[2] = cpos[2] - global<float>(d::flt_5000);
        box[3] = cpos[0] + global<float>(d::flt_1000);
        box[4] = cpos[1] + global<float>(d::flt_1000);
        std::memcpy(&box[5], &cpos[2], 4);
        if (global<void*>(d::g_world) == nullptr) return nullptr;
        i32 count = 0;
        u8** nodes = joan::orig::arg::as<u8**>(joan::orig::as_world(global<void*>(d::g_world))->cells_in_box(joan::orig::arg::mptr(static_cast<void*>(box)), &count));  // 0x00429090
        u32 sx = 0, sy = 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_grid(global<void*>(d::g_camera))->to_cell(joan::orig::arg::i32(x), joan::orig::arg::i32(y), &sx, &sy)) == 0) return nullptr;  // 0x006152b0
        Ray ray;
        joan::orig::as_camera(global<void*>(d::g_camera))->screen_ray(joan::orig::arg::i32(sx), joan::orig::arg::i32(sy), &ray);  // 0x006154d0
        void* best = nullptr;
        float best_t = 2.0f;
        for (i32 i = count - 1; i >= 0; --i) {
            u8* node = nodes[i];
            if (joan::orig::arg::as<i32>(joan::orig::segment_hits_box(&ray, joan::orig::arg::ptr(jo::as_cell(node)->box))) == 0) continue;  // 0x0060dfd0
            for (u8* e = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(node)->get_90()); e != nullptr;     // 0x0054eec0
                 e = joan::orig::arg::as<u8*>(joan::orig::get_341(joan::orig::arg::mptr(e)))) {                      // 0x0054ef40
                if (jo::view<u8*>(jo::as_unit(e)->actor) == nullptr || jo::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->plane == nullptr) continue;
                const u8* k = jo::view<u8*>(jo::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->plane);
                if (k != nullptr && jo::as_plane(k)->vertex_format == 7) {
                    float sphere[4];
                    joan::orig::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), 1u);  // 0x005acc80
                    if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&ray, joan::orig::arg::mptr(static_cast<void*>(sphere)))) == 0) continue;  // 0x0060e3f0
                    float t = 2.0f;
                    u8 hit = 0;
                    if (jo::view<u8*>(jo::as_unit(e)->actor) == nullptr) continue;
                    joan::orig::as_bone(jo::view<void*>(jo::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->bone))->update();  // 0x0060ee60
                    void* m = jo::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->bone->matrix_;
                    float inv[16];
                    import_call<void*>(d::iat_v3d_87_matrix_inverse_2, static_cast<void*>(inv), m);  // matrix_inverse
                    Vec3 lo, ld;
                    import_call<void*>(d::iat_v3d_87_point_transform_3, &lo, &ray.o, static_cast<void*>(inv));  // point_transform
                    import_call<void*>(d::iat_v3d_87_point_transform_3, &ld, &ray.d, static_cast<void*>(inv));  // point_transform
                    const u32 shape = jo::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->aim_mode;
                    if (shape == 0) continue;
                    if (shape == 1) {
                        Vec3 tmp;
                        const Vec3 dir = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &ld, &lo));  // 0x0060cc90
                        hit = joan::orig::arg::as<u8>(joan::orig::shape::sphere_hit(&t, lo, dir, joan::orig::arg::mptr(jo::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->aim_sphere)));  // 0x006a0f90
                    } else if (shape == 2) {
                        Vec3 tmp;
                        const Vec3 dir = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &ld, &lo));  // 0x0060cc90
                        Blob24 blk;
                        std::memcpy(&blk, jo::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->aim_box, sizeof blk);
                        hit = joan::orig::arg::as<u8>(joan::orig::shape::box_hit(&t, lo, dir, blk));  // 0x00698970
                    } else if (shape == 3) {
                        Vec3 tmp;
                        const Vec3 dir = *joan::orig::arg::as<Vec3*>(joan::orig::vec_sub(&tmp, &ld, &lo));  // 0x0060cc90
                        Blob20 blk;
                        std::memcpy(&blk, &jo::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->collide_kind, sizeof blk);

                        hit = joan::orig::arg::as<u8>(joan::orig::shape::cylinder_hit(&t, lo, dir, blk));  // 0x00699ec0
                    }
                    if (hit == 0) continue;
                    if (best == nullptr || best_t > t) {
                        best_t = t;
                        best = jo::as_unit(e)->actor;
                    }
                } else if (k != nullptr) {
                    float sphere[4];
                    joan::orig::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->bounding_sphere(joan::orig::arg::mptr(static_cast<void*>(sphere)), 1u);  // 0x005acc80
                    if (joan::orig::arg::as<i32>(joan::orig::segment_hits_sphere(&ray, joan::orig::arg::mptr(static_cast<void*>(sphere)))) == 0) continue;  // 0x0060e3f0
                    void* part = nullptr;
                    void* part2 = nullptr;  // select_mesh writes a pointer (the bone) here: pointer-sized, or x64 clobbers the neighbouring local
                    joan::orig::as_actor(jo::view<u8*>(jo::as_unit(e)->actor))->select_mesh(joan::orig::arg::mptr(global<void*>(d::g_camera)), 1, &part, &part2);  // 0x005ad060
                    float tt = 0.0f;
                    if (joan::orig::arg::as<i32>(joan::orig::as_plane(part)->segment_hit(&tt, &ray)) == 0) continue;  // 0x005d4a80
                    if (best == nullptr || best_t > tt) {
                        best_t = tt;
                        best = jo::as_unit(e)->actor;
                    }
                }
            }
        }
        if (best == nullptr) return nullptr;
        joan::orig::segment_point_at(joan::orig::arg::mptr(out), &ray, joan::orig::arg::f32(best_t));  // 0x0060de40
        return best;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("World::pick_object_447ab0", 0x00447ab0, &H::pick_object_447ab0);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01_2 {

using joan::orig::helpers::s16;
using joan::orig::helpers::s8;

JOAN_COUNTER(c_morale, "Unit::morale_level");

struct H {
    // ------------------------------------------------------------ 0x00448080
    std::int32_t morale_term() {  // ((stat(0x17) + 100) * level / 100 - 20) / 5
        const std::int32_t level = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->stat_6());  // 0x00523920
        const std::int32_t stat = joan::orig::arg::as<std::int32_t>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->sum(0x17, joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::self<jo::Unit>(this)->id)))));  // 0x00497e10
        const std::int32_t p = static_cast<std::int32_t>(static_cast<std::uint32_t>(stat + 100) * static_cast<std::uint32_t>(level));
        return sub20(p / 100) / 5;
    }
    static std::int32_t sub20(std::int32_t v) {  // 32-bit wrapping sub eax,0x14
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(v) - 20u);
    }
    std::int32_t morale_level() {
        c_morale.hit();
        std::int32_t v;
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
            std::int32_t t;
            if (morale_term() < 0) {
                t = 0;
            } else {
                t = morale_term();
            }
            v = t + 9;
        } else {
            std::int32_t t;
            if (sub20(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->stat_6())) / 5 < 0) {  // 0x00523920
                t = 0;
            } else {
                t = sub20(joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(this)->stat_6())) / 5;
            }
            v = t + 9;
        }
        if (v < s8(jo::view<std::uint8_t>(jo::as_group(jo::self<jo::Unit>(this)->group)->count))) v = s8(jo::view<std::uint8_t>(jo::as_group(jo::self<jo::Unit>(this)->group)->count));
        return v < 0x14 ? v : 0x14;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Unit::morale_level", 0x00448080, &H::morale_level);
#undef B

}  // namespace game_b92_01_2
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

using u32 = std::uint32_t;

// ---- static initialisers (direct calls, as the original) ----
u32 JOAN_CDECL init_4481f0() { joan::orig::init_pi_a33728(); return 0u; }  // eax dead: _initterm reloads it
u32 JOAN_CDECL init_448250() { joan::orig::init_a3372c(); return 0u; }  // eax dead: _initterm reloads it
u32 JOAN_CDECL init_4482a0() { joan::orig::init_4482c0(); return 0u; }  // eax dead: _initterm reloads it
u32 JOAN_CDECL init_4482f0() { joan::orig::init_a33734(); return 0u; }  // eax dead: _initterm reloads it

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("init_4481f0", 0x004481f0, &init_4481f0);
B("init_448250", 0x00448250, &init_448250);
B("init_4482a0", 0x004482a0, &init_4482a0);
B("init_4482f0", 0x004482f0, &init_4482f0);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

// ---------------------------------------------------------------------- static initialisers
void init_pi(img::Addr g) {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(g) = static_cast<float>(t * 4.0);        // qword 0x9cf788 = 4.0 (exact)
}

static void JOAN_CDECL init_pi_a33728() { init_pi(d::g_a33728); }  // 0x00448210

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("init_pi_a33728", 0x00448210, &init_pi_a33728);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a3372c() { div_global(d::g_a3372c, d::g_a33728, d::flt_2); }  // 0x00448270 (/2)

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_a3372c", 0x00448270, &init_a3372c);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

// ---- static initialisers / free functions ----
using joan::orig::helpers::div_init;
void JOAN_CDECL init_4482c0() { div_init(d::g_a33730, d::g_a33728, d::flt_3); }

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("init_4482c0", 0x004482c0, &init_4482c0);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01_2 {

void JOAN_CDECL init_a33734() { global<float>(d::g_a33734) = global<float>(d::g_a33728) / global<float>(d::flt_4); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("init_a33734", 0x00448310, &init_a33734);
#undef B95

}  // namespace game_b95_01_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Panel::load_cursor() -> void { joan::bind::member<&::game_b92_01::H::load_cursor>(this); }  // 0x43f7c0
auto joan::orig::Player::init_rts() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_19::H::reset_43f940>(this)); }  // 0x43f940
auto joan::orig::Player::enter_rts() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_02::H::rts_init>(this)); }  // 0x43fc90
auto joan::orig::Game::start_rts(std::int8_t a0) -> void { joan::bind::member<&::game_b85_06::H::start_rts>(this, a0); }  // 0x4404c0
auto joan::orig::Player::handle_input() -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b110_15::H::handle_input>(this)); }  // 0x440b30
auto joan::orig::World::unload() -> void { joan::bind::member<&::game_world4::H::world_unload>(this); }  // 0x4426a0
auto joan::orig::World::load_pfd() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_world5::H::load_pfd>(this)); }  // 0x442d30
auto joan::orig::Game::rts_mode_442ef0() -> void { joan::bind::member<&::game_b110_14::H::rts_mode>(this); }  // 0x442ef0
auto joan::orig::World::load_water() -> void { joan::bind::member<&::game_b96_01::H::load_water>(this); }  // 0x444e70
auto joan::orig::Unit::check_height() -> void { joan::bind::member<&::game_b99_01::H::check_height>(this); }  // 0x445000
auto joan::orig::Cursor::draw() -> void { joan::bind::member<&::game_b82_16::H::cursor_draw>(this); }  // 0x4450f0
auto joan::orig::World::free_far_cells(void* a0, std::int32_t a1) -> void { joan::bind::member<&::game_world::H::free_far_cells>(this, a0, a1); }  // 0x4457e0
auto joan::orig::Camera::update_follow_445990(std::int8_t a0) -> void { joan::bind::member<&::game_b110_12::H::update_follow>(this, a0); }  // 0x445990
auto joan::orig::check_v3d_module_447150() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b108_05::check_v3d_module>()); }  // 0x447150
auto joan::orig::Unit::sync_shadow() -> void { joan::bind::member<&::game_b86_06::H::sync_shadow>(this); }  // 0x447600
auto joan::orig::Player::rts_turn(std::uint32_t a0) -> void { joan::bind::member<&::game_b108_04::H::turn_447740>(this, a0); }  // 0x447740
auto joan::orig::World::finish_level() -> std::int8_t { return joan::bind::conv<std::int8_t>(joan::bind::member<&::game_b86_06::H::finish_level>(this)); }  // 0x447860
auto joan::orig::World::state() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b87_05::H::world_state>(this)); }  // 0x447900
auto joan::orig::World::lerp(float a0, float a1, float a2) -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b86_06::H::lerp>(this, a0, a1, a2)); }  // 0x447930
auto joan::orig::Unit::zoom_step(char a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_07::H::zoom_step>(this, a0)); }  // 0x447960
auto joan::orig::World::pick_object_447ab0(std::int32_t a0, std::int32_t a1, void* a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b103_01::H::pick_object_447ab0>(this, a0, a1, a2)); }  // 0x447ab0
auto joan::orig::Unit::morale_level() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_01_2::H::morale_level>(this)); }  // 0x448080
auto joan::orig::init_pi_a33728() -> void { joan::bind::free<&::game_b98_01::init_pi_a33728>(); }  // 0x448210
auto joan::orig::init_a3372c() -> void { joan::bind::free<&::game_b97_01::init_a3372c>(); }  // 0x448270
auto joan::orig::init_4482c0() -> void { joan::bind::free<&::game_b100_01::init_4482c0>(); }  // 0x4482c0
auto joan::orig::init_a33734() -> void { joan::bind::free<&::game_b95_01_2::init_a33734>(); }  // 0x448310
