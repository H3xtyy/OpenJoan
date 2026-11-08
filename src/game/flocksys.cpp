// game/flocksys.cpp: functions of the original d:\ww\flocksys.cpp (re/attribution.csv).
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
#include "../runtime/replay/replay.hpp"
#include <cstdarg>
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

void* const kHeap = obj(d::g_mem);

using u16 = std::uint16_t;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
//
//      0x0046dfa0 Flock::free            (thiscall, ret 0)
namespace {
namespace game_b94_01 {

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_flock_free, "Flock::free");

struct H {
    // 0x0046dfa0
    void flock_free() {
        c_flock_free.hit();
        if (jo::view<void*>(jo::self<jo::Flock>(this)->groups) != nullptr) {
            for (i32 i = static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_flock(this)->member_count())); i > 0; --i) {  // 0x004231c0
                void* p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_46e590(joan::orig::arg::i16(static_cast<i16>(i))));  // 0x0046e590
                if (p != nullptr) {
                    joan::orig::as_flock_group(p)->clear();  // 0x0046dbe0
                    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), str(d::s_d_ww_flocksys_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fe7cc)) + 9));  // 0x0058ed70
                }
                joan::orig::dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->groups))->linkout(joan::orig::arg::i32(i));  // 0x00584a10
            }
            void* const l = jo::view<void*>(jo::self<jo::Flock>(this)->groups);
            if (l != nullptr) (void)joan::orig::arg::as<void*>(joan::orig::as_dynarray(l)->destroy(1u));  // 0x00463fc0
            jo::view<void*>(jo::self<jo::Flock>(this)->groups) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Flock>(this)->members) != nullptr) {
            for (i32 i = static_cast<i16>(joan::orig::arg::as<i32>(joan::orig::as_unit(this)->list53_count())); i > 0; --i) {  // 0x0046efc0
                void* p = joan::orig::arg::as<void*>(joan::orig::as_unit(this)->list53_item(joan::orig::arg::i32(static_cast<i16>(i))));  // 0x0046e5d0
                if (p != nullptr) {
                    joan::orig::as_boid(p)->release_actor();  // 0x004207d0
                    joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), str(d::s_d_ww_flocksys_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fe7cc)) + 0x1a));  // 0x0058ed70
                }
                joan::orig::dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->members))->linkout(joan::orig::arg::i32(i));  // 0x00584a10
            }
            void* const l = jo::view<void*>(jo::self<jo::Flock>(this)->members);
            if (l != nullptr) (void)joan::orig::arg::as<void*>(joan::orig::as_dynarray(l)->destroy(1u));  // 0x00463fc0
            jo::view<void*>(jo::self<jo::Flock>(this)->members) = nullptr;
        }
        if (static_cast<i32>(jo::self<jo::Flock>(this)->kind) == 1 && jo::view<void*>(jo::self<jo::Flock>(this)->group_node) != nullptr && jo::view<void*>(jo::self<jo::Flock>(this)->scene) != nullptr) {
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::Flock>(this)->scene))->remove_group(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Flock>(this)->group_node)));  // 0x005ea5e0
            jo::view<void*>(jo::self<jo::Flock>(this)->group_node) = nullptr;
        }
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Flock::free", 0x0046dfa0, &H::flock_free);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b102_02.cpp (game/b102_02)
//
//      0x0046e1d0 Flock::set_active(byte)          (thiscall, ret 4) - (dead code).
//
//      0x0046e430 Flock::remove_group(word)        (thiscall, ret 4) - (dead code).
//
//      0x0046ec30 FlockList::~FlockList            (thiscall, ret 0) - 0x0046ecc0 + 0x00585710.
//
//      0x0046eed0 Flock::delete_member(word)       (thiscall, ret 4) - (dead code).
//
//  - SEH frames in 0x0046ec30 / 0x0047c3d0 / 0x004804f0 / 0x00481cd0 only guard destructors or
//    new + ctor: normal path reproduced.
namespace {
namespace game_b102_02 {

using joan::orig::helpers::addr;

JOAN_COUNTER(c_flock_active, "Flock::set_active");
JOAN_COUNTER(c_flock_remove, "Flock::remove_group");
JOAN_COUNTER(c_flock_delete, "Flock::delete_member");

void heap_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

const char* const kFlockSys = str(d::s_d_ww_flocksys_cpp);  // "d:\ww\flocksys.cpp"

struct H {
    // 0x0046e1d0
    std::uintptr_t flock_set_active(u32 on) {
        c_flock_active.hit();
        jo::self<jo::Flock>(this)->active = static_cast<u8>(on);
        if (jo::self<jo::Flock>(this)->active == 1) {
            for (i32 i = 1;; ++i) {
                const u32 n = joan::orig::arg::as<u32>(joan::orig::as_unit(this)->list53_count());  // 0x0046efc0
                if (i > static_cast<i32>(static_cast<i16>(n))) return n;
                u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_unit(this)->list53_item(static_cast<i32>(static_cast<i16>(i))));  // 0x0046e5d0
                void* const g = jo::view<void*>(jo::self<jo::Flock>(this)->group_node);
                if (g != nullptr && jo::view<void*>(jo::as_boid(e)->actor) != nullptr) joan::orig::as_scene(g)->attach(joan::orig::arg::mptr(jo::view<void*>(jo::as_boid(e)->actor)));  // 0x005efc50
            }
        }
        if (jo::self<jo::Flock>(this)->active != 0) return addr(this);
        for (i32 i = 1;; ++i) {
            const i32 n = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(this)->list53_count()));
            if (i > n) return static_cast<u32>(n);
            joan::orig::as_unit(this)->list53_item(static_cast<i32>(static_cast<i16>(i)));
        }
    }

    // 0x0046e430
    std::uintptr_t flock_remove_group(u32 arg) {
        c_flock_remove.hit();
        const i16 w = static_cast<i16>(arg);
        jo::self<jo::Flock>(this)->spawned_groups = static_cast<i16>(jo::self<jo::Flock>(this)->spawned_groups - 1);
        if (static_cast<i32>(jo::self<jo::Flock>(this)->spawned_groups) >= 5) return addr(this);
        u8* grp = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_46e590(joan::orig::arg::i16(static_cast<i32>(w))));  // 0x0046e590
        const i16 key = reinterpret_cast<const i16&>(jo::as_chain(grp)->index);
        for (i32 i = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_unit(this)->list53_count())); i >= 1; --i) {  // 0x0046efc0
            u8* m = joan::orig::arg::as<u8*>(joan::orig::as_unit(this)->list53_item(static_cast<i32>(static_cast<i16>(i))));  // 0x0046e5d0
            if (static_cast<i32>(jo::view<i16>(jo::as_boid(m)->u16_a)) != static_cast<i32>(key)) continue;
            joan::orig::as_flock_group(grp)->remove(joan::orig::arg::mptr(m));  // 0x0046dd70
            joan::orig::as_boid(m)->release_actor();        // 0x004207d0
            heap_free(m, kFlockSys, static_cast<i32>(global<i16>(d::g_9fe7d4)) + 0x14);
            joan::orig::dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->members))->linkout(joan::orig::arg::i32(i));  // 0x00584a10
        }
        joan::orig::as_flock_group(grp)->clear();  // 0x0046dbe0
        heap_free(grp, kFlockSys, static_cast<i32>(global<i16>(d::g_9fe7d4)) + 0x1c);
        joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->groups))->linkout_584a10(joan::orig::arg::i32(static_cast<i32>(key)));  // 0x00584a10
        return 0;  // no callers (dead code)
    }

    // 0x0046ec30
    u32 flock_list_dtor() {
        joan::orig::as_flock_list(this)->clear();              // 0x0046ecc0
        return thiscall<u32>(fn::dtor_DynArrayB, this);  // 0x00585710
    }

    // 0x0046eed0
    u32 flock_delete_member(u32 arg) {
        c_flock_delete.hit();
        const i16 w = static_cast<i16>(arg);
        void* const m = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_word(joan::orig::arg::i16(static_cast<i32>(w))));  // 0x004231f0
        joan::orig::as_flock(m)->free();                                                  // 0x0046dfa0
        joan::orig::as_flock(m)->destroy(0u);                                               // 0x0046ed90
        heap_free(m, kFlockSys, static_cast<i32>(global<i16>(d::g_9fe7e0)) + 6);
        return thiscall<u32>(fn::linkout_5858c0, this, static_cast<i32>(w));  // 0x005858c0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_02", name, addr, fn)
