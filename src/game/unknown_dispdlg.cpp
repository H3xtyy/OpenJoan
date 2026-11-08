// game/unknown_dispdlg.cpp: functions whose original source file is unknown (formerly game/_unplaced/dispdlg.cpp;
// re/attribution.csv: between ww/enlink/cpuusage.cpp and ww/ditemres.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Cleanup C1: the Direct3D adapter / display-mode dialog (select_adapter 0x00462f50, the game's only DialogBoxParamA,
// with no caller; its procedure 0x00463270 and helpers 0x00463190, 0x004631f0, 0x00463440, 0x00463520, 0x00463630,
// 0x00463780) is deleted.
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"

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

using joan::orig::helpers::at;

}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

struct H {
    // ------------------------------------------------------------------ 0x00463120
    H* remove_delete(i32 key) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(key)));  // 0x004af2d0
        if (p != nullptr) joan::orig::as_message(p)->destroy(1u);     // 0x004ae160 (scalar dtor)
        joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(key));  // 0x005858c0
        jo::view<u32>(jo::self<jo::MessageList>(this)->message_count) = jo::view<u32>(jo::self<jo::MessageList>(this)->message_count) - 1u;
        return this;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("List::remove_delete", 0x00463120, &H::remove_delete);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {

using i16 = std::int16_t;

// 0xc6 / 0xca on x86 = the end of the MapObject base (0xc2) + 4 / + 8; the two ints follow a 4-byte gap in the
// derived classes (Gate hp/max_hp, Wall u32_c6/u32_ca, Light enabled/u32_ca), so they move with sizeof(MapObject).
#if UINTPTR_MAX == 0xffffffffu
static_assert(sizeof(jo::MapObject) == 0xc2);
#endif
constexpr std::int32_t kC6 = static_cast<std::int32_t>(sizeof(jo::MapObject)) + 4;
constexpr std::int32_t kCa = static_cast<std::int32_t>(sizeof(jo::MapObject)) + 8;

struct H {
    template <class T>
    T& f(std::int32_t off) {
        return at<T>(this, off);
    }

    // ------------------------------------------------------------------ 0x004633e0
    void* set_c6_ca(const i16* p) {
        if (p != nullptr) {
            f<i32>(kC6) = 1;  // the i32 at +0xc6 on x86 (sizeof(MapObject) + 4: Gate / Wall / Order0b..., see kC6)
            f<i32>(kCa) = static_cast<i32>(*p);  // the i32 at +0xca
        } else {
            f<i32>(kC6) = 0;
            f<i32>(kCa) = 0;
        }
        return this;  // void in the original (eax = this / caller's eax; caller ignores it)
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Obj::set_c6_ca", 0x004633e0, &H::set_c6_ca);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::List::remove_delete_463120(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_01::H::remove_delete>(this, a0)); }  // 0x463120
auto joan::orig::MapObject::set_target_unit(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_02::H::set_c6_ca>(this, a0)); }  // 0x4633e0
