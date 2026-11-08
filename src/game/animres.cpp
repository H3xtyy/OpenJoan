// game/animres.cpp: functions of the original d:\ww\animres.cpp (re/attribution.csv).
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
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::orig::helpers::game_delete;                                    // operator delete

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
//
//      0x00417060 AnimRes::free          (thiscall, ret 0) - entries of 0x3a, slot table +0x276.
namespace {
namespace game_b93_01 {

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_animres_free, "AnimRes::free");

void mem_free(void* p, const char* file, i32 line) { joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058ed70

struct H {
    // ------------------------------------------------------------------------ 0x00417060
    void animres_free() {
        c_animres_free.hit();
        if (jo::self<jo::AnimRes>(this)->loaded == 0) return;
        for (i32 i = 1; i <= jo::self<jo::AnimRes>(this)->count; ++i) {
            u8* const e = jo::self<jo::AnimRes>(this)->entries + i * static_cast<i32>(sizeof(jo::AnimSet)) - static_cast<i32>(sizeof(jo::AnimSet));  // 0x3a-byte records on x86
            joan::orig::as_anim_set(e)->free_buffer();  // 0x004149c0
        }
        for (i32 i = joan::orig::arg::as<i32>(joan::orig::as_anim_res_mgr(this)->slot_count()); i > 0; --i) {  // 0x00417aa0
            u8* const slot = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(&jo::self<jo::AnimRes>(this)->list)->get(joan::orig::arg::i32(i)));  // 0x00417200
            if (jo::as_anim_slot(slot)->anim != nullptr) {
                void* const p = jo::view<void*>(jo::as_anim_slot(slot)->anim);
                if (p != nullptr) joan::orig::as_anim(p)->scalar_delete(1u);  // 0x00417250 deleting dtor
                jo::as_anim_slot(slot)->anim = nullptr;
            }
            if (jo::as_anim_slot(slot)->buffer != nullptr) {
                game_delete(jo::as_anim_slot(slot)->buffer);
                jo::as_anim_slot(slot)->buffer = nullptr;
            }
        }
        joan::orig::dynarray(&jo::self<jo::AnimRes>(this)->list)->zap(1);  // 0x00584580
        if (jo::view<void*>(jo::self<jo::AnimRes>(this)->entries) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::AnimRes>(this)->entries), str(d::s_d_ww_animres_cpp), static_cast<i32>(global<i16>(d::g_9f656c)) + 0x25);  // animres.cpp
            jo::view<void*>(jo::self<jo::AnimRes>(this)->entries) = nullptr;
        }
        jo::self<jo::AnimRes>(this)->loaded = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("AnimRes::free", 0x00417060, &H::animres_free);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
//
//      0x004171b0 Obj::set_4_8(a, b)            (thiscall, ret 8)
namespace {
namespace game_b96_01 {

struct H {
    // ------------------------------------------------------------------ 0x004171b0
    void set_4_8(std::uintptr_t a, std::uintptr_t b) {  // the facade passes two addresses (set_idle_callbacks): pointer-sized
        // The object is the global Error log (g_log, x86 +4 / +8 = on_report / fn_8), not an AnimRes: on x64 the
        // pointers before them are 8 bytes wide.
        jo::view<std::uintptr_t>(jo::self<jo::Error>(this)->on_report) = a;
        jo::view<std::uintptr_t>(jo::self<jo::Error>(this)->fn_8) = b;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Obj::set_4_8", 0x004171b0, &H::set_4_8);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
//
//    0x00417200 slot, count, null checks), effect-state flags, vector / matrix helpers
namespace {
namespace game_fx {

struct H {
    void* list_slot(std::int32_t i) {                             // 0x00417200
        if (i < 1 || i > jo::self<jo::List>(this)->last_ele) return nullptr;
        return jo::view<std::uint8_t*>(jo::self<jo::List>(this)->body_buf) + (i - 1) * jo::self<jo::List>(this)->ele_size;
    }
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("List::slot", 0x00417200, &H::list_slot);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
//
//    - destroy helpers 0x004866b0 / 0x004eb570 / 0x00425ef0, scalar deleting destructors
//      0x00416640 / 0x00417250.
namespace {
namespace game_b83_07 {

struct H {
    H* delete_418(std::uint32_t flags) {  // 0x00417250
        joan::orig::as_anim_def(this)->destruct();  // 0x00418c30
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Anim::scalar_delete", 0x00417250, &H::delete_418);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
//
//    - AnimRes::load_4172a0 0x004172a0 (ret 0, animres.cpp): opens "RESOURCE\\ANIM_AM2.RES" (+0xc) and
//      "RESOURCE\\ANIM_AM3.RES" (+0x141) (0x0059b920(path, 0, 0)), section "ANIMATE" (0x0058b180 on
//      0xa56b18), count +0 = 0x00415280, +4 = alloc(count * 0x3a, animres.cpp, word [0x9f6570] + 0xc)
//      (0x0058e7b0 on 0xa48838), memset 0; entry i: word = i + 1, 0x00590210 copies (0x20 / 8 bytes) from
//      0x00580c70(i + 1); then 0x00414a30-entry init 0x00414970(0x00590170(entry + 0x23, 8)). eax = this.
namespace {
namespace game_b108_20 {

using u16 = std::uint16_t;

JOAN_COUNTER(c_animres_load, "AnimRes::load_4172a0");

struct H {
    // -------------------------------------------------------------- 0x004172a0
    void* animres_load() {
        c_animres_load.hit();
        joan::orig::as_res_idx_l(&jo::self<jo::AnimRes>(this)->index_a)->init(str(d::s_RESOURCE_ANIM_AM2_RES), static_cast<char>(i32{0}), static_cast<char>(i32{0}));  // 0x0059b920
        joan::orig::as_res_idx_l(&jo::self<jo::AnimRes>(this)->index_b)->init(str(d::s_RESOURCE_ANIM_AM3_RES), static_cast<char>(i32{0}), static_cast<char>(i32{0}));
        void* const section = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(str(d::s_ANIMATE)));  // 0x0058b180 "ANIMATE"
        jo::self<jo::AnimRes>(this)->count = joan::orig::arg::as<i32>(joan::orig::as_database(section)->rec_count());                                       // 0x00415280
        jo::self<jo::AnimRes>(this)->entries = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(jo::self<jo::AnimRes>(this)->count * static_cast<i32>(sizeof(jo::AnimSet))), str(d::s_d_ww_animres_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9f6570)) + 0xc)));  // 0x0058e7b0
        std::memset(joan::orig::arg::mptr(jo::self<jo::AnimRes>(this)->entries), i32{0}, joan::orig::arg::u32(jo::self<jo::AnimRes>(this)->count * static_cast<i32>(sizeof(jo::AnimSet))));  // memset
        for (i32 i = 0; i < jo::self<jo::AnimRes>(this)->count; ++i) {
            u8* const src = joan::orig::arg::as<u8*>(joan::orig::as_database(section)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            u8* const e = jo::self<jo::AnimRes>(this)->entries + i * static_cast<i32>(sizeof(jo::AnimSet));
            jo::as_anim_set(e)->id = static_cast<u16>(i + 1);
            joan::orig::as_misc(obj(d::g_misc))->copy_field_590210(joan::orig::arg::str(e + 2), joan::orig::arg::str(src), i32{0x20});         // 0x00590210
            joan::orig::as_misc(obj(d::g_misc))->copy_field_590210(joan::orig::arg::str(e + 0x23), joan::orig::arg::str(src + 0x20), i32{8});
        }
        for (i32 i = 0; i < jo::self<jo::AnimRes>(this)->count; ++i) {
            u8* const e = jo::self<jo::AnimRes>(this)->entries + i * static_cast<i32>(sizeof(jo::AnimSet));
            const char* const v = (joan::orig::as_misc(obj(d::g_misc))->rtrim_field_590170(joan::orig::arg::str(e + 0x23), i32{8}));  // 0x00590170
            joan::orig::as_anim_set(e)->init(v);                                              // 0x00414970
        }
        return this;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("AnimRes::load_4172a0", 0x004172a0, &H::animres_load);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
//
//    - Obj417020::init 0x00417020 (ret 0): 0x00417060, 0x004172a0, +8 = 1, returns this.
namespace {
namespace game_b108_03 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x00417020
    void* init_417020() {
        joan::orig::as_anim_res_mgr(this)->free();  // 0x00417060
        joan::orig::as_anim_res_mgr(this)->load();  // 0x004172a0
        jo::view<u32>(jo::self<jo::AnimRes>(this)->loaded) = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("Obj417020::init", 0x00417020, &H::init_417020);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b86_05.cpp (game/b86_05)
//
//    - vector deleting destructor 0x00427930 (0x38-byte elements, dtor 0x0054ac60);
//      destructor 0x004214b0 (0x00440320, then +0x61 0x0054b460); 0x00417aa0 (+0x276
//      0x00417ad0); 0x0043ac60 (zeroes +0xd3 / +0xd7 / +0xdd).
//
//  - Leftover eax values that are well defined (this, a callee's result) are returned:
//    0x00461f10 / 0x0043ac60 / the setters leave this, 0x004214b0 / 0x00417aa0 their last
//    callee's result, 0x004236d0 this (empty list) or the last 0x0058ed70 result.
//    0x00432300 / 0x00438f80 return void (callers never read eax).
namespace {
namespace game_b86_05 {

struct H {
    // ---- 0x00417aa0: +0x276 0x00417ad0, eax = its result.
    std::uint32_t call_417aa0() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(&jo::self<jo::AnimRes>(this)->list)->size()); }  // 0x00417ad0
};

#define B86(name, addr, fn) JOAN_REPLACE("game/b86_05", name, addr, fn)
B86("Obj417aa0::call_276", 0x00417aa0, &H::call_417aa0);
#undef B86

}  // namespace game_b86_05
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using i32 = std::int32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_ctor_416f60, "Obj::Obj_416f60");

void* memset_(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {

    // ------------------------------------------------------------------ 0x00416f60
    void* ctor_416f60() {
        c_ctor_416f60.hit();
        joan::orig::as_res_idx(&jo::self<jo::AnimRes>(this)->index_a)->construct();    // 0x004179f0
        joan::orig::as_res_idx(&jo::self<jo::AnimRes>(this)->index_b)->construct();  // 0x004179f0
        joan::orig::dynarray(&jo::self<jo::AnimRes>(this)->list)->construct(static_cast<i32>(sizeof(jo::AnimSlot)), 5, 0);  // 0x005843d0
        jo::view<u32>(jo::self<jo::AnimRes>(this)->loaded) = 0;
        memset_(this, 0, offsetof(jo::AnimRes, list));
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::Obj_416f60", 0x00416f60, &H::ctor_416f60);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

using joan::hook::global;

using i32 = std::int32_t;

struct H {
    // 0x00417410 (this = the Credits screen 0xa52968: Screen::reset_slots in gamemode.cpp)
    void reset_slots() {
        jo::Credits* const c = jo::self<jo::Credits>(this);
        if (static_cast<i32>(c->active) == 0) return;
        if (global<i32>(d::g_key) != 0x1b) return;
        for (i32 i = 0; i < 5; ++i) (&c->label_0)[i].moving = 0;  // label_0 .. label_4
        c->stage = 4;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Obj::reset_slots", 0x00417410, &H::reset_slots);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

struct H {
    std::uint32_t dtor_417470(void* arg) {  // 0x00417470
        joan::orig::dynarray(&jo::self<jo::AnimRes>(this)->list)->linkin(joan::orig::arg::ptr(arg));  // 0x00584620
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_dynarray(&jo::self<jo::AnimRes>(this)->list)->cursor());  // 0x00417520
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj417470::dtor", 0x00417470, &H::dtor_417470);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01 {

using i8 = std::int8_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using u32 = std::uint32_t;

struct Obj {};
using joan::orig::helpers::virtual_call;
JOAN_COUNTER(c_refresh, "Unit::refresh_417d20");

struct H {
    // ------------------------------------------------------------------ 0x004174c0
    u32 call_584a10_4174c0(u32 a) {
        joan::orig::as_dynarray(&jo::self<jo::AnimRes>(this)->list)->linkout_584a10(joan::orig::arg::i32(a));  // 0x00584a10
        return 0;  // void callee; 0x004174c0 has no callers (result unused)
    }
    // ------------------------------------------------------------------ 0x00417d20
    void* refresh_417d20() {
        c_refresh.hit();
        if (static_cast<i8>(jo::self<jo::MapObject>(this)->active) != 0) return this;
        jo::self<jo::MapObject>(this)->active = 1;
        jo::self<jo::MapObject>(this)->refresh_depth = static_cast<u8>(jo::self<jo::MapObject>(this)->refresh_depth + 1);
        virtual_call<u32>(this, 0x10);
        virtual_call<u32>(this, 0x34);
        jo::self<jo::MapObject>(this)->refresh_depth = static_cast<u8>(jo::self<jo::MapObject>(this)->refresh_depth - 1);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::call_584a10_4174c0", 0x004174c0, &H::call_584a10_4174c0);
B("Unit::refresh_417d20", 0x00417d20, &H::refresh_417d20);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b82_18_2.cpp (game/b82_18)
namespace {
namespace game_b82_18_2 {

using i32 = std::int32_t;

struct H {
    void* get_4174f0(std::uint32_t i) {  // 0x004174f0
        return joan::orig::arg::as<void*>(joan::orig::as_dynarray(&jo::self<jo::AnimRes>(this)->list)->get(joan::orig::arg::i32(i)));  // 0x00417200
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("get_004174f0", 0x004174f0, &H::get_4174f0);
#undef B82

}  // namespace game_b82_18_2
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {

struct H {
    std::int32_t last_slot() { return jo::self<jo::DynArray>(this)->cur_pos; }  // 0x00417520
    void* file_ctor() { return joan::orig::as_file(this)->construct(); }  // 0x004179c0
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Table::last_slot", 0x00417520, &H::last_slot);
L("File::File", 0x004179c0, &H::file_ctor);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b111_20.cpp (fix/uninit)
namespace {
namespace game_b111_20 {

using joan::hook::global;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

void* const kFiles = obj(d::g_misc);      // file / text helpers

using u32 = std::uint32_t;

using i16 = std::int16_t;

using u16 = std::uint16_t;


JOAN_COUNTER(c_acquire, "AnimSet::acquire_417540");

// ---------------------------------------------------------------- 0x00417540 helpers
struct alignas(4) FileObj {  // 0x004179c0 / 0x005872b0 (frame slot 0x110 bytes)
    u8 bytes[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger File
};
struct alignas(4) Reader {  // 0x00587f80 / 0x00587fd0 (frame slot 0x11c bytes)
    u8 bytes[((sizeof(jo::FileMem) > 0x11c ? sizeof(jo::FileMem) : 0x11c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger FileMem
};
struct alignas(4) Path {  // 0x005a1170 (frame slot 0x194 bytes, no destructor)
    u8 bytes[0x194];
};
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

// Opens <dir><name><ext> from disk or from the archive `arc` into reader r; returns the size.
u32 open_anim(u8* p, Path& path, FileObj& file, Reader& r, img::Addr ext, void* arc) {
    joan::orig::as_string(&path)->assign(joan::orig::arg::str(jo::as_anim_slot(p)->path));  // 0x005a1440
    joan::orig::as_string(&path)->append(joan::orig::arg::str(jo::as_anim_slot(p)->name));  // 0x005a1550
    joan::orig::as_string(&path)->append(joan::orig::arg::str(str(ext)));  // 0x005a1550
    if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(path))) != 0) {  // 0x00590b80
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(0));  // 0x00586da0
        joan::orig::as_file_mem(&r)->open_file(joan::orig::arg::file(&file));                   // 0x005885e0
        return 0xffffffffu;
    }
    // fix/uninit: 0x0059c200 leaves the size unwritten when the name is not in the archive.
    u32 size = 0xffffffffu;
    void* const src = thiscall<void*>(fn::get_file_name_59c200, arc, jo::as_anim_slot(p)->name, &size);  // 0x0059c200
    joan::orig::as_file_mem(&r)->open_file(joan::orig::arg::file(src));                                         // 0x005885e0
    return size;
}

struct H {
    // ------------------------------------------------------------ 0x00417540
    i32 acquire(u32 type_arg) {
        c_acquire.hit();
        const i32 t = static_cast<i16>(static_cast<u16>(type_arg & 0xffffu));
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_anim_res_mgr(this)->slot_4174f0(joan::orig::arg::u32(t)));  // 0x004174f0
        while (joan::orig::arg::as<i32>(joan::orig::as_anim_res_mgr(this)->idle(joan::orig::arg::i16(t))) == 0) {  // 0x00417db0
            reinterpret_cast<u32(JOAN_STDCALL*)(u32, i32)>(global<std::uintptr_t>(d::iat_KERNEL32_SleepEx))(5, 1);  // SleepEx
            joan::orig::as_anim_res_mgr(this)->stream(joan::orig::arg::i16(t));  // 0x00417b60
        }
        jo::as_anim_slot(p)->refs = jo::as_anim_slot(p)->refs + 1;
        if (jo::as_anim_slot(p)->refs == 1 && jo::as_anim_slot(p)->anim == nullptr) {
            void* const mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::AnimDef)));  // operator new (0x18b on x86)
            void* const o = mem != nullptr ? joan::orig::as_anim_def(mem)->construct() : nullptr;  // 0x005b2390
            jo::view<void*>(jo::as_anim_slot(p)->anim) = o;
            FileObj f1, f2;
            Reader r1, r2;
            Path a, b;
            joan::orig::as_file(&f1)->construct();  // 0x004179c0
            joan::orig::as_file(&f2)->construct();
            joan::orig::as_file_mem(&r1)->construct();  // 0x00587f80
            joan::orig::as_file_mem(&r2)->construct();
            joan::orig::as_string(&a)->construct();  // 0x005a1170
            joan::orig::as_string(&b)->construct();
            const u32 size1 = open_anim(p, a, f1, r1, d::s_AM2_9cf5bc, &jo::self<jo::AnimRes>(this)->index_a);    // ".AM2"
            const u32 size2 = open_anim(p, b, f2, r2, d::s_AM3, &jo::self<jo::AnimRes>(this)->index_b);  // ".AM3"
            joan::orig::as_anim_def(jo::view<void*>(jo::as_anim_slot(p)->anim))->load_streamed(&r1, joan::orig::arg::u32(size1), &r2, joan::orig::arg::u32(size2), joan::orig::arg::str(jo::as_anim_slot(p)->path), joan::orig::arg::str(jo::as_anim_slot(p)->name), static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x005b2aa0
            joan::orig::as_file_mem(&r1)->close();  // 0x005886d0
            joan::orig::as_file_mem(&r2)->close();
            joan::orig::as_file(&f1)->file_close();  // 0x00587260
            joan::orig::as_file(&f2)->file_close();
            joan::orig::as_file_mem(&r2)->destruct();  // 0x00587fd0
            joan::orig::as_file_mem(&r1)->destruct();
            joan::orig::as_file(&f2)->destruct();  // 0x005872b0
            joan::orig::as_file(&f1)->destruct();
        }
        return jo::as_anim_slot(p)->refs;
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: archive size out-param starts at 0xffffffff (garbage when 0x0059c200 fails).
F("AnimSet::acquire_417540", 0x00417540, &H::acquire);
#undef F

}  // namespace game_b111_20
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using joan::img::obj;

using i16 = std::int16_t;

JOAN_COUNTER(c_set_kind, "Rec::set_kind");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x004178e0
    void* set_kind(std::int16_t id) {
        c_set_kind.hit();
        jo::self<jo::MapObject>(this)->type_id = id;
        void* const rec = joan::orig::arg::as<void*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(static_cast<std::int32_t>(id))));  // 0x00425ce0
        jo::self<jo::MapObject>(this)->damage = jo::view<std::uint32_t>(jo::as_item_def(rec)->damage);
        jo::self<jo::MapObject>(this)->defense = jo::view<std::uint32_t>(jo::as_item_def(rec)->defense);
        jo::self<jo::MapObject>(this)->duration = jo::view<std::uint32_t>(jo::as_item_def(rec)->duration);
        jo::view<std::uint16_t>(jo::self<jo::MapObject>(this)->class_id) = jo::as_item_def(rec)->class_id;
        jo::view<std::uint16_t>(jo::self<jo::MapObject>(this)->item_type) = jo::as_item_def(rec)->type_id;
        jo::self<jo::MapObject>(this)->busy = 0;
        jo::self<jo::MapObject>(this)->scale = 0x3f800000u;  // 1.0f
        reinterpret_cast<std::uint32_t&>(jo::self<jo::MapObject>(this)->sound_slots[4]) = 0;  // sound_slots[4..5]
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Rec::set_kind", 0x004178e0, &H::set_kind);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01_2 {

using u8 = std::uint8_t;

struct H {
    // ------------------------------------------------------------------------ 0x00417980
    void* animset_ctor() {
        joan::orig::as_file(this)->construct();  // 0x004179c0
        jo::as_res_idx(this)->init_flag = 0;
        return this;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("AnimSet::AnimSet", 0x00417980, &H::animset_ctor);
#undef B93

}  // namespace game_b93_01_2
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {

struct H {
    // ---------------------------------------------------------------- destructors / ctors
    void* dtor_417a30() {
        joan::orig::as_res_idx_l(this)->deinit();  // 0x0059bb40
        joan::orig::as_file(this)->destruct();  // 0x005872b0
        return this;  // EAX: 0x005872b0 -> 0x00587260 leaves this
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Archive::~Archive", 0x00417a30, &H::dtor_417a30);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx_2 {

struct H {
    // List: +0xc count, +0x10 stride, +0x19 data.
    std::int32_t list_count_() { return jo::self<jo::List>(this)->last_ele; }  // 0x00417ad0
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("List::count", 0x00417ad0, &H::list_count_);
#undef FX1

}  // namespace game_fx_2
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::str;

struct alignas(4) Path {  // 0x005a1170 string, NUL at +0x190
    std::uint8_t b[0x194];
};

using joan::orig::helpers::game_new;  // operator new

void path_init(Path& p) { joan::orig::as_string(&p)->construct(); }          // 0x005a1170
void path_set(Path& p, const void* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void path_add(Path& p, const void* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }  // 0x005a1550

JOAN_COUNTER(c_anim_stream, "AnimResMgr::stream");

struct H {
    std::int32_t anim_add_ref(std::int16_t id) {  // 0x00417af0
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_res_mgr(this)->slot_4174f0(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x004174f0
        jo::as_anim_slot(e)->refs = jo::as_anim_slot(e)->refs + 1;
        if (jo::as_anim_slot(e)->refs == 1) jo::as_anim_slot(e)->load_state = 1;
        return jo::as_anim_slot(e)->refs;
    }
    std::int32_t anim_idle(std::int16_t id) {  // 0x00417db0
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_res_mgr(this)->slot_4174f0(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x004174f0
        return jo::as_anim_slot(e)->load_state == 0 ? 1 : 0;
    }

    // ---- 0x00417b60 AnimResMgr::stream(id) ----------------------------------------------------
    void anim_stream(std::int16_t id) {
        c_anim_stream.hit();
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_res_mgr(this)->slot_4174f0(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x004174f0
        if (jo::as_anim_slot(e)->load_state == 0) return;
        if (jo::as_anim_slot(e)->load_state == 1) {
            if (jo::as_anim_slot(e)->anim != nullptr) {
                jo::as_anim_slot(e)->load_state = 0;
                return;
            }
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::AnimDef)));  // 0x18b on x86
            void* const loader = mem != nullptr ? joan::orig::as_anim_def(mem)->construct() : nullptr;  // 0x005b2390
            jo::view<void*>(jo::as_anim_slot(e)->anim) = loader;
            Path am2, am3;  // built and unused, as in the original
            path_init(am2);
            path_init(am3);
            path_set(am2, jo::as_anim_slot(e)->path);
            path_add(am2, jo::as_anim_slot(e)->name);
            path_add(am2, str(d::s_AM2_9cf5bc));  // ".AM2"
            path_set(am3, jo::as_anim_slot(e)->path);
            path_add(am3, jo::as_anim_slot(e)->name);
            path_add(am3, str(d::s_AM3));  // ".AM3"
            joan::orig::as_anim_def(jo::view<void*>(jo::as_anim_slot(e)->anim))->set_source(joan::orig::arg::str(jo::as_anim_slot(e)->path), joan::orig::arg::str(jo::as_anim_slot(e)->name));  // 0x005b2f70
            jo::as_anim_slot(e)->load_state = 2;
            return;
        }
        if (jo::as_anim_slot(e)->load_state != 2) return;
        joan::orig::as_anim_def(jo::view<void*>(jo::as_anim_slot(e)->anim))->stream_update();  // 0x005b3100
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_reader(jo::view<void*>(jo::as_anim_slot(e)->anim))->done()) != 0) jo::as_anim_slot(e)->load_state = 0;  // 0x005b4140
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("AnimResMgr::stream", 0x00417b60, &H::anim_stream);
B("AnimResMgr::add_ref", 0x00417af0, &H::anim_add_ref);
B("AnimResMgr::idle", 0x00417db0, &H::anim_idle);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b83_07_2.cpp (game/b83_07)
namespace {
namespace game_b83_07_2 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::call::stdcall_call;
JOAN_COUNTER(c_anim_release, "AnimResMgr::release");

void sleep_5() { stdcall_call<std::uint32_t>(global<std::uintptr_t>(d::iat_KERNEL32_SleepEx), 5u, 1u); }  // SleepEx(5, TRUE)
using joan::orig::helpers::game_delete;                      // operator delete

struct H {
    // ------------------------------------------------------------------ 0x00417e00
    std::int32_t anim_release(std::int16_t id) {
        c_anim_release.hit();
        std::uint8_t* const r = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_res_mgr(this)->slot_4174f0(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x004174f0
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_res_mgr(this)->idle(joan::orig::arg::i16(id))) == 0) {  // 0x00417db0
            sleep_5();
            joan::orig::as_anim_res_mgr(this)->stream(joan::orig::arg::i16(id));  // 0x00417b60
        }
        --jo::as_anim_slot(r)->refs;
        if (jo::as_anim_slot(r)->refs == 0) {
            if (jo::as_anim_slot(r)->anim != nullptr) {
                void* const p = jo::view<void*>(jo::as_anim_slot(r)->anim);
                if (p != nullptr) joan::orig::as_anim(p)->scalar_delete(1u);  // 0x00417250
                jo::as_anim_slot(r)->anim = nullptr;
            }
            if (jo::as_anim_slot(r)->buffer != nullptr) {
                game_delete(jo::as_anim_slot(r)->buffer);
                jo::as_anim_slot(r)->buffer = nullptr;
            }
        }
        return jo::as_anim_slot(r)->refs;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("AnimResMgr::release", 0x00417e00, &H::anim_release);
#undef B

}  // namespace game_b83_07_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::AnimResMgr::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_01::H::ctor_416f60>(this)); }  // 0x416f60
auto joan::orig::AnimResMgr::free() -> void { joan::bind::member<&::game_b93_01::H::animres_free>(this); }  // 0x417060
auto joan::orig::ErrorLog::set_idle_callbacks(std::uintptr_t a0, std::uintptr_t a1) -> void { joan::bind::member<&::game_b96_01::H::set_4_8>(this, a0, a1); }  // 0x4171b0
auto joan::orig::Anim::scalar_delete(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::delete_418>(this, a0)); }  // 0x417250
auto joan::orig::AnimResMgr::load() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b108_20::H::animres_load>(this)); }  // 0x4172a0
auto joan::orig::Screen::reset_slots() -> void { joan::bind::member<&::game_b94_01::H::reset_slots>(this); }  // 0x417410
auto joan::orig::AnimResMgr::add_slot(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b85_06::H::dtor_417470>(this, a0)); }  // 0x417470
auto joan::orig::AnimResMgr::slot_4174f0(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18_2::H::get_4174f0>(this, a0)); }  // 0x4174f0
auto joan::orig::AnimSet::acquire_417540(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b111_20::H::acquire>(this, a0)); }  // 0x417540
auto joan::orig::MapObject::set_kind(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_03::H::set_kind>(this, a0)); }  // 0x4178e0
auto joan::orig::AnimSet::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b93_01_2::H::animset_ctor>(this)); }  // 0x417980
auto joan::orig::Archive::dtor() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_leaves82b::H::dtor_417a30>(this)); }  // 0x417a30
auto joan::orig::AnimResMgr::slot_count() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_05::H::call_417aa0>(this)); }  // 0x417aa0
auto joan::orig::AnimResMgr::add_ref(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::anim_add_ref>(this, a0)); }  // 0x417af0
auto joan::orig::AnimResMgr::stream(std::int16_t a0) -> void { joan::bind::member<&::game_b84_05::H::anim_stream>(this, a0); }  // 0x417b60
auto joan::orig::AnimResMgr::idle(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::anim_idle>(this, a0)); }  // 0x417db0
auto joan::orig::AnimResMgr::release(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07_2::H::anim_release>(this, a0)); }  // 0x417e00