B("Flock::set_active", 0x0046e1d0, &H::flock_set_active);
B("Flock::remove_group", 0x0046e430, &H::flock_remove_group);
B("FlockList::~FlockList", 0x0046ec30, &H::flock_list_dtor);
B("Flock::delete_member", 0x0046eed0, &H::flock_delete_member);
#undef B

}  // namespace game_b102_02
}  // namespace

// ---------------------------------------- from modules/game_b106_17.cpp (game/b106_17)
//
//    - Flock::spawn_46e2d0 0x0046e2d0 (flocksys.cpp, thiscall, ret 4, void - the caller
//
//  - 0x0046e2d0: the short arguments of 0x0046dba0 / 0x00420420 are read with `mov ax, word`.
namespace {
namespace game_b106_17 {

JOAN_COUNTER(c_spawn, "Flock::spawn_46e2d0");

struct H {
    // 0x0046e2d0
    void spawn_46e2d0(i32 n) {
        c_spawn.hit();
        jo::view<u16>(jo::self<jo::Flock>(this)->spawned_groups) = static_cast<u16>(jo::view<u16>(jo::self<jo::Flock>(this)->spawned_groups) + 1);
        if (static_cast<i32>(jo::self<jo::Flock>(this)->spawned_groups) > 5) return;
        const char* const file = str(d::s_d_ww_flocksys_cpp);  // "d:\ww\flocksys.cpp"
        u8* group = joan::orig::arg::as<u8*>(joan::orig::as_mem(kHeap)->add_clear(joan::orig::arg::u32(static_cast<u32>(sizeof(jo::Chain))), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fe7d0)) + 5)));  // 0x0058e8d0
        joan::orig::as_flock_group(group)->construct();                       // 0x0046db60
        joan::orig::as_flock_group(group)->init(joan::orig::arg::i32(jo::self<jo::Flock>(this)->id));             // 0x0046dba0
        joan::orig::dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->groups))->linkin(&group);           // 0x00584620
        const u32 gi = joan::orig::arg::as<u32>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->groups))->cursor());  // 0x00417520
        jo::as_chain(group)->index = gi;
        for (i32 i = 1; i <= n; ++i) {
            u8* member = joan::orig::arg::as<u8*>(joan::orig::as_mem(kHeap)->add_clear(joan::orig::arg::u32(static_cast<u32>(sizeof(jo::Boid))), joan::orig::arg::str(file), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fe7d0)) + 0xf)));  // 0x0058e8d0
            joan::orig::as_boid(member)->construct();                                // 0x004203e0
            joan::orig::as_boid(member)->init(joan::orig::arg::i32(jo::self<jo::Flock>(this)->id), joan::orig::arg::i32(reinterpret_cast<const i16&>(jo::as_chain(group)->index)));  // 0x00420420
            joan::orig::dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->members))->linkin(&member);                  // 0x00584620
            const u32 mi = joan::orig::arg::as<u32>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->members))->cursor());  // 0x00417520
            jo::as_boid(member)->member_index = static_cast<u16>(mi);
            joan::orig::as_flock_group(group)->append(joan::orig::arg::mptr(member));  // 0x0046dc80
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_17", name, addr, fn)
B("Flock::spawn_46e2d0", 0x0046e2d0, &H::spawn_46e2d0);
#undef B

}  // namespace game_b106_17
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
//
//      0x0046e5d0 Unit::list53_item(short i) -> 0x0041b3f0(+0x53, i); ret 4
namespace {
namespace game_b92_02 {

struct H {
    // 0x0046e5d0
    void* list53_item(std::int32_t i) {
        return joan::orig::arg::as<void*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->members))->ptr_at(static_cast<std::int32_t>(static_cast<std::int16_t>(i))));  // 0x0041b3f0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Unit::list53_item", 0x0046e5d0, &H::list53_item);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
//
//    - Flock::update_members 0x0046e610 (ret 0, dead code - no references).
namespace {
namespace game_b108_07 {

JOAN_COUNTER(c_flock, "Flock::update_members");

struct H {
    // ------------------------------------------------------------------ 0x0046e610
    u32 update_members() {
        c_flock.hit();
        if (jo::self<jo::Flock>(this)->active == 0) return joan::call::register_low_bits(this);
        joan::orig::as_flock(this)->update_stub();  // 0x0046e690
        for (i32 i = 1;; ++i) {
            const i32 count = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_flock(this)->member_count()));  // 0x004231c0
            if (i > count) return static_cast<u32>(count);
            void* const member = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_46e590(joan::orig::arg::i16(i)));  // 0x0046e590 (word arg)
            joan::orig::as_flock_group(member)->update_members();                           // 0x0046dc20
        }
    }
};

