// game/unknown_water.cpp: functions whose original source file is unknown (formerly game/_unplaced/water.cpp;
// re/attribution.csv: between ww/uti3d/water.cpp and ww/world.cpp). They move to their original file when evidence
// names it (Phase 3).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::call::thiscall;

}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::orig::helpers::memset_;

struct H {
    std::uint8_t* self() { return reinterpret_cast<std::uint8_t*>(this); }

    // ---- ctors / leaves ------------------------------------------------------------------
    void* zero_34() { memset_(this, 0, sizeof(jo::Water)); return this; }  // 0x00546af0
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj546af0::ctor", 0x00546af0, &H::zero_34);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b86_02.cpp (game/b86_02)
namespace {
namespace game_b86_02 {

struct H {
    // ---- small wrappers ---------------------------------------------------------------------
    std::uint32_t water_clear() { joan::orig::as_water(this)->reset_sounds(); return 0; }    // 0x00546b20 -> 0x00546bc0
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_02", name, addr, fn)
B86("Water::clear", 0x00546b20, &H::water_clear);
#undef B86

}  // namespace game_b86_02
}  // namespace

// ---------------------------------------- from modules/game_b87_06.cpp (game/b87_06)
namespace {
namespace game_b87_06 {

struct H {
    // ------------------------------------------------------------ 0x00546bc0 (ret 0)
    void clear_2c_30() {
        jo::view<std::int32_t>(jo::self<jo::Water>(this)->u32_2c) = 0;
        jo::self<jo::Water>(this)->u32_30 = nullptr;
    }
};

#define B06(name, addr, fn) JOAN_REPLACE("game/b87_06", name, addr, fn)
B06("0x00546bc0 clear_2c_30", 0x00546bc0, &H::clear_2c_30);
#undef B06

}  // namespace game_b87_06
}  // namespace

// ---------------------------------------- from modules/game_b84_02.cpp (game/b84_02)
namespace {
namespace game_b84_02 {

using joan::img::obj;
using joan::img::str;

// 0x5a1170-family fixed string (no destructor); 0x194 bytes as in 0x00560120's frame.
struct alignas(4) String {
    std::uint8_t bytes[0x198];
};
const char* c_str(String& s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str()); }  // 0x00417d90

JOAN_COUNTER(c_add_sound, "Water::add_sound");
JOAN_COUNTER(c_clear_546d80, "Waters::clear_all");

struct H {
    // 0x00546bf0 Water::add_sound(name), ret 4.
    void add_sound(char* name) {
        c_add_sound.hit();
        String s;                       // ebp-0x198
        joan::orig::as_string(&s)->construct();  // 0x005a1170
        joan::orig::as_string(&s)->append(joan::orig::arg::str(name));  // 0x005a1550
        joan::orig::as_string(&s)->assign(joan::orig::arg::str(name));  // 0x005a1440
        joan::orig::as_string(&s)->append(str(d::s_TXT));         // ".TXT"
        const char* const p = c_str(s);
        const std::int32_t id = joan::orig::arg::as<std::int32_t>(joan::orig::as_name_list(obj(d::g_pemi_res))->find_4c2060(joan::orig::arg::str(p), static_cast<std::int8_t>(joan::orig::arg::i32(1))));  // 0x004c2060
        jo::view<std::int32_t>(jo::self<jo::Water>(this)->arr_24[jo::view<std::int32_t>(jo::self<jo::Water>(this)->u32_2c)]) = id;
        jo::view<std::int32_t>(jo::self<jo::Water>(this)->u32_2c) = jo::view<std::int32_t>(jo::self<jo::Water>(this)->u32_2c) + 1;
    }

    // 0x00546d80 Waters::clear_all (0xa527c8).
    void clear_546d80() {
        c_clear_546d80.hit();
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i >= 1; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->empty_at_42d8d0(joan::orig::arg::i32(i))) != 0) continue;           // 0x0042d8d0
            void* const p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x00425d90
            if (p != nullptr) joan::orig::as_water(p)->destroy(1u);                       // 0x00436780
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));                    // 0x005858c0
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
    }
};

#define B84(name, addr, fn) JOAN_REPLACE("game/b84_02", name, addr, fn)
B84("Waters::clear_all", 0x00546d80, &H::clear_546d80);
B84("Water::add_sound", 0x00546bf0, &H::add_sound);
#undef B84

}  // namespace game_b84_02
}  // namespace

// ---------------------------------------- from modules/game_b105_07.cpp (game/b105_07)
namespace {
namespace game_b105_07 {

struct H {
    void nop_546ca0(u32, u32, u32, u32) {}
    void nop_546d60() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_07", name, addr, fn)
B("Obj::nop_546ca0", 0x00546ca0, &H::nop_546ca0);
B("Obj::nop_546d60", 0x00546d60, &H::nop_546d60);
#undef B

}  // namespace game_b105_07
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

struct H {
    void* ctor_5856a0() {  // 0x00546cc0
        joan::orig::dynarray_b(this)->construct(static_cast<i32>(sizeof(void*)), i32{0x14}, i32{0}, i32{0});  // 0x005856a0 (Water* records: 4 on x86)
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("X::ctor_5856a0", 0x00546cc0, &H::ctor_5856a0);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

struct H {
    u32 dtor_546cf0() {
        joan::orig::as_waters(this)->clear_all();         // 0x00546d80
        return thiscall<u32>(fn::dtor_DynArrayB, this);  // 0x00585710 (eax left as the original does)
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("X546cf0::dtor", 0x00546cf0, &H::dtor_546cf0);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b102_06.cpp (game/b102_06)
namespace {
namespace game_b102_06 {

JOAN_COUNTER(c_remove_546ec0, "Container::remove_546ec0");

struct H {
    // 0x00546ec0
    u32 remove_546ec0(u32 id) {
        c_remove_546ec0.hit();
        void* p = joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(id));  // 0x00425d90
        if (p != nullptr) joan::orig::as_water(p)->destroy(1u);  // 0x00436780
        return thiscall<u32>(fn::linkout_5858c0, this, id);  // 0x005858c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_06", name, addr, fn)
B("Container::remove_546ec0", 0x00546ec0, &H::remove_546ec0);
#undef B

}  // namespace game_b102_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Water::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::zero_34>(this)); }  // 0x546af0
auto joan::orig::Water::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_02::H::water_clear>(this)); }  // 0x546b20
auto joan::orig::Water::reset_sounds() -> void { joan::bind::member<&::game_b87_06::H::clear_2c_30>(this); }  // 0x546bc0
auto joan::orig::Water::add_sound(const char* a0) -> void { joan::bind::member<&::game_b84_02::H::add_sound>(this, a0); }  // 0x546bf0
auto joan::orig::Waters::clear_all() -> void { joan::bind::member<&::game_b84_02::H::clear_546d80>(this); }  // 0x546d80
