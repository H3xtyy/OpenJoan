// game/unknown_units_sel.cpp: functions whose original source file is unknown (formerly game/_unplaced/units_sel.cpp;
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

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u16 = std::uint16_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x0051dbe0
    void* ctor_51dbe0() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);  // 0x005856a0 (unit pointers: 4 bytes on x86)
        jo::self<jo::SelectionList>(this)->u16_2e = 0;
        memset_(jo::self<jo::SelectionList>(this)->bytes_30, 0, 8);
        memset_(jo::self<jo::SelectionList>(this)->bytes_38, 0, 8);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Obj51d::ctor", 0x0051dbe0, &H::ctor_51dbe0);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

struct H {
    // ------------------------------------------------------------------------ 0x0051dc50
    i32 dtor_51dc50() {
        joan::orig::as_units(this)->clear_all();         // 0x0051df00
        return thiscall<i32>(fn::dtor_DynArrayB, this);  // 0x00585710
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::~Obj_51dc50", 0x0051dc50, &H::dtor_51dc50);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;

JOAN_COUNTER(c_sel_init, "Selection::init");

struct H {
    // ------------------------------------------------------------------------ 0x0051dcc0
    u32 load_model(img::Addr name) {
        return joan::orig::arg::as<u32>(joan::orig::as_model_res(obj(d::g_model_res_mgr))->add(str(d::s_MODEL_9d3750), joan::orig::arg::str(str(name)), 0));  // 0x004b18d0
    }
    void show_model(i32 marker) {
        joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->set_params(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(jo::view<std::uint32_t>(jo::self<jo::UnitManager>(this)->select_models[marker])))), static_cast<std::int8_t>(joan::orig::arg::i32(0)), static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::i16(0));  // 0x004b0ec0
    }
    // this: the unit table g_units (UnitManager: a DynArrayB, then the counters at +0x2e on x86; the Selection facade
    // names them by their x86 offsets after a 0x2e-byte gap, which is 8 bytes short on x64)
    void selection_init() {
        c_sel_init.hit();
        for (i32 i = 1; i <= 4; ++i) {
            jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_30[i - 1]) = 1;
            jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_38[i - 1]) = 1;
        }
        jo::view<std::uint32_t>(jo::self<jo::UnitManager>(this)->select_models[0]) = load_model(d::s_select);  // "select"
        show_model(0);
        jo::view<std::uint32_t>(jo::self<jo::UnitManager>(this)->select_models[1]) = load_model(d::s_select2);  // "select2"
        show_model(1);
        jo::view<std::uint32_t>(jo::self<jo::UnitManager>(this)->select_models[2]) = load_model(d::s_select3);  // "select3"
        show_model(2);
        jo::view<std::uint32_t>(jo::self<jo::UnitManager>(this)->select_models[3]) = load_model(d::s_select4);  // "select4"
        show_model(3);
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[0]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[1]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[3]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[4]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[5]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[6]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[8]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[9]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[10]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_a[11]) = 1;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[0]) = 0x32;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[1]) = 0x32;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[3]) = 0xa;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[4]) = 0xa;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[5]) = 0xa;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[6]) = 0xa;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[8]) = 3;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[9]) = 3;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[10]) = 3;
        jo::view<std::uint16_t>(jo::self<jo::UnitManager>(this)->counts_b[11]) = 3;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("Selection::init", 0x0051dcc0, &H::selection_init);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
namespace {
namespace game_b84_02 {

JOAN_COUNTER(c_clear_51df00, "Mgr51df00::clear_all");

struct H {
    // 0x0051df00 clear all (0xa47c08).
    void clear_51df00() {
        c_clear_51df00.hit();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i >= 1; --i) {  // 0x00417ad0
            if (joan::orig::as_dynarray(this)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00428b70
            void* const p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3c0
            if (p != nullptr) joan::orig::as_unit(p)->destroy(1u);                       // 0x00471530
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));                    // 0x005858c0
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
    }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("Mgr51df00::clear_all", 0x0051df00, &H::clear_51df00);
#undef B84

}  // namespace game_b84_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Units::clear_all() -> void { joan::bind::member<&::game_b84_02::H::clear_51df00>(this); }  // 0x51df00
