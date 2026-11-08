// game/battle.cpp: functions of the original d:\ww\battle.cpp (re/attribution.csv).
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
#include "../render/overrides.hpp"
#include <cstdarg>
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

struct Obj {};
using joan::call::thiscall;
using joan::call::cdecl_call;
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::img::obj;
using joan::img::str;

void* new_script() {
    void* const mem = joan::orig::arg::as<void*>(joan::orig::pool::alloc(joan::orig::arg::u32(static_cast<u32>(sizeof(jo::Script)))));           // 0x0063e450 (0x70 on x86)
    return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_ctrl(mem)->Ctrl_63f190()) : nullptr;  // 0x0063f190
}
// Scenario record stride: 0x1b2 on x86 (holds pointers, so wider on x64).
constexpr std::int32_t kScenarioSize = static_cast<std::int32_t>(sizeof(jo::ScenarioEntry));

}  // namespace

// ---------------------------------------- from modules/game_b115_12.cpp (game/b115_12)
//
//    - 0x0041bf40 Battle::load_scenario_41bf40(short hero) (thiscall, ret 4; dead code: no code or
//      data reference). Resets the battle state, loads "SCRIPT\\SCENARIO\\<G+0x82>.SCE" into a new
//      script object stored in [G+0x186] (G = [0xa58fb7]; the old one deleted through vslot 0),
//      creates the map, runs the script (0x0063ed40, vslot 3), the amazon swap when hero is
//      MAIN_CHARACTER_AMAZON, the world bounds (+-8000 / +-16000 boxes) and the counters.
//      Old skip reason: the script pointer [ebp-0x2bc] stays unwritten when the open 0x00586da0
//      fails after the file exists. 0x00586da0 only fails after the fatal-error handler
//
//    - 0x0041d400 Battle::load_scenario_41d400(int index) (thiscall, ret 4; dead code): the same
//      for scenario record index (G + (index-1)*0x1b2: name +0x82, des name +0xc3, unit file
//      +0x41), plus "MAP\\<map>_<name>.UNI" text files for every sub-map (0x005613d0 /
//      0x00548600 / 0x00558a30). Same unreachable uninitialised script pointer, same treatment.
namespace {
namespace game_b115_12 {

u8* gptr(img::Addr a) { return global<u8*>(a); }
using joan::orig::helpers::wrap_sub;
using joan::orig::helpers::wrap_mul;

constexpr img::Addr kUnits(d::g_units);
constexpr img::Addr kTypes(d::g_types);
constexpr img::Addr kUtil(d::g_misc);
constexpr img::Addr kMem(d::g_mem);
constexpr img::Addr kWorlds(d::g_a45f08);
constexpr img::Addr kHero(d::g_player_unit_recno);

JOAN_COUNTER(c_scen1, "Battle::load_scenario_41bf40");
JOAN_COUNTER(c_scen2, "Battle::load_scenario_41d400");

// ------------------------------------------------------------------ common helpers
struct alignas(4) Text {  // 0x005a1170 / 0x005a11a0 string, 0x194 bytes, no destructor
    u8 b[0x194];
};
struct alignas(4) File {  // 0x004179c0 file, 0x110 bytes ([ebp-0x2b4] .. [ebp-0x1a4])
    u8 b[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger File
};
struct alignas(4) TextFile {  // 0x00589550 text reader, 0x19c bytes
    u8 b[((sizeof(jo::FileTxt) > 0x19c ? sizeof(jo::FileTxt) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger FileTxt
};
void text_ctor(Text& t) { joan::orig::as_string(&t)->construct(); }               // 0x005a1170
void text_set(Text& t, const char* s) { joan::orig::as_string(&t)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void text_add(Text& t, const char* s) { joan::orig::as_string(&t)->append(joan::orig::arg::str(s)); }  // 0x005a1550
const char* c_str(Text& t) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&t)->c_str()); }  // 0x00417d90
i32 file_exists(const char* s) { return joan::orig::as_misc(img::obj(kUtil))->is_file_exist(joan::orig::arg::str(s)); }  // 0x00590b80
using joan::orig::helpers::pump;
i32 file_size(File& f) { return joan::orig::as_file(&f)->file_size(); }   // 0x00587f30
// Whole file into a new buffer (0x0058e7b0 alloc size + 1) and terminated.
char* read_all(File& f, i32 line) {
    const i32 n = file_size(f);
    char* const buf = joan::orig::arg::as<char*>(joan::orig::as_mem(img::obj(kMem))->add(joan::orig::arg::u32(n + 1), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
    const i32 size = file_size(f);
    joan::orig::as_file(&f)->file_read(joan::orig::arg::mptr(buf), joan::orig::arg::u32(size));  // 0x005873a0
    buf[file_size(f)] = 0;
    return buf;
}
void mem_free(void* p, i32 line) { joan::orig::as_mem(img::obj(kMem))->del(joan::orig::arg::mptr(p), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(line)); }  // 0x0058ed70
void* unit_of(i32 id) { return joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 hero_id() { return joan::orig::arg::as<i32>(joan::orig::as_player(img::obj(kHero))->unit_id()); }                  // 0x004203c0
// World bounds around the hero unit: +-[0x9cf7a4] and +-[0x9cf7a0] boxes to 0x00547630.
void set_bounds(u8* unit) {
    float near_box[6];
    joan::orig::as_aabb(near_box)->set_point(joan::orig::arg::ptr(jo::as_unit(unit)->position_));  // 0x0060d430
    const float d1 = global<float>(d::flt_8000);
    near_box[0] = near_box[0] - d1;
    near_box[1] = near_box[1] - d1;
    near_box[2] = near_box[2] - d1;
    near_box[3] = near_box[3] + d1;
    near_box[4] = near_box[4] + d1;
    near_box[5] = near_box[5] + d1;
    float far_box[6];
    joan::orig::as_aabb(far_box)->set_point(joan::orig::arg::ptr(jo::as_unit(unit)->position_));
    const float d2 = global<float>(d::flt_16000);
    far_box[0] = far_box[0] - d2;
    far_box[1] = far_box[1] - d2;
    far_box[2] = far_box[2] - d2;
    far_box[3] = far_box[3] + d2;
    far_box[4] = far_box[4] + d2;
    far_box[5] = far_box[5] + d2;
    joan::orig::as_world(global<void*>(d::g_world))->stream_cells(joan::orig::arg::mptr(near_box), joan::orig::arg::mptr(far_box), i32{1});  // 0x00547630
}
void delete_virtual(void* p) {
    if (p != nullptr) virtual_call<void*>(p, 0, i32{1});
}

struct H {
    // -------------------------------------------------------------- 0x0041bf40
    void load_scenario_a(i32 hero_arg) {
        c_scen1.hit();
        joan::orig::as_scenario(obj(d::g_mission))->set_4ccf40(joan::orig::arg::u32(i32{0}));  // 0x004ccf40
        global<u32>(d::g_target_stage) = 1;
        global<u32>(d::g_stage_index) = global<u32>(d::g_target_stage);
        global<u8>(d::g_flag_a59298) = 0;
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = 0;
        joan::orig::as_game(obj(d::g_game_state))->load_arrays();  // 0x00470d40
        Text path;
        text_ctor(path);
        text_set(path, str(d::s_SCRIPT_SCENARIO));  // "SCRIPT\\SCENARIO\\"
        text_add(path, reinterpret_cast<const char*>(gptr(d::g_stages) + 0x82));
        text_add(path, str(d::s_SCE));  // ".SCE"
        if (file_exists(c_str(path)) == 0) {
            global<u32>(d::g_mission) = 0;
            return;
        }
        File file;
        joan::orig::as_file(&file)->construct();  // 0x004179c0
        void* script = nullptr;              // [ebp-0x2bc]: see the header (unreachable when unset)
        bool have_script = false;
        if (joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), i32{1}, static_cast<char>(i32{0})) != 0) {  // 0x00586da0
            char* const buf = read_all(file, static_cast<i32>(global<i16>(d::g_9f7470)) + 0x100);
            if (jo::as_scenario_entry(gptr(d::g_stages))->script != nullptr) delete_virtual(jo::view<void*>(jo::as_scenario_entry(gptr(d::g_stages))->script));
            void* const created = new_script();
            jo::view<void*>(jo::as_scenario_entry(gptr(d::g_stages))->script) = created;
            script = jo::view<void*>(jo::as_scenario_entry(gptr(d::g_stages))->script);
            have_script = true;
            joan::orig::as_script(script)->compile(joan::orig::arg::mptr(buf), c_str(path));  // 0x00649280
            mem_free(buf, static_cast<i32>(global<i16>(d::g_9f7470)) + 0x111);
            joan::orig::as_file(&file)->file_close();  // 0x00587260
            text_set(path, str(d::s_SCRIPT_SCENARIO));
            text_add(path, reinterpret_cast<const char*>(gptr(d::g_stages) + 0x82));
            joan::orig::as_widget(script)->load_ex_res(joan::orig::arg::str(c_str(path)));  // 0x0063f580
        }
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) + 1;
        joan::orig::as_message_list(obj(d::g_names))->reset();  // 0x004ae6c0
        joan::orig::as_screen(img::obj(kHero))->reset();            // 0x0042de70
        pump();
        const i32 slot = joan::orig::arg::as<i32>(joan::orig::as_list(img::obj(kWorlds))->add_new());            // 0x005611b0
        void* const world = joan::orig::arg::as<void*>(joan::orig::as_units(img::obj(kWorlds))->get_420240(joan::orig::arg::i32(slot)));  // 0x00420240
        text_set(path, reinterpret_cast<const char*>(gptr(d::g_stages)));
        joan::orig::as_world(world)->init(joan::orig::arg::u16(i32{1}), joan::orig::arg::str(c_str(path)), joan::orig::arg::str(c_str(path)), joan::orig::arg::mptr(i32{0}));  // 0x0044e8c0
        joan::orig::as_unit_list(img::obj(kWorlds))->select(joan::orig::arg::i32(slot));                                     // 0x00561350
        pump();
        if (have_script) {
            joan::orig::as_script_block(script)->rewind();  // 0x0063ed40
            virtual_call(script, 0xc, static_cast<void*>(this));
        }
        pump();
        const i32 hero = static_cast<i16>(hero_arg);
        if (hero != joan::orig::arg::as<i32>(joan::orig::as_unit_types(img::obj(kTypes))->find(str(d::s_MAIN_CHARACTER_HUMAN)))) {      // 0x005270b0 'MAIN_CHARACTER_HUMAN'
            if (hero == joan::orig::arg::as<i32>(joan::orig::as_unit_types(img::obj(kTypes))->find(str(d::s_MAIN_CHARACTER_AMAZON)))) {  // 'MAIN_CHARACTER_AMAZON'
                const i32 h = hero_id();
                joan::orig::as_player(img::obj(kHero))->set_unit(joan::orig::arg::i32(global<u32>(d::g_a5b9ec)));  // 0x0042f9c0
                global<i32>(d::g_a5b9ec) = h;
                u8* const u = static_cast<u8*>(unit_of(hero_id()));
                jo::view<u16>(jo::as_unit(u)->layer) = 0;
                global<u16>(d::g_a47c36) = static_cast<u16>(static_cast<u32>(hero_id()));
                global<i32>(d::g_player_side) = jo::as_unit(u)->side;
                joan::orig::as_mini_map(obj(d::g_mini_map))->track_unit(joan::orig::arg::i16(hero_id()));  // 0x0070fb70
                joan::orig::as_camera_man(obj(d::g_camera_man))->set_8(static_cast<i32>(global<i16>(d::g_a47c36)));  // 0x005639f0
                u8* const u2 = static_cast<u8*>(unit_of(global<i32>(d::g_a5b9ec)));
                jo::view<u16>(jo::as_unit(u2)->layer) = 1;
            }
        }
        joan::orig::as_players(obj(d::g_physic_sys))->set_timer(joan::orig::arg::i16(i32{1}));  // 0x006ad900
        u8* const unit = static_cast<u8*>(unit_of(hero_id()));
        joan::orig::as_unit_list(img::obj(kWorlds))->select(joan::orig::arg::i32(slot));
        set_bounds(unit);
        pump();
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) + 1;
        joan::orig::as_critical_hud(obj(d::g_critical_hud))->setup_combos(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));                                    // 0x0045f860
        joan::orig::as_camera_man(obj(d::g_camera_man))->init();                                                 // 0x005615c0
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_8(static_cast<i32>(global<i16>(d::g_a47c36)));  // 0x005639f0
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(i32{0});                                    // 0x00429860
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = 10;
        pump();
        joan::orig::as_file(&file)->destruct();  // 0x005872b0
    }

