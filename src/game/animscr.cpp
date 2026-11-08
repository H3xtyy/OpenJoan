// game/animscr.cpp: functions of the original d:\ww\animscr.cpp (re/attribution.csv).
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
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

}  // namespace

// ---------------------------------------- from modules/game_b107_20.cpp (game/b107_20)
//
//    - AnimBuf::release 0x00418e00 (ret 0): when byte +6 == 1: 0x00418ec0, then if +0 != 0
//      Mem::free 0x0058ed70(+0, "d:\ww\animscr.cpp", [0x9f6a68] + 7), +0 = 0, word +4 = 0;
//      byte +6 = 0. The eax the original leaves (this, or 0x00418ec0's eax when +0 == 0) is
//      returned.
namespace {
namespace game_b107_20 {

using i8 = std::int8_t;

struct H {
    // 0x00418e00
    u32 anim_release() {
        u32 r = joan::call::register_low_bits(this);
        if (jo::self<jo::AnimBuf>(this)->state == 1) {
            r = joan::orig::arg::as<u32>(joan::orig::as_model_refs(this)->destruct_entries());  // 0x00418ec0
            if (jo::self<jo::AnimBuf>(this)->data != 0) {
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<jo::AnimBuf>(this)->data), joan::orig::arg::str(&img::global<const char>(d::s_d_ww_animscr_cpp)), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_9f6a68)) + 7));  // 0x0058ed70
                jo::self<jo::AnimBuf>(this)->data = 0;
                jo::self<jo::AnimBuf>(this)->ref_count = 0;
                r = joan::call::register_low_bits(this);
            }
            jo::view<u8>(jo::self<jo::AnimBuf>(this)->state) = 0;
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_20", name, addr, fn)
B("AnimBuf::release", 0x00418e00, &H::anim_release);
#undef B

}  // namespace game_b107_20
}  // namespace