JOAN_REPLACE("game/b108_07", "Flock::update_members", 0x0046e610, &H::update_members);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b105_03.cpp (game/b105_03)
//
//    - empty thiscall stubs (never write eax; the C++ versions leave eax untouched as well):
//        ret 0:    0x0046db80, 0x0046e690, 0x0046eca0, 0x0047b510, 0x0047b530, 0x0047b550,
//                  0x0047b570, 0x0047b590, 0x0047b780, 0x0047b7f0, 0x0047b810, 0x0047b920,
//                  0x0047b940, 0x0047b980, 0x0047b9a0, 0x0047b9c0, 0x0047ba00, 0x0047ba20,
//                  0x0047ba40, 0x0047ba60, 0x0047baa0, 0x0047bac0, 0x0047bae0, 0x0047bb20,
//                  0x0047bb40, 0x0047bb60, 0x0047bba0, 0x0047f560, 0x0047f5c0,
//                  0x004802d0 (vtable 0x9d07dc), 0x00480670 (vtable 0x9d0954),
//                  0x00480be0 (vtable 0x9d0854)
//        ret 8:    0x0046d800 (vtable slots 0x9d0794 / 0x9d080c / 0x9d090c / 0x9d0994 / 0x9d0a94)
//        ret 0x10: 0x0046ebe0
//      Direct callers: 0x0046e321, 0x0046e62d, 0x0047f4f8 and the state-switch functions
//      0x00499540 / 0x0052f930 (enter / leave handlers per state; 0x00499540 passes the enter
//      stub's eax out through its own `ret 4`, but all four of its callers overwrite eax).
//      Virtual call sites: the only thiscall 2-arg `call [reg+0x14]` in game code is 0x0044f67c
//      (eax reloaded right after); every thiscall `call [reg+0x5c]` (0x004802a4, 0x00480394,
//      0x00480601, 0x00480b13) is followed by `mov eax,1`. No vtable outside those listed
//      overrides either slot.
//      0x0046db80, 0x0046eca0, 0x0046ebe0, 0x0047b510..0x0047b590, 0x0047b780, 0x0047b7f0 and
namespace {
namespace game_b105_03 {

struct H {
    // ---------------------------------------------------------------- empty stubs
    void stub0() {}
    void stub10(u32, u32, u32, u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_03", name, addr, fn)
B("Obj::stub_46e690", 0x0046e690, &H::stub0);
B("Obj::stub_46ebe0", 0x0046ebe0, &H::stub10);
B("Obj::stub_46eca0", 0x0046eca0, &H::stub0);
#undef B

}  // namespace game_b105_03
}  // namespace

// ---------------------------------------- from modules/game_b111_14.cpp (game/b111_14)
//
//    - 0x0046e6b0 Flock::parse(reader) (thiscall, ret 4; dead code: no reference). The 0x400-byte
//      line buffer [ebp-0x404] is directly followed by the saved `this` [ebp-4], which the
//      original reloads at every use. read_line (0x00589f10) may write its NUL at buf[0x400],
//      i.e. into the low byte of the saved `this`: reproduced with one 0x404-byte frame block
//      (buffer + self) that is read back with memcpy at every use. eax on exit: the nonzero
//      result of 0x0046f060, or 0 (null token / empty token).
namespace {
namespace game_b111_14 {

using joan::orig::helpers::addr_of;

JOAN_COUNTER(c_flock, "Flock::parse_46e6b0");

// ---------------------------------------------------------------- 0x0046e6b0
// Original frame: [ebp-0x404] char buf[0x400], [ebp-4] this.
constexpr u32 kBufLen = 0x400;
struct FlockFrame {
    u8 raw[kBufLen + 4];  // buf, then the saved `this` (overwritable by read_line's NUL)
};
static_assert(sizeof(FlockFrame) == 0x404, "FlockFrame");
// x64: the 4-byte slot keeps the low half of `this`; hi is its upper half (0 on x86).
u8* frame_self(const FlockFrame& fr, std::uintptr_t hi) {
    u32 v;
    std::memcpy(&v, fr.raw + kBufLen, 4);
    return reinterpret_cast<u8*>(static_cast<std::uintptr_t>(v) | hi);
}

struct H {
    // 0x0046e6b0
    std::uintptr_t flock_parse(void* reader) {
        c_flock.hit();
        FlockFrame fr;
        const std::uintptr_t self = addr_of(this);
        std::memcpy(fr.raw + kBufLen, &self, 4);
        const std::uintptr_t hi_self = reinterpret_cast<std::uintptr_t>(this) & ~static_cast<std::uintptr_t>(0xffffffffu);
        fr.raw[0] = global<u8>(d::kEmpty);  // ""
        std::memset(fr.raw + 1, 0, kBufLen - 1);
        char* const buf = reinterpret_cast<char*>(fr.raw);
        for (;;) {
            const u32 r = joan::orig::arg::as<u32>(joan::orig::as_file_txt(reader)->at_eof());  // 0x0046f060
            if (r != 0) return r;
            char* const tok = joan::orig::as_file_txt(reader)->get_token(i32{1});  // 0x00589c70
            if (tok == nullptr) return 0;
            if (joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(tok))) == 0) return 0;  // strlen
            if (joan::orig::arg::as<i32>(joan::orig::libc::strncmp(joan::orig::arg::str(tok), str(d::s_slash_slash), joan::orig::arg::u32(u32{2}))) == 0 ||  // strncmp "//"
                joan::orig::arg::as<i32>(joan::orig::libc::strncmp(joan::orig::arg::str(tok), str(d::s_hash), joan::orig::arg::u32(u32{1}))) == 0) {  // "#"
                joan::orig::as_file_txt(reader)->next_line();  // 0x005899b0 skip line
                continue;
            }
            if (joan::crt::stricmp(joan::orig::arg::str(tok), str(d::s_TYPE)) == 0) {  // "TYPE"
                joan::orig::as_file_txt(reader)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(kBufLen));  // 0x00589f10 read_line
                char* const p = buf;
                if (joan::crt::stricmp(joan::orig::arg::str(p), str(d::s_BUTTERFLY)) == 0) {  // "BUTTERFLY"
                    jo::view<u16>(jo::as_flock(frame_self(fr, hi_self))->kind) = 0;
                } else if (joan::crt::stricmp(joan::orig::arg::str(p), str(d::s_BIRD)) == 0) {  // "BIRD"
                    jo::view<u16>(jo::as_flock(frame_self(fr, hi_self))->kind) = 1;
                }
                joan::orig::as_flock(frame_self(fr, hi_self))->load_steering(joan::orig::arg::str(p));  // 0x0046e950
            } else if (joan::crt::stricmp(joan::orig::arg::str(tok), str(d::s_WIDTH)) == 0) {  // "WIDTH"
                const i32 v = joan::crt::atoi(joan::orig::arg::str(joan::orig::as_file_txt(reader)->get_token(i32{1})));  // atoi
                jo::view<u16>(jo::as_flock(frame_self(fr, hi_self))->width) = static_cast<u16>(static_cast<u32>(v));
            } else if (joan::crt::stricmp(joan::orig::arg::str(tok), str(d::s_HEIGHT)) == 0) {  // "HEIGHT"
                const i32 v = joan::crt::atoi(joan::orig::arg::str(joan::orig::as_file_txt(reader)->get_token(i32{1})));
                jo::view<u16>(jo::as_flock(frame_self(fr, hi_self))->height) = static_cast<u16>(static_cast<u32>(v));
            } else if (joan::crt::stricmp(joan::orig::arg::str(tok), str(d::s_NUMFLOCK)) == 0) {  // "NUMFLOCK"
                const i32 v = joan::crt::atoi(joan::orig::arg::str(joan::orig::as_file_txt(reader)->get_token(i32{1})));
                jo::view<u16>(jo::as_flock(frame_self(fr, hi_self))->num_flock) = static_cast<u16>(static_cast<u32>(v));
            } else if (joan::crt::stricmp(joan::orig::arg::str(tok), str(d::s_NUMENTITY)) == 0) {  // "NUMENTITY"
                const i32 v = joan::crt::atoi(joan::orig::arg::str(joan::orig::as_file_txt(reader)->get_token(i32{1})));
                jo::view<u16>(jo::as_flock(frame_self(fr, hi_self))->num_entity) = static_cast<u16>(static_cast<u32>(v));
            }
        }
    }
};

JOAN_REPLACE("game/b111_14", "Flock::parse", 0x0046e6b0, &H::flock_parse);

}  // namespace game_b111_14
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
//
//    - FlockSys::flush_46e8d0 0x0046e8d0 (ret 0, callers drop eax).
namespace {
namespace game_b108_01 {

using i8 = std::int8_t;

JOAN_COUNTER(c_flock, "FlockSys::flush_46e8d0");

struct H {
    // -------------------------------------------------------------- 0x0046e8d0
    void flock_flush() {
        c_flock.hit();
        if (jo::self<jo::MessageList>(this)->paused != 0) return;
        for (i32 n = joan::orig::as_dynarray(this)->size(); n > 0; --n) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(n))) != 0) continue;   // 0x004af300
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(n)));         // 0x004af2d0
            if (jo::as_message(e)->state == 6) {
                joan::orig::as_list(this)->remove_delete_463120(joan::orig::arg::i32(jo::as_message(e)->list_index));  // 0x00463120
            } else {
                joan::orig::as_message(e)->update_slide();  // 0x004ace90
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("FlockSys::flush_46e8d0", 0x0046e8d0, &H::flock_flush);
#undef B

}  // namespace game_b108_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
//
//  * module game/b98_01:
//      0x00416600 / 0x004192d0 / 0x0045e930 / 0x0046ed90  Obj::scalar_dtor(flags)  (thiscall, ret 4)
//
//      0x0046e950 Steer::load(const char* name)            (thiscall, ret 4)
//
//  - 0x0046e950: the 16-byte name buffer is zeroed (original memsets 9 bytes; strncpy of at most
//    8 chars keeps it terminated, the rest is never read).
namespace {
namespace game_b98_01 {

void* const kSteer = obj(d::g_steer);

JOAN_COUNTER(c_steer_load, "Steer::load");

using joan::orig::helpers::strlen_;
using joan::orig::helpers::memset_;
using joan::orig::helpers::delete_;       // operator delete

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    H* sdtor(std::uintptr_t dtor, u32 flags) {
        thiscall(dtor, this);
        if ((flags & 1) != 0) delete_(this);
        return this;
    }
    H* sdtor_46ed90(u32 flags) { return sdtor(fn::code_value(fn::Obj_forward_46dfa0), flags); }  // 0x0046de60

