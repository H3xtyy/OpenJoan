// game/unknown_widglist.cpp: functions whose original source file is unknown (formerly game/_unplaced/widglist.cpp;
// re/attribution.csv: between ww/en2d/image2d.cpp and ww/en2d/popupwin.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

struct Obj {};
using joan::orig::helpers::virtual_call;

JOAN_COUNTER(c_index_of, "List::index_of");
JOAN_COUNTER(c_call_all, "List::call_all");

struct H {
    H* ctor_62a110() {
        joan::orig::dynarray(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 5, 0);  // 0x005843d0 (pointers: 4 on x86)
        return this;
    }
    u32 call_584580() { joan::orig::as_dynarray(this)->zap_584580(1); return 0; }  // 0x00584580 (void; 0x0062a3a0 has no callers)

    // ------------------------------------------------------------------ 0x0062a3d0
    i32 index_of(const void* p) {  // p: an element (pointer)
        c_index_of.hit();
        for (i32 i = joan::orig::as_dynarray(this)->size(); i >= 1; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<const void*>(joan::orig::as_list(this)->get_62a430(joan::orig::arg::i32(i))) == p) return i;    // 0x0062a430
        }
        return 0;
    }
    // ------------------------------------------------------------------ 0x0062a460
    i32 call_all() {
        c_call_all.hit();
        const i32 n = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        for (i32 i = n; i >= 1; --i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->get_62a430(joan::orig::arg::i32(i)));  // 0x0062a430
            virtual_call<void>(e, 0);
        }
        return n < 1 ? n : 0;  // eax: the count when the loop never ran, else the final index 0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::ctor_62a110", 0x0062a110, &H::ctor_62a110);
B("Obj::call_584580", 0x0062a3a0, &H::call_584580);
B("List::index_of", 0x0062a3d0, &H::index_of);
B("List::call_all", 0x0062a460, &H::call_all);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

struct H {
    // ------------------------------------------------------------------ 0x0062a140
    u32 list_dtor() {
        joan::orig::dynarray(this)->zap(1);  // 0x00584580
        joan::orig::as_dynarray(this)->destruct_5844b0();  // 0x005844b0
        return 0;  // void callee; 0x0062a140 has no callers
    }

    // ------------------------------------------------------------------ 0x0062a1b0 / 0x0062a250
    u32 find_call2(std::uintptr_t fn, void* a, const void* key) {  // a, key: elements (pointers)
        i32 i = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        for (; i >= 1; --i) {
            if (joan::orig::arg::as<const void*>(joan::orig::as_list(this)->get_62a430(joan::orig::arg::i32(i))) == key)  // 0x0062a430
                return thiscall<u32>(fn, this, a, i);
        }
        return static_cast<u32>(i);
    }
    u32 find_call_62a1b0(void* a, const void* key) { return find_call2(fn::code_value(fn::Obj_call_5848d0_62a220), a, key); }  // -> 0x0062a220
    u32 find_call_62a250(void* a, const void* key) { return find_call2(fn::code_value(fn::List62a_insert_at), a, key); }  // -> 0x0062a2c0

    // ------------------------------------------------------------------ 0x0062a300
    u32 find_call_62a300(const void* key) {  // key: an element (pointer)
        i32 i = joan::orig::as_dynarray(this)->size();  // 0x00417ad0
        for (; i >= 1; --i) {
            if (joan::orig::arg::as<const void*>(joan::orig::as_list(this)->get_62a430(joan::orig::arg::i32(i))) == key)  // 0x0062a430
                return joan::orig::arg::as<u32>(joan::orig::as_widget_list(this)->link_out(joan::orig::arg::u32(i)));   // 0x0062a370
        }
        return static_cast<u32>(i);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("WidgetList::dtor", 0x0062a140, &H::list_dtor);
B("WidgetList::find_call_62a1b0", 0x0062a1b0, &H::find_call_62a1b0);
B("WidgetList::find_call_62a250", 0x0062a250, &H::find_call_62a250);
B("WidgetList::find_call_62a300", 0x0062a300, &H::find_call_62a300);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

struct H {
    // ------------------------------------------------------------------ 0x0062a220
    // a: the element (a pointer).
    u32 call_5848d0(void* a, u32 b) { joan::orig::as_dynarray(this)->insert_at_5848d0(joan::orig::arg::i32(b), &a); return 0; }  // 0x005848d0 (void; only find_call_62a1b0, uncalled)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::call_5848d0", 0x0062a220, &H::call_5848d0);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

struct H {
    // ------------------------------------------------------------------------ 0x0062a2c0
    // v: the element (a pointer).
    u32 insert_at(void* v, u32 i) { joan::orig::as_dynarray(this)->insert_at_5848d0(joan::orig::arg::i32(i + 1), &v); return 0; }  // 0x005848d0 (void; only find_call_62a250, uncalled)
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("List62a::insert_at", 0x0062a2c0, &H::insert_at);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

struct H {
    // ------------------------------------------------------------------ 0x0062a370
    u32 call_584a10(u32 a) { joan::orig::as_dynarray(this)->linkout_584a10(joan::orig::arg::i32(a)); return 0; }  // 0x00584a10 (void; only find_call_62a300, uncalled)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::call_584a10", 0x0062a370, &H::call_584a10);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b90_05.cpp (game/b90_05)
namespace {
namespace game_b90_05 {

struct H {
    // 0x0062a430
    void* list_get(std::int32_t i) { return joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)); }  // 0x0041b3f0
};

#define B9005(name, addr, fn) JOAN_REPLACE("game/b90_05", name, addr, fn)
B9005("List::get", 0x0062a430, &H::list_get);
#undef B9005

}  // namespace game_b90_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::WidgetList::link_out(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_07::H::call_584a10>(this, a0)); }  // 0x62a370
auto joan::orig::List::get_62a430(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_05::H::list_get>(this, a0)); }  // 0x62a430
