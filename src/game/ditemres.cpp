// game/ditemres.cpp: functions of the original d:\ww\ditemres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>

#include "../lib/crt/crt.hpp"
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

using joan::hook::global;

using joan::call::thiscall;
using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

void* const kHeap = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b101_02.cpp (game/b101_02)
//
//      0x00463980 DropItemRes::free_all             (thiscall, ret 0) - 0x00463a40, heap free +4.
//
//      0x00463a10 DropItemRes::reload               (thiscall, ret 0) - 0x00463aa0, byte +8 = 1.
namespace {
namespace game_b101_02 {

using i8 = std::int8_t;

using joan::orig::helpers::u;

JOAN_COUNTER(c_free_all, "DropItemRes::free_all");

u32 heap_free(void* p, const char* file, i32 line) {
    joan::orig::as_mem(kHeap)->del(p, file, line);  // 0x0058ed70
    return 0;  // (the only caller discards the result)
}

struct H {
    // ------------------------------------------------------------------ 0x00463980
    std::uintptr_t free_all() {
        c_free_all.hit();
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::DropItemRes>(this)->loaded)) != 1) return u(this);  // eax = this (movsx went to ecx)
        std::uintptr_t r = thiscall<u32>(fn::DropItemRes_reset_all, this);  // 0x00463a40
        if (jo::view<void*>(jo::self<jo::DropItemRes>(this)->rows) != nullptr) {
            heap_free(jo::view<void*>(jo::self<jo::DropItemRes>(this)->rows), str(d::s_d_ww_ditemres_cpp), static_cast<i32>(global<i16>(d::g_9fd3a8)) + 7);
            jo::view<void*>(jo::self<jo::DropItemRes>(this)->rows) = nullptr;
            jo::self<jo::DropItemRes>(this)->count = 0;
            r = u(this);
        }
        jo::self<jo::DropItemRes>(this)->loaded = 0;
        return r;
    }

    // ------------------------------------------------------------------ 0x00463a10
    void* reload() {
        joan::orig::as_drop_item_res(this)->load();  // 0x00463aa0
        jo::self<jo::DropItemRes>(this)->loaded = 1;
        return this;
    }
};

#define B101(name, addr, fn) JOAN_REPLACE("game/b101_02", name, addr, fn)
B101("DropItemRes::free_all", 0x00463980, &H::free_all);
B101("DropItemRes::reload", 0x00463a10, &H::reload);
#undef B101

}  // namespace game_b101_02
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
//
//      0x00463a40 DropItemRes::reset_all          (thiscall, ret 0)
//
//      0x00463aa0 DropItemRes::load               (thiscall, ret 0) - ITEM\DROPITEM.RES.
//
//  - 0x00463aa0: the interleaved pushes give 0x0059bf70(i) and 0x00589780(obj, r2, r1);
//    the stack objects get the original frame gaps (0x138 / 0x194 / 0x19c bytes).
namespace {
namespace game_b100_02 {


JOAN_COUNTER(c_dropitem_load, "DropItemRes::load");

struct H {
    // ------------------------------------------------------------------ 0x00463a40
    void reset_all() {
        for (i16 i = 1; static_cast<i32>(i) <= jo::self<jo::DropItemRes>(this)->count; i = static_cast<i16>(i + 1)) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_names(this)->at(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x00464100
            joan::orig::as_drop_item(e)->free_list();                                                 // 0x00463f50
        }
    }