    // -------------------------------------------------------------- 0x0041d400
    static u8* record(i32 index) { return gptr(d::g_stages) + wrap_mul(wrap_sub(index, 1), kScenarioSize); }
    void load_scenario_b(i32 index) {
        c_scen2.hit();
        if (index == global<i32>(d::g_stage_index)) return;
        global<i32>(d::g_target_stage) = index;
        global<u32>(d::g_stage_index) = global<u32>(d::g_target_stage);
        global<u8>(d::g_flag_a59298) = 0;
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = 0;
        Text path;
        text_ctor(path);
        text_set(path, str(d::s_SCRIPT_SCENARIO));  // "SCRIPT\\SCENARIO\\"
        text_add(path, reinterpret_cast<const char*>(record(index) + 0x82));
        text_add(path, str(d::s_SCE));  // ".SCE"
        if (file_exists(c_str(path)) == 0) {
            global<u32>(d::g_mission) = 0;
            return;
        }
        File file;
        joan::orig::as_file(&file)->construct();
        void* script = nullptr;  // [ebp-0x2bc]: see the header (unreachable when unset)
        bool have_script = false;
        if (joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), i32{1}, static_cast<char>(i32{0})) != 0) {
            char* const buf = read_all(file, static_cast<i32>(global<i16>(d::g_9f7478)) + 0x25);
            if (jo::as_scenario_entry(record(index))->script != nullptr) delete_virtual(jo::view<void*>(jo::as_scenario_entry(record(index))->script));
            void* const created = new_script();
            jo::view<void*>(jo::as_scenario_entry(record(index))->script) = created;
            script = jo::view<void*>(jo::as_scenario_entry(record(index))->script);
            have_script = true;
            joan::orig::as_script(script)->compile(joan::orig::arg::mptr(buf), c_str(path));
            mem_free(buf, static_cast<i32>(global<i16>(d::g_9f7478)) + 0x35);
            joan::orig::as_file(&file)->file_close();
            text_set(path, str(d::s_SCRIPT_SCENARIO));
            text_add(path, reinterpret_cast<const char*>(record(index) + 0x82));
            joan::orig::as_widget(script)->load_ex_res(joan::orig::arg::str(c_str(path)));
        }
        pump();
        text_set(path, str(d::s_SCRIPT_SCENARIO));
        text_add(path, reinterpret_cast<const char*>(record(index) + 0xc3));
        text_add(path, str(d::s_SCE));
        if (file_exists(c_str(path)) == 0) {
            joan::orig::as_file(&file)->destruct();  // 0x005872b0
            return;
        }
        if (joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), i32{1}, static_cast<char>(i32{0})) != 0) {
            char* const buf = read_all(file, static_cast<i32>(global<i16>(d::g_9f7478)) + 0x57);
            void* const des = new_script();
            joan::orig::as_script(des)->compile(joan::orig::arg::mptr(buf), c_str(path));
            mem_free(buf, static_cast<i32>(global<i16>(d::g_9f7478)) + 0x60);
            joan::orig::as_file(&file)->file_close();
            text_set(path, str(d::s_SCRIPT_SCENARIO));
            text_add(path, reinterpret_cast<const char*>(record(index) + 0xc3));
            joan::orig::as_widget(des)->load_ex_res(joan::orig::arg::str(c_str(path)));
            virtual_call(des, 0xc, static_cast<void*>(nullptr));
            delete_virtual(des);
        }
        pump();
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) + 1;
        text_set(path, reinterpret_cast<const char*>(record(index)));
        i32 slot = joan::orig::arg::as<i32>(joan::orig::as_list(img::obj(kWorlds))->find_name_6(joan::orig::arg::str(c_str(path))));  // 0x005613d0
        if (slot != 0) {
            for (i32 i = 1;; ++i) {
                void* const w = joan::orig::arg::as<void*>(joan::orig::as_units(img::obj(kWorlds))->get_420240(joan::orig::arg::i32(slot)));
                if (i > jo::as_world(w)->cell_count) break;
                void* const w2 = joan::orig::arg::as<void*>(joan::orig::as_units(img::obj(kWorlds))->get_420240(joan::orig::arg::i32(slot)));
                u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_map(w2)->cell(joan::orig::arg::i32(i)));  // 0x00548600
                text_set(path, str(d::s_MAP_9cf818));                 // "MAP\\"
                text_add(path, jo::as_cell(m)->name);  // +0x20 on x86
                text_add(path, str(d::s_us_9cf774));  // "_"
                text_add(path, reinterpret_cast<const char*>(record(index) + 0x41));
                text_add(path, str(d::s_UNI_9cf810));  // ".UNI"
                if (file_exists(c_str(path)) != 0) {
                    TextFile tf;
                    joan::orig::as_file_txt(&tf)->construct(joan::orig::arg::str(c_str(path)));  // 0x00589550
                    joan::orig::as_world_cell(m)->load_units_thunk(&tf);             // 0x00558a30
                    joan::orig::as_file_txt(&tf)->destruct();            // 0x00589890
                }
            }
        } else {
            slot = joan::orig::arg::as<i32>(joan::orig::as_list(img::obj(kWorlds))->add_new());
            void* const w = joan::orig::arg::as<void*>(joan::orig::as_units(img::obj(kWorlds))->get_420240(joan::orig::arg::i32(slot)));
            text_set(path, reinterpret_cast<const char*>(record(index)));
            joan::orig::as_world(w)->init(joan::orig::arg::u16(i32{1}), joan::orig::arg::str(c_str(path)), joan::orig::arg::str(c_str(path)), joan::orig::arg::mptr(i32{0}));
        }
        pump();
        joan::orig::as_unit_list(img::obj(kWorlds))->select(joan::orig::arg::i32(slot));
        if (have_script) {
            joan::orig::as_script_block(script)->rewind();
            virtual_call(script, 0xc, static_cast<void*>(this));
        }
        joan::orig::as_message_list(obj(d::g_names))->reset();
        joan::orig::as_screen(img::obj(kHero))->reset();
        u8* const unit = static_cast<u8*>(unit_of(hero_id()));
        pump();
        set_bounds(unit);
        pump();
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) + 1;
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_8(static_cast<i32>(global<i16>(d::g_a47c36)));
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(i32{0});
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = 10;
        joan::orig::as_parser(obj(d::g_mouse))->drain();  // 0x005959c0
        joan::orig::as_file(&file)->destruct();
    }

    // -------------------------------------------------------------- 0x006a9570
    struct FV {
        float x, y, z;
    };
    static_assert(sizeof(FV) == 12, "sizes");
};

#define B115(name, addr, fn) JOAN_REPLACE("game/b115_12", name, addr, fn)
B115("Battle::load_scenario_41bf40", 0x0041bf40, &H::load_scenario_a);
B115("Battle::load_scenario_41d400", 0x0041d400, &H::load_scenario_b);
#undef B115

}  // namespace game_b115_12
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
//
//    - Unit::in_footprint_41c5e0 0x0041c5e0 (x, y, unused, margin), ret 0x10: point relative to
//      +0x56 / +0x5a rotated by -(+0x62) (angle helper 0x00577190 / 0x00577340, cos 0x00577b50,
//      sin 0x00577b10) inside the type's (+0xa -> 0x00425ce0) half extents +0x24 / +0x26 plus
//      margin. Returns al (callers use movsx al).
//
//  - 0x0041c5e0: cos / sin results (raw CRT st0 via 0x00577b50 / 0x00577b10, fst without pop)
//    are taken as double and combined with Fpu{true}; int-vs-float compares done in double
//    with the original NaN directions (test ah,0x41 je = greater; test ah,1 jne = less or NaN).
namespace {
namespace game_b108_08 {

using joan::hook::x87::Fpu;

using joan::orig::helpers::add32;
i32 sub32(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(b)); }

void* const kUnitTypes = obj(d::g_item_res);

JOAN_COUNTER(c_in_footprint, "Unit::in_footprint_41c5e0");

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x0041c5e0
    char in_footprint_41c5e0(float x, float y, u32 /*unused*/, i32 margin) {
        c_in_footprint.hit();
        const Fpu fpu{true};
        x = x - jo::self<jo::MapObject>(this)->pos[0];
        y = y - jo::self<jo::MapObject>(this)->pos[1];
        u32 angle = 0;
        joan::orig::as_angle(&angle)->construct_577190();                 // 0x00577190
        joan::orig::as_angle(&angle)->set_wrapped(joan::orig::arg::f32(-jo::self<jo::MapObject>(this)->yaw_));       // 0x00577340
        const double c1 = joan::orig::arg::as<double>(joan::orig::as_turns(&angle)->cos_577b50());  // 0x00577b50 cos
        const float a = static_cast<float>(fpu.mul(c1, x));
        const double s1 = joan::orig::arg::as<double>(joan::orig::as_turns(&angle)->sin_577b10());  // 0x00577b10 sin
        const float u = static_cast<float>(fpu.sub(a, fpu.mul(s1, y)));
        const double s2 = joan::orig::arg::as<double>(joan::orig::as_turns(&angle)->sin_577b10());
        const float b = static_cast<float>(fpu.mul(s2, x));
        const double c2 = joan::orig::arg::as<double>(joan::orig::as_turns(&angle)->cos_577b50());
        const float v = static_cast<float>(fpu.add(fpu.mul(c2, y), b));
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kUnitTypes)->get(joan::orig::arg::i16(jo::self<jo::MapObject>(this)->type_id)));  // 0x00425ce0
        const i32 w = jo::view<i16>(jo::as_item_def(t)->stats[0]), h = jo::view<i16>(jo::as_item_def(t)->stats[1]);
        if (static_cast<double>(sub32((-w) / 2, margin)) > u) return 0;
        if (!(static_cast<double>(add32(w / 2, margin)) >= u)) return 0;
        if (static_cast<double>(sub32((-h) / 2, margin)) > v) return 0;
        if (!(static_cast<double>(add32(h / 2, margin)) >= v)) return 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Unit::in_footprint_41c5e0", 0x0041c5e0, &H::in_footprint_41c5e0);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
