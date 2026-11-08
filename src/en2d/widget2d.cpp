// en2d/widget2d.cpp: functions of the original d:\ww\en2d\widget2d.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
//
//    - Widget2d::release 0x00634b90 (ret 0): heap free of +0 ("widget2d.cpp").
namespace {
namespace game_b103_12 {

using joan::img::str;

void* const kHeap = obj(d::g_mem);

struct H {
    // eax as the original leaves it: this, or what free returns (uintptr_t: the whole register on x64)
    std::uintptr_t self() { return reinterpret_cast<std::uintptr_t>(this); }

    std::uintptr_t widget_release() {  // 0x00634b90
        if (jo::self<jo::PtrList>(this)->items == nullptr) return self();
        return thiscall<std::uintptr_t>(fn::del, kHeap, jo::view<void*>(jo::self<jo::PtrList>(this)->items), str(d::s_d_ww_en2d_widget2d_cpp),
                             static_cast<i32>(global<i16>(d::g_a197b8)) + 2);  // 0x0058ed70 "widget2d.cpp"
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Widget2d::release", 0x00634b90, &H::widget_release);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
//
//    - PtrList::add 0x00634be0 (p), ret 4 / PtrList::remove 0x00634cf0 (p), ret 4 /
//      PtrList::free 0x00634dd0 (ret 0).
namespace {
namespace game_b102_12 {

using u8 = std::uint8_t;
using u16 = std::uint16_t;

// Memory manager 0xa48838: free(ptr, file, line) 0x0058ed70.
void mm_free(void* p, const char* file, i32 line) {
    joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

JOAN_COUNTER(c_list_add, "PtrList::add");
JOAN_COUNTER(c_list_remove, "PtrList::remove");

struct H {
    // ------------------------------------------------------------------ 0x00634be0
    u32 list_add(const void* p) {  // p: the pointer stored (a PtrSlot)
        c_list_add.hit();
        if (static_cast<i32>(jo::self<jo::PtrList>(this)->count) + 1 > static_cast<i32>(jo::self<jo::PtrList>(this)->capacity)) {
            jo::view<u16>(jo::self<jo::PtrList>(this)->capacity) = static_cast<u16>(jo::view<u16>(jo::self<jo::PtrList>(this)->capacity) + 10);
            jo::view<void*>(jo::self<jo::PtrList>(this)->items) = joan::orig::as_mem(obj(d::g_mem))->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::PtrList>(this)->items)), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::PtrList>(this)->capacity) * static_cast<i32>(sizeof(jo::PtrSlot))), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_en2d_widget2d_cpp)), joan::orig::arg::i32(// 'd:\ww\en2d\widget2d.cpp'
                                          static_cast<i32>(global<i16>(d::g_a197bc)) + 5));  // 0x0058eb30
        }
        for (i16 i = 0; static_cast<i32>(i) < static_cast<i32>(jo::self<jo::PtrList>(this)->count); i = static_cast<i16>(i + 1)) {
            if (jo::self<jo::PtrList>(this)->items[i].p == p) return 0;
        }
        jo::self<jo::PtrList>(this)->items[jo::self<jo::PtrList>(this)->count].p = const_cast<void*>(p);
        jo::view<u16>(jo::self<jo::PtrList>(this)->count) = static_cast<u16>(jo::view<u16>(jo::self<jo::PtrList>(this)->count) + 1);
        jo::self<jo::PtrList>(this)->dirty = 0;
        return 1;
    }

    // ------------------------------------------------------------------ 0x00634cf0
    u32 list_remove(const void* p) {
        c_list_remove.hit();
        for (i16 i = 0; static_cast<i32>(i) < static_cast<i32>(jo::self<jo::PtrList>(this)->count); i = static_cast<i16>(i + 1)) {
            if (jo::self<jo::PtrList>(this)->items[i].p != p) continue;
            if (static_cast<i32>(i) < static_cast<i32>(jo::self<jo::PtrList>(this)->count) - 1 && static_cast<i32>(jo::self<jo::PtrList>(this)->count) >= 2) {
                jo::self<jo::PtrList>(this)->items[i].p = jo::self<jo::PtrList>(this)->items[static_cast<i32>(jo::self<jo::PtrList>(this)->count) - 1].p;
                jo::self<jo::PtrList>(this)->dirty = 0;
            }
            jo::view<u16>(jo::self<jo::PtrList>(this)->count) = static_cast<u16>(jo::view<u16>(jo::self<jo::PtrList>(this)->count) - 1);
            return 1;
        }
        return 0;
    }

    // ------------------------------------------------------------------ 0x00634dd0
    H* list_free() {
        if (jo::self<jo::PtrList>(this)->items != nullptr) {
            mm_free(jo::view<void*>(jo::self<jo::PtrList>(this)->items), &img::global<const char>(d::s_d_ww_en2d_widget2d_cpp),
                    static_cast<i32>(global<i16>(d::g_a197c0)) + 3);
            jo::self<jo::PtrList>(this)->items = nullptr;
            jo::view<u16>(jo::self<jo::PtrList>(this)->capacity) = 0;
            jo::view<u16>(jo::self<jo::PtrList>(this)->count) = 0;
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("PtrList::add", 0x00634be0, &H::list_add);
B("PtrList::remove", 0x00634cf0, &H::list_remove);
B("PtrList::free", 0x00634dd0, &H::list_free);
#undef B

}  // namespace game_b102_12
}  // namespace