    // ------------------------------------------------------------------ 0x00463aa0
    void dropitem_load() {
        c_dropitem_load.hit();
        alignas(8) u8 res[((sizeof(jo::ResourceIdxL) > 0x138 ? sizeof(jo::ResourceIdxL) : 0x138) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];   // [ebp-0x148] (x64: the larger ResourceIdxL)
        alignas(8) u8 path[0x194];  // [ebp-0x2dc]
        alignas(8) u8 item[((sizeof(jo::FileTxt) > 0x19c ? sizeof(jo::FileTxt) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x480] (x64: the larger FileTxt)
        joan::orig::as_res_idx(res)->construct();            // 0x004179f0
        joan::orig::as_string(path)->construct();    // 0x005a1170
        joan::orig::as_string(path)->assign(str(d::s_ITEM_9cf648));     // 0x005a1440 "ITEM\\"
        joan::orig::as_string(path)->append(str(d::s_DROPITEM_RES));         // 0x005a1550 "DROPITEM.RES"
        void* const name = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        joan::orig::as_res_idx_l(res)->init(joan::orig::arg::str(name), static_cast<char>(1), static_cast<char>(0));  // 0x0059b920
        jo::self<jo::DropItemRes>(this)->count = jo::as_res_idx_l(res)->rec_count;
        jo::view<void*>(jo::self<jo::DropItemRes>(this)->rows) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(static_cast<i32>(static_cast<u32>(jo::self<jo::DropItemRes>(this)->count) * sizeof(jo::DropItem))), str(d::s_d_ww_ditemres_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9fd3ac)) + 0xb));  // 0x0058e7b0
        std::memset(jo::view<void*>(jo::self<jo::DropItemRes>(this)->rows), 0, static_cast<u32>(jo::self<jo::DropItemRes>(this)->count) * sizeof(jo::DropItem));
        for (i32 i = 1; i <= jo::as_res_idx_l(res)->rec_count; ++i) {
            jo::DropItem* const e = jo::self<jo::DropItemRes>(this)->rows + (i - 1);
            e->id = static_cast<i16>(i);
            void* const r1 = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(res)->get_data_size(joan::orig::arg::i32(i)));  // 0x0059bdd0
            void* const r2 = joan::orig::arg::as<void*>(joan::orig::as_res_idx_l(res)->read_rec(joan::orig::arg::i32(i)));  // 0x0059bf70
            joan::orig::as_file_txt(item)->construct_mem(joan::orig::arg::str(r2), joan::orig::arg::i32(reinterpret_cast<std::uintptr_t>(r1)));             // 0x00589780
            joan::orig::as_sound_list(this)->read(joan::orig::arg::mptr(static_cast<void*>(item)), joan::orig::arg::mptr(static_cast<void*>(e)));  // 0x00463cb0
            joan::orig::as_file_txt(item)->destruct();                            // 0x00589890
        }
        joan::orig::as_archive(res)->dtor();  // 0x00417a30
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("DropItemRes::reset_all", 0x00463a40, &H::reset_all);
B100("DropItemRes::load", 0x00463aa0, &H::dropitem_load);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
//
//    - Obj::fwd_463950 0x00463950, ret 0.
namespace {
namespace game_b102_01 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ---------------------------------------------------------------- 0x00463950
    u32 fwd_463950() { return joan::orig::arg::as<u32>(joan::orig::as_drop_item_res(this)->free_all()); }  // 0x00463980
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Obj::fwd_463950", 0x00463950, &H::fwd_463950);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
//
//      0x00463cb0 SoundList::read(reader, obj)             (thiscall, ret 8, SEH new)
//
//  - 0x0045c9c0 / 0x00463cb0: SEH frames only protect operator new + ctor (unwind delete); the
//    normal path is kept. 0x00463cb0's list element is the 8-byte {word, word, int} local.
namespace {
namespace game_b98_01 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

struct SoundEntry {  // 8-byte dynarr element of 0x00463cb0 (ebp-0x20), the DropItem::drops elements
    u16 id;
    u16 vol;
    i32 len;
};
static_assert(sizeof(SoundEntry) == 8, "SoundEntry");

void* const kSounds = obj(d::g_item_res);

JOAN_COUNTER(c_sound_read, "SoundList::read");

i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
using joan::orig::helpers::stricmp_;
char* strtok_(char* s, const char* d) { return joan::orig::arg::as<char*>(joan::orig::libc::strtok(joan::orig::arg::str(s), joan::orig::arg::str(d))); }
using joan::orig::helpers::new_;  // operator new
i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct H {
    // ------------------------------------------------------------------ 0x00463cb0
    void sound_read(void* reader, u8* list) {
        c_sound_read.hit();
        const auto read = [reader]() { return joan::orig::as_file_txt(reader)->get_token(1); };  // 0x00589c70
        char* tok = read();  // ebp-0x14
        tok = read();
        atoi_(tok);  // ebp-0x18 (dead)
        tok = read();
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_drop_item(list)->name)), joan::orig::arg::str(static_cast<const char*>(read())), 0x20u);  // strncpy
        void* const mem = new_(static_cast<std::uint32_t>(sizeof(jo::DynArray)));  // 0x1d on x86
        void* const arr = mem != nullptr ? joan::orig::dynarray(mem)->construct(8, 0x1e, 0) : nullptr;  // 0x005843d0
        jo::view<void*>(jo::as_drop_item(list)->drops) = arr;
        for (;;) {
            tok = read();
            if (tok == nullptr) break;
            if (stricmp_(tok, str(d::s_End)) == 0) break;  // "End"
            SoundEntry e;                                     // ebp-0x20
            e.id = static_cast<u16>(joan::orig::arg::as<i32>(joan::orig::as_name_table(kSounds)->find_4a36c0(joan::orig::arg::str(tok))));  // 0x004a36c0
            tok = strtok_(read(), str(d::s_comma_9d0728));
            e.len = atoi_(tok);  // ebp-0x1c
            tok = strtok_(nullptr, str(d::s_comma_9d0728));
            if (tok != nullptr) {
                e.vol = static_cast<u16>(atoi_(tok));
            } else {
                e.vol = reinterpret_cast<const u16&>(jo::as_item_def(joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kSounds)->get(joan::orig::arg::i16(sx16(e.id)))))->min_level);  // 0x00425ce0
            }
            joan::orig::dynarray(jo::view<void*>(jo::as_drop_item(list)->drops))->linkin(static_cast<void*>(&e));  // 0x00584620
            jo::view<i32>(jo::as_drop_item(list)->total_weight) = jo::view<i32>(jo::as_drop_item(list)->total_weight) + e.len;
        }
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("SoundList::read", 0x00463cb0, &H::sound_read);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

void* memset_(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    void* clear_9() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::DropItemRes)));  // 9 on x86 (the DropItemRes 0xa50288)
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::clear_9", 0x00463920, &H::clear_9);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using u32 = std::uint32_t;
using i32 = std::int32_t;