//
//      0x0041c710 Obj::dtor_41c710 (SEH)                   (thiscall, ret 0)
namespace {
namespace game_b98_01 {

struct H {
    std::uintptr_t dtor_41c710() {  // this = the Sms panel (0xa48ae8)
        joan::orig::as_panel(this)->reset_6e50c0();                         // 0x006e50c0
        joan::orig::as_widget(&jo::self<jo::Sms>(this)->bar)->dtor_6294c0();                // 0x006294c0
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_widget(&jo::self<jo::Sms>(this)->text)->dtor_6294c0());     // 0x006294c0
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::dtor_41c710", 0x0041c710, &H::dtor_41c710);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
//
//    - Handle::release 0x0041c780 (0x00471010 on 0x00a58f90, optional 0x005624c0 first).
namespace {
namespace game_b82_18 {

struct H {
    void handle_release(std::uint32_t arg) {  // 0x0041c780
        if (global<std::int32_t>(d::g_mission) != 5 || arg == 0) {
            joan::orig::as_camera_man(obj(d::g_camera_man))->release();  // 0x005624c0
        }
        joan::orig::as_game(obj(d::g_game_state))->end_session(joan::orig::arg::i32(arg));  // 0x00471010
        jo::self<jo::Handle>(this)->u32_0 = 0;
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("Handle::release", 0x0041c780, &H::handle_release);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
//
//    - Counter::at_least_10 0x0041c7e0 -> int: [0] >= 10 (signed).
//
//    - Timer::remaining 0x0041d280 -> float: (tick 0xa500ba - start [+0x4c]) of the +0x86
//      record, modulo ftol(duration) when [+0x50] (looping), then duration - elapsed (fisub)
//      when elapsed < duration (or NaN), else 0.0.
namespace {
namespace game_b90_03 {

JOAN_COUNTER(c_remaining, "Timer::remaining");

struct H {
    // 0x0041c7e0
    std::int32_t at_least_10() { return reinterpret_cast<const std::int32_t&>(jo::self<jo::Counter>(this)->bytes_0) >= 10 ? 1 : 0; }

    // 0x0041d280
    float remaining() {
        c_remaining.hit();
        if (jo::self<jo::MapObject>(this)->actor == nullptr) return global<float>(d::k_9cf5c4);
        void* const r = jo::view<void*>(jo::self<jo::MapObject>(this)->actor);
        if (jo::as_actor(r)->anim == nullptr) return global<float>(d::k_9cf5c4);
        std::uint32_t elapsed = global<std::uint32_t>(d::g_now) - jo::as_actor(r)->start_time;
        if (jo::as_actor(r)->anim_flags != 0) {
            void* const v = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::view<void*>(jo::as_actor(r)->anim))->frame(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_actor(r)->action)), 0u));  // 0x005b5dd0
            const float dur = joan::orig::arg::as<float>(joan::orig::as_range(v)->span());                                             // 0x0042d130
            const std::uint32_t div = static_cast<std::uint32_t>(joan::hook::x87::ftol(static_cast<double>(dur)));
            elapsed = elapsed % div;  // div ecx (unsigned)
        }
        void* const v1 = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::view<void*>(jo::as_actor(r)->anim))->frame(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_actor(r)->action)), 0u));  // 0x005b5dd0
        const float dur1 = joan::orig::arg::as<float>(joan::orig::as_range(v1)->span());                                            // 0x0042d130
        // fild qword {elapsed, 0}; fcompp; test ah,1 -> elapsed < dur1 or unordered
        if (!(static_cast<double>(elapsed) >= static_cast<double>(dur1))) {
            void* const v2 = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::view<void*>(jo::as_actor(r)->anim))->frame(joan::orig::arg::i32(jo::view<std::uint32_t>(jo::as_actor(r)->action)), 0u));  // 0x005b5dd0
            const float dur2 = joan::orig::arg::as<float>(joan::orig::as_range(v2)->span());                                            // 0x0042d130
            const joan::hook::x87::Fpu fpu{true};
            // fisub dword: elapsed as a signed int32
            return static_cast<float>(
                fpu.sub(static_cast<double>(dur2), static_cast<double>(static_cast<std::int32_t>(elapsed))));
        }
        return global<float>(d::k_9cf5c4);
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Counter::at_least_10", 0x0041c7e0, &H::at_least_10);
M("Timer::remaining", 0x0041d280, &H::remaining);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
//
//    - Battle::start_scenario 0x0041c810 (ret 0, battle.cpp): world load (0x0044e8c0 on the
//      selected map record) unless [0xa58fa7] == 6, script start through the record's object
//      (vtable +0x24 / +0xc), camera bounds box +-8000 / +-16000 around the start position
//      (0x00547630), timers / music / misc globals.
namespace {
namespace game_b108_03 {

template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}
JOAN_COUNTER(c_start_scenario, "Battle::start_scenario_41c810");

void* const kSnd = obj(d::g_snd);
void* const kMus = obj(d::g_mus);
void* const kUnits = obj(d::g_units);

void snd_flush() {
    joan::orig::as_sound_stream(kSnd)->update();  // 0x0059b1c0
    joan::orig::as_ambient_streams(kMus)->update();  // 0x005866d0
}

struct H {
    // ------------------------------------------------------------------ 0x0041c810
    void start_scenario() {
        c_start_scenario.hit();
        joan::orig::as_cutscene(obj(d::g_game_a5d380))->clear();  // 0x006ca170
        snd_flush();
        joan::orig::as_triple(obj(d::g_screen_mgr))->clear();  // 0x006f5e90
        void* const world_list = obj(d::g_a45f08);
        if (global<i32>(d::g_mission) != 6) {
            const u32 v8 = joan::orig::arg::as<u32>(joan::orig::as_list(world_list)->add_new());              // 0x005611b0
            void* const world = joan::orig::arg::as<void*>(joan::orig::as_units(world_list)->get_420240(joan::orig::arg::i32(v8)));  // 0x00420240
            u8* const rec = global<u8*>(d::g_stages) + (global<u32>(d::g_stage_index) - 1u) * static_cast<u32>(kScenarioSize);
            joan::orig::as_world(world)->init(joan::orig::arg::u16(static_cast<u16>(1)), joan::orig::arg::str(rec), joan::orig::arg::str(rec), joan::orig::arg::mptr(0u));  // 0x0044e8c0
            snd_flush();
            global<u32>(d::g_a50259) = 5;
            joan::orig::as_progress(obj(d::g_progress))->update();  // 0x0075f3c0
            joan::orig::as_progress(obj(d::g_progress))->redraw();  // 0x0075f4f0
            snd_flush();
            joan::orig::as_unit_list(world_list)->select(joan::orig::arg::i32(v8));   // 0x00561350
            joan::orig::as_camera_man(obj(d::g_camera_man))->init();       // 0x005615c0
            joan::orig::as_screen(obj(d::g_player_unit_recno))->reset();       // 0x0042de70
            snd_flush();
            global<u32>(d::g_a50259) = 0xf;
            joan::orig::as_progress(obj(d::g_progress))->update();  // 0x0075f3c0
            joan::orig::as_progress(obj(d::g_progress))->redraw();  // 0x0075f4f0
        }
        u8* const sc = joan::orig::arg::as<u8*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
        if (sc == nullptr) return;
        joan::orig::as_game(obj(d::g_game_state))->copy_word_table();  // 0x004728f0
        joan::orig::as_game(obj(d::g_game_state))->pull_buffer();  // 0x00472c40
        global<u32>(d::g_get_a596c4) = jo::as_profile(sc)->u32_38;
        global<u32>(d::g_play_ms) = jo::as_profile(sc)->u32_3c;
        {
            u8* const rec = global<u8*>(d::g_stages) + (global<u32>(d::g_stage_index) - 1u) * static_cast<u32>(kScenarioSize);
            void* const so = jo::view<void*>(jo::as_scenario_entry(rec)->script);
            if (so != nullptr) {
                void* const s = vcall<void*>(so, 0x24);
                joan::orig::as_script_block(s)->rewind();  // 0x0063ed40
                vcall<u32>(so, 0xc, static_cast<void*>(this));
            }
        }
        snd_flush();
        joan::orig::as_players(obj(d::g_physic_sys))->set_timer(joan::orig::arg::i16(1));                   // 0x006ad900
        joan::orig::as_message_list(obj(d::g_names))->reset();                           // 0x004ae6c0
        const u32 sel = joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());     // 0x004203c0
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(sel)));         // 0x0041b3c0
        alignas(4) float b1[6];
        alignas(4) float b2[6];
        joan::orig::as_aabb(b1)->set_point(joan::orig::arg::ptr(&jo::as_unit(unit)->position_));  // 0x0060d430
        {
            const float k = global<float>(d::flt_8000);  // 8000.0f
            b1[0] = b1[0] - k;
            b1[1] = b1[1] - k;
            b1[2] = b1[2] - k;
            b1[3] = b1[3] + k;
            b1[4] = b1[4] + k;
            b1[5] = b1[5] + k;
        }
        joan::orig::as_aabb(b2)->set_point(joan::orig::arg::ptr(&jo::as_unit(unit)->position_));  // 0x0060d430
        {
            const float k = global<float>(d::flt_16000);  // 16000.0f
            b2[0] = b2[0] - k;
            b2[1] = b2[1] - k;
            b2[2] = b2[2] - k;
            b2[3] = b2[3] + k;
            b2[4] = b2[4] + k;
            b2[5] = b2[5] + k;
        }
        joan::orig::as_world(global<void*>(d::g_world))->stream_cells(joan::orig::arg::mptr(static_cast<void*>(b1)), joan::orig::arg::mptr(static_cast<void*>(b2)), 1);  // 0x00547630
        snd_flush();
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) + 1;
        joan::orig::as_critical_hud(obj(d::g_critical_hud))->setup_combos(static_cast<std::int8_t>(joan::orig::arg::i32(0)));                                       // 0x0045f860
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_8(static_cast<i32>(global<i16>(d::g_a47c36)));  // 0x005639f0
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(0);                                       // 0x00429860
        joan::orig::as_parser(obj(d::g_mouse))->drain();                                               // 0x005959c0
        snd_flush();
        global<u32>(d::g_command_points) = jo::view<u32>(jo::as_profile(sc)->i32_40);
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = 10;
        global<u32>(d::g_a5916d) = global<u32>(d::g_now);
        joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(1.0f));  // 0x0042f930
        global<u32>(d::g_a5025d) = 100;
        joan::orig::as_progress(obj(d::g_progress))->redraw();  // 0x0075f4f0
        snd_flush();
        global<u8>(d::g_a58f96) = 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Battle::start_scenario", 0x0041c810, &H::start_scenario);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