// ---------------------------------------- from modules/game_b106_14.cpp (game/b106_14)
//
//    - AnimScript::stop_and_mark 0x00418e90 (thiscall, ret 0): 0x00418f30 on this, byte +6 = 1;
//      returns this (the eax the original leaves; the only caller ignores it).
namespace {
namespace game_b106_14 {

struct H {
    // 0x00418e90
    void* stop_and_mark() {
        joan::orig::as_model_refs(this)->load();  // 0x00418f30
        jo::view<u8>(jo::self<jo::AnimScript>(this)->state) = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_14", name, addr, fn)
B("AnimScript::stop_and_mark", 0x00418e90, &H::stop_and_mark);
#undef B

}  // namespace game_b106_14
}  // namespace

// ---------------------------------------- from modules/game_b106_07.cpp (game/b106_07)
//
//      0x00418ec0 AnimScr::for_each_418d80      (thiscall, ret 0) - for i = 1..(short)[+4]:
//                 0x00416f20(i)->0x00418d80(). Returns eax = (short)[+4] (last loop compare).
namespace {
namespace game_b106_07 {

struct H {
    // 0x00418ec0
    i32 for_each_418d80() {
        i16 i = 1;
        for (;;) {
            const i32 count = jo::view<i16>(jo::self<jo::AnimScr>(this)->ref_count);
            if (static_cast<i32>(i) > count) return count;
            void* e = joan::orig::arg::as<void*>(joan::orig::as_model_refs(this)->get(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x00416f20
            joan::orig::as_model_slot(e)->destruct();                                     // 0x00418d80
            i = static_cast<i16>(i + 1);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_07", name, addr, fn)
B("AnimScr::for_each_418d80", 0x00418ec0, &H::for_each_418d80);
#undef B

}  // namespace game_b106_07
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
//
//    - AnimScr::load 0x00418f30 (ret 0): new AnimScrRes (0x135, ctor 0x004179f0), path
//      "RESOURCE\\animscr.res" (0x005a1170 / 0x005a1440 / 0x005a1550 / 0x00417d90) loaded by
//      0x0059b920(path, 1, 0); +4 = word res+0x121; +0 = alloc(+4 * 0x21b, animscr.cpp,
//      [0x9f6a6c] + 0xb) (0x0058e7b0 on 0xa48838), memset 0; for i = 1..res+0x121: record
//      word = i, new (0x19a) 0x00589780(0x0059bf70(i), 0x0059bdd0(i)), 0x00419320(obj, rec),
//      scalar delete (0x00419280, 1); finally scalar delete of res (0x004192d0, 1).
//      Returns that delete's eax (0 when res is null). SEH frame: normal path only.
//
//  - 0x00418f30: 0x00417d90 (path getter) takes no arguments; the `push 0; push 1` before it
//    are the trailing args of 0x0059b920. The path object (0x191 bytes written by strncpy
//    0x190 + NUL) gets the original's 0x194-byte slot.
namespace {
namespace game_b105_01 {

using joan::call::cdecl_call;
using joan::img::str;

JOAN_COUNTER(c_animscr_load, "AnimScr::load");

struct H {
    // ------------------------------------------------------------------ 0x00418f30
    std::uintptr_t animscr_load() {
        c_animscr_load.hit();
        void* res = nullptr;
        if (void* mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::ResourceIdxL))))  // operator new (0x135 on x86)
            res = joan::orig::as_res_idx(mem)->construct();                 // 0x004179f0
        alignas(4) char path[0x194];
        joan::orig::as_string(path)->construct();                  // 0x005a1170
        joan::orig::as_string(path)->assign(str(d::s_RESOURCE));                  // 0x005a1440 "RESOURCE\\"
        joan::orig::as_string(path)->append(str(d::s_animscr_res));                      // 0x005a1550 "animscr.res"
        void* const name = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        joan::orig::as_res_idx_l(res)->init(joan::orig::arg::str(name), static_cast<char>(i32{1}), static_cast<char>(i32{0}));  // 0x0059b920
        jo::self<jo::AnimScr>(this)->ref_count = reinterpret_cast<const u16&>(jo::as_res_idx_l(res)->rec_count);
        const i32 line = static_cast<i32>(global<i16>(d::g_9f6a6c)) + 0xb;
        const i32 bytes = static_cast<i32>(jo::view<i16>(jo::self<jo::AnimScr>(this)->ref_count)) * 0x21b;
        jo::self<jo::AnimScr>(this)->data = joan::orig::arg::as<jo::ModelSlot*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(bytes), str(d::s_d_ww_animscr_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        std::memset(joan::orig::arg::mptr(reinterpret_cast<void*>(jo::self<jo::AnimScr>(this)->data)), i32{0}, joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::self<jo::AnimScr>(this)->ref_count)) * 0x21b));  // memset
        for (i32 i = 1; i <= jo::as_res_idx_l(res)->rec_count; ++i) {
            void* const rec = static_cast<void*>(jo::self<jo::AnimScr>(this)->data + (i - 1));  // AnimBuf::data: ModelSlot[] (0x21b-byte records)
            jo::as_model_slot(rec)->id = static_cast<u16>(i);
            void* item = nullptr;
            if (void* mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::FileTxt)))) {  // operator new (0x19a on x86)
                const u32 a = joan::orig::arg::as<u32>(joan::orig::as_res_idx_l(res)->get_data_size(joan::orig::arg::i32(i)));  // 0x0059bdd0
                const char* const b = joan::orig::arg::as<const char*>(joan::orig::as_res_idx_l(res)->read_rec(joan::orig::arg::i32(i)));  // 0x0059bf70
                item = joan::orig::arg::as<void*>(joan::orig::as_file_txt(mem)->construct_mem(joan::orig::arg::str(b), joan::orig::arg::i32(a)));  // 0x00589780
            }
            joan::orig::as_model_refs(this)->parse_entry(joan::orig::arg::mptr(item), joan::orig::arg::mptr(rec));  // 0x00419320
            if (item != nullptr) joan::orig::as_file_txt(item)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00419280
        }
        if (res != nullptr) return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_res_idx(res)->destroy(joan::orig::arg::u32(i32{1})));  // 0x004192d0
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("AnimScr::load", 0x00418f30, &H::animscr_load);
#undef B

}  // namespace game_b105_01
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
//
//    - Obj::ctor_4179f0 0x004179f0: base 0x004179c0, byte +0x11e = 0. Returns this.
namespace {
namespace game_b90_03 {

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x004179f0
    void* ctor_4179f0() {
        joan::orig::as_file(self())->construct();  // 0x004179c0
        jo::as_res_idx(self())->init_flag = 0;
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::ctor_4179f0", 0x004179f0, &H::ctor_4179f0);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
//
//  * module game/leaves80a:
//      Font::~Font 0x00623e70 (empty), String::c_str 0x00417d90 (returns this),
//      Table::last_slot 0x00417520 (+0x08), SoundEntry::id 0x004d0440 (+0x04),
//      SceneNode::attachments 0x005efff0 (+0xce), Textures::mode / set_mode 0x0060a720 /
//      0x0060a740 (+0x30), File::File 0x004179c0 (+0x105 = -1), Bytes::at 0x0041bf10 (byte
//      [this + i + 1]), SoundList::entry 0x004cf690 (this + i * 0x18 - 4), Sound::play / pause
//      0x0059f620 / 0x0059f650 (OpenAL alSourcePlay / alSourcePause), Reader::done 0x005b4140
//      (byte +1 == 0), Light::params 0x0061ddc0 (copies 0x68 bytes from +0x5e), Actor::position
//      0x005ac270 (bone matrix translation), Loader::Loader 0x00577be0 (zeroed, +0x13c = -1),
//      `vector constructor iterator' 0x00449af0 (stdcall), scalar deleting destructor 0x00459c40
//      (destructor 0x005cc750), Progress::redraw 0x0075f4f0 (Game::draw 0x004d48f0 on the
//      loading screen), Model::update_parts 0x005b77b0, Unit::attach_a / attach_b 0x00421960 /
//      0x005227f0 (adds the +0x4e / +0x4a object to the world's scene list +0x6e),
//      Texture::set_name 0x00605ad0, the float sqrt wrapper 0x004256d0 (CRT sqrt 0x007b2344
//      runs fsqrt under the caller's precision control, so its st0 is exactly a double: the
//      C++ double return is exact; the negative-argument result is the x87 indefinite NaN,
//      which also survives the double).
namespace {
namespace game_leaves80a {

struct H {
    void* c_str() { return this; }                            // 0x00417d90
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("String::c_str", 0x00417d90, &H::c_str);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
//
//    - 0x00418dd0 Obj::fwd_418e00        (thiscall, ret 0) -> eax of 0x00418e00(this).
namespace {
namespace game_b108_16 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // --------------------------------------------------------------------- 0x00418dd0
    u32 fwd_418e00() { return joan::orig::arg::as<u32>(joan::orig::as_model_refs(this)->release()); }  // 0x00418e00
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Obj::fwd_418e00", 0x00418dd0, &H::fwd_418e00);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
//
//  * module game/b98_01:
//      0x00416600 / 0x004192d0 / 0x0045e930 / 0x0046ed90  Obj::scalar_dtor(flags)  (thiscall, ret 4)
namespace {
namespace game_b98_01 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::orig::helpers::delete_;       // operator delete

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    H* sdtor(std::uintptr_t dtor, u32 flags) {
        thiscall(dtor, this);
        if ((flags & 1) != 0) delete_(this);
        return this;
    }
    H* sdtor_4192d0(u32 flags) { return sdtor(fn::code_value(fn::Archive_dtor), flags); }  // 0x00417a30
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::scalar_dtor_4192d0", 0x004192d0, &H::sdtor_4192d0);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b104_01.cpp (game/b104_01)
namespace {
namespace game_b104_01 {

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_parse_419320, "Emitter::parse_419320");

char* tok(void* rd) { return joan::orig::as_file_txt(rd)->get_token(1); }  // 0x00589c70
i32 game_atoi(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }
i32 game_cmp(const char* a, img::Addr b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(str(b))); }
void game_strcpy(void* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
// strncpy without a terminator into the fixed-size field, as the original does
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
void game_strncpy(void* d, const char* s, u32 n) { std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

void parse_body(void* rd, u8* out) {
    char* t = tok(rd);
    t = tok(rd);
    (void)game_atoi(t);  // dead result ([ebp-0x10])
    jo::as_model_slot(out)->gravity_level = 0x40400000;
    jo::as_model_slot(out)->attack_force = 0xbb8;
    jo::as_model_slot(out)->vertical_attack_force = 0x1f4;
    for (;;) {
        t = tok(rd);
        if (game_cmp(t, d::s_Code) == 0) {  // 'Code'
            char* s = tok(rd);
            game_strncpy(out + 2, s, 0x20);
            continue;
        }
        bool done = false;
        // ModelSlot::times[k] (+0x28 + k * 4)
        static const img::Addr ints1[8] = {d::s_StartTime, d::s_EndTime, d::s_Range, d::s_StartForceTime,
                                           d::s_StartVerticalAngle, d::s_EndVerticalAngle, d::s_StartHorizontalAngle,
                                           d::s_EndHorizontalAngle};
        for (int k = 0; k < 8; ++k) {
            if (game_cmp(t, ints1[k]) == 0) {
                jo::as_model_slot(out)->times[k] = game_atoi(tok(rd));
                done = true;
                break;
            }
        }
        if (done) continue;
        if (game_cmp(t, d::s_GravityLevel) == 0) {  // 'GravityLevel'
            char* s = tok(rd);
            jo::view<float>(jo::as_model_slot(out)->gravity_level) = static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))));  // atof
            continue;
        }
        if (game_cmp(t, d::s_SoundScriptName) == 0) {  // 'SoundScriptName'
            char* s = tok(rd);
            game_strcpy(out + 0x54, s);
            continue;
        }
        if (game_cmp(t, d::s_AttackForce) == 0) {  // 'AttackForce'
            jo::view<i32>(jo::as_model_slot(out)->attack_force) = game_atoi(tok(rd));
            continue;
        }
        if (game_cmp(t, d::s_VerticalAttackForce) == 0) {  // 'VerticalAttackForce'
            jo::view<i32>(jo::as_model_slot(out)->vertical_attack_force) = game_atoi(tok(rd));
            continue;
        }
        if (game_cmp(t, d::s_EmitterFlag) == 0) {  // 'EmitterFlag'
            const u8 flag = static_cast<u8>(game_atoi(tok(rd)));
            jo::as_model_slot(out)->emitter_flag = static_cast<i8>(flag);
            if (static_cast<i8>(flag) == 1) {
                t = tok(rd);
                jo::as_model_slot(out)->emitter_default = static_cast<i8>(game_atoi(tok(rd)));
                for (i32 i = 0; i < 6; ++i) {
                    t = tok(rd);
                    {
                        char* s = tok(rd);
                        game_strcpy(jo::as_model_slot(out)->emitter_names + i * 0x21, s);
                    }
                    t = tok(rd);
                    jo::as_model_slot(out)->emitter_a[i] = game_atoi(tok(rd));
                    t = tok(rd);
                    jo::as_model_slot(out)->emitter_b[i] = game_atoi(tok(rd));
                    if (jo::as_model_slot(out)->emitter_default != 0) {
                        t = tok(rd);
                        jo::as_model_slot(out)->emitter_c[i] = game_atoi(tok(rd));
                    }
                    t = tok(rd);
                    jo::as_model_slot(out)->emitter_pos[i * 3] = static_cast<float>(game_atoi(tok(rd)));
                    t = tok(rd);
                    jo::as_model_slot(out)->emitter_pos[i * 3 + 1] = static_cast<float>(game_atoi(tok(rd)));
                    t = tok(rd);
                    jo::as_model_slot(out)->emitter_pos[i * 3 + 2] = static_cast<float>(game_atoi(tok(rd)));
                    t = tok(rd);
                    jo::as_model_slot(out)->emitter_bytes[i] = static_cast<i8>(game_atoi(tok(rd)));
                    t = tok(rd);
                    jo::as_model_slot(out)->emitter_sel[i] = static_cast<i8>(game_atoi(tok(rd)));
                }
            }
            continue;
        }
        if (game_cmp(t, d::s_AttachModel) == 0) {  // 'AttachModel'
            t = tok(rd);
            jo::view<u32>(jo::as_model_slot(out)->attach_model) =
                joan::orig::arg::as<u32>(joan::orig::as_model_res(obj(d::g_model_res_mgr))->add(str(d::s_ITEM_9cf648), joan::orig::arg::str(t), 0));  // 0x004b18d0 'ITEM\'
            t = tok(rd);
            char buf[0x24];  // [ebp-0x38] .. [ebp-0x14]; read_line writes its NUL at buf[0x20]
            joan::orig::as_file_txt(rd)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), 0x20);  // 0x00589f10
            game_strcpy(jo::as_model_slot(out)->anim_name, buf);
            t = tok(rd);
            t = tok(rd);
            game_strncpy(&jo::as_model_slot(out)->u8_202, t, 1);
            continue;
        }
        if (game_cmp(t, d::s_END) == 0) break;  // 'END'
    }
    joan::orig::as_model_slot(out)->load_txt_ids();  // 0x00418c60
}

struct H {
    // ------------------------------------------------------------ 0x00419320
    void parse_419320(void* rd, u8* out) {
        c_parse_419320.hit();
        parse_body(rd, out);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_01", name, addr, fn)
B("Emitter::parse_419320", 0x00419320, &H::parse_419320);
#undef B

}  // namespace game_b104_01
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
//
//      0x00419280 Record::scalar_deleting_dtor(flags)          thiscall, ret 4 (dtor 0x00589890)
namespace {
namespace game_b91_02 {

using joan::call::cdecl_call;

struct H {
    // 0x00419280
    H* scalar_deleting_dtor(std::uint32_t flags) {
        joan::orig::as_file_txt(this)->destruct();  // 0x00589890
        if (flags & 1) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Record::scalar_deleting_dtor", 0x00419280, &H::scalar_deleting_dtor);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
namespace {
namespace game_b97_01 {

using joan::hook::global;

using joan::call::cdecl_call;

// ---------------------------------------------------------------------- static initialisers
void init_pi(img::Addr g) {
    const double t = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    global<float>(g) = static_cast<float>(t * 4.0);        // qword 0x9cf788 = 4.0 (exact)
}
using joan::orig::helpers::div_global;
static void JOAN_CDECL init_pi_a4f81c() { init_pi(d::g_a4f81c); }  // 0x00417ef0
static void JOAN_CDECL init_a59734() { div_global(d::g_a59734, d::g_a4f81c, d::flt_2); }  // 0x00418d40 (/2)

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("init_pi_a4f81c", 0x00417ef0, &init_pi_a4f81c);
B97("init_a59734", 0x00418d40, &init_a59734);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using joan::call::import_call;
struct V3 {
    float x, y, z;
};

using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_bind_bip, "AnimResMgr::bind_bip");

struct H {
    std::int32_t call_5b4170() {  // 0x00418c30 -> 0x005b4170
        joan::orig::as_anim_def(this)->release(0u);  // 0x005b4170
        return 0;  // AnimDef::release returns nothing; no caller of 0x00418c30 (atexit stub, unwind funclets, dtors) reads eax
    }

    // ---- 0x00417f20 AnimResMgr::bind_bip(id, name) ----------------------------------------------------
    void bind_bip(std::int16_t id, const char* name) {
        c_bind_bip.hit();
        std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_res_mgr(this)->slot_4174f0(joan::orig::arg::u32(static_cast<std::int32_t>(id))));  // 0x004174f0
        std::uint8_t* const m = jo::view<std::uint8_t*>(jo::as_anim_slot(e)->anim);
        if (!(jo::as_anim_def(m)->action_count > 0)) return;
        if (jo::view<void*>(jo::as_anim_def(m)->bones) == nullptr) return;
        const std::int32_t bip = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_table(jo::view<void*>(jo::as_anim_def(m)->bones))->index_of(joan::orig::arg::mptr(str(d::s_Bip01))));  // 0x005ba110 "Bip01"
        if (bip == -999) return;
        if (jo::as_anim_slot(e)->buffer == nullptr) {
            jo::as_anim_slot(e)->buffer = game_new(static_cast<std::uint32_t>(jo::as_anim_def(m)->action_count * static_cast<std::int32_t>(sizeof(jo::Curve))));
            for (std::int32_t i = 0; i < jo::as_anim_def(m)->action_count; ++i)
                joan::orig::as_path(static_cast<jo::Curve*>(jo::as_anim_slot(e)->buffer) + i)->parse(joan::orig::arg::str(0));  // 0x004185f0
        }
        std::int16_t w18 = 0, w1c = 0;
        joan::orig::as_anim_def(m)->find_action_rec(joan::orig::arg::mptr(static_cast<void*>(&w1c)), joan::orig::arg::mptr(static_cast<void*>(&w18)), name, static_cast<std::uint8_t>(joan::orig::arg::u32(0)), 0u);  // 0x005b4a60
        std::uint8_t* const track = jo::view<std::uint8_t*>(jo::as_anim_def(m)->tracks) + bip * static_cast<std::int32_t>(sizeof(jo::Track));  // 8-byte Track records on x86
        void* const slot = static_cast<jo::Curve*>(jo::as_anim_slot(e)->buffer) + (static_cast<std::int32_t>(w1c) - 1);
        std::uint8_t* const bone = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(m)->frame(static_cast<std::int32_t>(w1c), joan::orig::arg::u32(w18)));  // 0x005b5dd0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_path(slot)->has_points()) != 0) return;  // 0x00418400
        std::uintptr_t key = 0, next = 0;  // KeyFrame pointers (0x005b7c70 stores void*)
        const float frac = joan::orig::arg::as<float>(joan::orig::as_track(track)->sample(joan::orig::arg::f32(jo::view<std::uint32_t>(jo::as_anim_action(bone)->start)), joan::orig::arg::mptr(static_cast<void*>(&key)), joan::orig::arg::mptr(static_cast<void*>(&next)), joan::orig::arg::mptr(static_cast<void*>(nullptr))));  // 0x005b7c70
        if (frac > global<float>(d::flt_0_99)) key = next;
        alignas(4) std::uint8_t mat[0x40];
        std::memcpy(mat, img::obj(d::g_identity_matrix), sizeof mat);
        const std::int32_t mother = joan::orig::arg::as<std::int32_t>(joan::orig::as_bone_table(jo::view<void*>(jo::as_anim_def(m)->bones))->index_of(joan::orig::arg::mptr(str(d::s_Mother))));  // "Mother"
        if (mother != -999) {
            std::uint8_t* const mt = jo::view<std::uint8_t*>(jo::as_anim_def(m)->tracks) + mother * static_cast<std::int32_t>(sizeof(jo::Track));  // 8-byte Track records on x86
            if (jo::as_track(mt)->key_count > 0) joan::orig::as_anim_key(jo::view<void*>(jo::as_track(mt)->keys))->to_matrix(joan::orig::arg::mptr(static_cast<void*>(mat)));  // 0x005b8910
        }
        std::int32_t idx = static_cast<std::int32_t>(static_cast<std::uint32_t>(key - reinterpret_cast<std::uintptr_t>(jo::as_track(track)->keys))) >> 5;  // byte difference narrowed, then / 0x20 (the key record size)
        V3 first, first_t;
        std::memcpy(&first, img::obj(d::g_pos), 12);
        std::memcpy(&first_t, img::obj(d::g_pos), 12);
        if (idx < jo::as_track(track)->key_count) {
            std::memcpy(&first, jo::as_track(track)->keys[idx].position, 12);
            import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&first_t), static_cast<void*>(&first), static_cast<void*>(mat));  // point_transform
        }
        for (; idx < jo::as_track(track)->key_count; ++idx) {
            const float t = jo::view<float>(jo::as_track(track)->keys[idx].time);
            const float lo = jo::as_anim_action(bone)->start - global<float>(d::flt_0_001);
            if (lo > t) continue;
            const float hi = jo::as_anim_action(bone)->end + global<float>(d::flt_0_001);
            if (!(hi >= t)) return;
            const float start = jo::as_anim_action(bone)->start;
            V3 p, pt;
            std::memcpy(&p, jo::as_track(track)->keys[idx].position, 12);
            import_call<void*>(d::iat_v3d_87_point_transform_3, static_cast<void*>(&pt), static_cast<void*>(&p), static_cast<void*>(mat));
            float dt = t - start;
            if (!(dt >= global<float>(d::k_9cf5c4))) dt = 0.0f;
            const float dy = pt.y - first_t.y;
            const float dx = pt.x - first_t.x;
            joan::orig::as_path(slot)->add_point(joan::orig::arg::f32(dt), joan::orig::arg::f32(dx), joan::orig::arg::f32(dy), 1);  // 0x00418820
            std::memcpy(jo::as_track(track)->keys[idx].position, &first.x, 4);
            std::memcpy(&jo::as_track(track)->keys[idx].position[1], &first.y, 4);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("AnimResMgr::bind_bip", 0x00417f20, &H::bind_bip);
B("Obj::call_5b4170", 0x00418c30, &H::call_5b4170);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b98_01_2.cpp (game/b98_01)
namespace {
namespace game_b98_01_2 {

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using u32 = std::uint32_t;

using i16 = std::int16_t;

struct alignas(4) Path {  // 0x005a11a0 path string (frame slot 0x194 bytes)
    u8 bytes[0x194];
};

JOAN_COUNTER(c_copy_slot, "Group::copy_slot");

i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

struct H {

    // ------------------------------------------------------------------ 0x00418310
    u8 copy_slot(i32 idx_arg) {
        c_copy_slot.hit();
        const i32 idx = sx16(idx_arg);
        if (idx == 0) return 0;
        if (idx >= 10) return 0;
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0])) == 0) return 0;
        for (i32 i = 0; i < static_cast<i32>(jo::view<i8>(jo::self<jo::Player>(this)->group_counts[0])); ++i) {
            jo::self<jo::Player>(this)->group_units[idx * 100 + i] = jo::self<jo::Player>(this)->group_units[i];  // 100 units per group
        }
        jo::self<jo::Player>(this)->group_counts[idx] = jo::self<jo::Player>(this)->group_counts[0];
        Path path;                                                         // ebp-0x19c
        joan::orig::as_string(&path)->construct(str(d::s_RTS_NARRATOR_001));                   // 0x005a11a0 "RTS_NARRATOR_001"
        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
        joan::orig::as_sound(obj(d::g_sfx_player))->play_kind(s);                     // 0x00492c40
        return 1;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Group::copy_slot", 0x00418310, &H::copy_slot);
#undef B98

}  // namespace game_b98_01_2
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::hook::global;

using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

void* const kFiles = obj(d::g_misc);      // file / text helpers

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;
std::uint32_t strlen_(const char* s) { return joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(s))); }
char* strncpy_(char* d, const char* s, std::uint32_t n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
std::uint32_t strcspn_(const char* s, const char* set) { return joan::orig::arg::as<std::uint32_t>(joan::orig::libc::strcspn(joan::orig::arg::str(s), joan::orig::arg::str(set))); }
using joan::orig::helpers::stricmp_;
double fabs_(double v) { return joan::orig::arg::as<double>(joan::orig::libc::fabs(joan::orig::arg::f64(v))); }

JOAN_COUNTER(c_path_parse, "Path::parse");
JOAN_COUNTER(c_path_add, "Path::add_point");
JOAN_COUNTER(c_path_find, "Path::find");
JOAN_COUNTER(c_path_init, "Path::init");

struct H {
    // ---- paths ---------------------------------------------------------------------------
    void path_parse(const char* text) {  // 0x004185f0
        c_path_parse.hit();
        jo::self<jo::Curve>(this)->count = 0;
        if (text == nullptr) return;
        const std::int32_t len = static_cast<std::int32_t>(strlen_(text));
        const char* const delims = str(d::s_comma_semi);  // ",;"
        std::int32_t i = 0;
        while (i < len) {
            float v[3] = {0.0f, 0.0f, 0.0f};
            char ch = 0;
            for (std::int32_t k = 0; k < 3; ++k) {
                if (k > 0 && ch != 0x2c) break;
                const std::int32_t n = static_cast<std::int32_t>(strcspn_(text + i, delims));
                v[k] = static_cast<float>(joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(kFiles)->atoi_field_590270(joan::orig::arg::str(text + i), joan::orig::arg::u32(n))));  // 0x00590270
                i += n;
                if (i < len) {
                    ch = text[i];
                    ++i;
                } else {
                    ch = 0;
                }
                if (k == 0 && ch == 0) return;
            }
            joan::orig::as_path(this)->add_point(joan::orig::arg::f32(v[0]), joan::orig::arg::f32(v[1]), joan::orig::arg::f32(v[2]), 0);  // 0x00418820
        }
    }

    void path_add(float x, float y, float z, std::int32_t merge) {  // 0x00418820
        c_path_add.hit();
        if (merge != 0 && jo::self<jo::Curve>(this)->count > 0) {
            float zero[3] = {0.0f, 0.0f, 0.0f};  // ebp-0x10
            const float* const p = jo::self<jo::Curve>(this)->count > 1 ? &jo::self<jo::Curve>(this)->points[(jo::self<jo::Curve>(this)->count - 2) * 3] : zero;
            const float* const q = &jo::self<jo::Curve>(this)->points[(jo::self<jo::Curve>(this)->count - 1) * 3];
            const float t = (q[0] - p[0]) / (x - p[0]);
            const float yy = (y - p[1]) * t + p[1];
            const float zz = (z - p[2]) * t + p[2];
            const float dy = q[1] - yy;
            if (!(fabs_(static_cast<double>(dy)) > global<double>(d::dbl_1))) {
                const float dz = q[2] - zz;
                if (!(fabs_(static_cast<double>(dz)) > global<double>(d::dbl_1))) jo::self<jo::Curve>(this)->count -= 1;
            }
        }
        float* const e = &jo::self<jo::Curve>(this)->points[jo::self<jo::Curve>(this)->count * 3];
        e[0] = x;
        e[1] = y;
        e[2] = z;
        jo::self<jo::Curve>(this)->count += 1;
    }

    std::int32_t path_find(const char* name, const char* group) {  // 0x004184b0
        c_path_find.hit();
        for (std::int32_t i = joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_res_mgr(this)->slot_count()); i > 0; --i) {  // 0x00417aa0
            if (stricmp_(jo::as_anim_slot(joan::orig::as_anim_res_mgr(this)->slot_4174f0(joan::orig::arg::u32(i)))->name, group) != 0) continue;  // 0x004174f0
            if (stricmp_(jo::as_anim_slot(joan::orig::as_anim_res_mgr(this)->slot_4174f0(joan::orig::arg::u32(i)))->path, name) != 0) continue;
            return i;
        }
        return 0;
    }

    void* path_init(const char* name, const char* group) {  // 0x00418550
        c_path_init.hit();
        // this is an AnimSlot (0x130 bytes on x86): path +4, name +0x105
        memset_(this, 0, static_cast<std::uint32_t>(sizeof(jo::AnimSlot)));
        if (name != nullptr) {
            strncpy_(jo::self<jo::AnimSlot>(this)->path, name, 0x100);
            jo::self<jo::AnimSlot>(this)->path[0x100] = 0;
        }
        if (group != nullptr) {
            strncpy_(jo::self<jo::AnimSlot>(this)->name, group, 0x20);
            jo::self<jo::AnimSlot>(this)->name[0x20] = 0;
        }
        return this;
    }

    std::int32_t path_has_points() { return jo::self<jo::Curve>(this)->count > 0 ? 1 : 0; }  // 0x00418400
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Path::parse", 0x004185f0, &H::path_parse);
B85("Path::add_point", 0x00418820, &H::path_add);
B85("Path::find", 0x004184b0, &H::path_find);
B85("Path::init", 0x00418550, &H::path_init);
B85("Path::has_points", 0x00418400, &H::path_has_points);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::cdecl_call;

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x00418430
    i32 index_of(const char* name) {
        for (i32 i = 0; i < jo::self<jo::AnimRes>(this)->count; ++i) {
            if (stricmp_(jo::self<jo::AnimRes>(this)->entries + i * static_cast<i32>(sizeof(jo::AnimSet)) + 2, name) == 0) return i + 1;
        }
        return 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("AnimRes::index_of", 0x00418430, &H::index_of);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;

struct H {
    // ------------------------------------------------------------------ SEH dtors
    u32 dtor_4187b0() {
        joan::orig::dynarray(&jo::self<jo::AnimRes>(this)->list)->destruct();           // 0x005844b0
        joan::orig::as_archive(&jo::self<jo::AnimRes>(this)->index_b)->dtor();                // 0x00417a30
        joan::orig::as_archive(&jo::self<jo::AnimRes>(this)->index_a)->dtor();      // 0x00417a30
        return 0;  // ~Archive returns nothing; only called from the atexit stub 0x00567480 (result unused)
    }

    // ------------------------------------------------------------------ 0x00418da0 / 0x004203e0
    H* clear7() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::AnimBuf)));  // 7 on x86
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("Obj::dtor_4187b0", 0x004187b0, &H::dtor_4187b0);
B99("Obj::clear7", 0x00418da0, &H::clear7);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
namespace {
namespace game_b100_01 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;

// 0x004189a0
void JOAN_CDECL game_tick_4189a0() {
    if (global<std::int32_t>(d::g_sys3d) < 2) return;
    if (global<std::int32_t>(d::g_sys3d) >= 3) {
        if (joan::orig::arg::as<std::uint8_t>(joan::orig::as_canvas(obj(d::g_vga_back))->valid()) != 0)  // 0x0056ddc0
            joan::orig::as_bit_writer(obj(d::g_vga_back))->put();                       // 0x0060ff70
    }
    joan::orig::as_sys_en(obj(d::g_sys3d))->flush_extra();  // 0x005ff180
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("game_tick_4189a0", 0x004189a0, &game_tick_4189a0);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::hook::global;

using i32 = std::int32_t;

JOAN_COUNTER(c_curve, "Curve::lookup");

struct H {
    // ------------------------------------------------------------------ 0x00418a00
    void curve_lookup(float x, float* a, float* b) {
        c_curve.hit();
        *a = 0.0f;
        *b = 0.0f;
        if (jo::self<jo::Curve>(this)->count <= 0) return;
        auto key = [&](i32 i) -> float { return jo::self<jo::Curve>(this)->points[i * 3]; };
        auto va = [&](i32 i) -> float { return jo::self<jo::Curve>(this)->points[i * 3 + 1]; };
        auto vb = [&](i32 i) -> float { return jo::self<jo::Curve>(this)->points[i * 3 + 2]; };
        if (x >= key(jo::self<jo::Curve>(this)->count - 1)) {  // not (< or NaN)
            *a = va(jo::self<jo::Curve>(this)->count - 1);
            *b = vb(jo::self<jo::Curve>(this)->count - 1);
            return;
        }
        if (x >= key(0)) {
            for (i32 i = 1; i < jo::self<jo::Curve>(this)->count; ++i) {
                if (x >= key(i)) continue;  // C0 clear
                const float t = (x - key(i - 1)) / (key(i) - key(i - 1));
                *a = (va(i) - va(i - 1)) * t + va(i - 1);
                *b = (vb(i) - vb(i - 1)) * t + vb(i - 1);
                break;
            }
            return;
        }
        if (!(x >= global<float>(d::k_9cf5c4))) return;  // < 0 or NaN
        const float t = x / key(0);
        *a = t * va(0);
        *b = t * vb(0);
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("Curve::lookup", 0x00418a00, &H::curve_lookup);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

using joan::hook::global;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::img::obj;

using u32 = std::uint32_t;

// ------------------------------------------------------------------ 0x00418bc0
static void JOAN_CDECL tick_418bc0() {
    if (global<i32>(d::g_sys3d) < 2) return;
    joan::orig::as_sys_en(obj(d::g_sys3d))->flush_all();  // 0x005ff210
    if (global<i32>(d::g_sys3d) < 3) return;
    if (joan::orig::arg::as<u8>(joan::orig::as_canvas(obj(d::g_vga_back))->valid()) != 0)  // 0x0056ddc0
        joan::orig::as_image(obj(d::g_vga_back))->pop_lock();      // 0x0060ffd0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("Game::tick_418bc0", 0x00418bc0, &tick_418bc0);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using i8 = std::int8_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

struct alignas(4) Path {  // 0x194 bytes in the frame (0x005a1550 writes its NUL at +0x190)
    char text[0x194];
};
static_assert(sizeof(Path) == 0x194, "Path");

void* const kEvents = obj(d::g_pemi_res);

JOAN_COUNTER(c_load_txt, "Obj::load_txt_ids");

struct H {
    // ------------------------------------------------------------------ 0x00418c60
    void load_txt_ids() {
        c_load_txt.hit();
        if (jo::self<jo::ModelSlot>(this)->emitter_flag == 0) return;
        for (i32 i = 0; i < 6; ++i) {
            char* const name = jo::self<jo::ModelSlot>(this)->emitter_names + i * 0x21;
            if (joan::crt::stricmp(joan::orig::arg::str(static_cast<const char*>(name)), str(d::s_none)) == 0) continue;  // _stricmp "none"
            Path path;
            joan::orig::as_string(&path)->construct();              // 0x005a1170
            joan::orig::as_string(&path)->assign(joan::orig::arg::str(static_cast<const char*>(name)));  // 0x005a1440
            if (jo::self<jo::ModelSlot>(this)->emitter_default != 0) joan::orig::as_string(&path)->append(str(d::s_default_9cf5fc));                // 0x005a1550 "_default"
            joan::orig::as_string(&path)->append(str(d::s_TXT));                  // 0x005a1550 ".TXT"
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_string(&path)->c_str());  // 0x00417d90
            jo::self<jo::ModelSlot>(this)->emitter_txt_ids[i] = joan::orig::arg::as<i32>(joan::orig::as_name_list(kEvents)->find_4c2060(joan::orig::arg::str(s), static_cast<std::int8_t>(joan::orig::arg::i32(1))));  // 0x004c2060
        }
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::load_txt_ids", 0x00418c60, &H::load_txt_ids);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b105_01.cpp (game/b105_01)
namespace {
namespace game_b105_01_2 {

struct H {
    // ---- tiny thiscall stubs ----
    void nop_418d80() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_01", name, addr, fn)
B("Obj::nop_418d80", 0x00418d80, &H::nop_418d80);
#undef B

}  // namespace game_b105_01_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_pi_a4f81c() -> void { joan::bind::free<&::game_b97_01::init_pi_a4f81c>(); }  // 0x417ef0
auto joan::orig::AnimResMgr::bind_bip(std::int16_t a0, const char* a1) -> void { joan::bind::member<&::game_b84_05::H::bind_bip>(this, a0, a1); }  // 0x417f20
auto joan::orig::Group::copy_slot(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b98_01_2::H::copy_slot>(this, a0)); }  // 0x418310
auto joan::orig::Path::has_points() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_06::H::path_has_points>(this)); }  // 0x418400
auto joan::orig::AnimResMgr::index_of(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_01::H::index_of>(this, a0)); }  // 0x418430
auto joan::orig::Path::find(const char* a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b85_06::H::path_find>(this, a0, a1)); }  // 0x4184b0
auto joan::orig::Path::init(const char* a0, const char* a1) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::path_init>(this, a0, a1)); }  // 0x418550
auto joan::orig::Path::parse(const char* a0) -> void { joan::bind::member<&::game_b85_06::H::path_parse>(this, a0); }  // 0x4185f0
auto joan::orig::Path::add_point(float a0, float a1, float a2, std::int32_t a3) -> void { joan::bind::member<&::game_b85_06::H::path_add>(this, a0, a1, a2, a3); }  // 0x418820
auto joan::orig::game_tick_4189a0() -> void { joan::bind::free<&::game_b100_01::game_tick_4189a0>(); }  // 0x4189a0
auto joan::orig::Path::lookup(float a0, void* a1, void* a2) -> void { joan::bind::member<&::game_b96_01::H::curve_lookup>(this, a0, a1, a2); }  // 0x418a00
auto joan::orig::game::tick_418bc0() -> void { joan::bind::free<&::game_b108_10::tick_418bc0>(); }  // 0x418bc0
auto joan::orig::AnimDef::destruct() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::call_5b4170>(this)); }  // 0x418c30
auto joan::orig::ModelSlot::load_txt_ids() -> void { joan::bind::member<&::game_b95_01::H::load_txt_ids>(this); }  // 0x418c60
auto joan::orig::init_a59734() -> void { joan::bind::free<&::game_b97_01::init_a59734>(); }  // 0x418d40
auto joan::orig::ModelSlot::destruct() -> void { joan::bind::member<&::game_b105_01_2::H::nop_418d80>(this); }  // 0x418d80
auto joan::orig::ModelRefs::release_thunk() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::fwd_418e00>(this)); }  // 0x418dd0
auto joan::orig::ModelRefs::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_20::H::anim_release>(this)); }  // 0x418e00
auto joan::orig::ModelRefs::destruct_entries() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b106_07::H::for_each_418d80>(this)); }  // 0x418ec0
auto joan::orig::ModelRefs::load() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_01::H::animscr_load>(this)); }  // 0x418f30
auto joan::orig::FileTxt::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::scalar_deleting_dtor>(this, a0)); }  // 0x419280
auto joan::orig::ModelRefs::parse_entry(void* a0, void* a1) -> void { joan::bind::member<&::game_b104_01::H::parse_419320>(this, a0, a1); }  // 0x419320
