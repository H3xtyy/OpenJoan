// game/gamefil2.cpp: functions of the original d:\ww\gamefil2.cpp (re/attribution.csv).
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
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../save/gen/records_s0.hpp"
#include "../save/s5.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::img::obj;

using joan::hook::x87::Fpu;

using joan::call::cdecl_call;
using joan::img::str;

using joan::orig::helpers::game_new;  // operator new

using i8 = std::int8_t;

i32 file_read(void* file, void* p, i32 n) {
    return joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(p), joan::orig::arg::u32(n));  // 0x005873a0
}

}  // namespace

// ---------------------------------------- from modules/game_b107_03.cpp (game/b107_03)
//
//    - GameFile::save_subsystems 0x004735c0 (thiscall, 1 arg: File*, `ret 4`; `this` unused;
//      only caller 0x00478009 via thunk 0x004084ef tests eax). Writes the 4-byte section marker
namespace {
namespace game_b107_03 {

using joan::img::global;

JOAN_COUNTER(c_save_subsystems, "GameFile::save_subsystems");

// Subsystem save calls in original order: {global object, thunk} (real address in comment).
struct SaveStep {
    img::Addr global;
    fn::Code thunk;
};
constexpr SaveStep kSteps[20] = {
    {d::g_calendar, fn::Sub_load_50},  // 0x004767c0
    {d::g_game_a50088, fn::Sub_load_44},  // 0x00476990
    {d::g_a45f08, fn::List_load_4855b0},  // 0x004855b0
    {d::g_game_state, fn::Obj_save_4848a0},  // 0x004848a0
    {d::g_scenarios, fn::Sub_load_4},  // 0x00483b50
    {d::g_tasks, fn::List_load_4831c0},  // 0x004831c0
    {d::g_nations, fn::List_load_4b3db0},  // 0x0047fb30
    {d::g_place_array, fn::List_load_422b50},  // 0x0047ff50
    {d::g_places, fn::List_load_480ce0},  // 0x00480ce0
    {d::g_items, fn::GFile3_load_list_4820d0},  // 0x004820d0
    {d::g_a50530, fn::Sub_load_objects},  // 0x00482c60
    {d::g_orders, fn::List_load_482f70},  // 0x00482f70
    {d::g_units, fn::Obj_load_47d8f0},  // 0x0047d8f0
    {d::g_profile_list, fn::Obj_load_4839c0},  // 0x004839c0
    {d::g_player_unit_recno, fn::Obj_load_485930},  // 0x00485930
    {d::g_light_sys, fn::Obj_read_485bf0},  // 0x00485bf0
    {d::g_player_hud, fn::Sub_load_14d},  // 0x004845a0
    {d::g_territory_array, fn::List_load_e0},  // 0x00485dc0
    {d::g_a5ccd0, fn::List_load_485f50},  // 0x00485f50
    {d::g_portraits, fn::Gfile3Rec_load_486280},  // 0x00486280
};

// Failure tail shared by all 41 failure sites (dead assert block omitted).
i32 save_failed() {
    if (global<u8>(d::g_gamefile_error_quiet) == 0 && global<u8>(d::kEmpty) != 0) {
        joan::orig::as_popup(obj(d::g_saves))->set_text_4775f0(joan::orig::arg::mptr(&img::global<const char>(d::kEmpty)));  // 0x004775f0
    }
    return 0;
}

struct H {
    // 0x004735c0
    i32 save_subsystems(void* file) {
        c_save_subsystems.hit();
        // The markers: one 4-byte chunk each (joan::save::s5::read_u32; [Debug] SaveFieldwise=0: the raw read); their
        // value is not checked, as in the original.
        jo::File& f = *jo::as_file(file);
        u32 marker = joan::save::s5::kMarker;
        if (joan::save::s5::read_u32(f, marker, "marker") == 0)  // File::file_read 0x005873a0
            return save_failed();
        for (const SaveStep& s : kSteps) {
            if (thiscall<i32>(s.thunk, obj(s.global), file) == 0) return save_failed();
            if (joan::save::s5::read_u32(f, marker, "marker") == 0)  // 0x005873a0
                return save_failed();
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_03", name, addr, fn)
B("GameFile::save_subsystems", 0x004735c0, &H::save_subsystems);
#undef B

}  // namespace game_b107_03
}  // namespace

// ---------------------------------------- from modules/game_b83_06.cpp (game/b83_06)
//
//    - Scene::create_projective_shadow 0x004749c0 (reader, flag) ret 8 (gamefil2.cpp; a
//      script loader, writes no file): node list (strtok ","), texture, 8 floats, a rate
//      (stored as 2pi/1000 / rate, 0 when rate == 0), optional item index.
//
//  * SEH frames (0x004d2b10, 0x004749c0) only guard operator new + constructor: normal path
//    reproduced (null new -> 0 like the original).
namespace {
namespace game_b83_06 {

using joan::hook::global;

// Line buffers with a pad in front (buf[-1] write on an empty line) and behind (read_line
// 0x00589f10 stores the NUL at buf[n], and n == N for the 0x40 / 0x240 reads; in the original
// that byte lands in a dead / later-overwritten local, strip_last then terminates at N - 1).
template <std::uint32_t N>
struct alignas(4) Line {
    char pad[4];
    char b[N];
    char tail[4];
};

JOAN_COUNTER(c_shadow, "Scene::create_projective_shadow");

std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
double atof_(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }
char* strtok_(char* s, const char* d) { return joan::orig::arg::as<char*>(joan::orig::libc::strtok(joan::orig::arg::str(s), joan::orig::arg::str(d))); }

char* token(void* r) { return joan::orig::as_file_txt(r)->get_token(1); }                     // 0x00589c70
void read_line(void* r, char* buf, std::int32_t n) { joan::orig::as_file_txt(r)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(n)); }  // 0x00589f10
void strip_last(char* buf) { buf[static_cast<std::int32_t>(strlen_(buf)) - 1] = 0; }

struct H {
    // ------------------------------------------------------------------ 0x004749c0
    std::int32_t create_projective_shadow(void* reader, std::int32_t flag) {
        c_shadow.hit();
        Line<0x240> nodes;  // [ebp-0x57c]
        Line<0x100> tex;    // [ebp-0x23c]
        Line<0x100> item;   // [ebp-0x13c]
        Line<0x100> unused; // [ebp-0x33c]
        token(reader);
        token(reader);
        read_line(reader, nodes.b, 0x240);
        strip_last(nodes.b);
        token(reader);
        read_line(reader, tex.b, 0xff);
        strip_last(tex.b);
        float v[9];  // [ebp-0x18] .. [ebp-0x38]
        for (std::int32_t i = 0; i < 9; ++i) {
            token(reader);
            v[i] = static_cast<float>(atof_(token(reader)));
        }
        token(reader);
        read_line(reader, item.b, 0xff);
        strip_last(item.b);
        read_line(reader, unused.b, 0xff);
        if (flag == 0) return 1;
        const char* const comma = str(d::s_comma_9d0728);  // ","
        char* tok = strtok_(nodes.b, comma);
        void* const texture = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(joan::orig::arg::str(static_cast<char*>(tex.b)), 1, 0u));  // 0x006099c0
        std::int32_t item_index = 0;
        if (strlen_(item.b) != 0)
            item_index = joan::orig::arg::as<std::int32_t>(joan::orig::as_item_list(obj(d::g_items))->find_by_name(joan::orig::arg::str(static_cast<char*>(item.b))));  // 0x0049f420
        while (tok != nullptr) {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::TexGen)));  // 0x30 on x86 (TexGen / TexProjector)
            std::uint8_t* const o = mem != nullptr ? joan::orig::arg::as<std::uint8_t*>(joan::orig::as_scene_object(mem)->SceneObject_5e5540()) : nullptr;  // 0x005e5540
            void* const node = joan::orig::arg::as<void*>(joan::orig::as_world(this)->find_sector(joan::orig::arg::str(tok)));                                    // 0x0054e6a0
            jo::as_tex_projector(o)->u32_0 = 0;
            for (std::int32_t i = 0; i < 8; ++i) jo::as_tex_projector(o)->params[i] = v[i];
            joan::orig::as_tex_projector(o)->set_texture(joan::orig::arg::mptr(texture));  // 0x005e55f0
            const float rate = v[8];
            if (!(rate < global<float>(d::k_9cf5c4)) && !(rate > global<float>(d::k_9cf5c4))) {  // == 0 or NaN
                jo::view<std::uint32_t>(jo::as_tex_projector(o)->rate) = 0;
            } else {
                jo::as_tex_projector(o)->rate = static_cast<float>(Fpu{true}.div(global<double>(d::dbl_0_006283185307179587), rate));
            }
            jo::as_tex_projector(o)->item_index = item_index;
            joan::orig::as_scene_s(node)->push(o);  // 0x005f39a0
            tok = strtok_(nullptr, comma);
        }
        joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(texture));  // 0x0060a550
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_06", name, addr, fn)
B("Scene::create_projective_shadow", 0x004749c0, &H::create_projective_shadow);
#undef B

}  // namespace game_b83_06
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
//
//    - List::add_546af0 0x00474df0 (b, b, V6, a, b, c) ret 0x2c (gamefil2.cpp).
//
//  * SEH frames (0x00474df0, 0x0041a650, 0x00417b60, 0x00478180, 0x004731e0, 0x00439ff0) only
//    guard operator new + ctor and the stack objects' destructors: normal path reproduced.
namespace {
namespace game_b84_05 {

struct alignas(4) Six {  // 24-byte by-value argument
    std::uint32_t v[6];
};

JOAN_COUNTER(c_list_add, "List::add_546af0");

struct H {
    // ---- 0x00474df0 List::add_546af0(b, -, V6, a, b, c) (gamefil2.cpp) ------------------------
    std::int32_t list_add(char a, char b, Six v, std::uint32_t x, std::uint32_t y, void* z) {  // z: the Sector (x64: a pointer)
        c_list_add.hit();
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Water)));  // 0x34 on x86
        void* p = mem != nullptr ? joan::orig::as_water(mem)->construct() : nullptr;  // 0x00546af0
        joan::orig::dynarray_b(this)->linkin(static_cast<void*>(&p));                   // 0x005857a0
        joan::orig::as_water(p)->init(static_cast<std::uint8_t>(joan::orig::arg::u32(a)), joan::orig::arg::u32(b), v, joan::orig::arg::u32(x), joan::orig::arg::u32(y), z);                               // 0x00546b50
        const std::int16_t n = joan::orig::arg::as<std::int16_t>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
        reinterpret_cast<std::int16_t&>(jo::as_water(p)->bytes_0) = n;
        return joan::orig::as_dynarray(this)->cursor();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("List::add_546af0", 0x00474df0, &H::list_add);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b85_01.cpp (game/b85_01)
//
//    - GameFile::write_body 0x00474ec0 (file) ret 4 (gamefil2.cpp): marker 0x2facebee, then for
//      each of the 20 subsystems (0x00476890 .. 0x00486110 on their globals) its save(file)
//      followed by the marker again; any failure -> error popup ("" 0x009cfb9c) and 0.
//
//  * 0x00474ec0 never touches `this` (only stored in the frame); 0x00478cc0 writes this's fields.
namespace {
namespace game_b85_01 {

using joan::hook::global;

// Error popup used after every failed write: if (!*(char*)0xa54b0a && *text) 0x004775f0.
void popup(const char* text) {
    if (global<std::int8_t>(d::g_gamefile_error_quiet) != 0) return;
    if (*reinterpret_cast<const std::int8_t*>(text) == 0) return;
    joan::orig::as_popup(obj(d::g_saves))->set_text_4775f0(joan::orig::arg::mptr(text));  // 0x004775f0
}

JOAN_COUNTER(c_body, "GameFile::write_body");

// The 20 subsystem save(file) calls of 0x00474ec0, in order: {thunk, object}.
struct SaveStep {
    fn::Code thunk;
    img::Addr object;
};
constexpr SaveStep kSteps[20] = {
    {fn::Sub_save_50, d::g_calendar},  // 0x00476890
    {fn::GameFile_write_header_476c40, d::g_game_a50088},  // 0x00476c40
    {fn::List_save_4854c0, d::g_a45f08},  // 0x004854c0
    {fn::Obj_save_484780, d::g_game_state},  // 0x00484780
    {fn::Sub_save_4, d::g_scenarios},  // 0x00483b10
    {fn::List_save_483130, d::g_tasks},  // 0x00483130
    {fn::ListB_save, d::g_nations},  // 0x0047fa50
    {fn::List_save_items, d::g_place_array},  // 0x0047fe80
    {fn::List_save_480c00, d::g_places},  // 0x00480c00
    {fn::Units_save, d::g_items},  // 0x00481fb0
    {fn::List_write, d::g_a50530},  // 0x00482b50
    {fn::List_save_482ea0, d::g_orders},  // 0x00482ea0
    {fn::Obj_save_47d7a0, d::g_units},  // 0x0047d7a0
    {fn::List_save_4838e0, d::g_profile_list},  // 0x004838e0
    {fn::GameFile_write_camera_4857c0, d::g_player_unit_recno},  // 0x004857c0
    {fn::ObjH_save, d::g_light_sys},  // 0x00485ab0
    {fn::Sub_save_14d, d::g_player_hud},  // 0x004843d0
    {fn::List_save_e0, d::g_territory_array},  // 0x00485d30
    {fn::ListC_save, d::g_a5ccd0},  // 0x00485e70
    {fn::List_save_486110, d::g_portraits},  // 0x00486110
};

struct H {
    // ---- 0x00474ec0 GameFile::write_body(file) (gamefil2.cpp) ------------------------------
    std::int32_t write_body(void* file) {
        c_body.hit();
        // The markers: one 4-byte chunk each (joan::save::s5::write_u32; [Debug] SaveFieldwise=0: the raw write).
        jo::File& f = *jo::as_file(file);
        if (joan::save::s5::write_u32(f, joan::save::s5::kMarker, "marker") == 0) {  // File::file_write 0x005872e0
            popup(str(d::kEmpty));
            return 0;
        }
        for (int i = 0; i < 20; ++i) {
            if (thiscall<std::int32_t>(kSteps[i].thunk, obj(kSteps[i].object), file) == 0) {
                popup(str(d::kEmpty));
                return 0;
            }
            if (joan::save::s5::write_u32(f, joan::save::s5::kMarker, "marker") == 0) {
                popup(str(d::kEmpty));
                return 0;
            }
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_01", name, addr, fn)
B("GameFile::write_body", 0x00474ec0, &H::write_body);
#undef B

}  // namespace game_b85_01
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
//
//      0x004762c0 Unit::drop_position_4762c0    (thiscall(out, unused, obj), ret 0xc) - dead code
//                 (no references); hidden return vec3 *out; tries random spots around the unit
//                 (rand 0x80, then 10 tries each at 0x100 / 0x200 / 0x400) until 0x00693170 accepts one.
namespace {
namespace game_b108_14 {

using joan::hook::global;
using i16 = std::int16_t;

i32 ftol(float v) { return joan::hook::x87::ftol(static_cast<double>(v)); }

void* const kUnits = obj(d::g_units);
void* const kRandom = obj(d::g_misc);

i32 rnd(i32 n) { return joan::orig::as_misc(kRandom)->random(joan::orig::arg::i32(n)); }  // 0x00590540

JOAN_COUNTER(c_drop, "Unit::drop_position_4762c0");

struct H {
    // ------------------------------------------------------------------------ 0x004762c0
    float* drop_position(float* out, u32 /*unused*/, void* o) {
        c_drop.hit();
        const Fpu fpu{true};
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(jo::self<jo::Unit>(this)->id));  // 0x0041b3c0
        float v[3];
        auto pick = [&](i32 n, img::Addr k) {
            const i32 rx = rnd(n);
            v[0] = static_cast<float>(fpu.sub(fpu.add(static_cast<double>(rx), reinterpret_cast<const float*>(jo::as_unit(u)->position_)[0]), global<float>(k)));
            const i32 ry = rnd(n);
            v[1] = static_cast<float>(fpu.sub(fpu.add(static_cast<double>(ry), reinterpret_cast<const float*>(jo::as_unit(u)->position_)[1]), global<float>(k)));
            v[2] = reinterpret_cast<const float*>(jo::as_unit(u)->position_)[2];
        };
        pick(0x80, d::flt_64);
        void* const m = joan::orig::arg::as<void*>(joan::orig::as_world_cell(jo::as_unit(u)->cell)->grid(joan::orig::arg::i16(jo::as_unit(u)->layer)));  // 0x005600c0
        auto attempt = [&]() -> bool {
            const std::uint32_t res = (joan::orig::as_path_space(m)->nearest_zone(joan::orig::arg::i32(ftol(v[0])), joan::orig::arg::i32(ftol(v[1])), joan::orig::arg::u32(ftol(v[2])), joan::orig::arg::bits(20.0f)));  // 0x0066c710
            if (res == 0) return false;
            v[2] = joan::orig::arg::as<float>(joan::orig::as_terrain(m)->height_66cae0(static_cast<std::int32_t>(res), joan::orig::arg::f32(v[0]), joan::orig::arg::f32(v[1])));  // 0x0066cae0
            joan::orig::as_actor(jo::as_unit(o)->actor)->set_position_5ac310(joan::orig::arg::mptr(static_cast<float*>(v)));  // 0x005ac310
            return joan::orig::arg::as<u8>(joan::orig::as_collision_grid(obj(d::g_collision_grid))->is_free(joan::orig::arg::mptr(jo::as_unit(o)->actor), joan::orig::arg::u32(jo::as_unit(o)->layer))) != 0;  // 0x00693170
        };
        auto store = [&]() {
            out[0] = v[0];
            out[1] = v[1];
            out[2] = v[2];
            return out;
        };
        if (attempt()) return store();
        for (i32 i = 0; i < 10; ++i) {
            pick(0x100, d::flt_128);
            if (attempt()) return store();
        }
        for (i32 i = 0; i < 10; ++i) {
            pick(0x200, d::flt_256);
            if (attempt()) return store();
        }
        for (i32 i = 0; i < 10; ++i) {
            pick(0x400, d::flt_512);
            if (attempt()) return store();
        }
        return store();
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Unit::drop_position_4762c0", 0x004762c0, &H::drop_position);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
//
//      0x004767c0 Sub::load_50(file)            (thiscall, ret 4) - read 0x50, +0x3c = time(0).
//
//  - 0x004767c0: the sprintf / log branch is dead (xor ecx,ecx; test; je) and omitted.
namespace {
namespace game_b99_02 {

using joan::hook::global;

JOAN_COUNTER(c_load_50, "Sub::load_50");

struct H {
    // ------------------------------------------------------------------ 0x004767c0
    // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S0): save::get_Calendar; 0 = the raw read.
    i32 load_50(void* file) {
        c_load_50.hit();
        i32 ok;
        if (joan::save::fieldwise()) {
            joan::save::In in(*joan::orig::as_file(file), joan::save::kCalendar, this);  // File::file_read 0x005873a0
            joan::save::get_Calendar(in, *jo::self<jo::Calendar>(this));
            ok = in.end();
        } else {
            ok = file_read(file, this, 0x50);
        }
        if (ok == 0) {
            if (global<i8>(d::g_gamefile_error_quiet) == 0 && global<i8>(d::kEmpty) != 0)
                joan::orig::as_popup(obj(d::g_saves))->set_text_4775f0(joan::orig::arg::mptr(str(d::kEmpty)));  // 0x004775f0
            return 0;
        }
        jo::view<i32>(jo::self<jo::Calendar>(this)->start_time) = joan::orig::arg::as<i32>(joan::orig::libc::time(joan::orig::arg::mptr(static_cast<void*>(nullptr))));  // time(0)
        return 1;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Sub::load_50", 0x004767c0, &H::load_50);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b97_02.cpp (game/b97_02)
//
//      0x00476890 Sub::save(file)               (thiscall, ret 4) - time() bookkeeping, write 0x50.
//
//  - 0x00476890: the sprintf / log branch is dead (xor eax,eax; test; je) and contains no call
//    with side effects on the taken path; omitted.
namespace {
namespace game_b97_02 {

using joan::hook::global;
using joan::hook::original;

JOAN_COUNTER(c_save_50, "Sub::save_50");

i32 crt_time(i32* t) { return original<i32(JOAN_CDECL*)(i32*)>(fn::crt_time)(t); }

// File I/O on the save file object.
i32 file_write(void* file, const void* p, i32 n) {
    return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n));  // 0x005872e0
}

struct H {
    // ------------------------------------------------------------------ 0x00476890
    i32 save_50(void* file) {
        c_save_50.hit();
        const i32 t0 = crt_time(nullptr);
        jo::view<i32>(jo::self<jo::Calendar>(this)->u32_40) = jo::view<i32>(jo::self<jo::Calendar>(this)->u32_40) + (t0 - jo::view<i32>(jo::self<jo::Calendar>(this)->start_time));
        jo::view<i32>(jo::self<jo::Calendar>(this)->start_time) = crt_time(nullptr);
        i32 ok;  // [Debug] SaveFieldwise=1 (default): save::put_Calendar; 0 = the raw write
        if (joan::save::fieldwise()) {
            joan::save::Out o(*joan::orig::as_file(file), joan::save::kCalendar, this);
            joan::save::put_Calendar(o, *jo::self<jo::Calendar>(this));
            ok = o.end();  // File::file_write 0x005872e0
        } else {
            ok = file_write(file, this, 0x50);
        }
        if (ok != 0) return 1;
        // (dead sprintf / log branch of the original)
        if (global<i8>(d::g_gamefile_error_quiet) == 0 && global<i8>(d::kEmpty) != 0)
            joan::orig::as_popup(obj(d::g_saves))->set_text_4775f0(joan::orig::arg::mptr(str(d::kEmpty)));  // 0x004775f0
        return 0;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_02", name, addr, fn)
B97("Sub::save_50", 0x00476890, &H::save_50);
#undef B97

}  // namespace game_b97_02
}  // namespace

// ---------------------------------------- from modules/game_b98_02.cpp (game/b98_02)
//
//      0x00476990 Sub::load_44(file)            (thiscall, ret 4) - +0x44/+0x48/+0x4a/+0x32.
//
//  - 0x00476990: the sprintf / log branches are dead (xor reg,reg; test; je); omitted.
namespace {
namespace game_b98_02 {

using joan::hook::global;

JOAN_COUNTER(c_load_44, "Sub::load_44");

// Shared tail of the failed reads in gamefil2.cpp (the log branch before it is dead).
void read_fail_notice() {
    if (global<i8>(d::g_gamefile_error_quiet) == 0 && global<i8>(d::kEmpty) != 0)
        joan::orig::as_popup(obj(d::g_saves))->set_text_4775f0(joan::orig::arg::mptr(str(d::kEmpty)));  // 0x004775f0
}

struct H {
    // ------------------------------------------------------------------ 0x00476990
    i32 load_44(void* file) {
        c_load_44.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S5): save::get_Wnd* (Wnd's x64 offsets differ from
        // the x86 ones the raw reads used); 0 = the raw reads.
        namespace sv = joan::save;
        jo::Wnd& w = *jo::self<jo::Wnd>(this);
        jo::File& f = *jo::as_file(file);
        const bool fieldwise = sv::fieldwise();
        auto chunk = [&](const sv::Record& r, void* at_field, void (*get)(sv::In&, jo::Wnd&)) -> i32 {
            if (!fieldwise) return file_read(file, at_field, static_cast<i32>(r.size));  // File::file_read 0x005873a0
            sv::In in(f, r, at_field);
            get(in, w);
            return in.end();
        };
        if (chunk(sv::kWndSpeed, &w.speed, &sv::get_WndSpeed<jo::Wnd>) == 0) {
            read_fail_notice();
            return 0;
        }
        if (chunk(sv::kWndI16_48, &w.i16_48, &sv::get_WndI16_48<jo::Wnd>) == 0) {
            read_fail_notice();
            return 0;
        }
        if (chunk(sv::kWndU8_4a, &w.u8_4a, &sv::get_WndU8_4a<jo::Wnd>) == 0) {
            read_fail_notice();
            return 0;
        }
        w.u8_4a = 1;
        if (chunk(sv::kWndGameTime, &w.game_time, &sv::get_WndGameTime<jo::Wnd>) == 0) {
            read_fail_notice();
            return 0;
        }
        return 1;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_02", name, addr, fn)
B98("Sub::load_44", 0x00476990, &H::load_44);
#undef B98

}  // namespace game_b98_02
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
//
//    - Clock::advance_476bb0 0x00476bb0, ret 0: +0xc += ticks(0xa500be) * 24 / 60 / 1000 unless
//      the speed 0xa500cc is 0 / NaN; at >= 1440 wraps to +0x1c and calls 0x004953d0.
namespace {
namespace game_b108_08 {

using joan::hook::global;

// x87 fcomp + test ah,0x40: equal or unordered.
bool eq_or_nan(double a, double b) { return !(a < b) && !(a > b); }

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x00476bb0
    void advance_476bb0() {
        const float speed = global<float>(d::g_game_speed);
        if (eq_or_nan(speed, 0.0)) return;
        const Fpu fpu{true};
        const float k = (24.0f / 60.0f) / 1000.0f;
        const double ticks = static_cast<double>(global<u32>(d::g_frame_step_ms));
        jo::self<jo::Calendar>(this)->time_prev = static_cast<float>(fpu.add(fpu.mul(ticks, k), jo::self<jo::Calendar>(this)->time_prev));
        if (1440.0 > static_cast<double>(jo::self<jo::Calendar>(this)->time_prev)) return;
        jo::view<u32>(jo::self<jo::Calendar>(this)->time_prev) = jo::view<u32>(jo::self<jo::Calendar>(this)->time_of_day);
        joan::orig::as_calendar(this)->next_day();  // 0x004953d0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Clock::advance_476bb0", 0x00476bb0, &H::advance_476bb0);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b100_03.cpp (game/b100_03)
//
//      0x00476c40 GameFile::write_header     (thiscall(file), ret 4)
//
//  - 0x00476c40: the `if (0)` sprintf / log blocks are statically dead (xor reg / test / je).
namespace {
namespace game_b100_03 {

using joan::hook::global;

JOAN_COUNTER(c_write_header, "GameFile::write_header");

// file helpers (thiscall on the file object)
i32 fwrite_(void* file, const void* p, i32 n) { return joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(p), joan::orig::arg::u32(n)); }  // 0x005872e0

struct H {

    // ------------------------------------------------------------------------ 0x00476c40
    static i32 write_failed() {
        if (global<i8>(d::g_gamefile_error_quiet) == 0 && global<i8>(d::kEmpty) != 0)
            joan::orig::as_popup(obj(d::g_saves))->set_text_4775f0(joan::orig::arg::mptr(str(d::kEmpty)));  // 0x004775f0
        return 0;
    }
    i32 write_header(void* file) {
        c_write_header.hit();
        // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S5): save::put_Wnd*; 0 = the raw writes.
        namespace sv = joan::save;
        const jo::Wnd& w = *jo::self<jo::Wnd>(this);
        jo::File& f = *jo::as_file(file);
        const bool fieldwise = sv::fieldwise();
        auto chunk = [&](const sv::Record& r, const void* at_field, void (*put)(sv::Out&, const jo::Wnd&)) -> i32 {
            if (!fieldwise) return fwrite_(file, at_field, static_cast<i32>(r.size));  // File::file_write 0x005872e0
            sv::Out o(f, r, at_field);
            put(o, w);
            return o.end();
        };
        if (chunk(sv::kWndSpeed, &w.speed, &sv::put_WndSpeed<jo::Wnd>) == 0) return write_failed();
        if (chunk(sv::kWndI16_48, &w.i16_48, &sv::put_WndI16_48<jo::Wnd>) == 0) return write_failed();
        if (chunk(sv::kWndU8_4a, &w.u8_4a, &sv::put_WndU8_4a<jo::Wnd>) == 0) return write_failed();
        if (chunk(sv::kWndGameTime, &w.game_time, &sv::put_WndGameTime<jo::Wnd>) == 0) return write_failed();
        return 1;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_03", name, addr, fn)
B100("GameFile::write_header", 0x00476c40, &H::write_header);
#undef B100

}  // namespace game_b100_03
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::GameFile::save_subsystems(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b107_03::H::save_subsystems>(this, a0)); }  // 0x4735c0
auto joan::orig::Scene::create_projective_shadow(void* a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_06::H::create_projective_shadow>(this, a0, a1)); }  // 0x4749c0
auto joan::orig::GameFile::write_body(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_01::H::write_body>(this, a0)); }  // 0x474ec0