//
//  - SEH dtors / 0x0045a9f0: the frames only matter when an exception unwinds; normal path kept.
//    0x0041cba0's member array is destroyed with the CRT `eh vector destructor iterator`
//    0x007b3054 (stdcall, ret 0x10), called as in the original with the dtor thunk 0x00401ff5.
namespace {
namespace game_b99_01 {

struct H {
    u32 dtor_41cba0() {  // this = the Hud (0xa4d618)
        joan::orig::as_screen(this)->close_6f9720();                          // 0x006f9720
        joan::orig::as_screen(&jo::self<jo::Hud>(this)->minimap)->release_thunk();               // 0x0056e060
        using VecDtor = u32(JOAN_STDCALL*)(void*, u32, i32, std::uintptr_t);
        // the 8 squad buttons (0x346 bytes each on x86)
        return reinterpret_cast<VecDtor>(fn::code_value(fn::crt_eh_vector_destructor_iterator))(&jo::self<jo::Hud>(this)->squad_button0, static_cast<u32>(sizeof(jo::Button)), 8, fn::code_value(fn::Widget_dtor_6211a0));  // -> 0x006211a0
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::dtor_41cba0", 0x0041cba0, &H::dtor_41cba0);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
//
//    - Obj::init_41cc20 0x0041cc20 (ret 0): returns this.
namespace {
namespace game_b103_01 {

using joan::orig::helpers::at;

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // ------------------------------------------------------------------ 0x0041cc20
    // The class is unknown and nothing reaches this slot (no C++ caller, no vtable or relocation in the image refers to the
    // thunk 0x004030a3). Only whole u32 integers are written, so a local view with the same offsets on both widths is exact.
    struct Obj41cc20 {
        u32 f00, f04, f08, f0c;
        u32 unknown_10[9];  // 0x10..0x33, not touched here
        u32 f34;
    };
    static_assert(offsetof(Obj41cc20, f0c) == 0xc && offsetof(Obj41cc20, f34) == 0x34, "x86 offsets of 0x0041cc20");

    void* init_41cc20() {
        Obj41cc20* const o = static_cast<Obj41cc20*>(static_cast<void*>(this));
        o->f00 = 0;
        o->f0c = 3;
        o->f08 = 0;
        o->f04 = 0;
        o->f34 = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::init_41cc20", 0x0041cc20, &H::init_41cc20);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
//
//    - Battle::load_scenario 0x0041cc80 (ret 0xc, battle.cpp): resets globals, builds
//      "SCRIPT\\SCENARIO\\" + entry+0x82 + ".SCE" (entry = [0xa58fb7] + (idx - 1) * 0x1b2, re-read
//      at every use like the original); when the file exists: reads it into a heap buffer,
//      deletes the old entry+0x186 script object (virtual slot 0, arg 1), new(0x70) + ctor
namespace {
namespace game_b108_05 {

void* const kHeap = obj(d::g_mem);

JOAN_COUNTER(c_load_scenario, "Battle::load_scenario_41cc80");

struct H {
    // ------------------------------------------------------------------ 0x0041cc80
    // Entry of the scenario table, re-read from [0xa58fb7] at every use as in the original.
    static void* scenario(i32 idx, i32 off) {
        const u32 rel = static_cast<u32>(idx - 1) * static_cast<u32>(kScenarioSize);
        return global<u8*>(d::g_stages) + rel + static_cast<u32>(off);
    }
    // Reads the open file into a fresh heap buffer (NUL terminated) allocated at `line`.
    static char* read_script(void* file, i32 line) {
        const u32 len = joan::orig::arg::as<u32>(joan::orig::as_file(file)->file_size());  // 0x00587f30
        char* const buf = joan::orig::arg::as<char*>(joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(len + 1u), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        joan::orig::as_file(file)->file_read(buf, static_cast<u32>(joan::orig::as_file(file)->file_size()));  // 0x005873a0
        buf[joan::orig::arg::as<u32>(joan::orig::as_file(file)->file_size())] = 0;
        return buf;
    }
    static void* new_script() {
        void* const mem = joan::orig::arg::as<void*>(joan::orig::pool::alloc(joan::orig::arg::u32(static_cast<u32>(sizeof(jo::Script)))));  // 0x0063e450 operator new (0x70 on x86)
        return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_ctrl(mem)->Ctrl_63f190()) : nullptr;  // 0x0063f190
    }
    void load_scenario(i32 mode, i32 idx, i32 arg3) {
        c_load_scenario.hit();
        alignas(4) char name[0x194];
        alignas(4) jo::File file_obj;  // the 0x110-byte frame slot of the original; a real File (x64: 0x112 bytes)
        void* const file = &file_obj;
        if (global<u32>(d::g_mission) == 0) joan::orig::as_msg_log(obj(d::g_portraits))->reset();  // 0x004b31c0
        joan::orig::as_sms(obj(d::g_sms))->clear();                                     // 0x006e5e00
        global<u16>(d::g_a48c5e) = 0;
        joan::orig::as_handle(this)->release(joan::orig::arg::u32(arg3));               // 0x0041c780
        joan::orig::as_scenario(obj(d::g_mission))->set_4ccf40(joan::orig::arg::u32(mode));    // 0x004ccf40
        global<i32>(d::g_target_stage) = idx;
        global<u32>(d::g_stage_index) = global<u32>(d::g_target_stage);
        global<u8>(d::g_flag_a59298) = 0;
        jo::self<jo::Battle>(this)->u32_0 = 0;
        joan::orig::as_game(obj(d::g_game_state))->load_arrays();  // 0x00470d40
        global<u8>(d::g_a5ba64) = 0;
        joan::orig::as_sound_stream(obj(d::g_snd))->update();  // 0x0059b1c0
        joan::orig::as_ambient_streams(obj(d::g_mus))->update();  // 0x005866d0
        void* const s = name;
        auto c_str = [&] { return joan::orig::arg::as<const char*>(joan::orig::as_string(s)->c_str()); };  // 0x00417d90
        joan::orig::as_string(s)->construct();                                // 0x005a1170
        joan::orig::as_string(s)->assign(str(d::s_SCRIPT_SCENARIO));                                // 0x005a1440 "SCRIPT\\SCENARIO\\"
        joan::orig::as_string(s)->append(joan::orig::arg::str(static_cast<const char*>(scenario(idx, 0x82))));  // 0x005a1550
        joan::orig::as_string(s)->append(str(d::s_SCE));                                    // ".SCE"
        if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(c_str())) == 0) {  // 0x00590b80
            joan::orig::as_scenario(obj(d::g_mission))->set_4ccf40(joan::orig::arg::u32(i32{0}));               // 0x004ccf40
            return;
        }
        joan::orig::as_file(file)->construct();  // 0x004179c0
        if (joan::orig::as_file(file)->file_open(joan::orig::arg::str(c_str()), i32{1}, static_cast<char>(i32{0})) != 0) {  // 0x00586da0
            char* const buf = read_script(file, static_cast<i32>(global<i16>(d::g_9f7474)) + 0x32);
            if (jo::as_scenario_entry(scenario(idx, 0))->script != nullptr) {
                void* const old = jo::view<void*>(jo::as_scenario_entry(scenario(idx, 0))->script);
                if (old != nullptr) virtual_call<u32>(old, 0, i32{1});
            }
            jo::view<void*>(jo::as_scenario_entry(scenario(idx, 0))->script) = new_script();
            void* const script = jo::view<void*>(jo::as_scenario_entry(scenario(idx, 0))->script);
            joan::orig::as_script(script)->compile(joan::orig::arg::mptr(static_cast<const char*>(buf)), c_str());  // 0x00649280
            joan::orig::as_mem(kHeap)->del(static_cast<void*>(buf), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9f7474)) + 0x42));                         // 0x0058ed70
            joan::orig::as_file(file)->file_close();                                     // 0x00587260
            joan::orig::as_string(s)->assign(str(d::s_SCRIPT_SCENARIO));                              // 0x005a1440
            joan::orig::as_string(s)->append(joan::orig::arg::str(static_cast<const char*>(scenario(idx, 0x82))));  // 0x005a1550
            joan::orig::as_widget(script)->load_ex_res(joan::orig::arg::str(c_str()));                         // 0x0063f580
        }
        joan::orig::as_sound_stream(obj(d::g_snd))->update();  // 0x0059b1c0
        joan::orig::as_ambient_streams(obj(d::g_mus))->update();  // 0x005866d0
        joan::orig::as_string(s)->assign(str(d::s_SCRIPT_SCENARIO));                                           // 0x005a1440
        joan::orig::as_string(s)->append(joan::orig::arg::str(static_cast<const char*>(scenario(idx, 0xc3))));  // 0x005a1550
        joan::orig::as_string(s)->append(str(d::s_SCE));                                               // ".SCE"
        if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(c_str())) == 0) {  // 0x00590b80
            joan::orig::as_file(file)->destruct();                           // 0x005872b0
            return;
        }
        if (joan::orig::as_file(file)->file_open(joan::orig::arg::str(c_str()), i32{1}, static_cast<char>(i32{0})) != 0) {  // 0x00586da0
            char* const buf = read_script(file, static_cast<i32>(global<i16>(d::g_9f7474)) + 0x61);
            void* const script = new_script();
            joan::orig::as_script(script)->compile(joan::orig::arg::mptr(static_cast<const char*>(buf)), c_str());  // 0x00649280
            joan::orig::as_mem(kHeap)->del(static_cast<void*>(buf), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9f7474)) + 0x6a));                         // 0x0058ed70
            joan::orig::as_file(file)->file_close();                                     // 0x00587260
            joan::orig::as_string(s)->assign(str(d::s_SCRIPT_SCENARIO));                              // 0x005a1440
            joan::orig::as_string(s)->append(joan::orig::arg::str(static_cast<const char*>(scenario(idx, 0xc3))));  // 0x005a1550
            joan::orig::as_widget(script)->load_ex_res(joan::orig::arg::str(c_str()));                         // 0x0063f580
            virtual_call<u32>(script, 0xc, i32{0});
            if (script != nullptr) virtual_call<u32>(script, 0, i32{1});
        }
        joan::orig::as_sound_stream(obj(d::g_snd))->update();  // 0x0059b1c0
        joan::orig::as_ambient_streams(obj(d::g_mus))->update();  // 0x005866d0
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) + 1;
        joan::orig::as_file(file)->destruct();  // 0x005872b0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Battle::load_scenario_41cc80", 0x0041cc80, &H::load_scenario);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
//
//    - scalar deleting destructors 0x0041d3a0, 0x0041dfa0, 0x00420150, 0x00487790.
namespace {
namespace game_leaves82b {

using joan::orig::helpers::game_delete;                         // operator delete

struct H {
    void* sdd_41d3a0(std::uint32_t flags) {
        joan::orig::as_sector(this)->destruct();  // 0x0054dba0
        if (flags & 1) game_delete(this);
        return this;
    }
    void* sdd_41dfa0(std::uint32_t flags) {
        joan::orig::as_tracks(this)->destruct();  // 0x004eb520
        if (flags & 1) game_delete(this);
        return this;
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("sdd_0041d3a0", 0x0041d3a0, &H::sdd_41d3a0);
L82("sdd_0041dfa0", 0x0041dfa0, &H::sdd_41dfa0);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b101_01.cpp (game/b101_01)
//
//    - Obj::nop_41dd80 0x0041dd80 (ret 0): empty body (no references: dead code).
//
//    - Unit::reset_aim_41e850 0x0041e850 (ret 0, vtable slot): aim block of +0x86 from the unit
//      type (0x00425ce0(word +0xa)): kinds 0xd..0x12 use the global vector 0xa69cd8 and 10.0,
//      others the model's +0x34 quad; then type +0x1e9 overrides mode 1/2/3 blocks.
namespace {
namespace game_b101_01 {

void* const kTypes = obj(d::g_item_res);

u8* type_record(u16 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0

void copy_dwords(u32* dst, const u32* src, i32 n) {  // rep movsd
    for (i32 i = 0; i < n; ++i) dst[i] = src[i];
}

JOAN_COUNTER(c_reset_aim, "Unit::reset_aim_41e850");

struct H {
    // ------------------------------------------------------------------ 0x0041dd80
    void nop_41dd80() {}

    // ------------------------------------------------------------------ 0x0041e850
    void reset_aim_41e850() {
        c_reset_aim.hit();
        u8* const t = type_record(jo::view<u16>(jo::self<jo::MapObject>(this)->type_id));
        const i32 k = static_cast<i32>(jo::view<i16>(jo::as_item_def(t)->type_id));
        if (k == 0xd || k == 0xe || k == 0xf || k == 0x10 || k == 0x11 || k == 0x12) {
            jo::self<jo::MapObject>(this)->actor->aim_mode = 1;
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[0]) = global<u32>(d::g_pos);
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[1]) = global<u32>(d::g_a69cdc);
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[2]) = global<u32>(d::g_a69ce0);
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[3]) = 0x41200000u;  // 10.0f
        } else {
            jo::self<jo::MapObject>(this)->actor->aim_mode = 1;
            const float* const s = jo::self<jo::MapObject>(this)->actor->def->model->sphere;
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[0]) = jo::view<u32>(s[0]);
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[1]) = jo::view<u32>(s[1]);
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[2]) = jo::view<u32>(s[2]);
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[3]) = jo::view<u32>(s[3]);
        }
        if (jo::as_item_def(t)->collision_type == 0) return;
        jo::self<jo::MapObject>(this)->actor->aim_mode = jo::as_item_def(t)->collision_type;
        const u32 mode = jo::self<jo::MapObject>(this)->actor->aim_mode;
        if (mode == 1) {
            const float* const s = jo::as_item_def(t)->collision_params;
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[0]) = jo::view<u32>(s[0]);
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[1]) = jo::view<u32>(s[1]);
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[2]) = jo::view<u32>(s[2]);
            jo::view<u32>(jo::self<jo::MapObject>(this)->actor->aim_sphere[3]) = jo::view<u32>(s[3]);
        } else if (mode == 2) {
            copy_dwords(reinterpret_cast<u32*>(jo::self<jo::MapObject>(this)->actor->aim_box), reinterpret_cast<const u32*>(&jo::as_item_def(t)->collision_params[4]), 6);
        } else if (mode == 3) {
            copy_dwords(&jo::self<jo::MapObject>(this)->actor->collide_kind, reinterpret_cast<const u32*>(&jo::as_item_def(t)->collision_params[10]), 5);  // collide_kind .. collide_height
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_01", name, addr, fn)
B("Obj::nop_41dd80", 0x0041dd80, &H::nop_41dd80);
B("Unit::reset_aim_41e850", 0x0041e850, &H::reset_aim_41e850);
#undef B

}  // namespace game_b101_01
}  // namespace