    // ------------------------------------------------------------------ 0x0046e950
    char* steer_value(char* name, img::Addr key) {
        // The original pushes key before calling Ini::section(name) (ret 4), so it is Ini::value's argument.
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_ini(kSteer)->section(joan::orig::arg::str(name)));  // 0x0047cbf0
        return joan::orig::arg::as<char*>(joan::orig::as_ini(e)->value(joan::orig::arg::str(str(key)))) + 9;  // 0x0047c180
    }
    float steer_float(char* name, img::Addr key) {
        return joan::orig::arg::as<float>(joan::orig::libc::atof(joan::orig::arg::str(static_cast<const char*>(steer_value(name, key)))));  // atof
    }
    void steer_load(const char* s) {
        c_steer_load.hit();
        char name[16] = {};  // ebp-0x10
        memset_(name, 0, 9);
        const i32 n = joan::orig::arg::as<i32>(joan::orig::min_int(static_cast<i32>(strlen_(s)), 8));  // 0x0046f020 min
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(name))), joan::orig::arg::str(s), joan::orig::arg::u32(n));  // strncpy
        jo::self<jo::Flock>(this)->max_speed = steer_float(name, d::s_MAXSPEED);   // "MAXSPEED"
        jo::self<jo::Flock>(this)->max_urgency = steer_float(name, d::s_MAXURGEN);  // "MAXURGEN"
        jo::self<jo::Flock>(this)->max_urgency2 = steer_float(name, d::s_MAXURGEN);  // "MAXURGEN"
        jo::self<jo::Flock>(this)->keep_away = steer_float(name, d::s_KEEPAWAY);  // "KEEPAWAY"
        jo::self<jo::Flock>(this)->separate = steer_float(name, d::s_SEPARATE);  // "SEPARATE"
        jo::self<jo::Flock>(this)->def_range = steer_float(name, d::s_DEFRANGE);  // "DEFRANGE"
        const char* const t = steer_value(name, d::s_USETRUTH);  // "USETRUTH"
        jo::self<jo::Flock>(this)->use_truth = (*t == 0x6e || *t == 0x4e) ? 0 : 1;
        const char* const r = steer_value(name, d::s_REACTENE);  // "REACTENE"
        jo::self<jo::Flock>(this)->react_enemy = (*r == 0x6e || *r == 0x4e) ? 0 : 1;
        jo::self<jo::Flock>(this)->speed_urgency = jo::self<jo::Flock>(this)->max_speed * jo::self<jo::Flock>(this)->max_urgency2;
        jo::self<jo::Flock>(this)->half_speed = jo::self<jo::Flock>(this)->max_speed / global<float>(d::flt_2);  // / 2.0f
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Steer::load", 0x0046e950, &H::steer_load);
B98("Obj::scalar_dtor_46ed90", 0x0046ed90, &H::sdtor_46ed90);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b96_02.cpp (game/b96_02)
//
//      0x0046ec00 ObjA::ctor                   (thiscall, ret 0)    - 0x005856a0(this, 4, 5, 0, 0).
namespace {
namespace game_b96_02 {

struct H {
    // ------------------------------------------------------------------------ 0x0046ec00
    void* ctor_a() {
        joan::orig::dynarray_b(this)->construct(static_cast<i32>(sizeof(void*)), 5, 0, 0);  // 0x005856a0 (Flock* records: 4 on x86)
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_02", name, addr, fn)
B96("ObjA::ctor", 0x0046ec00, &H::ctor_a);
#undef B96

}  // namespace game_b96_02
}  // namespace

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
//
//      0x0046ecc0 FlockList::clear                  (thiscall, ret 0)
namespace {
namespace game_b101_02 {

JOAN_COUNTER(c_flock_clear, "FlockList::clear");

i32 list_size(void* l) { return joan::orig::as_dynarray(l)->size(); }  // 0x00417ad0
void heap_free(void* p, const char* file, i32 line) {
    joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct H {
    // ------------------------------------------------------------------ 0x0046ecc0
    u32 flock_clear() {
        c_flock_clear.hit();
        for (i16 i = static_cast<i16>(list_size(this)); static_cast<i32>(i) >= 1; i = static_cast<i16>(i - 1)) {
            if (joan::orig::arg::as<i32>(joan::orig::as_flock_list(this)->is_empty_at(static_cast<i32>(i))) != 0) continue;  // 0x0046eff0
            void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_word(joan::orig::arg::i16(static_cast<i32>(i))));   // 0x004231f0
            joan::orig::as_flock(p)->free();                                                  // 0x0046dfa0
            joan::orig::as_flock(p)->destroy(0u);                                               // 0x0046ed90
            heap_free(p, str(d::s_d_ww_flocksys_cpp), static_cast<i32>(global<i16>(d::g_9fe7d8)) + 0xb);
            joan::orig::dynarray_b(this)->linkout(static_cast<i32>(i));  // 0x005858c0
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        return 0;  // callers discard the result
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("FlockList::clear", 0x0046ecc0, &H::flock_clear);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_b107_09.cpp (game/b107_09)
//
//    - 0x0046ede0 FlockSys::create (8 stack args: 5 words, 3 dwords), ret 0x20: pool alloc
//
//  - 0x0046ede0 pushes its word arguments with garbage upper halves; 0x0046de90 only reads
//    its last (dword) argument. 0x005857a0 copies [this+0x10] bytes from &obj (a pointer
//    slot, 4 bytes expected) - dead code anyway.
namespace {
namespace game_b107_09 {

JOAN_COUNTER(c_flock_create, "FlockSys::create");

struct H {
    // 0x0046ede0
    u32 flock_create(i16 a, i16 b, i16 c, i16 d, i16 e, u32 f, u32 g, jo::Scene* h) {
        c_flock_create.hit();
        void* o = joan::orig::as_mem(obj(d::g_mem))->add(static_cast<u32>(sizeof(jo::Flock)), str(d::s_d_ww_flocksys_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<i16>(d::g_9fe7dc)) + 1));  // 0x0058e7b0
        joan::orig::as_flock(o)->construct();                                                         // 0x0046de30
        joan::orig::as_flock(o)->init(joan::orig::arg::u32(a), joan::orig::arg::u32(b), joan::orig::arg::u32(c), joan::orig::arg::u32(d), joan::orig::arg::u32(e), joan::orig::arg::u32(f), joan::orig::arg::u32(g), h);                                // 0x0046de90
        joan::orig::dynarray_b(this)->linkin(&o);                                                    // 0x005857a0
        jo::view<u16>(jo::as_flock(o)->id) = joan::orig::arg::as<u16>(joan::orig::as_dynarray(this)->cursor());             // 0x00417520
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::as_flock(o)->num_flock); ++i) {
            joan::orig::as_flock(o)->spawn_group(static_cast<std::int32_t>(jo::as_flock(o)->num_entity));        // 0x0046e2d0
        }
        // eax = the object pointer with ax replaced by the id; no caller reads the high half (low half on x64)
        return (joan::call::register_low_bits(o) & 0xffff0000u) | jo::view<u16>(jo::as_flock(o)->id);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_09", name, addr, fn)
B("FlockSys::create", 0x0046ede0, &H::flock_create);
#undef B

}  // namespace game_b107_09
}  // namespace

// ---------------------------------------- from modules/game_b105_03.cpp (game/b105_03)
//
//    - Obj::self_46db60 0x0046db60 (ret 0) -> this.
namespace {
namespace game_b105_03_2 {

struct H {
    // ---------------------------------------------------------------- getters
    H* self_46db60() { return this; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_03", name, addr, fn)
// getters
B("Obj::self_46db60", 0x0046db60, &H::self_46db60);
#undef B

}  // namespace game_b105_03_2
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
//
//      0x0046dba0 Obj::init_46dba0(id)          (thiscall, ret 4)
//
//      0x0046dd70 Chain::remove(node)           (thiscall, ret 4)
namespace {
namespace game_b99_02 {

using joan::call::thiscall;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::u;

JOAN_COUNTER(c_chain_remove, "Chain::remove");

struct H {
    // ------------------------------------------------------------------ 0x0046dba0
    u32 init_46dba0(i32 id) {
        jo::view<u32>(jo::self<jo::Chain>(this)->count) = 0;
        jo::self<jo::Chain>(this)->head = nullptr;
        jo::self<jo::Chain>(this)->flock_id = static_cast<i16>(id);
        return (u(this) & 0xffff0000u) | (static_cast<u32>(id) & 0xffffu);
    }

    // ------------------------------------------------------------------ 0x0046dd70
    std::uintptr_t chain_remove(void* node) {
        c_chain_remove.hit();
        void* p = jo::view<void*>(jo::self<jo::Chain>(this)->head);
        std::uintptr_t r = u(this);
        while (p != nullptr && p != node) {
            p = joan::orig::arg::as<void*>(joan::orig::as_boid(p)->next());  // 0x00422fa0
            r = u(p);
        }
        if (p == nullptr) return r;
        if (p == jo::view<void*>(jo::self<jo::Chain>(this)->head)) {
            if (jo::self<jo::Chain>(this)->count == 1)
                jo::self<jo::Chain>(this)->head = nullptr;
            else
                jo::view<void*>(jo::self<jo::Chain>(this)->head) = joan::orig::arg::as<void*>(joan::orig::as_boid(p)->next());  // 0x00422fa0
        }
        joan::orig::as_boid(p)->unlink();     // 0x00423030
        joan::orig::as_boid(p)->set_next(0u);  // 0x00422f70
        joan::orig::as_boid(p)->set_prev(0u);  // 0x00422fc0
        const i32 c = jo::self<jo::Chain>(this)->count - 1;
        jo::self<jo::Chain>(this)->count = c;
        return static_cast<u32>(c);
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::init_46dba0", 0x0046dba0, &H::init_46dba0);
B99("Chain::remove", 0x0046dd70, &H::chain_remove);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
//
//      0x0046dbe0 Obj::clear_2_6_a       (thiscall, ret 0)
namespace {
namespace game_b93_01 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x0046dbe0
    void clear_2_6_a() {
        jo::view<u32>(jo::self<jo::Chain>(this)->count) = 0;
        jo::self<jo::Chain>(this)->index = 0;
        jo::self<jo::Chain>(this)->head = nullptr;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Obj::clear_2_6_a", 0x0046dbe0, &H::clear_2_6_a);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
//
//      0x0046dc80 Chain::append(node)             (thiscall, ret 4) - returns this.
//
//      0x0046eff0 Obj::missing(short)             (thiscall, ret 4) - 0x0041b3f0(..) == 0.
namespace {
namespace game_b100_02 {

using joan::call::thiscall;
using i16 = std::int16_t;
using i32 = std::int32_t;

struct H {
    // ------------------------------------------------------------------ 0x0046dc80
    void* chain_append(void* node) {
        if (jo::view<void*>(jo::self<jo::Chain>(this)->head) == nullptr) {
            jo::view<void*>(jo::self<jo::Chain>(this)->head) = node;
            joan::orig::as_boid(node)->set_next(nullptr);  // 0x00422f70
            joan::orig::as_boid(node)->set_prev(nullptr);  // 0x00422fc0
        } else {
            void* p = jo::view<void*>(jo::self<jo::Chain>(this)->head);
            while (joan::orig::arg::as<void*>(joan::orig::as_boid(p)->next()) != nullptr) p = joan::orig::arg::as<void*>(joan::orig::as_boid(p)->next());  // 0x00422fa0
            joan::orig::as_boid(p)->set_next(static_cast<jo::Boid*>(node));  // 0x00422f70
            joan::orig::as_boid(node)->set_prev(static_cast<jo::Boid*>(p));  // 0x00422fc0
        }
        jo::self<jo::Chain>(this)->count = jo::self<jo::Chain>(this)->count + 1;
        return this;
    }

    // ------------------------------------------------------------------ 0x0046eff0
    i32 missing(i32 id) {
        return joan::orig::as_dynarray(this)->ptr_at(static_cast<i32>(static_cast<i16>(id))) == nullptr ? 1 : 0;  // 0x0041b3f0
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Chain::append", 0x0046dc80, &H::chain_append);
B100("Obj::missing_46eff0", 0x0046eff0, &H::missing);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01_2 {

struct H {
    H* ctor_5b() {  // 0x0046de30
        std::memset(static_cast<void*>(this), 0, static_cast<std::uint32_t>(sizeof(jo::Flock)));                        // memset (0x5b on x86)
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::ctor_5b", 0x0046de30, &H::ctor_5b);
#undef B94

}  // namespace game_b94_01_2
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
//
//      0x0046de60 Obj::forward_46dfa0                      (thiscall, ret 0)
namespace {
namespace game_b97_01 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0046de60
    u32 forward_46dfa0() { joan::orig::as_flock(this)->free(); return 0; }  // 0x0046dfa0 (result unused: a destructor)
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Obj::forward_46dfa0", 0x0046de60, &H::forward_46dfa0);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
//
//      0x0046de90 Obj::init_46de90(8 args)       (thiscall, ret 0x20) - +6 = arg8, two new(0x1d) lists.
//
//  - SEH frames in 0x00492e20 / 0x004b0640 / 0x0046de90 only guard a constructor after
//    operator new (delete on throw); 0x004b0480 / 0x004b2a90 guard member dtors. Normal path only.
namespace {
namespace game_b95_02 {

using joan::call::cdecl_call;
using u32 = std::uint32_t;

using joan::orig::helpers::operator_new;

JOAN_COUNTER(c_init_46de90, "Obj::init_46de90");

struct H {
    // ------------------------------------------------------------------ 0x0046de90
    void* init_46de90(u32, u32, u32, u32, u32, u32, u32, jo::Scene* a8) {
        c_init_46de90.hit();
        jo::self<jo::Flock>(this)->scene = a8;
        void* p = operator_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
        void* r = p != nullptr ? joan::orig::dynarray(p)->construct(static_cast<i32>(sizeof(void*)), 0x1e, 0) : nullptr;  // 0x005843d0 (group pointers: 4 on x86)
        jo::view<void*>(jo::self<jo::Flock>(this)->groups) = r;
        p = operator_new(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
        r = p != nullptr ? joan::orig::dynarray(p)->construct(static_cast<i32>(sizeof(void*)), 0x1e, 0) : nullptr;  // 0x005843d0 (Boid pointers: 4 on x86)
        jo::view<void*>(jo::self<jo::Flock>(this)->members) = r;
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Obj::init_46de90", 0x0046de90, &H::init_46de90);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
//
//      0x0046efc0 Unit::list53_count()       -> 0x00417ad0(+0x53)
namespace {
namespace game_b92_02_2 {

struct H {
    // 0x0046efc0
    std::uint32_t list53_count() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->members))->size()); }  // 0x00417ad0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Unit::list53_count", 0x0046efc0, &H::list53_count);
#undef B

}  // namespace game_b92_02_2
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
//
//    - List::renumber 0x00477b10 (n) ret 4, Ending::start 0x00471f90 (i) ret 4,
//      ModelSlot::add_ref 0x00419cf0, AnimResMgr::add_ref 0x00417af0 (id) ret 4,
//      AnimSet::add_ref 0x004152b0, scalar deleting dtor 0x00471530, Obj::set_pos 0x00456fe0
//      (V3) ret 0xc, ModelObj::ctor 0x0041a1b0, AnimResMgr::idle 0x00417db0 (id) ret 4,
//      min 0x0046f020 (cdecl), Obj::ctor 0x00456eb0, ModelSlot::idle 0x00419ff0,
//      Names::at 0x00464100 (i) ret 4, setters 0x004725f0 / 0x004571b0, 0x00418c30.
namespace {
namespace game_b84_05 {

// 0x0046f020 min(a, b) (cdecl).
std::int32_t JOAN_CDECL min_int(std::int32_t a, std::int32_t b) { return a < b ? a : b; }

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("min_int", 0x0046f020, &min_int);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
//
//    - Obj::is_1a 0x0046f060 -> int: *(char*)[+0x10e] == 0x1a.
namespace {
namespace game_b90_03 {

struct H {
    // 0x0046f060
    std::int32_t is_1a() { return static_cast<std::int32_t>(*jo::view<std::int8_t*>(jo::self<jo::FileTxt>(this)->ptr)) == 0x1a ? 1 : 0; }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::is_1a", 0x0046f060, &H::is_1a);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
//
//      0x0046e590 List::at(short)                              thiscall, ret 4
namespace {
namespace game_b91_02 {

struct H {
    // 0x0046e590
    void* list_at(std::int16_t i) { return joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::Flock>(this)->groups))->ptr_at(static_cast<std::int32_t>(i)); }  // 0x0041b3f0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("List::at", 0x0046e590, &H::list_at);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::orig::helpers::addr_of;

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x0046dc20
    std::uintptr_t foreach_46dc20() {
        void* p = jo::view<void*>(jo::self<jo::Chain>(this)->head);
        if (p == nullptr) return addr_of(this);
        while (p != nullptr) {
            joan::orig::as_boid(p)->update(joan::orig::arg::u32(jo::self<jo::Chain>(this)->index), jo::self<jo::Chain>(this)->head);  // 0x00420890

            p = joan::orig::arg::as<void*>(joan::orig::as_boid(p)->next());                                   // 0x00422fa0
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Obj::foreach_46dc20", 0x0046dc20, &H::foreach_46dc20);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b105_03.cpp (game/b105_03)
namespace {
namespace game_b105_03_3 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {
    u32 get_6_46dd30() { return jo::view<u32>(jo::self<jo::Chain>(this)->count); }
    jo::Boid* get_a_46dd50() { return jo::self<jo::Chain>(this)->head; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_03", name, addr, fn)
B("Obj::get_6_46dd30", 0x0046dd30, &H::get_6_46dd30);
B("Obj::get_a_46dd50", 0x0046dd50, &H::get_a_46dd50);
#undef B

}  // namespace game_b105_03_3
}  // namespace

// ---------------------------------------- from modules/game_b103_02.cpp (game/b103_02)
namespace {
namespace game_b103_02 {

using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;
using joan::orig::helpers::addr;

JOAN_COUNTER(c_is_tree, "Obj::is_tree");

struct H {
    // ------------------------------------------------------------------ 0x0046ef50
    u32 is_tree(const char* name) {
        c_is_tree.hit();
        char buf[0x108];  // original: 0x100 bytes at [ebp-0x108] + 8 bytes of frame above
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(name));  // strcpy
        joan::orig::libc::_strupr(joan::orig::arg::str(buf));                                // _strupr
        const char* const r = joan::orig::arg::as<const char*>(joan::orig::libc::strstr(joan::orig::arg::str(buf), str(d::s_TREE)));  // strstr "TREE"
        const std::uintptr_t e = addr(r);
        if (e == 0) return 0;
        return (e & 0xffffff00u) | 1u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_02", name, addr, fn)
B("Obj::is_tree", 0x0046ef50, &H::is_tree);
#undef B

}  // namespace game_b103_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::FlockGroup::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b105_03_2::H::self_46db60>(this)); }  // 0x46db60
auto joan::orig::FlockGroup::init(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::init_46dba0>(this, a0)); }  // 0x46dba0
auto joan::orig::FlockGroup::clear() -> void { joan::bind::member<&::game_b93_01::H::clear_2_6_a>(this); }  // 0x46dbe0
auto joan::orig::FlockGroup::update_members() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_19::H::foreach_46dc20>(this)); }  // 0x46dc20
auto joan::orig::FlockGroup::append(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_02::H::chain_append>(this, a0)); }  // 0x46dc80
auto joan::orig::FlockGroup::first_member() -> Boid* { return joan::bind::conv<Boid*>(joan::bind::member<&::game_b105_03_3::H::get_a_46dd50>(this)); }  // 0x46dd50
auto joan::orig::FlockGroup::remove(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::chain_remove>(this, a0)); }  // 0x46dd70
auto joan::orig::Flock::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_01_2::H::ctor_5b>(this)); }  // 0x46de30
auto joan::orig::Flock::init(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6, Scene* a7) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_02::H::init_46de90>(this, a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x46de90
auto joan::orig::Flock::free() -> void { joan::bind::member<&::game_b94_01::H::flock_free>(this); }  // 0x46dfa0
auto joan::orig::Flock::spawn_group(std::int32_t a0) -> void { joan::bind::member<&::game_b106_17::H::spawn_46e2d0>(this, a0); }  // 0x46e2d0
auto joan::orig::List::at_46e590(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::list_at>(this, a0)); }  // 0x46e590
auto joan::orig::Unit::list53_item(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_02::H::list53_item>(this, a0)); }  // 0x46e5d0
auto joan::orig::Flock::update_stub() -> void { joan::bind::member<&::game_b105_03::H::stub0>(this); }  // 0x46e690
auto joan::orig::Flock::load_steering(const char* a0) -> void { joan::bind::member<&::game_b98_01::H::steer_load>(this, a0); }  // 0x46e950
auto joan::orig::FlockList::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_02::H::flock_clear>(this)); }  // 0x46ecc0
auto joan::orig::Flock::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b98_01::H::sdtor_46ed90>(this, a0)); }  // 0x46ed90
auto joan::orig::Unit::list53_count() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_02_2::H::list53_count>(this)); }  // 0x46efc0
auto joan::orig::FlockList::is_empty_at(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_02::H::missing>(this, a0)); }  // 0x46eff0
auto joan::orig::min_int(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b84_05::min_int>(a0, a1)); }  // 0x46f020
auto joan::orig::FileTxt::at_eof() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b90_03::H::is_1a>(this)); }  // 0x46f060