using u8 = std::uint8_t;
using i16 = std::int16_t;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x00463c40
    void* set_c6_463c40(const u8* p) {
        if (p != nullptr) {
            jo::self<jo::Order0c_b>(this)->u32_c6 = 2;
            jo::view<i32>(jo::self<jo::Order0c_b>(this)->u32_ca) = at<i16>(p, 4);  // x64: p's class unknown (no caller in the C++)
        } else {
            jo::self<jo::Order0c_b>(this)->u32_c6 = 0;
            jo::self<jo::Order0c_b>(this)->u32_ca = 0;
        }
        return this;  // p == 0: original leaves the caller's eax (no references to this function)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Unit::set_c6_463c40", 0x00463c40, &H::set_c6_463c40);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

using i32 = std::int32_t;

using joan::call::cdecl_call;

using joan::img::obj;

void* const kFiles = obj(d::g_misc);

std::int32_t list_count(const void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

JOAN_COUNTER(c_grass_pick, "Grass::pick");
JOAN_COUNTER(c_names_find, "Names::find");

struct H {
    void* names_at(std::int16_t i) {                                          // 0x00464100
        return jo::self<jo::DropItemRes>(this)->rows + static_cast<std::int32_t>(i) - 1;  // 0x2b-byte records on x86
    }

    // ---- 0x00463ed0 Names::find(name): index + 1 or 0 ----------------------------------------
    std::int32_t names_find(const char* name) {
        c_names_find.hit();
        for (std::int32_t i = 0; i < jo::self<jo::DropItemRes>(this)->count; ++i) {
            if (joan::crt::stricmp(joan::orig::arg::str(static_cast<const void*>(jo::self<jo::DropItemRes>(this)->rows[i].name)), joan::orig::arg::str(name)) == 0)
                return i + 1;
        }
        return 0;
    }

    // ---- 0x00464030 Grass::pick(short* type, int* value) --------------------------------------
    void grass_pick(std::int16_t* type, std::int32_t* value) {
        c_grass_pick.hit();
        if (jo::view<void*>(jo::self<jo::DropItem>(this)->drops) == nullptr) {
            *type = 0;
            return;
        }
        std::int32_t left = joan::orig::as_misc(kFiles)->random(joan::orig::arg::i32(jo::self<jo::DropItem>(this)->total_weight)) + 1;  // 0x00590540
        for (std::int32_t i = list_count(jo::view<void*>(jo::self<jo::DropItem>(this)->drops)); i > 0; --i) {
            const game_b98_01::SoundEntry* const e = joan::orig::arg::as<game_b98_01::SoundEntry*>(joan::orig::as_dynarray(jo::view<void*>(jo::self<jo::DropItem>(this)->drops))->get(joan::orig::arg::i32(i)));  // 0x00417200
            left = static_cast<std::int32_t>(static_cast<std::uint32_t>(left) - jo::view<std::uint32_t>(e->len));
            if (!(left > 0)) {
                *type = jo::view<std::int16_t>(e->id);
                *value = static_cast<std::int32_t>(jo::view<std::int16_t>(e->vol));
                return;
            }
        }
        *type = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("Grass::pick", 0x00464030, &H::grass_pick);
B("Names::find", 0x00463ed0, &H::names_find);
B("Names::at", 0x00464100, &H::names_at);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using joan::orig::helpers::u;

struct H {
    // ------------------------------------------------------------------ 0x00463f50
    std::uintptr_t delete_23_463f50() {
        if (jo::view<void*>(jo::self<jo::DropItem>(this)->drops) == nullptr) return u(this);
        void* const p = jo::view<void*>(jo::self<jo::DropItem>(this)->drops);
        std::uintptr_t r = 0;
        if (p != nullptr) r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_dynarray(p)->destroy(1u));  // 0x00463fc0
        jo::self<jo::DropItem>(this)->drops = nullptr;
        return r;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::delete_23_463f50", 0x00463f50, &H::delete_23_463f50);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

using joan::call::cdecl_call;

using joan::call::thiscall;

using joan::orig::helpers::game_delete;  // operator delete

struct H {
    // scalar deleting destructors
    void* sdd(std::uintptr_t dtor, std::uint32_t flags) {
        thiscall(dtor, this);
        if (flags & 1u) game_delete(this);
        return this;
    }
    void* sdd_463fc0(std::uint32_t flags) { return sdd(fn::code_value(fn::dtor_DynArray), flags); }  // 0x005844b0
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("sdd_00463fc0", 0x00463fc0, &H::sdd_463fc0);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b105_02.cpp (game/b105_02)
namespace {
namespace game_b105_02 {

struct H {
    // ------------------------------------------------------------------ tiny methods
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_02", name, addr, fn)
B("Obj::nop_464010", 0x00464010, &H::nop);
#undef B

}  // namespace game_b105_02
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::DropItemRes::free_all() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_02::H::free_all>(this)); }  // 0x463980
auto joan::orig::DropItemRes::reset_all() -> void { joan::bind::member<&::game_b100_02::H::reset_all>(this); }  // 0x463a40
auto joan::orig::DropItemRes::load() -> void { joan::bind::member<&::game_b100_02::H::dropitem_load>(this); }  // 0x463aa0
auto joan::orig::SoundList::read(void* a0, void* a1) -> void { joan::bind::member<&::game_b98_01::H::sound_read>(this, a0, a1); }  // 0x463cb0
auto joan::orig::Names::find_463ed0(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::names_find>(this, a0)); }  // 0x463ed0
auto joan::orig::DropItem::free_list() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_02::H::delete_23_463f50>(this)); }  // 0x463f50
auto joan::orig::DynArray::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18::H::sdd_463fc0>(this, a0)); }  // 0x463fc0
auto joan::orig::Grass::pick(void* a0, void* a1) -> void { joan::bind::member<&::game_b84_05::H::grass_pick>(this, a0, a1); }  // 0x464030
auto joan::orig::Names::at(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b84_05::H::names_at>(this, a0)); }  // 0x464100