// ---------------------------------------- from modules/game_b109_13.cpp (game/b109_13)
//
//    - Battle::load_scenario_41dfe0 0x0041dfe0 (battle.cpp, thiscall (const char* map, const char*
//      scenario), ret 8; single caller 0x005301a2 drops eax): resets the battle state, loads
//      "SCRIPT\\SCENARIO\\<name>.SCE" into a new script object (0x0063e450 new 0x70 + ctor
//      0x0063f190, stored in [0xa59141]; the old one is deleted through vslot 0), runs the optional
//      "<name>_des.SCE" description script (vslot 3, then deleted), creates the map, sets the world
//      bounds (unit box +-8000 / +-16000) and the counters. SEH frame: normal path only.
//
//  - 0x0041dfe0: the script pointer kept in [ebp-0x2bc] is only assigned when the first open
//    (0x00586da0) succeeds; it fails only after the fatal-error handler 0x00586cb0 returned, which
//    it does only while the non-reentrancy flag 0xa64528 is set: by an earlier fatal error that is
//    exiting the process (exit(-2) 0x007b47fc), or inside the 0x00586bc0 warning box (whose modal
//    loop only reaches the WndProc 0x004d7d60 -> 0x005a1e20, never the main loop 0x00499960 that
//    calls 0x0052f930 -> here). That unreachable case uses nullptr here (the original reads stack
//    garbage).
namespace {
namespace game_b109_13 {

constexpr img::Addr kUnits(d::g_units);

JOAN_COUNTER(c_scenario, "Battle::load_scenario_41dfe0");

// ------------------------------------------------------------------ 0x0041dfe0 helpers
using joan::orig::helpers::pump;
char* c_str(void* s) { return joan::orig::as_string(s)->c_str(); }  // 0x00417d90
void str_set(void* s, const char* t) { joan::orig::as_string(s)->assign(joan::orig::arg::str(t)); }  // 0x005a1440
void str_add(void* s, const char* t) { joan::orig::as_string(s)->append(joan::orig::arg::str(t)); }  // 0x005a1550
i32 file_size(void* file) { return joan::orig::as_file(file)->file_size(); }   // 0x00587f30
// Reads the whole open file into a new buffer (0x0058e7b0 alloc size + 1) and terminates it.
char* read_all(void* file, i32 line) {
    const i32 n = file_size(file);
    char* const buf = joan::orig::arg::as<char*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(n + 1), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
    const i32 size = file_size(file);
    joan::orig::as_file(file)->file_read(joan::orig::arg::mptr(buf), joan::orig::arg::u32(size));  // 0x005873a0
    buf[file_size(file)] = 0;
    return buf;
}
void delete_script(void* p) {
    if (p != nullptr) virtual_call<void*>(p, 0, i32{1});
}
i32 assert_line(i32 delta) { return static_cast<i32>(global<i16>(d::g_9f747c)) + delta; }

struct H {
    // -------------------------------------------------------------- 0x0041dfe0
    void load_scenario(const char* map, const char* name) {
        c_scenario.hit();
        joan::orig::as_msg_log(obj(d::g_portraits))->reset();         // 0x004b31c0
        joan::orig::as_handle(this)->release(joan::orig::arg::u32(i32{0}));            // 0x0041c780
        pump();
        joan::orig::as_triple(obj(d::g_screen_mgr))->clear();         // 0x006f5e90
        joan::orig::as_scenario(obj(d::g_mission))->set_4ccf40(joan::orig::arg::u32(i32{0}));  // 0x004ccf40
        global<u32>(d::g_target_stage) = 0;
        global<u32>(d::g_stage_index) = global<u32>(d::g_target_stage);
        global<u8>(d::g_flag_a59298) = 0;
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(&img::global<char>(d::g_a5903d))), joan::orig::arg::str(name));  // strcpy
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(&img::global<char>(d::g_a5907e))), joan::orig::arg::str(name));
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(&img::global<char>(d::g_a5907e))), str(d::s_des));  // strcat "_des"
        global<i32>(d::g_a59165) = -1;
        global<i32>(d::g_a59161) = -1;
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = 0;
        joan::orig::as_game(obj(d::g_game_state))->load_arrays();  // 0x00470d40
        joan::orig::as_players(obj(d::g_physic_sys))->reset();  // 0x006ad950
        pump();
        alignas(4) u8 path[0x194];
        joan::orig::as_string(path)->construct();  // 0x005a1170
        str_set(path, str(d::s_SCRIPT_SCENARIO));     // "SCRIPT\\SCENARIO\\"
        str_add(path, name);
        str_add(path, str(d::s_SCE));  // ".SCE"
        if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(c_str(path))) == 0) {  // 0x00590b80
            joan::orig::as_scenario(obj(d::g_mission))->set_4ccf40(joan::orig::arg::u32(i32{0}));                    // 0x004ccf40
            return;
        }
        pump();
        alignas(4) jo::File file_obj;  // the 0x110-byte frame slot of the original; a real File (x64: 0x112 bytes)
        void* const file = &file_obj;
        joan::orig::as_file(file)->construct();  // 0x004179c0
        void* script = nullptr;             // see notes: only unassigned when the open cannot fail
        if (joan::orig::as_file(file)->file_open(joan::orig::arg::str(c_str(path)), i32{1}, static_cast<char>(i32{0})) != 0) {  // 0x00586da0
            char* const buf = read_all(file, assert_line(0x35));
            if (global<void*>(d::g_scenario_script) != nullptr) delete_script(global<void*>(d::g_scenario_script));
            global<void*>(d::g_scenario_script) = new_script();
            script = global<void*>(d::g_scenario_script);
            joan::orig::as_script(script)->compile(joan::orig::arg::mptr(buf), c_str(path));  // 0x00649280
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(buf), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(assert_line(0x45)));  // 0x0058ed70
            joan::orig::as_file(file)->file_close();                                                     // 0x00587260
            str_set(path, str(d::s_SCRIPT_SCENARIO));
            str_add(path, name);
            joan::orig::as_widget(script)->load_ex_res(joan::orig::arg::str(c_str(path)));  // 0x0063f580
        }
        pump();
        str_set(path, str(d::s_SCRIPT_SCENARIO));
        str_add(path, name);
        str_add(path, str(d::s_des));  // "_des"
        str_add(path, str(d::s_SCE));  // ".SCE"
        if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(c_str(path))) != 0 &&
            joan::orig::as_file(file)->file_open(joan::orig::arg::str(c_str(path)), i32{1}, static_cast<char>(i32{0})) != 0) {
            char* const buf = read_all(file, assert_line(0x65));
            void* const des = new_script();
            joan::orig::as_script(des)->compile(joan::orig::arg::mptr(buf), c_str(path));  // 0x00649280
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(buf), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(assert_line(0x6e)));  // 0x0058ed70
            joan::orig::as_file(file)->file_close();
            str_set(path, str(d::s_SCRIPT_SCENARIO));
            str_add(path, name);
            str_add(path, str(d::s_des));
            joan::orig::as_widget(des)->load_ex_res(joan::orig::arg::str(c_str(path)));  // 0x0063f580
            virtual_call(des, 0xc, static_cast<void*>(nullptr));
            delete_script(des);
        }
        pump();
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) + 1;
        joan::orig::as_cutscene(obj(d::g_game_a5d380))->clear();  // 0x006ca170
        const i32 slot = joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_a45f08))->add_new());         // 0x005611b0
        void* const world = joan::orig::arg::as<void*>(joan::orig::as_units(obj(d::g_a45f08))->get_420240(joan::orig::arg::i32(slot)));  // 0x00420240
        str_set(path, map);
        joan::orig::as_world(world)->init(joan::orig::arg::u16(i32{1}), joan::orig::arg::str(c_str(path)), joan::orig::arg::str(c_str(path)), joan::orig::arg::mptr(i32{0}));  // 0x0044e8c0
        joan::orig::as_unit_list(obj(d::g_a45f08))->select(joan::orig::arg::i32(slot));                            // 0x00561350
        joan::orig::as_camera_man(obj(d::g_camera_man))->init();                                  // 0x005615c0
        joan::orig::as_screen(obj(d::g_player_unit_recno))->reset();                                  // 0x0042de70
        joan::orig::as_script_block(script)->rewind();                                           // 0x0063ed40
        virtual_call(script, 0xc, static_cast<void*>(this));
        pump();
        joan::orig::as_players(obj(d::g_physic_sys))->set_timer(joan::orig::arg::i16(i32{1}));  // 0x006ad900
        joan::orig::as_message_list(obj(d::g_names))->reset();          // 0x004ae6c0
        joan::orig::as_screen(obj(d::g_player_unit_recno))->reset();          // 0x0042de70
        const u32 hero = joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());  // 0x004203c0
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(img::obj(kUnits))->ptr_at(joan::orig::arg::i32(hero)));     // 0x0041b3c0
        float near_box[6];
        joan::orig::as_aabb(near_box)->set_point(joan::orig::arg::ptr(jo::as_unit(unit)->position_));  // 0x0060d430
        const float d1 = global<float>(d::flt_8000);
        near_box[0] = near_box[0] - d1;
        near_box[1] = near_box[1] - d1;
        near_box[2] = near_box[2] - d1;
        near_box[3] = near_box[3] + d1;
        near_box[4] = near_box[4] + d1;
        near_box[5] = near_box[5] + d1;
        float far_box[6];
        joan::orig::as_aabb(far_box)->set_point(joan::orig::arg::ptr(jo::as_unit(unit)->position_));
        const float d2 = global<float>(d::flt_16000);
        far_box[0] = far_box[0] - d2;
        far_box[1] = far_box[1] - d2;
        far_box[2] = far_box[2] - d2;
        far_box[3] = far_box[3] + d2;
        far_box[4] = far_box[4] + d2;
        far_box[5] = far_box[5] + d2;
        joan::orig::as_world(global<void*>(d::g_world))->stream_cells(joan::orig::arg::mptr(near_box), joan::orig::arg::mptr(far_box), i32{1});  // 0x00547630
        pump();
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) + 1;
        joan::orig::as_critical_hud(obj(d::g_critical_hud))->setup_combos(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));                                    // 0x0045f860
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_8(static_cast<i32>(global<i16>(d::g_a47c36)));  // 0x005639f0
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(i32{0});                                    // 0x00429860
        jo::view<i32>(jo::self<jo::Battle>(this)->u32_0) = 10;
        global<u32>(d::g_a5916d) = global<u32>(d::g_now);
        std::memset(joan::orig::arg::mptr(obj(d::g_a59244)), i32{0}, u32{0x50});  // memset
        pump();
        joan::orig::as_file(file)->destruct();  // 0x005872b0
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_13", name, addr, fn)
B("Battle::load_scenario_41dfe0", 0x0041dfe0, &H::load_scenario);
#undef B

}  // namespace game_b109_13
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
//
//    - Battle::init 0x0041ea20: 0x00620a20 on +0x61, memset(this, 0, 0x92), returns this.
namespace {
namespace game_b92_01 {

struct H {
    // ------------------------------------------------------------ 0x0041ea20
    H* battle_init() {
        joan::orig::as_rect9(&jo::self<jo::World>(this)->bitmaps)->clear();  // 0x00620a20 (+0x61 on x86; the 0x92-byte object is a World)
        std::memset(static_cast<void*>(this), 0, static_cast<std::uint32_t>(offsetof(jo::World, gap_92)));                                         // memset (0x92 on x86)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("Battle::init", 0x0041ea20, &H::battle_init);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b109_14.cpp (game/b109_14)
//
//    - Battle::start_41ea60 0x0041ea60 (thiscall(arg), ret 4, battle.cpp; callers drop eax): battle
//      start-up. Releases the old 0xa5ba00 object, resets subsystems with progress steps
//      ([0xa50259] = 5, 8, 0xd, 0xf, 0x14, 0x19, 0x1a, 0x1c, 0x64), saves globals 0xa48394..0xa483b0
//      and the 0xa47e14 / 0xa47fa0 / 0xa48134 tables, loads the two scenario scripts
//      "SCRIPT\\SCENARIO\\<name>.SCE" (campaign record [0xa58fab] at [0xa58fb7] + (i - 1) * 0x1b2,
//      or the names at 0xa5903d / 0xa5907e) into new 0x70-byte script objects (0x0063e450 +
//      0x0063f190, parsed by 0x00649280, which catches every exception itself), then sets up the
//      level and restores the saved globals. SEH frame: normal path only (the file object
//      0x004179c0 / 0x005872b0 is destroyed explicitly on every path that the original does).
namespace {
namespace game_b109_14 {

using i8 = std::int8_t;

i32 sx16(i16 v) { return static_cast<i32>(v); }

struct alignas(4) Path {  // 0x005a1170 string/path object (no destructor in the originals)
    u8 bytes[0x194];
};
struct alignas(4) File {  // 0x004179c0 / 0x005872b0
    u8 bytes[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger File
};

JOAN_COUNTER(c_battle, "Battle::start_41ea60");

// ------------------------------------------------------------------ 0x0041ea60 helpers
void progress(u32 step) {
    joan::orig::as_sound_stream(obj(d::g_snd))->update();  // 0x0059b1c0
    joan::orig::as_ambient_streams(obj(d::g_mus))->update();  // 0x005866d0
    global<u32>(d::g_a50259) = step;
    joan::orig::as_progress(obj(d::g_progress))->update();  // 0x0075f3c0
    joan::orig::as_progress(obj(d::g_progress))->redraw();  // 0x0075f4f0
}
using joan::orig::helpers::pump;
const char* path_str(Path* p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(p)->c_str()); }  // 0x00417d90
void path_set(Path* p, const char* s) { joan::orig::as_string(p)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void path_cat(Path* p, const char* s) { joan::orig::as_string(p)->append(joan::orig::arg::str(s)); }  // 0x005a1550
i32 file_exists(Path* p) { return joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(path_str(p))); }  // 0x00590b80
i32 file_open(File* f, Path* p) {
    const char* const name = path_str(p);
    return joan::orig::as_file(f)->file_open(joan::orig::arg::str(name), i32{1}, static_cast<char>(i32{0}));  // 0x00586da0
}
i32 line_base() { return sx16(global<i16>(d::g_9f7480)); }
// Reads the whole file into a new NUL-terminated buffer (0x0058e7b0 at battle.cpp:line).
char* read_all(File* f, i32 line) {
    const i32 size = joan::orig::as_file(f)->file_size() + 1;   // 0x00587f30
    char* const buf = joan::orig::arg::as<char*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
    const i32 n = joan::orig::as_file(f)->file_size();
    joan::orig::as_file(f)->file_read(joan::orig::arg::mptr(buf), joan::orig::arg::u32(n));  // 0x005873a0
    buf[joan::orig::as_file(f)->file_size()] = 0;
    return buf;
}
void parse_script(void* script, char* buf, Path* p) {
    const char* const name = path_str(p);
    joan::orig::as_script(script)->compile(joan::orig::arg::mptr(buf), name);  // 0x00649280
}
void delete_virtual(void* o) {
    if (o != nullptr) virtual_call<u32>(o, 0, i32{1});
}

struct H {
    // -------------------------------------------------------------- 0x0041ea60
    void battle_start(void* arg) {
        joan::hitch::Span hitch_span("battle_start");  // [Debug] HitchLog
        c_battle.hit();
        joan::orig::as_screen(obj(d::g_player_unit_recno))->reset();  // 0x0042de70
        if (global<void*>(d::g_a5ba00) != nullptr) {
            void* const old = global<void*>(d::g_a5ba00);
            if (old != nullptr) joan::orig::as_camera_zones(old)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00420150
            global<void*>(d::g_a5ba00) = nullptr;
        }
        joan::orig::as_sms(obj(d::g_sms))->clear();  // 0x006e5e00
        global<u16>(d::g_a48c5e) = 0;
        joan::orig::as_handle(this)->release(joan::orig::arg::u32(i32{0}));  // 0x0041c780
        progress(5);
        joan::orig::as_screen(obj(d::g_mini_map))->release_70e3c0();  // 0x0070e3c0
        joan::orig::as_unit_list(obj(d::g_profile_list))->clear_4c9f00();  // 0x004c9f00
        joan::orig::as_game(obj(d::g_game_state))->load_arrays();  // 0x00470d40
        joan::orig::as_players(obj(d::g_physic_sys))->reset();  // 0x006ad950
        joan::orig::as_cutscene(obj(d::g_game_a5d380))->clear();  // 0x006ca170
        progress(8);
        joan::orig::as_game_file(arg)->load_477e80();  // 0x00477e80
        progress(0xd);
        u32 saved[8];
        for (int i = 0; i < 8; ++i) saved[i] = global<u32>(d::g_a48394 + 4 * i);
        u32 l1[0x63], l2[0x65], l3[0x65];
        std::memcpy(l1, obj(d::g_a47e14), sizeof l1);
        std::memcpy(l2, obj(d::g_a47fa0), sizeof l2);
        std::memcpy(l3, obj(d::g_a48134), sizeof l3);
        jo::self<jo::Battle>(this)->u32_0 = 0;
        const i32 index = static_cast<i32>(global<u32>(d::g_stage_index));
        const auto rec = [index]() { return global<u8*>(d::g_stages) + (index - 1) * kScenarioSize; };
        if (index != 0) {
            Path path;
            joan::orig::as_string(&path)->construct();  // 0x005a1170
            path_set(&path, str(d::s_SCRIPT_SCENARIO));
            path_cat(&path, reinterpret_cast<const char*>(rec() + 0x82));
            path_cat(&path, str(d::s_SCE));
            if (file_exists(&path) == 0) {
                joan::orig::as_scenario(obj(d::g_mission))->set_4ccf40(joan::orig::arg::u32(i32{0}));  // 0x004ccf40
                return;
            }
            progress(0xf);
            File file;
            joan::orig::as_file(&file)->construct();  // 0x004179c0
            if (file_open(&file, &path) != 0) {
                char* const buf = read_all(&file, line_base() + 0x5d);
                if (jo::as_scenario_entry(rec())->script != nullptr) delete_virtual(jo::view<void*>(jo::as_scenario_entry(rec())->script));
                void* const script = new_script();
                jo::view<void*>(jo::as_scenario_entry(rec())->script) = script;
                void* const s = jo::view<void*>(jo::as_scenario_entry(rec())->script);
                parse_script(s, buf, &path);
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(buf), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(line_base() + 0x6d));  // 0x0058ed70
                joan::orig::as_file(&file)->file_close();   // 0x00587260
                path_set(&path, str(d::s_SCRIPT_SCENARIO));
                path_cat(&path, reinterpret_cast<const char*>(rec() + 0x82));
                joan::orig::as_widget(s)->load_ex_res(joan::orig::arg::str(path_str(&path)));  // 0x0063f580
            }
            progress(0x14);
            path_set(&path, str(d::s_SCRIPT_SCENARIO));
            path_cat(&path, reinterpret_cast<const char*>(rec() + 0xc3));
            path_cat(&path, str(d::s_SCE));
            if (file_exists(&path) == 0) {
                joan::orig::as_file(&file)->destruct();  // 0x005872b0
                return;
            }
            if (file_open(&file, &path) != 0) {
                char* const buf = read_all(&file, line_base() + 0x8f);
                void* const s = new_script();
                parse_script(s, buf, &path);
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(buf), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(line_base() + 0x98));  // 0x0058ed70
                joan::orig::as_file(&file)->file_close();   // 0x00587260
                path_set(&path, str(d::s_SCRIPT_SCENARIO));
                path_cat(&path, reinterpret_cast<const char*>(rec() + 0xc3));
                joan::orig::as_widget(s)->load_ex_res(joan::orig::arg::str(path_str(&path)));  // 0x0063f580
                virtual_call<u32>(s, 0xc, i32{0});
                delete_virtual(s);
            }
            progress(0x19);
            joan::orig::as_file(&file)->destruct();  // 0x005872b0
        } else {
            Path path;
            joan::orig::as_string(&path)->construct();  // 0x005a1170
            path_set(&path, str(d::s_SCRIPT_SCENARIO));
            path_cat(&path, str(d::g_a5903d));
            path_cat(&path, str(d::s_SCE));
            if (file_exists(&path) == 0) {
                joan::orig::as_scenario(obj(d::g_mission))->set_4ccf40(joan::orig::arg::u32(i32{0}));  // 0x004ccf40
                return;
            }
            progress(0xf);
            File file;
            joan::orig::as_file(&file)->construct();  // 0x004179c0
            if (file_open(&file, &path) != 0) {
                char* const buf = read_all(&file, line_base() + 0xca);
                if (global<void*>(d::g_scenario_script) != nullptr) delete_virtual(global<void*>(d::g_scenario_script));
                void* const script = new_script();
                global<void*>(d::g_scenario_script) = script;
                void* const s = global<void*>(d::g_scenario_script);
                parse_script(s, buf, &path);
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(buf), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(line_base() + 0xda));  // 0x0058ed70
                joan::orig::as_file(&file)->file_close();   // 0x00587260
                path_set(&path, str(d::s_SCRIPT_SCENARIO));
                path_cat(&path, str(d::g_a5903d));
                joan::orig::as_widget(s)->load_ex_res(joan::orig::arg::str(path_str(&path)));  // 0x0063f580
            }
            progress(0x14);
            path_set(&path, str(d::s_SCRIPT_SCENARIO));
            path_cat(&path, str(d::g_a5907e));
            path_cat(&path, str(d::s_SCE));
            if (file_exists(&path) == 0) {
                joan::orig::as_file(&file)->destruct();  // 0x005872b0
                return;
            }
            if (file_open(&file, &path) != 0) {
                char* const buf = read_all(&file, line_base() + 0xfc);
                void* const s = new_script();
                parse_script(s, buf, &path);
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(buf), str(d::s_d_ww_battle_cpp), joan::orig::arg::i32(line_base() + 0x105));  // 0x0058ed70
                joan::orig::as_file(&file)->file_close();   // 0x00587260
                path_set(&path, str(d::s_SCRIPT_SCENARIO));
                path_cat(&path, str(d::g_a5907e));
                joan::orig::as_widget(s)->load_ex_res(joan::orig::arg::str(path_str(&path)));  // 0x0063f580
                virtual_call<u32>(s, 0xc, i32{0});
                delete_virtual(s);
            }
            progress(0x19);
            joan::orig::as_file(&file)->destruct();  // 0x005872b0
        }
        progress(0x1a);
        jo::self<jo::Battle>(this)->u32_0 += 1;
        joan::orig::as_hud(obj(d::g_mini_map))->init_70e1d0();  // 0x0070e1d0
        char* name = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(reinterpret_cast<char*>(global<u8*>(d::g_world) + 6)), i32{0x5c}));
        name = reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(name) + 1);
        name = joan::orig::arg::as<char*>(joan::orig::libc::_strupr(joan::orig::arg::str(name)));
        jo::view<u32>(jo::as_world(global<u8*>(d::g_world))->bitmaps.count) = 0;
        jo::as_world(global<u8*>(d::g_world))->bitmaps.entries = nullptr;
        jo::as_world(global<u8*>(d::g_world))->bitmaps.loaded = 0;
        if (joan::orig::arg::as<i8>(joan::orig::as_bmp_res(global<u8*>(d::g_world) + 0x61)->load(joan::orig::arg::str(name))) != 0)  // 0x00620a50
            joan::orig::as_panel(obj(d::g_mini_map))->set_unit(joan::orig::arg::i16(sx16(jo::as_world(global<u8*>(d::g_world))->world_id)));  // 0x00715d50
        else
            joan::orig::as_panel(obj(d::g_mini_map))->set_unit(joan::orig::arg::i16(i32{0}));
        joan::orig::as_marker(obj(d::g_mini_map))->update();       // 0x00714040
        joan::orig::as_camera_man(obj(d::g_camera_man))->init();       // 0x005615c0
        joan::orig::as_players(obj(d::g_physic_sys))->set_timer(joan::orig::arg::i16(i32{1}));  // 0x006ad900
        progress(0x1c);
        joan::orig::as_player(obj(d::g_player_unit_recno))->set_unit(joan::orig::arg::i32(sx16(global<i16>(d::g_a47c36))));  // 0x0042f9c0
        const u32 side = joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id());                // 0x004203c0
        u8* const army = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(side)));  // 0x0041b3c0
        joan::orig::as_player(obj(d::g_player_unit_recno))->switch_unit(joan::orig::arg::i32(global<u32>(d::g_control_mode) == 1 ? i32{0} : i32{1}));  // 0x0042b6e0
        float box1[6];
        float box2[6];
        joan::orig::as_aabb(box1)->set_point(joan::orig::arg::ptr(army + 0xe));  // 0x0060d430
        {
            const float d = global<float>(d::flt_8000);
            box1[0] -= d;
            box1[1] -= d;
            box1[2] -= d;
            box1[3] += d;
            box1[4] += d;
            box1[5] += d;
        }
        joan::orig::as_aabb(box2)->set_point(joan::orig::arg::ptr(army + 0xe));  // 0x0060d430
        {
            const float d = global<float>(d::flt_16000);
            box2[0] -= d;
            box2[1] -= d;
            box2[2] -= d;
            box2[3] += d;
            box2[4] += d;
            box2[5] += d;
        }
        joan::orig::as_light_sys(obj(d::g_light_sys))->clear();             // 0x004a5970
        joan::orig::as_sound3d(obj(d::g_weather_system))->release(static_cast<char>(joan::orig::arg::i32(i32{0})));     // 0x00689d00
        joan::orig::as_markers(obj(d::g_slots_a3ce90))->delete_all();             // 0x004876d0
        joan::orig::as_grass(obj(d::g_dyn_grass))->set_enabled(static_cast<char>(joan::orig::arg::i32(i32{0})));     // 0x00464600
        joan::orig::as_grass(obj(d::g_dyn_grass))->set_enabled(static_cast<char>(joan::orig::arg::i32(static_cast<i32>(global<i8>(d::g_cfg_grass_enable)))));
        global<void*>(d::g_main_light) = nullptr;
        joan::orig::as_actor_list(obj(d::g_actor_list))->delete_all();  // 0x0041ba00
        pump();
        {
            void* const o = jo::view<void*>(jo::as_world(global<u8*>(d::g_world))->path_space);
            if (o != nullptr) joan::orig::as_pfd_data(o)->destroy(joan::orig::arg::u32(i32{1}));  // 0x004201a0
            jo::as_world(global<u8*>(d::g_world))->path_space = nullptr;
        }
        {
            void* const o = jo::view<void*>(jo::as_world(global<u8*>(d::g_world))->scene);
            if (o != nullptr) joan::orig::as_scene(o)->destroy(joan::orig::arg::u32(i32{1}));  // 0x004201f0
            jo::as_world(global<u8*>(d::g_world))->scene = nullptr;
        }
        jo::as_world(global<u8*>(d::g_world))->loaded = 0;
        pump();
        joan::orig::as_world(global<u8*>(d::g_world))->setup_sky();  // 0x0049ae00
        pump();
        joan::orig::as_world(global<u8*>(d::g_world))->load_water();  // 0x00444e70
        pump();
        joan::orig::as_world(global<u8*>(d::g_world))->stream_cells(joan::orig::arg::mptr(box1), joan::orig::arg::mptr(box2), i32{1});  // 0x00547630
        pump();
        jo::self<jo::Battle>(this)->u32_0 += 1;
        joan::orig::as_critical_hud(obj(d::g_critical_hud))->setup_combos(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));                          // 0x0045f860
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_8(joan::orig::arg::i32(sx16(global<i16>(d::g_a47c36))));  // 0x005639f0
        joan::orig::as_camera_man(obj(d::g_camera_man))->set_mode(i32{0});                          // 0x00429860
        joan::orig::as_unit(obj(d::g_player_unit_recno))->finish_state_42d980();                                  // 0x0042d980
        global<u32>(d::g_a48394) = saved[0];
        global<u32>(d::g_a48398) = saved[1];
        global<u32>(d::g_a4839c) = saved[2];
        global<u32>(d::g_a483a0) = 0;
        global<u32>(d::g_a483a4) = saved[4];
        global<u32>(d::g_a483a8) = saved[5];
        global<u32>(d::g_a483ac) = saved[6];
        global<u32>(d::g_a483b0) = 0;
        std::memcpy(obj(d::g_a47e14), l1, sizeof l1);
        std::memcpy(obj(d::g_a47fa0), l2, sizeof l2);
        std::memcpy(obj(d::g_a48134), l3, sizeof l3);
        joan::orig::as_message_list(obj(d::g_names))->reset();  // 0x004ae6c0
        joan::orig::as_parser(obj(d::g_mouse))->drain();  // 0x005959c0
        jo::self<jo::Battle>(this)->u32_0 = 0xa;
        global<u32>(d::g_a5916d) = global<u32>(d::g_now);
        joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::u32(u32{0x3f800000}));  // 1.0f  // 0x0042f930
        progress(0x64);
        if (global<i8>(d::g_flag_a59298) == 1) {
            if (joan::orig::arg::as<i8>(joan::orig::as_glory_screen(obj(d::g_glory_screen))->is_open()) == 0)  // 0x006e7b50
                joan::orig::as_screen(obj(d::g_glory_screen))->win_6e7900();      // 0x006e7900
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_14", name, addr, fn)
B("Battle::start_41ea60", 0x0041ea60, &H::battle_start);
#undef B

}  // namespace game_b109_14
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {

using joan::call::thiscall;

constexpr fn::Code kListGet = fn::List_get_41b3f0;     // 0x0041b3f0

void* list_get(const void* list, std::int32_t i) { return thiscall<void*>(kListGet, list, i); }

struct H {
    void* list_get_wrap(std::int32_t i) { return list_get(this, i); }                      // 0x0041b3c0 / 0x0041bd60 / 0x00487b80
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("List::get_b", 0x0041b3c0, &H::list_get_wrap);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_leaves81.cpp (game/leaves81)
//
//    - NodeList::delete_all 0x0041ba00, Unit::detach_scene 0x0041fcc0,
//      Grass::remove_patch 0x00466f40 (owner), List<0x4ea370>::delete_all 0x004e9f10,
//      List<0x487790>::delete_all 0x004876d0, Units::remove 0x0049f370 (id),
//      StreamSlot::release 0x004cea40, Pool::last_used 0x004b9c30.
//
//    - scalar deleting destructors 0x00420790, 0x004201f0, 0x004201a0, 0x0041b5f0.
namespace {
namespace game_leaves81 {

using joan::call::thiscall;
using joan::call::cdecl_call;
JOAN_COUNTER(c_nodes_delete, "NodeList::delete_all");

using joan::orig::helpers::node_remove;  // 0x005efdb0
std::int32_t list_count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0
void list_remove(void* list, std::int32_t i) { joan::orig::dynarray_b(list)->linkout(joan::orig::arg::i32(i)); }  // 0x005858c0
using joan::orig::helpers::game_delete;                                    // operator delete

struct H {
    // 0x0041ba00 NodeList::delete_all: entries detached from their parent (+0x18) and deleted.
    void nodes_delete_all() {
        c_nodes_delete.hit();
        for (std::int32_t i = list_count(this); i >= 1; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->empty_at_56f3f0(joan::orig::arg::i32(i))) != 0) continue;  // 0x0056f3f0
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_56f320(joan::orig::arg::i32(i)));           // 0x0056f320
            if (jo::as_scene_object(e)->scene != nullptr) node_remove(jo::as_scene_object(e)->scene, e);
            if (e != nullptr) joan::orig::as_scene_object(e)->destroy(1u);  // 0x00420840 delete
            list_remove(this, i);
        }
    }

    void* sdd_4201f0(std::uint32_t flags) {
        joan::orig::as_scene(this)->dtor();  // 0x005e9a90
        if (flags & 1) game_delete(this);
        return this;
    }
    void* sdd_4201a0(std::uint32_t flags) {
        joan::orig::as_path_space(this)->destruct();  // 0x00665000
        if (flags & 1) game_delete(this);
        return this;
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("NodeList::delete_all", 0x0041ba00, &H::nodes_delete_all);
L81("sdd_004201f0", 0x004201f0, &H::sdd_4201f0);
L81("sdd_004201a0", 0x004201a0, &H::sdd_4201a0);
#undef L81

}  // namespace game_leaves81
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
//
//    - scalar deleting destructors 0x0041d3a0, 0x0041dfa0, 0x00420150, 0x00487790.
//
//    - Units::get 0x00420240 / List::get 0x0056f320 (0x0041b3f0), List::empty_at 0x00561320 /
//      0x0056f3f0, Types::empty_at 0x00485410 (+4 list), Types::remove 0x004afaf0 (+4 list).
namespace {
namespace game_leaves82b_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::game_delete;                         // operator delete
void* list_get(void* list, std::int32_t i) { return joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0

struct H {
    void* sdd_420150(std::uint32_t flags) {
        joan::orig::as_camera_zones(this)->destruct();  // 0x00425df0
        if (flags & 1) game_delete(this);
        return this;
    }

    // ---------------------------------------------------------------- list wrappers
    void* get_420240(std::int32_t i) { return list_get(this, i); }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("sdd_00420150", 0x00420150, &H::sdd_420150);
L82("Units::get", 0x00420240, &H::get_420240);
#undef L82

}  // namespace game_leaves82b_2
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
//
//    - byte setters 0x004d7e70 (+0x3c), 0x0042cf20 (+0x10a); getters 0x00497560 (+0xa),
//      0x004301c0 (byte +0x744), 0x004203c0 (+0), 0x00415280 (+0x112).
namespace {
namespace game_b82_18_2 {

struct H {
    std::uint32_t get_00() { return jo::view<std::uint32_t>(jo::self<jo::Player>(this)->unit_id_); }                                 // 0x004203c0
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("get_dword_0", 0x004203c0, &H::get_00);
#undef B82

}  // namespace game_b82_18_2
}  // namespace

// ---------------------------------------- from modules/game_leaves81_2.cpp (game/leaves81)
namespace {
namespace game_leaves81_2 {

using joan::call::thiscall;
using joan::img::obj;

using joan::call::thiscall;
constexpr img::Addr kUnits(d::g_items);

JOAN_COUNTER(c_detach_scene, "Unit::detach_scene");

using joan::orig::helpers::node_remove;  // 0x005efdb0

struct H {
    // 0x0041fcc0 Unit::detach_scene
    void detach_scene() {
        c_detach_scene.hit();
        if (static_cast<std::int8_t>(jo::self<jo::Unit>(this)->in_scene) == 0) return;
        jo::self<jo::Unit>(this)->in_scene = 0;
        jo::self<jo::Unit>(this)->instantiated = 1;
        if (jo::self<jo::Unit>(this)->scene != nullptr) {
            // sound +0x331 stopped, then unit +0x1bf's nodes; sound +0x32d, then unit +0x1bd.
            jo::Unit* const me = jo::self<jo::Unit>(this);
            jo::Trail* const* const sounds[2] = {&me->trail_b, &me->trail_a};
            const std::int16_t* const links[2] = {&me->carried_b, &me->carried_a};
            for (std::int32_t k = 0; k < 2; ++k) {
                if (*sounds[k] != nullptr) joan::orig::as_trail(*sounds[k])->set_scene(joan::orig::arg::mptr(0));  // 0x006891d0
                if (joan::orig::as_units(img::obj(kUnits))->list.empty_at(static_cast<std::int32_t>(*links[k])) != 0)
                    continue;  // 0x00425d50
                std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_units(img::obj(kUnits))->list.ptr_at(static_cast<std::int32_t>(*links[k])));  // 0x00425d20
                if (jo::view<void*>(jo::as_map_object(u)->actor_b) != nullptr) node_remove(jo::self<jo::Unit>(this)->scene, jo::view<void*>(jo::as_map_object(u)->actor_b));
                if (jo::view<void*>(jo::as_map_object(u)->actor) != nullptr) node_remove(jo::self<jo::Unit>(this)->scene, jo::view<void*>(jo::as_map_object(u)->actor));
            }
            if (jo::self<jo::Unit>(this)->weapon_model != nullptr) node_remove(jo::self<jo::Unit>(this)->scene, jo::self<jo::Unit>(this)->weapon_model);
            if (jo::self<jo::Unit>(this)->model != nullptr) node_remove(jo::self<jo::Unit>(this)->scene, jo::self<jo::Unit>(this)->model);
            if (jo::self<jo::Unit>(this)->actor != nullptr) node_remove(jo::self<jo::Unit>(this)->scene, jo::self<jo::Unit>(this)->actor);
            jo::self<jo::Unit>(this)->scene = nullptr;
        }
        if (jo::self<jo::Unit>(this)->cell != nullptr) {
            const std::int16_t* const links[2] = {&jo::self<jo::Unit>(this)->carried_a, &jo::self<jo::Unit>(this)->carried_b};
            for (std::int32_t k = 0; k < 2; ++k) {
                if (joan::orig::as_units(img::obj(kUnits))->list.empty_at(static_cast<std::int32_t>(*links[k])) != 0) continue;
                void* const u = joan::orig::as_units(img::obj(kUnits))->list.ptr_at(static_cast<std::int32_t>(*links[k]));
                joan::orig::as_unit(jo::self<jo::Unit>(this)->cell)->set_sector(joan::orig::arg::mptr(u));  // 0x0054f110
            }
        }
        joan::orig::as_unit(this)->release_parts();  // 0x004f07f0
        jo::self<jo::Unit>(this)->instantiated = 0;
    }
};

#define L81(name, addr, fn) JOAN_REPLACE("game/leaves81", name, addr, fn)
L81("Unit::detach_scene", 0x0041fcc0, &H::detach_scene);
#undef L81

}  // namespace game_leaves81_2
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

using joan::call::thiscall;
using joan::img::obj;

struct V3 {
    float x, y, z;
};

struct alignas(4) Angle {  // 0x00576120 (byte at +0)
    std::uint32_t b;
};

float table_cos(Angle& a) { return joan::orig::arg::as<float>(joan::orig::as_angle(&a)->cos()); }  // 0x00576ac0
float table_sin(Angle& a) { return joan::orig::arg::as<float>(joan::orig::as_angle(&a)->sin()); }  // 0x005768d0

JOAN_COUNTER(c_unit_pos, "Unit::set_position");

struct H {
    // ---- 0x0041ff50 Unit::set_position(V3) ----------------------------------------------------
    void unit_set_position(V3 pos) {  // floats: the System V ABI passes the vector in XMM registers, as the callers declare it
        c_unit_pos.hit();
        std::memcpy(jo::self<jo::MapObject>(this)->pos, &pos, sizeof pos);
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->actor) != nullptr)
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::MapObject>(this)->actor))->set_position_5ac310(joan::orig::arg::mptr(static_cast<void*>(&pos)));  // 0x005ac310
        else
            joan::orig::as_unit(this)->update_view();  // 0x004311d0
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->actor_b) != nullptr) {
            const float z = pos.z;
            const joan::hook::x87::Fpu fpu{true};
            const float zz = static_cast<float>(fpu.add(static_cast<double>(z), 0.5));  // + qword 0x9d3e18
            joan::orig::as_actor(jo::view<void*>(jo::self<jo::MapObject>(this)->actor_b))->set_position_5ac4b0(joan::orig::arg::bits(pos.x), joan::orig::arg::bits(pos.y), joan::orig::arg::bits(zz));  // 0x005ac4b0
        }
        if (jo::view<void*>(jo::self<jo::MapObject>(this)->world_item) != nullptr) joan::orig::as_world_item(jo::view<void*>(jo::self<jo::MapObject>(this)->world_item))->set_pos(joan::orig::arg::bits(pos.x), joan::orig::arg::bits(pos.y), joan::orig::arg::bits(pos.z));  // 0x00456fe0
        void* const models = obj(d::g_particle_effect_array);
        for (std::int32_t i = 0; i < 2; ++i) {
            if (joan::orig::as_dynarray(models)->empty_at(static_cast<std::int32_t>(jo::self<jo::MapObject>(this)->sound_slots[i])) != 0)
                continue;  // 0x0041bd90
            std::uint8_t* const m =
                joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(models)->ptr_at(static_cast<std::int32_t>(jo::self<jo::MapObject>(this)->sound_slots[i])));  // 0x0041bd60
            V3 v;
            std::memcpy(&v, &pos, 12);
            Angle ang{0};
            joan::orig::as_angle(&ang)->construct();                         // 0x00576120
            joan::orig::as_angle(&ang)->set(joan::orig::arg::f32(jo::as_model_obj(m)->angle));    // 0x00576240
            const float c1 = table_cos(ang);
            const float t24 = c1 * jo::as_model_obj(m)->pos[0];
            const float s1 = table_sin(ang);
            const float s1y = s1 * jo::as_model_obj(m)->pos[1];
            const float dx = t24 - s1y;
            v.x = dx + v.x;
            const float c2 = table_cos(ang);
            const float t28 = c2 * jo::as_model_obj(m)->pos[1];
            const float s2 = table_sin(ang);
            const float s2x = s2 * jo::as_model_obj(m)->pos[0];
            const float dy = t28 - s2x;
            v.y = dy + v.y;
            v.z = v.z + jo::as_model_obj(m)->pos[2];
            joan::orig::as_effect_state(&jo::as_model_obj(m)->fx)->set_position(v);  // 0x0041b2a0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("Unit::set_position", 0x0041ff50, &H::unit_set_position);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x00420110
    void* clear_6c() {
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::Missile)));  // 0x6c on x86
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Block6c::clear", 0x00420110, &H::clear_6c);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01 {

using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

// ---- CRT initialiser wrappers: call the body directly, return its eax ----
template <fn::Code Body>
u32 JOAN_CDECL init_wrapper() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("init_0x00420270", 0x00420270, &init_wrapper<fn::init_a3363c>);
B("init_0x004202d0", 0x004202d0, &init_wrapper<fn::init_a33640>);
B("init_0x00420320", 0x00420320, &init_wrapper<fn::init_420340>);
B("init_0x00420370", 0x00420370, &init_wrapper<fn::init_a33648>);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::hook::global;

using joan::call::cdecl_call;

// ---------------------------------------------------------------------------- static inits
using joan::orig::helpers::pi_from_atan;
void JOAN_CDECL init_a3363c() { global<float>(d::g_a3363c) = pi_from_atan(); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("init_a3363c", 0x00420290, &init_a3363c);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using joan::hook::global;

void JOAN_CDECL init_a33640() { global<float>(d::g_a33640) = global<float>(d::g_a3363c) / global<float>(d::flt_2); }

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("init_a33640", 0x004202f0, &init_a33640);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

using joan::hook::global;

// ---- static initialisers / free functions ----
using joan::orig::helpers::div_init;
void JOAN_CDECL init_420340() { div_init(d::g_a33644, d::g_a3363c, d::flt_3); }

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("init_420340", 0x00420340, &init_420340);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::hook::global;

using joan::orig::helpers::div_global;
static void JOAN_CDECL init_a33648() { div_global(d::g_a33648, d::g_a3363c, d::flt_4); }  // 0x00420390 (/4)

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_a33648", 0x00420390, &init_a33648);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ActorList::delete_all() -> void { joan::bind::member<&::game_leaves81::H::nodes_delete_all>(this); }  // 0x41ba00
auto joan::orig::Unit::in_footprint_41c5e0(float a0, float a1, std::uint32_t a2, std::int32_t a3) -> char { return joan::bind::conv<char>(joan::bind::member<&::game_b108_08::H::in_footprint_41c5e0>(this, a0, a1, a2, a3)); }  // 0x41c5e0
auto joan::orig::Counter::at_least_10() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_03::H::at_least_10>(this)); }  // 0x41c7e0
auto joan::orig::Timer::remaining_41d280() -> float { return joan::bind::conv<float>(joan::bind::member<&::game_b90_03::H::remaining>(this)); }  // 0x41d280
auto joan::orig::Sector::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::sdd_41d3a0>(this, a0)); }  // 0x41d3a0
auto joan::orig::Tracks::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::sdd_41dfa0>(this, a0)); }  // 0x41dfa0
auto joan::orig::Battle::load_scenario_41dfe0(const char* a0, const char* a1) -> void { joan::bind::member<&::game_b109_13::H::load_scenario>(this, a0, a1); }  // 0x41dfe0
auto joan::orig::Battle::init() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_01::H::battle_init>(this)); }  // 0x41ea20
auto joan::orig::Battle::start_41ea60(void* a0) -> void { joan::bind::member<&::game_b109_14::H::battle_start>(this, a0); }  // 0x41ea60
auto joan::orig::Unit::detach_scene() -> void { joan::bind::member<&::game_leaves81_2::H::detach_scene>(this); }  // 0x41fcc0
auto joan::orig::Missile::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_01::H::clear_6c>(this)); }  // 0x420110
auto joan::orig::CameraZones::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b_2::H::sdd_420150>(this, a0)); }  // 0x420150
auto joan::orig::PfdData::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves81::H::sdd_4201a0>(this, a0)); }  // 0x4201a0
auto joan::orig::Scene::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves81::H::sdd_4201f0>(this, a0)); }  // 0x4201f0
auto joan::orig::Units::get_420240(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b_2::H::get_420240>(this, a0)); }  // 0x420240
auto joan::orig::Player::unit_id() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_18_2::H::get_00>(this)); }  // 0x4203c0
