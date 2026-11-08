// game/unknown_escomp.cpp: functions whose original source file is unknown (formerly game/_unplaced/escomp.cpp;
// re/attribution.csv: between ww/uti3d/esstate2.cpp and ww/uti3d/lflare.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cstddef>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/cxx_eh.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

struct Obj {};
// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;
using joan::img::obj;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::str;

void* const kObjects = obj(d::g_items);

using joan::hook::global;

using joan::call::thiscall;

using i16 = std::int16_t;

using joan::call::cdecl_call;

using joan::hook::x87::ftol;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

struct H {
    // ------------------------------------------------------------------ 0x00649860
    char* set_name_649860(const char* s) { return crt_strcpy(jo::self<jo::Script>(this)->name_, s); }
};

// ---------------------------------------------------------------------------- 0x00650e10
static u32 JOAN_CDECL object_field_ca(i32 id) {
    if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50
    void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
    u8* const p = virtual_call<u8*>(o, 0x58);
    return jo::as_wall(p)->max_hp;  // slot 0x58 returns the Wall (this; 0 for other kinds, as us_tactics.cpp)
}

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Obj::set_name_649860", 0x00649860, &H::set_name_649860);
B("object_field_ca", 0x00650e10, &object_field_ca);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

char* strncpy_(void* d, const char* s, std::uint32_t n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }

struct H {
    // 0x006498a0
    void set_name_a6a118(const char* s) {
        strncpy_(obj(d::g_a6a118), s, 0xffu);
        global<std::uint8_t>(d::g_a6a217) = 0;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("set_name_a6a118", 0x006498a0, &H::set_name_a6a118);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b100_12.cpp (game/b100_12)
namespace {
namespace game_b100_12 {

i32 object_missing(i32 k) { return joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(k)); }  // 0x00425d50
void* object_get(i32 k) { return joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(k)); }  // 0x00425d20

struct H {
    // ------------------------------------------------------------------ 0x006498e0
    H* init_6498e0() {
        joan::orig::as_counter(&jo::self<jo::ScriptLexer>(this)->tokens)->clamp_to_one();  // 0x004b9d40
        jo::view<u32>(jo::self<jo::ScriptLexer>(this)->i32_29) = 1;
        return this;
    }
};

// ---------------------------------------------------------------------------- cdecl helpers
static u32 JOAN_CDECL script_obj_c6(i32 k) {  // 0x00650d50
    if (object_missing(k) != 0) return 0;
    u8* const sub = virtual_call<u8*>(object_get(k), 0x58);
    return jo::as_wall(sub)->u32_c6;  // slot 0x58: the Wall
}

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("Obj::init_6498e0", 0x006498e0, &H::init_6498e0);
B("script_obj_c6", 0x00650d50, &script_obj_c6);
#undef B

}  // namespace game_b100_12
}  // namespace

// ---------------------------------------- from modules/game_b110_02.cpp (game/b110_02)
namespace {
namespace game_b110_02 {

// thiscall through the object's vtable slot `off`.
template <class R = void, class... A>
R vcall(const void* self, u32 off, A... args) {
    using Virtual = R (Obj::*)(A...);
    JOAN_MEMBER_POINTER_CHECK(Virtual);
    Virtual method{};
    void* const fn = joan::call::vtable_of(self)[joan::call::x86_vslot(off)];
    std::memcpy(&method, &fn, sizeof(void*));
    return (static_cast<Obj*>(const_cast<void*>(self))->*method)(args...);
}

constexpr u32 kStr = 0x194;  // String object is 0x191 bytes

JOAN_COUNTER(c_exec_cmd, "ScriptNode::exec_cmd");

// A script vector argument (three dwords of float bits) passed by value to a replacement that takes three floats:
// float members, so that System V x86-64 passes it in XMM registers like the callee's struct (the same 12 stack bytes
// on x86).
struct Vec3f {
    float x, y, z;
};
static_assert(sizeof(Vec3f) == 12, "Vec3f");
inline Vec3f vec3_bits(u32 x, u32 y, u32 z) {
    Vec3f v;
    std::memcpy(&v.x, &x, 4);
    std::memcpy(&v.y, &y, 4);
    std::memcpy(&v.z, &z, 4);
    return v;
}

struct H {
    // eax residue (this) of exec_cmd; its only caller (vtable slot 0x30, ScriptNode::run_params in uti3d/esstate2.cpp) drops the result.
    u32 self_eax() { return joan::call::register_low_bits(this); }
    void* child(i32 k) { return joan::orig::arg::as<void*>(joan::orig::as_reader(this)->list_get(joan::orig::arg::i32(k))); }  // 0x0063e9f0
    i32 count() { return joan::orig::arg::as<i32>(joan::orig::as_reader(this)->count()); }              // 0x0063ecd0
    // ScriptNode::value (+0x10) by the node's value type: the integer itself, the text, or a float's bits
    // (phase 1 typed it void*; x64: integer / float kept in a pointer field).
    static u32 ival(void* c) {
        u32 r;  // the integer sits in the first 4 bytes of the pointer-typed slot (x86: whole slot; x64: low half)
        std::memcpy(&r, &jo::as_script_node(c)->value, sizeof r);
        return r;
    }
    void set_value(u32 r) { jo::self<jo::ScriptFuncCall>(this)->value = reinterpret_cast<void*>(static_cast<std::uintptr_t>(r)); }  // x64: integer passed as pointer
    float& fvalue() { return *static_cast<float*>(static_cast<void*>(&jo::self<jo::ScriptFuncCall>(this)->value)); }
    u32 dval(i32 k) {
        void* const c = child(k);
        return ival(c);
    }
    // The whole value word of child k: a format argument (case_363) may be a string (%s reads it as a pointer, %d its
    // low half); varargs slots are pointer-sized on x64.
    std::uintptr_t aval(i32 k) { return reinterpret_cast<std::uintptr_t>(jo::as_script_node(child(k))->value); }
    const char* sval(i32 k) {
        void* const c = child(k);
        return static_cast<const char*>(jo::as_script_node(c)->value);
    }
    u8 bval(i32 k) {
        void* const c = child(k);
        return static_cast<u8>(ival(c));
    }
    float fval(i32 k) {  // fild dword; fstp dword [esp]
        void* const c = child(k);
        return static_cast<float>(static_cast<i32>(ival(c)));
    }
    static char* cstr(u8* s) { return joan::orig::as_string(s)->c_str(); }  // 0x00417d90

    // Walk +4 to the enclosing node whose +8 is 1 (or the top).
    void* find_root() {
        void* p = this;
        while (jo::view<void*>(jo::as_script_node(p)->parent) != nullptr) {
            p = jo::view<void*>(jo::as_script_node(p)->parent);
            if (jo::as_script_node(p)->kind == 1) break;
        }
        return p;
    }

    // Fills the local String `s` from child k; true when the table path copied an entry.
    bool load_string(i32 k, u8* s, void** root_out = nullptr) {
        void* const root = find_root();
        if (root_out != nullptr) *root_out = root;
        void* const tbl = joan::orig::arg::as<void*>(joan::orig::as_script(root)->dlg_res());  // 0x006509c0
        if (tbl != nullptr) {
            const char* const v = sval(k);
            const i16 idx = joan::orig::arg::as<i16>(joan::orig::as_table(tbl)->index_of_ref(joan::orig::arg::str(v)));  // 0x006380b0
            if (idx != 0) {
                u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dlg_res(tbl)->entry(static_cast<i32>(idx)));  // 0x00638150
                const i32 n = joan::orig::arg::as<i32>(joan::orig::min_int(joan::orig::arg::i32(static_cast<i32>(jo::as_dlg_res_entry(e)->len) - 2), 0x190));  // 0x0046f020 min
                u8* const e2 = joan::orig::arg::as<u8*>(joan::orig::as_dlg_res(tbl)->entry(static_cast<i32>(idx)));  // 0x00638150
                const char* const src = jo::view<const char*>(jo::as_dlg_res_entry(e2)->text);
                char* const d = cstr(s);
                std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(src), joan::orig::arg::u32(n));  // strncpy
                cstr(s)[n] = 0;
                return true;
            }
        }
        const char* const v = sval(k);
        joan::orig::as_string(s)->assign(joan::orig::arg::str(v));  // 0x005a1440
        return false;
    }

    // Root name up to '.', registered with child k's buffer size (commands 345 / 346).
    i16 register_name(void* root, i32 k) {
        char buf[0x104];
        char* const name = joan::orig::arg::as<char*>(joan::orig::as_script(root)->name());  // 0x006509a0
        char* const dot = joan::orig::arg::as<char*>(joan::orig::libc::strchr(joan::orig::arg::str(name), 0x2e));  // strchr
        const i32 len = static_cast<i32>(dot - name);
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(buf))), joan::orig::arg::str(static_cast<const char*>(name)), joan::orig::arg::u32(len));  // strncpy
        buf[len] = 0;
        void* const c = child(k);
        // vtable slot 0x1c is the node's data pointer (as Value::ref_data 0x00644330); its text after the first character
        // is the number: atoi(data + 1) (x64: the pointer was kept in a u32 and the atoi result typed as a pointer).
        const char* const n = vcall<const char*>(c, 0x1c) + 1;
        const i32 m = joan::crt::atoi(joan::orig::arg::str(n));  // 0x007b15cb atoi
        return joan::orig::arg::as<i16>(joan::orig::as_sound_res_table(obj(d::g_sound_res_table))->find_positive(static_cast<const char*>(buf), static_cast<std::uint32_t>(m)));  // 0x00639380
    }

    // +0x10 = new String when 0 (commands 120 / 363 / 364).
    void ensure_string() {
        if (jo::self<jo::ScriptFuncCall>(this)->value == nullptr) {
            void* const m = joan::crt::operator_new(0x191u);  // operator new
            void* const o = m != nullptr ? joan::orig::as_string(m)->construct() : nullptr;  // 0x005a1170
            jo::self<jo::ScriptFuncCall>(this)->value = o;
        }
    }

    u32 case_363() {  // 0x0064def3
        ensure_string();
        u8 s[kStr];
        joan::orig::as_string(s)->construct();  // 0x005a1170
        load_string(1, s);
        const i32 cnt = count();
        switch (cnt) {
            case 2: {
                void* c = child(2);
                if (jo::as_script_node(c)->value_type != 3) {
                    c = child(2);
                    if (jo::as_script_node(c)->value_type != 1) return joan::call::register_low_bits(c);  // eax residue, result of exec_cmd is dropped by run_params
                }
                const std::uintptr_t v2 = aval(2);
                const char* const p = cstr(s);
                return joan::orig::arg::as<u32>(joan::orig::format_into(joan::orig::arg::mptr(jo::self<jo::ScriptFuncCall>(this)->value), joan::orig::arg::str(p), v2));  // 0x0065e060
            }
            case 3: {
                const std::uintptr_t v3 = aval(3), v2 = aval(2);
                const char* const p = cstr(s);
                return joan::orig::arg::as<u32>(joan::orig::format_into(joan::orig::arg::mptr(jo::self<jo::ScriptFuncCall>(this)->value), joan::orig::arg::str(p), v2, v3));  // 0x0065e060
            }
            case 4: {
                const std::uintptr_t v4 = aval(4), v3 = aval(3), v2 = aval(2);
                const char* const p = cstr(s);
                return joan::orig::arg::as<u32>(joan::orig::format_into(joan::orig::arg::mptr(jo::self<jo::ScriptFuncCall>(this)->value), joan::orig::arg::str(p), v2, v3, v4));  // 0x0065e060
            }
            case 5: {
                const std::uintptr_t v5 = aval(5), v4 = aval(4), v3 = aval(3), v2 = aval(2);
                const char* const p = cstr(s);
                return joan::orig::arg::as<u32>(joan::orig::format_into(joan::orig::arg::mptr(jo::self<jo::ScriptFuncCall>(this)->value), joan::orig::arg::str(p), v2, v3, v4, v5));  // 0x0065e060
            }
            case 6: {
                const std::uintptr_t v6 = aval(6), v5 = aval(5), v4 = aval(4), v3 = aval(3), v2 = aval(2);
                const char* const p = cstr(s);
                return joan::orig::arg::as<u32>(joan::orig::format_into(joan::orig::arg::mptr(jo::self<jo::ScriptFuncCall>(this)->value), joan::orig::arg::str(p), v2, v3, v4, v5, v6));  // 0x0065e060
            }
            case 7: {
                const std::uintptr_t v7 = aval(7), v6 = aval(6), v5 = aval(5), v4 = aval(4), v3 = aval(3),
                          v2 = aval(2);
                const char* const p = cstr(s);
                return joan::orig::arg::as<u32>(joan::orig::format_into(joan::orig::arg::mptr(jo::self<jo::ScriptFuncCall>(this)->value), joan::orig::arg::str(p), v2, v3, v4, v5, v6, v7));  // 0x0065e060
            }
            case 8: {
                const std::uintptr_t v8 = aval(8), v7 = aval(7), v6 = aval(6), v5 = aval(5), v4 = aval(4),
                          v3 = aval(3), v2 = aval(2);
                const char* const p = cstr(s);
                return joan::orig::arg::as<u32>(joan::orig::format_into(joan::orig::arg::mptr(jo::self<jo::ScriptFuncCall>(this)->value), joan::orig::arg::str(p), v2, v3, v4, v5, v6, v7, v8));  // 0x0065e060
            }
            default:
                return static_cast<u32>(cnt);
        }
    }

    u32 exec_cmd(std::uintptr_t arg) {  // arg: the evaluator context (a pointer; uti3d/esstate2.cpp passes it whole)
        c_exec_cmd.hit();
        const i32 code = static_cast<i32>(jo::self<jo::ScriptFuncCall>(this)->func_id);
        switch (code) {
            case 49: {  // 0x00649981
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::is_building(joan::orig::arg::i32(v2)));  // 0x006512d0
                set_value(r3);
                return r3;
            }
            case 50: {  // 0x006499a3
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::in_arc(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00651430
                set_value(r5);
                return r5;
            }
            case 1: {  // 0x006499d4
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_random(joan::orig::arg::i32(v2)));  // 0x00651010
                set_value(r3);
                return r3;
            }
            case 2: {  // 0x006499f5
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a50298(joan::orig::arg::i16(v2)));  // 0x00651100
                set_value(r3);
                return r3;
            }
            case 3: {  // 0x00649a16
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_capture_place(joan::orig::arg::i32(v2)));  // 0x00651130
                set_value(r3);
                return r3;
            }
            case 161: {  // 0x00649a37
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_rgb_component(joan::orig::arg::str(v4), joan::orig::arg::i32(v2)));  // 0x006511f0
                set_value(r5);
                return r5;
            }
            case 325: {  // 0x00649a66
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_2a1_1d(joan::orig::arg::u32(v2)));  // 0x00651260
                set_value(r3);
                return r3;
            }
            case 47: {  // 0x00649a88
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::set_mode(joan::orig::arg::i32(v4), joan::orig::arg::str(v2)));  // 0x006515d0
                set_value(r5);
                return r5;
            }
            case 48: {  // 0x00649ab8
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::set_target_mode(joan::orig::arg::i32(v4), joan::orig::arg::str(v2)));  // 0x00651690
                set_value(r5);
                return r5;
            }
            case 51: {  // 0x00649ae8
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::mode_gt1(joan::orig::arg::i32(v2)));  // 0x00651550
                set_value(r3);
                return r3;
            }
            case 52: {  // 0x00649b0a
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::mode_is1(joan::orig::arg::i32(v2)));  // 0x00651590
                set_value(r3);
                return r3;
            }
            case 53: {  // 0x00649b2c
                void* const c1 = child(6);
                const u32 v2 = ival(c1);
                void* const c3 = child(5);
                const u32 v4 = ival(c3);
                void* const c5 = child(4);
                const u32 v6 = ival(c5);
                void* const c7 = child(3);
                const u32 v8 = ival(c7);
                void* const c9 = child(2);
                const char* const v10 = static_cast<const char*>(jo::as_script_node(c9)->value);
                void* const c11 = child(1);
                const i16 v12 = static_cast<i16>(ival(c11));
                const u32 r13 = joan::orig::arg::as<u32>(joan::orig::unit_forward_54b480(joan::orig::arg::i32(v12), v10, joan::orig::arg::i32(v8), joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2), 0));  // 0x006517c0
                set_value(r13);
                return r13;
            }
            case 54: {  // 0x00649b96
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::group_try_find_651830(joan::orig::arg::i32(v4), joan::orig::arg::u32(v2)));  // 0x00651830
                set_value(r5);
                return r5;
            }
            case 5: {  // 0x00649d10
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const u32 v4 = ival(c3);
                void* const c5 = child(2);
                const char* const v6 = static_cast<const char*>(jo::as_script_node(c5)->value);
                void* const c7 = child(1);
                const i16 v8 = static_cast<i16>(ival(c7));
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::units::call_503680_6519d0(joan::orig::arg::i32(v8), v6, joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x006519d0
                set_value(r9);
                return r9;
            }
            case 9: {  // 0x00649d5c
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::call_4a36c0(v2));  // 0x00651a30
                set_value(r3);
                return r3;
            }
            case 10: {  // 0x00649d7d
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_651a60(v2));  // 0x00651a60
                set_value(r3);
                return r3;
            }
            case 11: {  // 0x00649d9e
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_attach(v4, v2));  // 0x00651ad0
                set_value(r5);
                return r5;
            }
            case 12: {  // 0x00649dcd
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_a460a0_49f370(joan::orig::arg::i16(v2)));  // 0x00651e70
                set_value(r3);
                return r3;
            }
            case 13: {  // 0x00649def
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_remove_objects(v2));  // 0x00651ea0
                set_value(r3);
                return r3;
            }
            case 17: {  // 0x00649e10
                void* const c1 = child(7);
                const u32 v2 = ival(c1);
                void* const c3 = child(6);
                const u32 v4 = ival(c3);
                void* const c5 = child(5);
                const u32 v6 = ival(c5);
                void* const c7 = child(4);
                const u32 v8 = ival(c7);
                void* const c9 = child(3);
                const u32 v10 = ival(c9);
                void* const c11 = child(2);
                const char* const v12 = static_cast<const char*>(jo::as_script_node(c11)->value);
                void* const c13 = child(1);
                const u32 v14 = ival(c13);
                const u32 r15 = joan::orig::arg::as<u32>(joan::orig::spawn_named(joan::orig::arg::u32(v14), joan::orig::arg::str(v12), joan::orig::arg::u32(v10), joan::orig::arg::u32(v8), joan::orig::arg::u32(v6), joan::orig::arg::i32(v4), joan::orig::arg::u32(v2)));  // 0x00651c90
                set_value(r15);
                return r15;
            }
            case 14: {  // 0x00649e85
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const u32 v4 = ival(c3);
                void* const c5 = child(2);
                const u32 v6 = ival(c5);
                void* const c7 = child(1);
                const i16 v8 = static_cast<i16>(ival(c7));
                const Vec3f pos9 = vec3_bits(v6, v4, v2);  // Vec3 passed by value: three floats (XMM registers on System V x86-64, like the callee's struct)
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::object_set_pos(v8, pos9));  // 0x00651f00
                set_value(r9);
                return r9;
            }
            case 15: {  // 0x00649ed1
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_object_set_heading(joan::orig::arg::i16(v4), joan::orig::arg::i32(v2)));  // 0x00651fa0
                set_value(r5);
                return r5;
            }
            case 16: {  // 0x00649f01
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_set_type(joan::orig::arg::u32(v4), v2));  // 0x00652010
                set_value(r5);
                return r5;
            }
            case 18: {  // 0x00649f31
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::units_lookup(v2));  // 0x0065b570
                set_value(r3);
                return r3;
            }
            case 19: {  // 0x00649f52
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_object_49f4b0(v2));  // 0x0065b5b0
                set_value(r3);
                return r3;
            }
            case 20: {  // 0x00649f73
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_a501e0_4ce900(v2));  // 0x0065b5f0
                set_value(r3);
                return r3;
            }
            case 21: {  // 0x00649f94
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_object_name_differs(joan::orig::arg::i32(v4), joan::orig::arg::str(v2)));  // 0x0065b630
                set_value(r5);
                return r5;
            }
            case 22: {  // 0x00649fc3
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit::is_kind10(joan::orig::arg::i32(v2)));  // 0x0065b6a0
                set_value(r3);
                return r3;
            }
            case 23: {  // 0x00649fe4
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::object_is_type1(joan::orig::arg::i32(v2)));  // 0x0065b700
                set_value(r3);
                return r3;
            }
            case 24: {  // 0x0064a005
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::object_is_type4(joan::orig::arg::i32(v2)));  // 0x0065b760
                set_value(r3);
                return r3;
            }
            case 25: {  // 0x0064a026
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_4cad00(v2));  // 0x0065b7c0
                set_value(r3);
                return r3;
            }
            case 55: {  // 0x0064a047
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_facing_check(joan::orig::arg::i16(v4), joan::orig::arg::i32(v2)));  // 0x006526e0
                set_value(r5);
                return r5;
            }
            case 56: {  // 0x0064a077
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_has_enemy_near(joan::orig::arg::i16(v2)));  // 0x00652820
                set_value(r3);
                return r3;
            }
            case 57: {  // 0x0064a099
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_check_652a40(joan::orig::arg::u32(v2)));  // 0x00652a40
                set_value(r3);
                return r3;
            }
            case 58: {  // 0x0064a0bb
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::get_unit_flag22(joan::orig::arg::i16(v2)));  // 0x00652aa0
                set_value(r3);
                return r3;
            }
            case 59: {  // 0x0064a0dd
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_busy(joan::orig::arg::i32(v2)));  // 0x00652b00
                set_value(r3);
                return r3;
            }
            case 60: {  // 0x0064a0ff
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_check_652c10(joan::orig::arg::i16(v2)));  // 0x00652c10
                set_value(r3);
                return r3;
            }
            case 83: {  // 0x0064a121
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_52(joan::orig::arg::i16(v4), static_cast<std::int8_t>(joan::orig::arg::i32(v2))));  // 0x00652de0
                set_value(r5);
                return r5;
            }
            case 84: {  // 0x0064a152
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::reset_all());  // 0x00652e60
                set_value(r1);
                return r1;
            }
            case 61: {  // 0x0064a162
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::angle_to(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00652ee0
                set_value(r5);
                return r5;
            }
            case 62: {  // 0x0064a192
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_value(joan::orig::arg::i16(v4), joan::orig::arg::i32(v2)));  // 0x00652f80
                set_value(r5);
                return r5;
            }
            case 6: {  // 0x0064a1c2
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_heading_delta(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00653000
                set_value(r5);
                return r5;
            }
            case 7: {  // 0x0064a1f2
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::distance_to_object(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00653060
                set_value(r5);
                return r5;
            }
            case 8: {  // 0x0064a222
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a46059());  // 0x00653110
                set_value(r1);
                return r1;
            }
            case 67: {  // 0x0064a249
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::distance(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00653150
                set_value(r5);
                return r5;
            }
            case 68: {  // 0x0064a27a
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_1c5(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00653210
                set_value(r5);
                return r5;
            }
            case 69: {  // 0x0064a2ab
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_1c7(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x006532a0
                set_value(r5);
                return r5;
            }
            case 70: {  // 0x0064a2db
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_427b50(joan::orig::arg::i16(v4), v2));  // 0x00653300
                set_value(r5);
                return r5;
            }
            case 71: {  // 0x0064a30b
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_group_entry(joan::orig::arg::i16(v4), v2));  // 0x00653360
                set_value(r5);
                return r5;
            }
            case 72: {  // 0x0064a33b
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_any_quest_item(v2));  // 0x00653400
                set_value(r3);
                return r3;
            }
            case 73: {  // 0x0064a35c
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_2e_653590(joan::orig::arg::i16(v4), static_cast<std::int8_t>(joan::orig::arg::i32(v2))));  // 0x00653590
                set_value(r5);
                return r5;
            }
            case 74: {  // 0x0064a38c
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_2e(joan::orig::arg::i16(v4), static_cast<std::int8_t>(joan::orig::arg::i32(v2))));  // 0x00653640
                set_value(r5);
                return r5;
            }
            case 75: {  // 0x0064a3bc
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_1ae(joan::orig::arg::i16(v2)));  // 0x0065c0d0
                set_value(r3);
                return r3;
            }
            case 76: {  // 0x0064a3de
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::soldier_sum_1b0(joan::orig::arg::i32(v2)));  // 0x0065c130
                set_value(r3);
                return r3;
            }
            case 77: {  // 0x0064a3ff
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_124(joan::orig::arg::i32(v2)));  // 0x0065c1a0
                set_value(r3);
                return r3;
            }
            case 81: {  // 0x0064a420
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a596c4());  // 0x0065c200
                set_value(r1);
                return r1;
            }
            case 82: {  // 0x0064a430
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_add_a596c4(joan::orig::arg::i32(v2)));  // 0x0065c220
                set_value(r3);
                return r3;
            }
            case 78: {  // 0x0064a451
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_523190(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065c250
                set_value(r5);
                return r5;
            }
            case 79: {  // 0x0064a480
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_type(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065c2e0
                set_value(r5);
                return r5;
            }
            case 80: {  // 0x0064a4b1
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_1c1_word2(joan::orig::arg::i32(v2)));  // 0x0065c3a0
                set_value(r3);
                return r3;
            }
            case 64: {  // 0x0064a4d2
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_state_is_42(joan::orig::arg::i32(v2)));  // 0x00653c60
                set_value(r3);
                return r3;
            }
            case 65: {  // 0x0064a4f4
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_in_zone(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00653ca0
                set_value(r5);
                return r5;
            }
            case 66: {  // 0x0064a524
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_find_unit_for(joan::orig::arg::i32(v2)));  // 0x00653d60
                set_value(r3);
                return r3;
            }
            case 26: {  // 0x0064a545
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_target(joan::orig::arg::u32(v2)));  // 0x00650b90
                set_value(r3);
                return r3;
            }
            case 27: {  // 0x0064a566
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_value_c6(joan::orig::arg::u32(v2)));  // 0x00650c10
                set_value(r3);
                return r3;
            }
            case 28: {  // 0x0064a587
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_set_c2(joan::orig::arg::i32(v4), joan::orig::arg::u32(v2)));  // 0x00650c90
                set_value(r5);
                return r5;
            }
            case 29: {  // 0x0064a5b6
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_object_set_c6(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00650cf0
                set_value(r5);
                return r5;
            }
            case 30: {  // 0x0064a5e5
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_slot1_d1());  // 0x00652440
                set_value(r1);
                return r1;
            }
            case 31: {  // 0x0064a5f5
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::object_d1(joan::orig::arg::i32(v2)));  // 0x006524a0
                set_value(r3);
                return r3;
            }
            case 32: {  // 0x0064a616
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_obj_set_ce(joan::orig::arg::i32(v4), static_cast<std::int8_t>(joan::orig::arg::i32(v2))));  // 0x00652520
                set_value(r5);
                return r5;
            }
            case 33: {  // 0x0064a645
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::object_byte_ce(joan::orig::arg::i32(v2)));  // 0x00652590
                set_value(r3);
                return r3;
            }
            case 34: {  // 0x0064a666
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_object_set_cf(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x006525f0
                set_value(r5);
                return r5;
            }
            case 35: {  // 0x0064a695
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_byte_d0(joan::orig::arg::u32(v2)));  // 0x00652660
                set_value(r3);
                return r3;
            }
            case 36: {  // 0x0064a6b6
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_obj_c6(joan::orig::arg::i32(v2)));  // 0x00650d50
                set_value(r3);
                return r3;
            }
            case 37: {  // 0x0064a6d7
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::set_c6(joan::orig::arg::i32(v4), joan::orig::arg::u32(v2)));  // 0x00650db0
                set_value(r5);
                return r5;
            }
            case 38: {  // 0x0064a706
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::object_field_ca(joan::orig::arg::i32(v2)));  // 0x00650e10
                set_value(r3);
                return r3;
            }
            case 39: {  // 0x0064a727
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::unit_say(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x00650e70
                set_value(r5);
                return r5;
            }
            case 40: {  // 0x0064a756
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_set_e6(joan::orig::arg::i32(v4), joan::orig::arg::u32(v2)));  // 0x00650f40
                set_value(r5);
                return r5;
            }
            case 162: {  // 0x0064a785
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::formation(joan::orig::arg::i32(v2)));  // 0x006520d0
                set_value(r3);
                return r3;
            }
            case 164: {  // 0x0064a7a7
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_value_2ba(joan::orig::arg::i16(v2)));  // 0x00652280
                set_value(r3);
                return r3;
            }
            case 165: {  // 0x0064a7c9
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::set_2ba(joan::orig::arg::i16(v4), joan::orig::arg::u32(v2)));  // 0x006522f0
                set_value(r5);
                return r5;
            }
            case 163: {  // 0x0064a7f9
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::formation_slot(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00652190
                set_value(r5);
                return r5;
            }
            case 166: {  // 0x0064a829
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::unit_528300(joan::orig::arg::i32(v2)));  // 0x00652370
                set_value(r3);
                return r3;
            }
            case 167: {  // 0x0064a84b
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_kill_type(joan::orig::arg::i16(v2)));  // 0x006523c0
                set_value(r3);
                return r3;
            }
            case 168: {  // 0x0064a86d
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_a484d0_4c54a0(v2));  // 0x00653eb0
                set_value(r3);
                return r3;
            }
            case 169: {  // 0x0064a88e
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_building_49c240(joan::orig::arg::i16(v4), v2));  // 0x00653ef0
                set_value(r5);
                return r5;
            }
            case 170: {  // 0x0064a8be
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::group_contains(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00653f30
                set_value(r5);
                return r5;
            }
            case 171: {  // 0x0064a8ed
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_49c470(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x00653fe0
                set_value(r5);
                return r5;
            }
            case 172: {  // 0x0064a91e
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::unit_call_654020(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x00654020
                set_value(r5);
                return r5;
            }
            case 173: {  // 0x0064a94f
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_654060(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x00654060
                set_value(r5);
                return r5;
            }
            case 174: {  // 0x0064a980
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_6540a0(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x006540a0
                set_value(r5);
                return r5;
            }
            case 175: {  // 0x0064a9b1
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_6540e0(joan::orig::arg::i16(v4), v2));  // 0x006540e0
                set_value(r5);
                return r5;
            }
            case 176: {  // 0x0064a9e1
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_654120(joan::orig::arg::i16(v4), v2));  // 0x00654120
                set_value(r5);
                return r5;
            }
            case 177: {  // 0x0064aa11
                const char* const v2 = sval(2);  // the owner name (a string value; ival cut its pointer to 32 bits on x64)
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_654160(joan::orig::arg::i16(v4), v2));  // 0x00654160
                set_value(r5);
                return r5;
            }
            case 178: {  // 0x0064aa41
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_squad_1ca(joan::orig::arg::i32(v2)));  // 0x006541a0
                set_value(r3);
                return r3;
            }
            case 192: {  // 0x0064aa63
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::reset_object_1ca(joan::orig::arg::u32(v2)));  // 0x006541d0
                set_value(r3);
                return r3;
            }
            case 179: {  // 0x0064aa85
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                void* const c5 = child(1);
                const i16 v6 = static_cast<i16>(ival(c5));
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_building_4c4440(joan::orig::arg::i16(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00654240
                set_value(r7);
                return r7;
            }
            case 180: {  // 0x0064aac3
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_a484d0_654530(joan::orig::arg::i32(v2)));  // 0x00654280
                set_value(r3);
                return r3;
            }
            case 181: {  // 0x0064aae4
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::place_switch_side(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x006542b0
                set_value(r5);
                return r5;
            }
            case 182: {  // 0x0064ab13
                const char* const v2 = sval(2);  // the side name: a string value (a pointer; ival would cut it to 32 bits)
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = cdecl_call<u32>(fn::Script_place_set_side, v4, v2);  // 0x00654560 (the name passed as the pointer it is)
                set_value(r5);
                return r5;
            }
            case 183: {  // 0x0064ab42
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::group_angle(joan::orig::arg::i32(v2)));  // 0x006547a0
                set_value(r3);
                return r3;
            }
            case 184: {  // 0x0064ab63
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::group_set_angle(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x006547e0
                set_value(r5);
                return r5;
            }
            case 185: {  // 0x0064ab92
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_squad_set_rates(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00654830
                set_value(r5);
                return r5;
            }
            case 186: {  // 0x0064abc1
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_6548b0(v2));  // 0x006548b0
                set_value(r3);
                return r3;
            }
            case 187: {  // 0x0064abe2
                void* const c1 = child(6);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(5);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                void* const c5 = child(4);
                const u32 v6 = ival(c5);
                void* const c7 = child(3);
                const i16 v8 = static_cast<i16>(ival(c7));
                void* const c9 = child(2);
                const i16 v10 = static_cast<i16>(ival(c9));
                void* const c11 = child(1);
                const i16 v12 = static_cast<i16>(ival(c11));
                const u32 r13 = joan::orig::arg::as<u32>(joan::orig::script_place_467dd0(joan::orig::arg::i16(v12), joan::orig::arg::i16(v10), joan::orig::arg::i16(v8), joan::orig::arg::i32(v6), v4, v2));  // 0x006548f0
                set_value(r13);
                return r13;
            }
            case 188: {  // 0x0064ac4c
                void* const c1 = child(5);
                const u32 v2 = ival(c1);
                void* const c3 = child(4);
                const i16 v4 = static_cast<i16>(ival(c3));
                void* const c5 = child(3);
                const i16 v6 = static_cast<i16>(ival(c5));
                void* const c7 = child(2);
                const i16 v8 = static_cast<i16>(ival(c7));
                void* const c9 = child(1);
                const i16 v10 = static_cast<i16>(ival(c9));
                const u32 r11 = joan::orig::arg::as<u32>(joan::orig::script_place_467ea0(joan::orig::arg::i16(v10), joan::orig::arg::i16(v8), joan::orig::arg::i16(v6), joan::orig::arg::i16(v4), joan::orig::arg::i32(v2)));  // 0x00654960
                set_value(r11);
                return r11;
            }
            case 189: {  // 0x0064aca9
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_object_dist(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x006549c0
                set_value(r5);
                return r5;
            }
            case 190: {  // 0x0064acd8
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_place_set_13c(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00654ae0
                set_value(r5);
                return r5;
            }
            case 191: {  // 0x0064ad07
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_place_sum(joan::orig::arg::i16(v2)));  // 0x00654b30
                set_value(r3);
                return r3;
            }
            case 193: {  // 0x0064ad29
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_place_set_97(joan::orig::arg::i16(v4), static_cast<std::int8_t>(joan::orig::arg::i32(v2))));  // 0x00654bc0
                set_value(r5);
                return r5;
            }
            case 194: {  // 0x0064ad5a
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_place_set_98(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00654c20
                set_value(r5);
                return r5;
            }
            case 195: {  // 0x0064ad8b
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_place_654c80(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x00654c80
                set_value(r5);
                return r5;
            }
            case 196: {  // 0x0064adbc
                void* const c1 = child(9);
                const u32 v2 = ival(c1);
                void* const c3 = child(8);
                const u32 v4 = ival(c3);
                void* const c5 = child(7);
                const u32 v6 = ival(c5);
                void* const c7 = child(6);
                const u32 v8 = ival(c7);
                void* const c9 = child(5);
                const u32 v10 = ival(c9);
                void* const c11 = child(4);
                const u8 v12 = static_cast<u8>(ival(c11));
                void* const c13 = child(3);
                const u8 v14 = static_cast<u8>(ival(c13));
                void* const c15 = child(2);
                const u32 v16 = ival(c15);
                void* const c17 = child(1);
                const i16 v18 = static_cast<i16>(ival(c17));
                const u32 r19 = joan::orig::arg::as<u32>(joan::orig::script_place_setup(joan::orig::arg::i16(v18), joan::orig::arg::i32(v16), static_cast<std::int8_t>(joan::orig::arg::i32(v14)), joan::orig::arg::i32(v12), joan::orig::arg::i32(v10), joan::orig::arg::i32(v8), joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00654d70
                set_value(r19);
                return r19;
            }
            case 197: {  // 0x0064ae4e
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_place_654e30(joan::orig::arg::i16(v4), joan::orig::arg::u32(v2)));  // 0x00654e30
                set_value(r5);
                return r5;
            }
            case 198: {  // 0x0064ae7e
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::object_set_81(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x00654ea0
                set_value(r5);
                return r5;
            }
            case 199: {  // 0x0064aeae
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const u32 v4 = ival(c3);
                void* const c5 = child(2);
                const i16 v6 = static_cast<i16>(ival(c5));
                void* const c7 = child(1);
                const i16 v8 = static_cast<i16>(ival(c7));
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::script_place_654f10(joan::orig::arg::i16(v8), joan::orig::arg::i16(v6), joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x00654f10
                set_value(r9);
                return r9;
            }
            case 200: {  // 0x0064aefb
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::object_call_46b730(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x00654fa0
                set_value(r5);
                return r5;
            }
            case 201: {  // 0x0064af2b
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                const char* const v4 = sval(2);  // the two side names: string values (pointers)
                const char* const v6 = sval(1);
                const u32 r7 = cdecl_call<u32>(fn::Script_set_side_relation, v6, v4, v2);  // 0x00653df0 (the names passed as the pointers they are)
                set_value(r7);
                return r7;
            }
            case 202: {  // 0x0064af68
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                void* const c5 = child(1);
                const char* const v6 = static_cast<const char*>(jo::as_script_node(c5)->value);
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script::fn_655010(v6, v4, static_cast<char>(joan::orig::arg::i32(v2))));  // 0x00655010
                set_value(r7);
                return r7;
            }
            case 203: {  // 0x0064afa5
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_6550d0(v4, joan::orig::arg::u32(v2)));  // 0x006550d0
                set_value(r5);
                return r5;
            }
            case 85: {  // 0x0064afd4
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_spawn_unit(v4, v2));  // 0x00655150
                set_value(r5);
                return r5;
            }
            case 86: {  // 0x0064b003
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);  // the script strings (pointers, as case 85)
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_load_unit(v4, v2));  // 0x00655200
                set_value(r5);
                return r5;
            }
            case 87: {  // 0x0064b032
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_reset_655420(joan::orig::arg::u32(v2)));  // 0x00655420
                set_value(r3);
                return r3;
            }
            case 88: {  // 0x0064b054
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const u32 v4 = ival(c3);
                void* const c5 = child(2);
                const u32 v6 = ival(c5);
                void* const c7 = child(1);
                const i16 v8 = static_cast<i16>(ival(c7));
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::script_unit_place(joan::orig::arg::i32(v8), joan::orig::arg::u32(v6), joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x006554f0
                set_value(r9);
                return r9;
            }
            case 89: {  // 0x0064b0a0
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_heading(joan::orig::arg::i16(v4), joan::orig::arg::i32(v2)));  // 0x00655590
                set_value(r5);
                return r5;
            }
            case 90: {  // 0x0064b0d0
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_50abd0(joan::orig::arg::i16(v4), joan::orig::arg::i32(v2)));  // 0x00655620
                set_value(r5);
                return r5;
            }
            case 91: {  // 0x0064b100
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_heading_deg(joan::orig::arg::i16(v2)));  // 0x00655680
                set_value(r3);
                return r3;
            }
            case 92: {  // 0x0064b122
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::unit_set_5089e0(joan::orig::arg::u32(v4), v2));  // 0x006556e0
                set_value(r5);
                return r5;
            }
            case 93: {  // 0x0064b152
                const char* const v2 = sval(2);  // the side name: a string value (a pointer)
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = cdecl_call<u32>(fn::Script_unit_set_side, static_cast<u32>(v4), v2);  // 0x00655750 (the name passed as the pointer it is)
                set_value(r5);
                return r5;
            }
            case 94: {  // 0x0064b182
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_41fcc0(joan::orig::arg::i32(v2)));  // 0x00655810
                set_value(r3);
                return r3;
            }
            case 96: {  // 0x0064b1a4
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::set_2d7(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x006558d0
                set_value(r5);
                return r5;
            }
            case 97: {  // 0x0064b1d5
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_4fc220(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x00655930
                set_value(r5);
                return r5;
            }
            case 98: {  // 0x0064b206
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_1e2(joan::orig::arg::i16(v2)));  // 0x00655990
                set_value(r3);
                return r3;
            }
            case 99: {  // 0x0064b228
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::unit_field_1e2(joan::orig::arg::u32(v2)));  // 0x00655b20
                set_value(r3);
                return r3;
            }
            case 100: {  // 0x0064b24a
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_51a250(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x006559e0
                set_value(r5);
                return r5;
            }
            case 101: {  // 0x0064b27b
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_unlink_1f5(joan::orig::arg::i16(v2)));  // 0x00655a40
                set_value(r3);
                return r3;
            }
            case 102: {  // 0x0064b29d
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_2e3(joan::orig::arg::i32(v2)));  // 0x00655bd0
                set_value(r3);
                return r3;
            }
            case 103: {  // 0x0064b2bf
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_2e3(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x00655c20
                set_value(r5);
                return r5;
            }
            case 104: {  // 0x0064b2f0
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_2e5(joan::orig::arg::i16(v2)));  // 0x00655c80
                set_value(r3);
                return r3;
            }
            case 105: {  // 0x0064b312
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::unit_set_2e5(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x00655cd0
                set_value(r5);
                return r5;
            }
            case 113: {  // 0x0064b343
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_2e7(joan::orig::arg::i16(v2)));  // 0x00655d30
                set_value(r3);
                return r3;
            }
            case 114: {  // 0x0064b365
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_2e7(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00655d80
                set_value(r5);
                return r5;
            }
            case 106: {  // 0x0064b396
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_2e9(joan::orig::arg::i16(v2)));  // 0x00655de0
                set_value(r3);
                return r3;
            }
            case 107: {  // 0x0064b3b8
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_2e9(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00655e30
                set_value(r5);
                return r5;
            }
            case 108: {  // 0x0064b3e9
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_2eb(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x00655e90
                set_value(r5);
                return r5;
            }
            case 109: {  // 0x0064b41a
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_2eb(joan::orig::arg::i16(v2)));  // 0x00655f10
                set_value(r3);
                return r3;
            }
            case 110: {  // 0x0064b43c
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::units_set_2eb(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x00655f80
                set_value(r5);
                return r5;
            }
            case 111: {  // 0x0064b46d
                void* const c1 = child(3);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(2);
                const i16 v4 = static_cast<i16>(ival(c3));
                void* const c5 = child(1);
                const i16 v6 = static_cast<i16>(ival(c5));
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_building_units_set_2eb(joan::orig::arg::i16(v6), joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x00656030
                set_value(r7);
                return r7;
            }
            case 112: {  // 0x0064b4ad
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::unit_2eb_is_2(joan::orig::arg::u32(v2)));  // 0x006561b0
                set_value(r3);
                return r3;
            }
            case 115: {  // 0x0064b4cf
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_114(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00656210
                set_value(r5);
                return r5;
            }
            case 116: {  // 0x0064b4fe
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_114(joan::orig::arg::i32(v2)));  // 0x00656260
                set_value(r3);
                return r3;
            }
            case 117: {  // 0x0064b51f
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_15f(joan::orig::arg::i32(v4), joan::orig::arg::i16(v2)));  // 0x006562b0
                set_value(r5);
                return r5;
            }
            case 118: {  // 0x0064b54e
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_15f(joan::orig::arg::i32(v2)));  // 0x00656300
                set_value(r3);
                return r3;
            }
            case 119: {  // 0x0064b56f
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_name(joan::orig::arg::i32(v4), joan::orig::arg::str(v2)));  // 0x00656350
                set_value(r5);
                return r5;
            }
            case 121: {  // 0x0064b628
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::units::detach_656410(joan::orig::arg::i32(v2)));  // 0x00656410
                set_value(r3);
                return r3;
            }
            case 122: {  // 0x0064b649
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::unit_copy_13d(joan::orig::arg::u32(v2)));  // 0x006568d0
                set_value(r3);
                return r3;
            }
            case 124: {  // 0x0064b66a
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_135(joan::orig::arg::i32(v2)));  // 0x00656940
                set_value(r3);
                return r3;
            }
            case 123: {  // 0x0064b68b
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_135(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00656990
                set_value(r5);
                return r5;
            }
            case 125: {  // 0x0064b6ba
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::leader_id(joan::orig::arg::i32(v2)));  // 0x006569f0
                set_value(r3);
                return r3;
            }
            case 126: {  // 0x0064b6db
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_key_alive(joan::orig::arg::i32(v2)));  // 0x00656a60
                set_value(r3);
                return r3;
            }
            case 127: {  // 0x0064b6fc
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                void* const c5 = child(2);
                const i16 v6 = static_cast<i16>(ival(c5));
                void* const c7 = child(1);
                const i16 v8 = static_cast<i16>(ival(c7));
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::script_unit_take_slot(joan::orig::arg::i32(v8), joan::orig::arg::i32(v6), joan::orig::arg::str(v4), joan::orig::arg::i32(v2)));  // 0x00657670
                set_value(r9);
                return r9;
            }
            case 128: {  // 0x0064b749
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_384(joan::orig::arg::i16(v2)));  // 0x00656af0
                set_value(r3);
                return r3;
            }
            case 129: {  // 0x0064b76b
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_release_slots(joan::orig::arg::i16(v4), joan::orig::arg::i32(v2)));  // 0x00657cc0
                set_value(r5);
                return r5;
            }
            case 130: {  // 0x0064b79b
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_38c(joan::orig::arg::i16(v2)));  // 0x00656b50
                set_value(r3);
                return r3;
            }
            case 131: {  // 0x0064b7bd
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::unit::guard_check(joan::orig::arg::i16(v4), joan::orig::arg::str(v2)));  // 0x00656bb0
                set_value(r5);
                return r5;
            }
            case 132: {  // 0x0064b7ed
                void* const c1 = child(3);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(2);
                const i16 v4 = static_cast<i16>(ival(c3));
                void* const c5 = child(1);
                const i16 v6 = static_cast<i16>(ival(c5));
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::seat::find_free_656dc0(joan::orig::arg::i16(v6), joan::orig::arg::i16(v4), joan::orig::arg::str(v2)));  // 0x00656dc0
                set_value(r7);
                return r7;
            }
            case 133: {  // 0x0064b82c
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_hardpoint_x(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00657110
                set_value(r5);
                return r5;
            }
            case 134: {  // 0x0064b85c
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_hardpoint_y(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00657240
                set_value(r5);
                return r5;
            }
            case 135: {  // 0x0064b88c
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_hardpoint_z(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00657370
                set_value(r5);
                return r5;
            }
            case 136: {  // 0x0064b8bc
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_group_flag_2f(joan::orig::arg::i16(v4), joan::orig::arg::i32(v2)));  // 0x006574a0
                set_value(r5);
                return r5;
            }
            case 137: {  // 0x0064b8ec
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_2f_zero(joan::orig::arg::i16(v2)));  // 0x006575c0
                set_value(r3);
                return r3;
            }
            case 138: {  // 0x0064b90e
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit::rank_value(joan::orig::arg::i16(v2)));  // 0x006592c0
                set_value(r3);
                return r3;
            }
            case 139: {  // 0x0064b930
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_unit_dist(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00659400
                set_value(r5);
                return r5;
            }
            case 140: {  // 0x0064b95f
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_508b90(joan::orig::arg::i16(v2)));  // 0x00657610
                set_value(r3);
                return r3;
            }
            case 95: {  // 0x0064b981
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_call_442820(joan::orig::arg::i32(v2)));  // 0x00655870
                set_value(r3);
                return r3;
            }
            case 141: {  // 0x0064b9a3
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::unit_join_group(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x00657fd0
                set_value(r5);
                return r5;
            }
            case 142: {  // 0x0064b9d4
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_group_size(joan::orig::arg::i16(v2)));  // 0x00658160
                set_value(r3);
                return r3;
            }
            case 143: {  // 0x0064b9f6
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::find_unit_in_groups(joan::orig::arg::i16(v2)));  // 0x006581f0
                set_value(r3);
                return r3;
            }
            case 144: {  // 0x0064ba18
                const char* const v2 = sval(2);  // the side name: a string value (a pointer)
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = cdecl_call<u32>(fn::Script_group_set_side, static_cast<u32>(v4), v2);  // 0x006582f0 (the name passed as the pointer it is)
                set_value(r5);
                return r5;
            }
            case 145: {  // 0x0064ba48
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_units_set_group(joan::orig::arg::i32(v4), v2));  // 0x00658420
                set_value(r5);
                return r5;
            }
            case 146: {  // 0x0064ba78
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                void* const c5 = child(1);
                const i16 v6 = static_cast<i16>(ival(c5));
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_unit_spawn_members(joan::orig::arg::i32(v6), v4, joan::orig::arg::i32(v2)));  // 0x006584e0
                set_value(r7);
                return r7;
            }
            case 147: {  // 0x0064bab6
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_group_call_41fcc0(joan::orig::arg::i16(v2)));  // 0x00658650
                set_value(r3);
                return r3;
            }
            case 148: {  // 0x0064bad8
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_group_442820(joan::orig::arg::i16(v2)));  // 0x00658730
                set_value(r3);
                return r3;
            }
            case 149: {  // 0x0064bafa
                void* const c1 = child(5);
                const u32 v2 = ival(c1);
                void* const c3 = child(4);
                const u32 v4 = ival(c3);
                void* const c5 = child(3);
                const u32 v6 = ival(c5);
                void* const c7 = child(2);
                const u32 v8 = ival(c7);
                void* const c9 = child(1);
                const i16 v10 = static_cast<i16>(ival(c9));
                const u32 r11 = joan::orig::arg::as<u32>(joan::orig::script_unit_group_move(joan::orig::arg::i16(v10), joan::orig::arg::i32(v8), joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::f32(v2)));  // 0x00658810
                set_value(r11);
                return r11;
            }
            case 150: {  // 0x0064bb54
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::group_follow_leader(joan::orig::arg::u32(v2)));  // 0x00658920
                set_value(r3);
                return r3;
            }
            case 151: {  // 0x0064bb76
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_658ad0(joan::orig::arg::u32(v2)));  // 0x00658ad0
                set_value(r3);
                return r3;
            }
            case 152: {  // 0x0064bb97
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_units_clear_1c5(joan::orig::arg::i32(v2)));  // 0x00658d90
                set_value(r3);
                return r3;
            }
            case 153: {  // 0x0064bbb8
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                void* const c5 = child(1);
                const u32 v6 = ival(c5);
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::units_order_kind_65dc20(joan::orig::arg::i32(v6), joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x0065dc20
                set_value(r7);
                return r7;
            }
            case 154: {  // 0x0064bbf5
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::units_sweep_65dcf0(joan::orig::arg::i32(v2)));  // 0x0065dcf0
                set_value(r3);
                return r3;
            }
            case 155: {  // 0x0064bc16
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_units_set_2f(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00658e10
                set_value(r5);
                return r5;
            }
            case 156: {  // 0x0064bc45
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                void* const c5 = child(2);
                const i16 v6 = static_cast<i16>(ival(c5));
                void* const c7 = child(1);
                const i16 v8 = static_cast<i16>(ival(c7));
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::script_unit_attach(joan::orig::arg::i16(v8), joan::orig::arg::i16(v6), joan::orig::arg::str(v4), static_cast<std::int8_t>(joan::orig::arg::i32(v2))));  // 0x00658ea0
                set_value(r9);
                return r9;
            }
            case 157: {  // 0x0064bc92
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_select_unit(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x00658f70
                set_value(r5);
                return r5;
            }
            case 158: {  // 0x0064bcc3
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a5b9e8());  // 0x00659060
                set_value(r1);
                return r1;
            }
            case 159: {  // 0x0064bcd3
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_release_group(joan::orig::arg::i32(v2)));  // 0x00659080
                set_value(r3);
                return r3;
            }
            case 160: {  // 0x0064bcf4
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::units::release_owned(joan::orig::arg::i32(v2)));  // 0x00659210
                set_value(r3);
                return r3;
            }
            case 204: {  // 0x0064bd15
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_outcome(joan::orig::arg::str(v2)));  // 0x006594e0
                set_value(r3);
                return r3;
            }
            case 205: {  // 0x0064bd36
                void* const c1 = child(6);
                const u32 v2 = ival(c1);
                void* const c3 = child(5);
                const u32 v4 = ival(c3);
                void* const c5 = child(4);
                const u32 v6 = ival(c5);
                void* const c7 = child(3);
                const u32 v8 = ival(c7);
                void* const c9 = child(2);
                const u32 v10 = ival(c9);
                void* const c11 = child(1);
                const u32 v12 = ival(c11);
                const u32 r13 = joan::orig::arg::as<u32>(joan::orig::script_659550(joan::orig::arg::u32(v12), joan::orig::arg::u32(v10), joan::orig::arg::u32(v8), joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00659550
                set_value(r13);
                return r13;
            }
            case 206: {  // 0x0064bd9d
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::flag_a59298());  // 0x006595e0
                set_value(r1);
                return r1;
            }
            case 207: {  // 0x0064bdad
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_range_pair(joan::orig::arg::i32(v2)));  // 0x00659610
                set_value(r3);
                return r3;
            }
            case 208: {  // 0x0064bdce
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_current_1aa());  // 0x006596a0
                set_value(r1);
                return r1;
            }
            case 209: {  // 0x0064bdde
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                void* const c5 = child(1);
                const u32 v6 = ival(c5);
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_a52928_57f990(joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x006596f0
                set_value(r7);
                return r7;
            }
            case 210: {  // 0x0064be1b
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a47bb0());  // 0x00659720
                set_value(r1);
                return r1;
            }
            case 213: {  // 0x0064be50
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a47bb8());  // 0x006597d0
                set_value(r1);
                return r1;
            }
            case 214: {  // 0x0064be60
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a47bbc());  // 0x006597f0
                set_value(r1);
                return r1;
            }
            case 215: {  // 0x0064be70
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a47bc0());  // 0x00659810
                set_value(r1);
                return r1;
            }
            case 216: {  // 0x0064be80
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a58faf(joan::orig::arg::i32(v2)));  // 0x00659830
                set_value(r3);
                return r3;
            }
            case 217: {  // 0x0064bea1
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::call_51c4b0(joan::orig::arg::u32(v2)));  // 0x00659860
                set_value(r3);
                return r3;
            }
            case 218: {  // 0x0064bec2
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a48cdc(joan::orig::arg::i32(v2)));  // 0x00659890
                set_value(r3);
                return r3;
            }
            case 219: {  // 0x0064bee3
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a591cc(joan::orig::arg::i32(v2)));  // 0x006598c0
                set_value(r3);
                return r3;
            }
            case 220: {  // 0x0064bf04
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591cc());  // 0x006598f0
                set_value(r1);
                return r1;
            }
            case 221: {  // 0x0064bf14
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::clear_a59171());  // 0x00659910
                set_value(r1);
                return r1;
            }
            case 222: {  // 0x0064bf24
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a59171());  // 0x00659940
                set_value(r1);
                return r1;
            }
            case 223: {  // 0x0064bf34
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_clear_a59175());  // 0x00659960
                set_value(r1);
                return r1;
            }
            case 224: {  // 0x0064bf44
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a59175());  // 0x00659990
                set_value(r1);
                return r1;
            }
            case 225: {  // 0x0064bf54
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_clear_a59179());  // 0x006599b0
                set_value(r1);
                return r1;
            }
            case 226: {  // 0x0064bf64
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a59179());  // 0x006599e0
                set_value(r1);
                return r1;
            }
            case 227: {  // 0x0064bf74
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a591d0(joan::orig::arg::i32(v2)));  // 0x00659a00
                set_value(r3);
                return r3;
            }
            case 228: {  // 0x0064bf95
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a591d4(joan::orig::arg::i32(v2)));  // 0x00659a30
                set_value(r3);
                return r3;
            }
            case 229: {  // 0x0064bfb6
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_add_a591d8(joan::orig::arg::i32(v2)));  // 0x00659a60
                set_value(r3);
                return r3;
            }
            case 230: {  // 0x0064bfd7
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a591d8(joan::orig::arg::i32(v2)));  // 0x00659a90
                set_value(r3);
                return r3;
            }
            case 231: {  // 0x0064bff8
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591d8());  // 0x00659ac0
                set_value(r1);
                return r1;
            }
            case 232: {  // 0x0064c008
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::clear_a5918d());  // 0x00659ae0
                set_value(r1);
                return r1;
            }
            case 233: {  // 0x0064c018
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a5918d());  // 0x00659e80
                set_value(r1);
                return r1;
            }
            case 234: {  // 0x0064c028
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_clear_a59199());  // 0x00659ea0
                set_value(r1);
                return r1;
            }
            case 235: {  // 0x0064c038
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a59199());  // 0x00659ed0
                set_value(r1);
                return r1;
            }
            case 236: {  // 0x0064c048
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_clear_a591a9());  // 0x00659b30
                set_value(r1);
                return r1;
            }
            case 237: {  // 0x0064c058
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591a9());  // 0x00659b10
                set_value(r1);
                return r1;
            }
            case 239: {  // 0x0064c068
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591b6());  // 0x00659b60
                set_value(r1);
                return r1;
            }
            case 240: {  // 0x0064c078
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_goldbags_to_barrels());  // 0x00659b80
                set_value(r1);
                return r1;
            }
            case 241: {  // 0x0064c088
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591be());  // 0x00659ca0
                set_value(r1);
                return r1;
            }
            case 242: {  // 0x0064c098
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591c2());  // 0x00659cc0
                set_value(r1);
                return r1;
            }
            case 243: {  // 0x0064c0a8
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a591c6(static_cast<std::int8_t>(joan::orig::arg::i32(v2))));  // 0x00659ce0
                set_value(r3);
                return r3;
            }
            case 244: {  // 0x0064c0c9
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a591ba(joan::orig::arg::i32(v2)));  // 0x00659d10
                set_value(r3);
                return r3;
            }
            case 245: {  // 0x0064c0ea
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591ba());  // 0x00659d40
                set_value(r1);
                return r1;
            }
            case 246: {  // 0x0064c0fa
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::clear_a591c8());  // 0x00659d60
                set_value(r1);
                return r1;
            }
            case 247: {  // 0x0064c10a
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591c8());  // 0x00659d90
                set_value(r1);
                return r1;
            }
            case 248: {  // 0x0064c11a
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_counter_post_inc());  // 0x00659db0
                set_value(r1);
                return r1;
            }
            case 249: {  // 0x0064c12a
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::call_4cdd40(joan::orig::arg::u32(v2)));  // 0x00659de0
                set_value(r3);
                return r3;
            }
            case 250: {  // 0x0064c14b
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_a58fa7_4cdc40(joan::orig::arg::i16(v2)));  // 0x00659e10
                set_value(r3);
                return r3;
            }
            case 238: {  // 0x0064c16c
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591ad());  // 0x00659e40
                set_value(r1);
                return r1;
            }
            case 251: {  // 0x0064c17c
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591b1());  // 0x00659e60
                set_value(r1);
                return r1;
            }
            case 252: {  // 0x0064c18c
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_clear_a591a5());  // 0x00659ef0
                set_value(r1);
                return r1;
            }
            case 253: {  // 0x0064c19c
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591a5());  // 0x00659f20
                set_value(r1);
                return r1;
            }
            case 254: {  // 0x0064c1ac
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                void* const c5 = child(1);
                const char* const v6 = static_cast<const char*>(jo::as_script_node(c5)->value);
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_set_stat(joan::orig::arg::str(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x00659f40
                set_value(r7);
                return r7;
            }
            case 255: {  // 0x0064c1e9
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_stat_table(joan::orig::arg::str(v4), joan::orig::arg::i32(v2)));  // 0x0065a010
                set_value(r5);
                return r5;
            }
            case 256: {  // 0x0064c218
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                void* const c5 = child(1);
                const char* const v6 = static_cast<const char*>(jo::as_script_node(c5)->value);
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script::set_stat(joan::orig::arg::str(v6), joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x0065a0e0
                set_value(r7);
                return r7;
            }
            case 257: {  // 0x0064c255
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_attribute(joan::orig::arg::str(v4), joan::orig::arg::i32(v2)));  // 0x0065a1b0
                set_value(r5);
                return r5;
            }
            case 258: {  // 0x0064c284
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::set_a59294(joan::orig::arg::u32(v2)));  // 0x0065a2a0
                set_value(r3);
                return r3;
            }
            case 259: {  // 0x0064c2a5
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a59294());  // 0x0065a280
                set_value(r1);
                return r1;
            }
            case 260: {  // 0x0064c2b5
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::wrap_65a2d0());  // 0x0065a2d0
                set_value(r1);
                return r1;
            }
            case 261: {  // 0x0064c2c5
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_a5b9e8_432300(joan::orig::arg::i32(v2)));  // 0x0065a2f0
                set_value(r3);
                return r3;
            }
            case 262: {  // 0x0064c2e6
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_42e040());  // 0x0065a320
                set_value(r1);
                return r1;
            }
            case 263: {  // 0x0064c2f6
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::wrap_65a350());  // 0x0065a350
                set_value(r1);
                return r1;
            }
            case 264: {  // 0x0064c306
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::wrap_65a370());  // 0x0065a370
                set_value(r1);
                return r1;
            }
            case 265: {  // 0x0064c316
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_table_set_byte(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065a390
                set_value(r5);
                return r5;
            }
            case 268: {  // 0x0064c619
                const char* const v2 = sval(2);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::objective_467390(joan::orig::arg::u32(v4), v2));  // 0x0065a460
                set_value(r5);
                return r5;
            }
            case 269: {  // 0x0064c648
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_objective_state(joan::orig::arg::i32(v4), joan::orig::arg::str(v2)));  // 0x0065a4a0
                set_value(r5);
                return r5;
            }
            case 270: {  // 0x0064c677
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_table_word(joan::orig::arg::i32(v2)));  // 0x0065a670
                set_value(r3);
                return r3;
            }
            case 271: {  // 0x0064c698
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_table_set_word(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065a6a0
                set_value(r5);
                return r5;
            }
            case 272: {  // 0x0064c6c7
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::set_a592a8_word(joan::orig::arg::i32(v4), joan::orig::arg::u16(v2)));  // 0x0065a6e0
                set_value(r5);
                return r5;
            }
            case 273: {  // 0x0064c6f6
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_objective_set(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065a720
                set_value(r5);
                return r5;
            }
            case 274: {  // 0x0064c725
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_table_word_aa(joan::orig::arg::i32(v2)));  // 0x0065a7e0
                set_value(r3);
                return r3;
            }
            case 275: {  // 0x0064c746
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_table_set(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065a810
                set_value(r5);
                return r5;
            }
            case 276: {  // 0x0064c775
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::objective_set_15(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x0065a840
                set_value(r5);
                return r5;
            }
            case 277: {  // 0x0064c7a4
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::call_472000(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x0065a870
                set_value(r5);
                return r5;
            }
            case 278: {  // 0x0064c7d3
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::call_4720d0(joan::orig::arg::u32(v2)));  // 0x0065a8a0
                set_value(r3);
                return r3;
            }
            case 279: {  // 0x0064c7f4
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_65a8d0(joan::orig::arg::u32(v4), static_cast<std::uint8_t>(joan::orig::arg::u32(v2))));  // 0x0065a8d0
                set_value(r5);
                return r5;
            }
            case 280: {  // 0x0064c823
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::call_472220(joan::orig::arg::u32(v2)));  // 0x0065a900
                set_value(r3);
                return r3;
            }
            case 282: {  // 0x0064c844
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_set_bit(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065a960
                set_value(r5);
                return r5;
            }
            case 283: {  // 0x0064c873
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_65a990(joan::orig::arg::u32(v2)));  // 0x0065a990
                set_value(r3);
                return r3;
            }
            case 284: {  // 0x0064c894
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_a58f90_set_bit(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065a9c0
                set_value(r5);
                return r5;
            }
            case 285: {  // 0x0064c8c3
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::call_4724c0(joan::orig::arg::u32(v2)));  // 0x0065a9f0
                set_value(r3);
                return r3;
            }
            case 281: {  // 0x0064c8e4
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::call_472540());  // 0x0065a930
                set_value(r1);
                return r1;
            }
            case 286: {  // 0x0064c8f4
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_19a(joan::orig::arg::i32(v2)));  // 0x0065aa20
                set_value(r3);
                return r3;
            }
            case 287: {  // 0x0064c915
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::current_19a());  // 0x0065aa80
                set_value(r1);
                return r1;
            }
            case 288: {  // 0x0064c925
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::call_4cd570(v2));  // 0x0065aad0
                set_value(r3);
                return r3;
            }
            case 291: {  // 0x0064cbfe
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_copy_rect_a58f97());  // 0x0065ab90
                set_value(r1);
                return r1;
            }
            case 292: {  // 0x0064cc0e
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_store_rect());  // 0x0065aca0
                set_value(r1);
                return r1;
            }
            case 293: {  // 0x0064cc1e
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_flag_a501da(joan::orig::arg::i32(v2)));  // 0x0065adb0
                set_value(r3);
                return r3;
            }
            case 294: {  // 0x0064cc3f
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::camera::nudge_6cb9e0(joan::orig::arg::i32(v2)));  // 0x0065ade0
                set_value(r3);
                return r3;
            }
            case 298: {  // 0x0064ce92
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::fn_65aff0(static_cast<char>(joan::orig::arg::i32(v2)), static_cast<char>(joan::orig::arg::i32(0u))));  // 0x0065aff0
                set_value(r3);
                return r3;
            }
            case 299: {  // 0x0064ceb5
                void* const c1 = child(2);
                const u8 v2 = static_cast<u8>(ival(c1));
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::fn_65aff0(static_cast<char>(joan::orig::arg::i32(v4)), static_cast<char>(joan::orig::arg::i32(v2))));  // 0x0065aff0
                set_value(r5);
                return r5;
            }
            case 300: {  // 0x0064cfce
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::wrap_65afd0());  // 0x0065afd0
                set_value(r1);
                return r1;
            }
            case 303: {  // 0x0064cfde
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::game_state_is_7());  // 0x0065b0e0
                set_value(r1);
                return r1;
            }
            case 295: {  // 0x0064cfee
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::game_flag_744());  // 0x0065b130
                set_value(r1);
                return r1;
            }
            case 301: {  // 0x0064cffe
                void* const c1 = child(6);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(5);
                const i16 v4 = static_cast<i16>(ival(c3));
                void* const c5 = child(4);
                const u32 v6 = ival(c5);
                void* const c7 = child(3);
                const u32 v8 = ival(c7);
                void* const c9 = child(2);
                const char* const v10 = static_cast<const char*>(jo::as_script_node(c9)->value);
                void* const c11 = child(1);
                const i16 v12 = static_cast<i16>(ival(c11));
                const u32 r13 = joan::orig::arg::as<u32>(joan::orig::script_unit_6537a0(joan::orig::arg::i16(v12), v10, joan::orig::arg::i32(v8), joan::orig::arg::u32(v6), joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x006537a0
                set_value(r13);
                return r13;
            }
            case 304: {  // 0x0064d068
                void* const c1 = child(2);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_open_buy(joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x00653890
                set_value(r5);
                return r5;
            }
            case 305: {  // 0x0064d099
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_toggle_a56398());  // 0x00653930
                set_value(r1);
                return r1;
            }
            case 306: {  // 0x0064d0a9
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_653970());  // 0x00653970
                set_value(r1);
                return r1;
            }
            case 307: {  // 0x0064d0b9
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a5027e(joan::orig::arg::i32(v2)));  // 0x006539a0
                set_value(r3);
                return r3;
            }
            case 308: {  // 0x0064d0da
                void* const c1 = child(6);
                const u8 v2 = static_cast<u8>(ival(c1));
                void* const c3 = child(5);
                const u32 v4 = ival(c3);
                void* const c5 = child(4);
                const u32 v6 = ival(c5);
                void* const c7 = child(3);
                const u32 v8 = ival(c7);
                void* const c9 = child(2);
                const u32 v10 = ival(c9);
                void* const c11 = child(1);
                const i16 v12 = static_cast<i16>(ival(c11));
                const Vec3f pos13 = vec3_bits(v10, v8, v6);  // V3 passed by value: three floats (XMM registers on System V x86-64, like the callee's struct)
                const u32 r13 = joan::orig::arg::as<u32>(joan::orig::script_fx_718010(v12, pos13, static_cast<std::int8_t>(v4), static_cast<std::int8_t>(v2)));  // 0x006539e0
                set_value(r13);
                return r13;
            }
            case 309: {  // 0x0064d142
                void* const c1 = child(3);
                const u8 v2 = static_cast<u8>(ival(c1));
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                void* const c5 = child(1);
                const i16 v6 = static_cast<i16>(ival(c5));
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_653a50(joan::orig::arg::i16(v6), joan::orig::arg::u32(v4), static_cast<std::uint8_t>(joan::orig::arg::u32(v2))));  // 0x00653a50
                set_value(r7);
                return r7;
            }
            case 310: {  // 0x0064d180
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const u32 v4 = ival(c3);
                void* const c5 = child(2);
                const u32 v6 = ival(c5);
                void* const c7 = child(1);
                const i16 v8 = static_cast<i16>(ival(c7));
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::unit::distance_to(joan::orig::arg::i16(v8), joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065b800
                set_value(r9);
                return r9;
            }
            case 311: {  // 0x0064d1cc
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::soldier_set_374(joan::orig::arg::i16(v4), joan::orig::arg::u32(v2)));  // 0x0065b8c0
                set_value(r5);
                return r5;
            }
            case 314: {  // 0x0064d1fc
                void* const c1 = child(3);
                const i16 v2 = static_cast<i16>(ival(c1));
                void* const c3 = child(2);
                const i16 v4 = static_cast<i16>(ival(c3));
                void* const c5 = child(1);
                const i16 v6 = static_cast<i16>(ival(c5));
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script::mount_65bce0(joan::orig::arg::i16(v6), joan::orig::arg::i16(v4), static_cast<char>(joan::orig::arg::i32(v2))));  // 0x0065bce0
                set_value(r7);
                return r7;
            }
            case 315: {  // 0x0064d23c
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::save_list::add(joan::orig::arg::str(v4), joan::orig::arg::str(v2), 0u));  // 0x0065bed0
                set_value(r5);
                return r5;
            }
            case 312: {  // 0x0064d26d
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_374(joan::orig::arg::i16(v2)));  // 0x0065b950
                set_value(r3);
                return r3;
            }
            case 313: {  // 0x0064d28f
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_refresh_items(joan::orig::arg::i16(v2)));  // 0x0065b9b0
                set_value(r3);
                return r3;
            }
            case 41: {  // 0x0064d2b1
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_type_5270b0(v2));  // 0x0065b160
                set_value(r3);
                return r3;
            }
            case 42: {  // 0x0064d2d2
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_type_activate(v2));  // 0x0065b190
                set_value(r3);
                return r3;
            }
            case 43: {  // 0x0064d2f3
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_type_51fd50(v2));  // 0x0065b200
                set_value(r3);
                return r3;
            }
            case 44: {  // 0x0064d314
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::soldier_51fdd0(v2));  // 0x0065b240
                set_value(r3);
                return r3;
            }
            case 45: {  // 0x0064d335
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::unit::range_for_level(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065b280
                set_value(r5);
                return r5;
            }
            case 46: {  // 0x0064d364
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_135_pct(joan::orig::arg::i32(v2)));  // 0x0065b500
                set_value(r3);
                return r3;
            }
            case 328: {  // 0x0064d385
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const i16 v4 = static_cast<i16>(ival(c3));
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::unit_set_mood(joan::orig::arg::i32(v4), joan::orig::arg::str(v2)));  // 0x0065c050
                set_value(r5);
                return r5;
            }
            case 329: {  // 0x0064d3b5
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const u32 v4 = ival(c3);
                void* const c5 = child(2);
                const u32 v6 = ival(c5);
                void* const c7 = child(1);
                const char* const v8 = static_cast<const char*>(jo::as_script_node(c7)->value);
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::script::call_761680(v8, joan::orig::arg::u32(v6), joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x0065c420
                set_value(r9);
                return r9;
            }
            case 330: {  // 0x0064d400
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a6a254(joan::orig::arg::i32(v2)));  // 0x0065c4a0
                set_value(r3);
                return r3;
            }
            case 331: {  // 0x0064d421
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a596ab(joan::orig::arg::i32(v2)));  // 0x0065c500
                set_value(r3);
                return r3;
            }
            case 332: {  // 0x0064d442
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_post_event_65c530(joan::orig::arg::u32(v4), joan::orig::arg::i32(v2)));  // 0x0065c530
                set_value(r5);
                return r5;
            }
            case 333: {  // 0x0064d471
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const i16 v4 = static_cast<i16>(ival(c3));
                void* const c5 = child(2);
                const i16 v6 = static_cast<i16>(ival(c5));
                void* const c7 = child(1);
                const char* const v8 = static_cast<const char*>(jo::as_script_node(c7)->value);
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::unit_order_65c5f0(v8, joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::u32(v2)));  // 0x0065c5f0
                set_value(r9);
                return r9;
            }
            case 334: {  // 0x0064d4be
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const i16 v4 = static_cast<i16>(ival(c3));
                void* const c5 = child(2);
                const i16 v6 = static_cast<i16>(ival(c5));
                void* const c7 = child(1);
                const char* const v8 = static_cast<const char*>(jo::as_script_node(c7)->value);
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::script_icon_65c770(v8, joan::orig::arg::u32(v6), joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x0065c770
                set_value(r9);
                return r9;
            }
            case 335: {  // 0x0064d50b
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_call_510030(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065c870
                set_value(r5);
                return r5;
            }
            case 336: {  // 0x0064d53a
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit::idle(joan::orig::arg::i32(v2)));  // 0x0065c8d0
                set_value(r3);
                return r3;
            }
            case 337: {  // 0x0064d55b
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_name_check(joan::orig::arg::i32(v4), joan::orig::arg::str(v2)));  // 0x0065c930
                set_value(r5);
                return r5;
            }
            case 338: {  // 0x0064d58a
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_time_since());  // 0x0065c9b0
                set_value(r1);
                return r1;
            }
            case 339: {  // 0x0064d59a
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::soldier_5229a0(joan::orig::arg::i32(v4), joan::orig::arg::u32(v2)));  // 0x0065c9e0
                set_value(r5);
                return r5;
            }
            case 340: {  // 0x0064d5c9
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_65ca40(v4, joan::orig::arg::i32(v2)));  // 0x0065ca40
                set_value(r5);
                return r5;
            }
            case 341: {  // 0x0064d5f8
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_call_510170(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065cac0
                set_value(r5);
                return r5;
            }
            case 342: {  // 0x0064d627
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_count_allies(joan::orig::arg::i32(v2)));  // 0x0065cb80
                set_value(r3);
                return r3;
            }
            case 343: {  // 0x0064d648
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_count_allies(joan::orig::arg::i32(v2)));  // 0x0065cd00
                set_value(r3);
                return r3;
            }
            case 347: {  // 0x0064dbde
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_65cfd0(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065cfd0
                set_value(r5);
                return r5;
            }
            case 348: {  // 0x0064dc0d
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::fn_65d070(joan::orig::arg::i32(v4), joan::orig::arg::u32(v2)));  // 0x0065d070
                set_value(r5);
                return r5;
            }
            case 349: {  // 0x0064dc3c
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_set_unit_light(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065d0d0
                set_value(r5);
                return r5;
            }
            case 350: {  // 0x0064dc6b
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_apply_group(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065d350
                set_value(r5);
                return r5;
            }
            case 351: {  // 0x0064dc9a
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                void* const c5 = child(1);
                const u32 v6 = ival(c5);
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_unit_65d450(joan::orig::arg::u32(v6), joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x0065d450
                set_value(r7);
                return r7;
            }
            case 352: {  // 0x0064dcd7
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::soldier_near_target(joan::orig::arg::i32(v2)));  // 0x0065d560
                set_value(r3);
                return r3;
            }
            case 355: {  // 0x0064dd62
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_group_size_of(joan::orig::arg::i32(v2)));  // 0x0065d8a0
                set_value(r3);
                return r3;
            }
            case 356: {  // 0x0064dd83
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                const char* const v6 = sval(1);
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_place_effect(v6, joan::orig::arg::i16(v4), joan::orig::arg::i16(v2)));  // 0x0065d9c0
                set_value(r7);
                return r7;
            }
            case 357: {  // 0x0064ddc0
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                void* const c5 = child(1);
                const u32 v6 = ival(c5);
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_spawn_effect(joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065da50
                set_value(r7);
                return r7;
            }
            case 359: {  // 0x0064de61
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::unit_kick_65dba0(joan::orig::arg::i32(v2)));  // 0x0065dba0
                set_value(r3);
                return r3;
            }
            case 361: {  // 0x0064dea3
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_unit_388(joan::orig::arg::i32(v2)));  // 0x0065dfa0
                set_value(r3);
                return r3;
            }
            case 362: {  // 0x0064dec4
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_object_65dff0(joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x0065dff0
                set_value(r5);
                return r5;
            }
            case 365: {  // 0x0064e409
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a50522(joan::orig::arg::i32(v2)));  // 0x0065e7d0
                set_value(r3);
                return r3;
            }
            case 367: {  // 0x0064e469
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_6dff10());  // 0x0065e850
                set_value(r1);
                return r1;
            }
            case 368: {  // 0x0064e479
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a50526(joan::orig::arg::i32(v2)));  // 0x0065e880
                set_value(r3);
                return r3;
            }
            case 370: {  // 0x0064e4d9
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_a50518_6e01d0());  // 0x0065e900
                set_value(r1);
                return r1;
            }
            case 372: {  // 0x0064e569
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_object_set_7e(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065e930
                set_value(r5);
                return r5;
            }
            case 373: {  // 0x0064e598
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_3c0(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065e9a0
                set_value(r5);
                return r5;
            }
            case 374: {  // 0x0064e5c7
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_object_a2(joan::orig::arg::i32(v2)));  // 0x0065ea00
                set_value(r3);
                return r3;
            }
            case 378: {  // 0x0064e64b
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_a4f381_call(v4, joan::orig::arg::i32(v2)));  // 0x0065ec90
                set_value(r5);
                return r5;
            }
            case 379: {  // 0x0064e67a
                void* const c1 = child(1);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::call_71d1e0(v2));  // 0x0065ecf0
                set_value(r3);
                return r3;
            }
            case 380: {  // 0x0064e69b
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a5b9f8());  // 0x0065ed50
                set_value(r1);
                return r1;
            }
            case 381: {  // 0x0064e6ab
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_state_is_4());  // 0x0065ed70
                set_value(r1);
                return r1;
            }
            case 382: {  // 0x0064e6bb
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_word_a4f3ac(joan::orig::arg::i16(v2)));  // 0x0065eda0
                set_value(r3);
                return r3;
            }
            case 383: {  // 0x0064e6dc
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_a4f381_set_11(joan::orig::arg::i16(v2)));  // 0x0065edd0
                set_value(r3);
                return r3;
            }
            case 384: {  // 0x0064e6fd
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::set_a4f858(static_cast<std::uint8_t>(joan::orig::arg::u32(v2))));  // 0x0065ee30
                set_value(r3);
                return r3;
            }
            case 385: {  // 0x0064e71e
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::f_65ee60());  // 0x0065ee60
                set_value(r1);
                return r1;
            }
            case 386: {  // 0x0064e72e
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script_flush_436190());  // 0x0065eee0
                set_value(r1);
                return r1;
            }
            case 387: {  // 0x0064e73e
                void* const c1 = child(3);
                const u32 v2 = ival(c1);
                void* const c3 = child(2);
                const u32 v4 = ival(c3);
                void* const c5 = child(1);
                const u32 v6 = ival(c5);
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::script_group_transfer(joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065ef40
                set_value(r7);
                return r7;
            }
            case 388: {  // 0x0064e77b
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_heading_to(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065f0f0
                set_value(r5);
                return r5;
            }
            case 389: {  // 0x0064e7aa
                void* const c1 = child(3);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(2);
                const i16 v4 = static_cast<i16>(ival(c3));
                void* const c5 = child(1);
                const i16 v6 = static_cast<i16>(ival(c5));
                const u32 r7 = joan::orig::arg::as<u32>(joan::orig::formation::assign_slot(joan::orig::arg::i16(v6), joan::orig::arg::i16(v4), joan::orig::arg::str(v2)));  // 0x0065f160
                set_value(r7);
                return r7;
            }
            case 391: {  // 0x0064e829
                void* const c1 = child(1);
                const i16 v2 = static_cast<i16>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_byte_a5ba64(static_cast<std::uint8_t>(joan::orig::arg::u32(v2))));  // 0x0065f670
                set_value(r3);
                return r3;
            }
            case 392: {  // 0x0064e84b
                static_cast<void>(child(1));  // its value was pushed but 0x0065f6a0 reads no argument
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script::flush_a50088());  // 0x0065f6a0
                set_value(r3);
                return r3;
            }
            case 393: {  // 0x0064e86d
                void* const c1 = child(5);
                const u32 v2 = ival(c1);
                void* const c3 = child(4);
                const u32 v4 = ival(c3);
                void* const c5 = child(3);
                const u32 v6 = ival(c5);
                void* const c7 = child(2);
                const u32 v8 = ival(c7);
                void* const c9 = child(1);
                const u32 v10 = ival(c9);
                const u32 r11 = joan::orig::arg::as<u32>(joan::orig::script_a4f381_7196e0(joan::orig::arg::i32(v10), joan::orig::arg::i32(v8), joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065f740
                set_value(r11);
                return r11;
            }
            case 394: {  // 0x0064e8c6
                void* const c1 = child(4);
                const u32 v2 = ival(c1);
                void* const c3 = child(3);
                const u32 v4 = ival(c3);
                void* const c5 = child(2);
                const u32 v6 = ival(c5);
                void* const c7 = child(1);
                const u32 v8 = ival(c7);
                const u32 r9 = joan::orig::arg::as<u32>(joan::orig::script_a4f381_719770(joan::orig::arg::i32(v8), joan::orig::arg::i32(v6), joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065f780
                set_value(r9);
                return r9;
            }
            case 395: {  // 0x0064e911
                void* const c1 = child(9);
                const u32 v2 = ival(c1);
                void* const c3 = child(8);
                const u32 v4 = ival(c3);
                void* const c5 = child(7);
                const u32 v6 = ival(c5);
                void* const c7 = child(6);
                const u32 v8 = ival(c7);
                void* const c9 = child(5);
                const u32 v10 = ival(c9);
                void* const c11 = child(4);
                const u32 v12 = ival(c11);
                void* const c13 = child(3);
                const u32 v14 = ival(c13);
                void* const c15 = child(2);
                const u32 v16 = ival(c15);
                void* const c17 = child(1);
                const u32 v18 = ival(c17);
                const u32 r19 = joan::orig::arg::as<u32>(joan::orig::script_a4f381_7197f0(joan::orig::arg::u32(v18), joan::orig::arg::u32(v16), joan::orig::arg::u32(v14), joan::orig::arg::u32(v12), joan::orig::arg::u32(v10), joan::orig::arg::u32(v8), joan::orig::arg::u32(v6), joan::orig::arg::u32(v4), joan::orig::arg::u32(v2)));  // 0x0065f7c0
                set_value(r19);
                return r19;
            }
            case 396: {  // 0x0064e9a2
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_call_7198d0(joan::orig::arg::i32(v2)));  // 0x0065f820
                set_value(r3);
                return r3;
            }
            case 397: {  // 0x0064e9c3
                void* const c1 = child(2);
                const char* const v2 = static_cast<const char*>(jo::as_script_node(c1)->value);
                void* const c3 = child(1);
                const char* const v4 = static_cast<const char*>(jo::as_script_node(c3)->value);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::set_side(v4, joan::orig::arg::str(v2)));  // 0x0065f850
                set_value(r5);
                return r5;
            }
            case 398: {  // 0x0064e9f2
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_65f910(joan::orig::arg::i16(v2)));  // 0x0065f910
                set_value(r3);
                return r3;
            }
            case 399: {  // 0x0064ea13
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_3c1_65f980(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065f980
                set_value(r5);
                return r5;
            }
            case 400: {  // 0x0064ea42
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_3c1(joan::orig::arg::i32(v4), joan::orig::arg::i32(v2)));  // 0x0065f9e0
                set_value(r5);
                return r5;
            }
            case 401: {  // 0x0064ea71
                void* const c1 = child(2);
                const u32 v2 = ival(c1);
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script_unit_set_30(joan::orig::arg::i32(v4), static_cast<std::int8_t>(joan::orig::arg::i32(v2))));  // 0x0065fb50
                set_value(r5);
                return r5;
            }
            case 402: {  // 0x0064eaa0
                void* const c1 = child(1);
                const u32 v2 = ival(c1);
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_a596c8(joan::orig::arg::i32(v2)));  // 0x0065fba0
                set_value(r3);
                return r3;
            }
            case 403: {  // 0x0064eac1
                const u32 r1 = joan::orig::arg::as<u32>(joan::orig::script::get_a591c7());  // 0x0065fbd0
                set_value(r1);
                return r1;
            }
            case 405: {  // 0x0064ec2a
                void* const c1 = child(1);
                const u8 v2 = static_cast<u8>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_set_byte_a5d386(static_cast<std::uint8_t>(joan::orig::arg::u32(v2))));  // 0x0065fc90
                set_value(r3);
                return r3;
            }
            case 406: {  // 0x0064ec48
                void* const c1 = child(1);
                const u8 v2 = static_cast<u8>(ival(c1));
                const u32 r3 = joan::orig::arg::as<u32>(joan::orig::script_65fcc0(static_cast<std::int8_t>(joan::orig::arg::i32(v2))));  // 0x0065fcc0
                set_value(r3);
                return r3;
            }
            case 407: {  // 0x0064ec66
                void* const c1 = child(2);
                const u8 v2 = static_cast<u8>(ival(c1));
                void* const c3 = child(1);
                const u32 v4 = ival(c3);
                const u32 r5 = joan::orig::arg::as<u32>(joan::orig::script::soldier_set_358(joan::orig::arg::i32(v4), static_cast<std::uint8_t>(joan::orig::arg::u32(v2))));  // 0x0065fd10
                set_value(r5);
                return r5;
            }
            case 4: {  // 0x00649bc6
                u8 s[kStr];
                joan::orig::as_string(s)->construct();  // 0x005a1170
                load_string(1, s);
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script_set_6b8260(cstr(s)));  // 0x0065ebd0
                set_value(r);
                return r;
            }
            case 63: {  // 0x0064a232
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script::get_word_ptr(joan::orig::arg::mptr(arg)));  // 0x00653130
                set_value(r);
                return r;
            }
            case 120: {  // 0x0064b59e
                ensure_string();
                void* const c = child(1);
                const u32 v = ival(c);
                return joan::orig::arg::as<u32>(joan::orig::script_unit_name(joan::orig::arg::mptr(jo::self<jo::ScriptFuncCall>(this)->value), joan::orig::arg::i32(v)));  // 0x006563b0
            }
            case 211: {  // 0x0064be2b
                fvalue() = joan::orig::arg::as<float>(joan::orig::script::wrap_659740());  // 0x00659740
                return self_eax();
            }
            case 212: {  // 0x0064be3b: _ftol of an fmod(float, 60.0) result (exact)
                const double d = joan::orig::arg::as<double>(joan::orig::clock::minutes_mod());  // 0x00659790
                const u32 r = static_cast<u32>(joan::hook::x87::ftol(d));
                set_value(r);
                return r;
            }
            case 266: {  // 0x0064c345
                u8 s[kStr];
                joan::orig::as_string(s)->construct();  // 0x005a1170
                load_string(2, s);
                const char* const p = cstr(s);
                void* const c = child(1);
                const u32 v = ival(c);
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script::objective_467300(joan::orig::arg::u32(v), p));  // 0x0065a3c0
                set_value(r);
                return r;
            }
            case 267: {  // 0x0064c4af
                u8 s[kStr];
                joan::orig::as_string(s)->construct();  // 0x005a1170
                load_string(2, s);
                const char* const p = cstr(s);
                void* const c = child(1);
                const u32 v = ival(c);
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script::objective_update(joan::orig::arg::u32(v), p));  // 0x0065a400
                set_value(r);
                return r;
            }
            case 289: {  // 0x0064c946
                u8 s[kStr];
                joan::orig::as_string(s)->construct();  // 0x005a1170
                load_string(1, s);
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script::call_4cde30(cstr(s)));  // 0x0065ab10
                set_value(r);
                return r;
            }
            case 290: {  // 0x0064caa2
                u8 s[kStr];
                joan::orig::as_string(s)->construct();  // 0x005a1170
                load_string(1, s);
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script_list_a58fa7_call(cstr(s)));  // 0x0065ab50
                set_value(r);
                return r;
            }
            case 296: {  // 0x0064cc60
                u32 r;
                if (count() == 6) {
                    const float f6 = fval(6), f5 = fval(5), f4 = fval(4);
                    const u32 v3 = dval(3), v2 = dval(2), v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::menu_text_65aef0(static_cast<char>(joan::orig::arg::i32(v1)), static_cast<char>(joan::orig::arg::i32(v2)), joan::orig::arg::u32(v3), joan::orig::arg::bits(f4), joan::orig::arg::bits(f5), joan::orig::arg::bits(f6), static_cast<char>(joan::orig::arg::i32(0u))));  // 0x0065aef0
                } else {
                    const float f8 = fval(8), f7 = fval(7), f6 = fval(6);
                    const u32 v5 = dval(5), v4 = dval(4), v3 = dval(3), v2 = dval(2), v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::spawn_fx_65ae60(static_cast<std::int8_t>(joan::orig::arg::i32(v1)), static_cast<std::int8_t>(joan::orig::arg::i32(v2)), joan::orig::arg::i32(v3), joan::orig::arg::i32(v4), joan::orig::arg::i32(v5), joan::orig::arg::bits(f6), joan::orig::arg::bits(f7), joan::orig::arg::bits(f8), static_cast<std::int8_t>(joan::orig::arg::i32(0u))));  // 0x0065ae60
                }
                set_value(r);
                return r;
            }
            case 297: {  // 0x0064cd6d
                u32 r;
                if (count() == 7) {
                    const u8 b7 = bval(7);
                    const float f6 = fval(6), f5 = fval(5), f4 = fval(4);
                    const u32 v3 = dval(3), v2 = dval(2), v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::menu_text_65aef0(static_cast<char>(joan::orig::arg::i32(v1)), static_cast<char>(joan::orig::arg::i32(v2)), joan::orig::arg::u32(v3), joan::orig::arg::bits(f4), joan::orig::arg::bits(f5), joan::orig::arg::bits(f6), static_cast<char>(joan::orig::arg::i32(b7))));  // 0x0065aef0
                } else {
                    const u8 b9 = bval(9);
                    const float f8 = fval(8), f7 = fval(7), f6 = fval(6);
                    const u32 v5 = dval(5), v4 = dval(4), v3 = dval(3), v2 = dval(2), v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::spawn_fx_65ae60(static_cast<std::int8_t>(joan::orig::arg::i32(v1)), static_cast<std::int8_t>(joan::orig::arg::i32(v2)), joan::orig::arg::i32(v3), joan::orig::arg::i32(v4), joan::orig::arg::i32(v5), joan::orig::arg::bits(f6), joan::orig::arg::bits(f7), joan::orig::arg::bits(f8), static_cast<std::int8_t>(joan::orig::arg::i32(b9))));  // 0x0065ae60
                }
                set_value(r);
                return r;
            }
            case 302: {  // 0x0064cee4
                u32 r;
                if (count() == 5) {
                    const float f5 = fval(5), f4 = fval(4), f3 = fval(3);
                    const u32 v2 = dval(2);
                    const u8 b1 = bval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::forward_6cd720(static_cast<std::int8_t>(joan::orig::arg::i32(b1)), joan::orig::arg::u32(v2), joan::orig::arg::bits(f3), joan::orig::arg::bits(f4), joan::orig::arg::bits(f5)));  // 0x0065b030
                } else {
                    const float f7 = fval(7), f6 = fval(6), f5 = fval(5);
                    const u32 v4 = dval(4), v3 = dval(3), v2 = dval(2);
                    const u8 b1 = bval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::fx_at_cell_65b070(static_cast<char>(joan::orig::arg::i32(b1)), joan::orig::arg::i32(v2), joan::orig::arg::i32(v3), joan::orig::arg::i32(v4), joan::orig::arg::bits(f5), joan::orig::arg::bits(f6), joan::orig::arg::bits(f7)));  // 0x0065b070
                }
                set_value(r);
                return r;
            }
            case 344: {  // 0x0064d669
                count();  // result unused
                u8 s[kStr];
                joan::orig::as_string(s)->construct();  // 0x005a1170
                load_string(1, s);
                u32 r;
                if (count() == 1) {
                    r = joan::orig::arg::as<u32>(joan::orig::message_65cea0(cstr(s), 0u));  // 0x0065cea0
                } else {
                    const u32 v2 = dval(2);
                    r = joan::orig::arg::as<u32>(joan::orig::message_65cea0(cstr(s), joan::orig::arg::u32(v2)));  // 0x0065cea0
                }
                set_value(r);
                return r;
            }
            case 345: {  // 0x0064d806
                u8 s[kStr];
                joan::orig::as_string(s)->construct();  // 0x005a1170
                i16 id = 0;
                void* root = nullptr;
                if (load_string(1, s, &root)) id = register_name(root, 1);
                void* c = child(2);
                if (jo::as_script_node(c)->value_type == 3) {
                    const i32 a = static_cast<i32>(id);
                    const char* const v2 = sval(2);
                    const u32 r = joan::orig::arg::as<u32>(joan::orig::msg::post_65cf00(cstr(s), v2, joan::orig::arg::u32(a)));  // 0x0065cf00
                    set_value(r);
                    return r;
                }
                c = child(2);
                if (jo::as_script_node(c)->value_type == 1) {
                    const i32 a = static_cast<i32>(id);
                    const u32 v2 = dval(2);
                    const u32 r = joan::orig::arg::as<u32>(joan::orig::script_call_65cf40(cstr(s), joan::orig::arg::u32(v2), joan::orig::arg::u32(a)));  // 0x0065cf40
                    set_value(r);
                    return r;
                }
                return joan::call::register_low_bits(c);  // eax residue, result of exec_cmd is dropped by run_params
            }
            case 346: {  // 0x0064da7c
                void* const root = find_root();
                i16 id = 0;
                void* const tbl = joan::orig::arg::as<void*>(joan::orig::as_script(root)->dlg_res());  // 0x006509c0
                if (tbl != nullptr) {
                    const char* const v = sval(2);
                    const i16 idx = joan::orig::arg::as<i16>(joan::orig::as_table(tbl)->index_of_ref(joan::orig::arg::str(v)));  // 0x006380b0
                    if (idx != 0) id = register_name(root, 2);
                }
                const i32 a = static_cast<i32>(id);
                const u32 v1 = dval(1);
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script_name_set_19(joan::orig::arg::i32(v1), joan::orig::arg::i32(a)));  // 0x0065cf80
                set_value(r);
                return r;
            }
            case 353: {  // 0x0064dcf8: float result; original eax = callee's stale eax (unused)
                const u32 v3 = dval(3), v2 = dval(2), v1 = dval(1);
                fvalue() = joan::orig::arg::as<float>(joan::orig::unit::marker_distance_65d6f0(joan::orig::arg::i32(v1), joan::orig::arg::i32(v2), joan::orig::arg::i32(v3)));  // 0x0065d6f0
                return 0;
            }
            case 354: {  // 0x0064dd35
                const u32 v1 = dval(1);
                const i32 r = joan::orig::arg::as<i32>(joan::orig::script_unit_3c9(joan::orig::arg::i32(v1)));  // 0x0065d850
                fvalue() = static_cast<float>(r);
                return static_cast<u32>(r);
            }
            case 358: {  // 0x0064ddfd
                count();  // result unused
                u32 r;
                if (count() == 2) {
                    const u32 v2 = dval(2), v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::unit_order_65daf0(joan::orig::arg::i32(v1), joan::orig::arg::u32(v2)));  // 0x0065daf0
                } else {
                    const u32 v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::unit_order_65daf0(joan::orig::arg::i32(v1), 1u));  // 0x0065daf0
                }
                set_value(r);
                return r;
            }
            case 360: {  // 0x0064de82
                const u32 v1 = dval(1);
                fvalue() = joan::orig::arg::as<float>(joan::orig::script_target_hardpoint_dist(joan::orig::arg::i32(v1)));  // 0x0065ddb0
                return self_eax();
            }
            case 363:  // 0x0064def3
                return case_363();
            case 364: {  // 0x0064e37f
                ensure_string();
                void* const c = child(1);
                const char* const v = static_cast<const char*>(jo::as_script_node(c)->value);
                return joan::orig::arg::as<u32>(joan::orig::script_bind_input(joan::orig::arg::mptr(jo::self<jo::ScriptFuncCall>(this)->value), joan::orig::arg::str(v)));  // 0x0065e0a0
            }
            case 366: {  // 0x0064e42a
                u32 r;
                if (count() == 0) {
                    r = joan::orig::arg::as<u32>(joan::orig::script::call_6dfe10(static_cast<char>(joan::orig::arg::i32(0u))));  // 0x0065e810
                } else {
                    const u32 v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::script::call_6dfe10(static_cast<char>(joan::orig::arg::i32(v1))));  // 0x0065e810
                }
                set_value(r);
                return r;
            }
            case 369: {  // 0x0064e49a
                u32 r;
                if (count() == 0) {
                    r = joan::orig::arg::as<u32>(joan::orig::script::call_6e00b0(static_cast<char>(joan::orig::arg::i32(0u))));  // 0x0065e8c0
                } else {
                    const u32 v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::script::call_6e00b0(static_cast<char>(joan::orig::arg::i32(v1))));  // 0x0065e8c0
                }
                set_value(r);
                return r;
            }
            case 371: {  // 0x0064e4e9
                count();  // result unused
                u32 r;
                if (count() == 2) {
                    const u32 v2 = dval(2), v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::script_unit_65cb20(joan::orig::arg::u32(v1), joan::orig::arg::i32(v2), 0u));  // 0x0065cb20
                } else {
                    const u32 v3 = dval(3), v2 = dval(2), v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::script_unit_65cb20(joan::orig::arg::u32(v1), joan::orig::arg::i32(v2), joan::orig::arg::u32(v3)));  // 0x0065cb20
                }
                set_value(r);
                return r;
            }
            case 375: {  // 0x0064e5e8
                const u32 v1 = dval(1);
                fvalue() = joan::orig::arg::as<float>(joan::orig::script_object_height_65ea50(joan::orig::arg::i32(v1)));  // 0x0065ea50
                return self_eax();
            }
            case 376: {  // 0x0064e609: original eax = callee's stale eax (unused)
                const u32 v1 = dval(1);
                fvalue() = joan::orig::arg::as<float>(joan::orig::script::object_height(joan::orig::arg::u32(v1)));  // 0x0065ead0
                return 0;
            }
            case 377: {  // 0x0064e62a: original eax = callee's stale eax (unused)
                const u32 v1 = dval(1);
                fvalue() = joan::orig::arg::as<float>(joan::orig::script_object_height(joan::orig::arg::i32(v1)));  // 0x0065eb50
                return 0;
            }
            case 390: {  // 0x0064e7e9
                u32 r;
                if (count() == 1) {
                    const u32 v1 = dval(1);
                    r = joan::orig::arg::as<u32>(joan::orig::script_flag(joan::orig::arg::i32(v1)));  // 0x0065f600
                } else {
                    r = joan::orig::arg::as<u32>(joan::orig::script_flag(0));  // 0x0065f600
                }
                set_value(r);
                return r;
            }
            case 404: {  // 0x0064ead1
                u8 s[kStr];
                joan::orig::as_string(s)->construct();  // 0x005a1170
                load_string(1, s);
                const u32 r = joan::orig::arg::as<u32>(joan::orig::script_name_unused(joan::orig::arg::str(cstr(s))));  // 0x0065fbf0
                set_value(r);
                return r;
            }
            case 316:  // jump-table holes -> 0x0064ec90 with eax = code - 1 (the table index)
            case 317:
            case 318:
            case 319:
            case 320:
            case 321:
            case 322:
            case 323:
            case 324:
            case 326:
            case 327:
                return static_cast<u32>(code - 1);
            default:  // outside 1..0x197: `ja` with eax = this
                return self_eax();
        }
    }
};

JOAN_REPLACE("game/b110_02", "ScriptNode::exec_cmd", 0x00649920, &H::exec_cmd);

}  // namespace game_b110_02
}  // namespace

// ---------------------------------------- from modules/game_b110_09.cpp (game/b110_09)
namespace {
namespace game_b110_09 {

using joan::hook::x87::Fpu;

struct Vec3 {
    float x, y, z;
};

JOAN_COUNTER(c_on_removed, "Unit::on_removed");

constexpr img::Addr kUnits = d::g_units;
constexpr img::Addr kTypes = d::g_types;
constexpr img::Addr kEffects = d::g_items;
constexpr img::Addr kOrders = d::g_item_res;

struct H {
    // ======================================================================= 0x0064f320
    // One pending-order list (+0x1c1 -> +0x16 or +0x1a), walked from the back.
    void release_orders(jo::Grid* jo::UnitData::* list_m) {
        auto list = [&]() { return jo::self<jo::Unit>(this)->modifiers->*list_m; };
        for (i32 i = joan::orig::as_dynarray(list())->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_ptr_list(list())->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;    // 0x00457f60
            u8* e = joan::orig::arg::as<u8*>(joan::orig::as_ptr_list(list())->get(joan::orig::arg::i32(i)));                 // 0x00457e90
            u8* order = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(kOrders))->get(joan::orig::arg::i16(jo::as_inv_item(e)->type)));  // 0x00425ce0
            if (jo::view<i16>(jo::as_item_def(order)->class_id) != 0xf) continue;
            const i32 id = joan::orig::arg::as<i32>(joan::orig::as_spawner(this)->fire_512830(joan::orig::arg::i16(jo::as_inv_item(e)->type), joan::orig::arg::i16(jo::as_inv_item(e)->level), joan::orig::arg::i16(jo::as_inv_item(e)->count), joan::orig::arg::u32(jo::view<i32>(jo::as_inv_item(e)->u32_6)), joan::orig::arg::u32(jo::view<i32>(jo::as_inv_item(e)->u32_a)), joan::orig::arg::u32(jo::view<i32>(jo::as_inv_item(e)->u32_e))));  // 0x00512830
            if (id != 0) {
                u8* fx = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(kEffects))->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
                if (jo::as_map_object(fx)->actor != nullptr && (jo::self<jo::Unit>(this)->markers == 1 || jo::self<jo::Unit>(this)->markers == -2))
                    joan::orig::as_model_instance(jo::view<void*>(jo::as_map_object(fx)->actor))->set_10a(0u);  // 0x0042cf20
            }
            joan::orig::as_item_list(list())->remove(static_cast<i32>(jo::as_inv_item(e)->type), 0);  // 0x00458430
        }
    }

    // 0x0050a480 on member m with the effect id's order slot.
    static void member_effect(u8* m, i32 id, i32 a3) {
        u8* fx = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(kEffects))->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
        u8* order = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(kOrders))->get(joan::orig::arg::i16(jo::as_map_object(fx)->type_id)));  // 0x00425ce0
        joan::orig::as_cell_object(m)->attach_unit(joan::orig::arg::i16(static_cast<i16>(id)), static_cast<i32>(jo::view<i16>(jo::as_item_def(order)->bone)), static_cast<std::int8_t>(joan::orig::arg::i32(a3)), joan::orig::arg::mptr(0));  // 0x0050a480
    }
    static i32 effect_of(u8* e) {
        return e != nullptr ? joan::orig::arg::as<i32>(joan::orig::as_units(obj(kEffects))->create_49ea90(joan::orig::arg::u32(static_cast<i32>(jo::view<i16>(jo::self<jo::HeldItem>(e)->item)))))  // 0x0049ea90
                            : 0;
    }

    // Sprite fade of a sound / sprite object (+0x30 / +0x34 / +0x38).
    void fade_alpha(u8* o) {
        if (jo::self<jo::Unit>(this)->markers != -1) {
            jo::as_actor(o)->alpha = 0xff;
        } else {
            u32 tmp = 0;
            const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_plane(jo::view<void*>(jo::as_actor(o)->plane))->colour(&tmp));  // 0x005d5eb0
            jo::view<u32>(jo::as_actor(o)->alpha) = colour >> 24;
        }
    }
    void fade_effect(std::int16_t jo::Unit::* id_m) {
        if (joan::orig::as_units(obj(kEffects))->list.empty_at(static_cast<i32>(jo::self<jo::Unit>(this)->*id_m)) != 0) return;  // 0x00425d50
        u8* fx = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(kEffects))->list.ptr_at(static_cast<i32>(jo::self<jo::Unit>(this)->*id_m)));  // 0x00425d20
        if (jo::as_map_object(fx)->actor == nullptr || jo::self<jo::Unit>(this)->layer == 0) return;
        jo::as_map_object(fx)->actor->fade_38 = global<u32>(d::g_now);
        jo::view<u32>(jo::as_map_object(fx)->actor->fade) = 0xbdcccccdu;
        fade_alpha(jo::view<u8*>(jo::as_map_object(fx)->actor));
        if (jo::as_map_object(fx)->actor != nullptr) joan::orig::as_actor(jo::view<void*>(jo::as_map_object(fx)->actor))->set_byte_fd(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x005ad000
    }

    void on_removed() {
        c_on_removed.hit();
        u8* info = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(kTypes))->get(joan::orig::arg::i16(jo::self<jo::Unit>(this)->type_id)));  // 0x0042d0a0
        if (joan::orig::arg::as<i32>(joan::orig::as_scenario(obj(d::g_mission))->mode()) == 2 && jo::self<jo::Unit>(this)->formation_slot == 1) return;  // 0x004cd510

        if (jo::self<jo::Unit>(this)->rank > 3) {
            if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
                if (jo::self<jo::Unit>(this)->modifiers->grid_16 != nullptr) release_orders(&jo::UnitData::grid_16);
                if (jo::self<jo::Unit>(this)->modifiers->grid_1a != nullptr) release_orders(&jo::UnitData::grid_1a);
            }
            joan::orig::as_scenario(obj(d::g_mission))->add_copy(joan::orig::arg::mptr(this));  // 0x004cda10
        }

        // Detach from the group leader.
        if (joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->weapon)) == 0) {  // 0x00428b70
            u8* leader = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->weapon)));  // 0x0041b3c0
            u8* linfo = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(kTypes))->get(joan::orig::arg::i16(jo::as_unit(leader)->type_id)));  // 0x0042d0a0
            if (jo::self<jo::Unit>(this)->weapon_slot >= 0 && jo::self<jo::Unit>(this)->weapon_slot < jo::self<jo::UnitType>(linfo)->slot_count &&
                jo::view<i32*>(jo::as_unit(leader)->mask_slots)[jo::self<jo::Unit>(this)->weapon_slot] == static_cast<i32>(jo::self<jo::Unit>(this)->id)) {
                jo::view<i32*>(jo::as_unit(leader)->mask_slots)[jo::self<jo::Unit>(this)->weapon_slot] = 0;
                jo::view<i32*>(jo::as_unit(leader)->mask_slots_b)[jo::self<jo::Unit>(this)->weapon_slot] = 0;
            }
        }

        // Free the group members.
        for (i32 i = jo::self<jo::UnitType>(info)->slot_count - 1; i >= 0; --i) {
            if (joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(jo::view<i32*>(jo::self<jo::Unit>(this)->mask_slots)[i])) != 0) continue;  // 0x00428b70
            u8* m = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::view<i32*>(jo::self<jo::Unit>(this)->mask_slots)[i])));  // 0x0041b3c0
            jo::as_unit(m)->weapon = 0;
            jo::as_unit(m)->weapon_slot = 0;
            jo::as_unit(m)->slot_index = -1;
            joan::orig::as_unit(m)->release_slot();  // 0x00528300
            if (jo::as_unit(m)->orders != nullptr) {
                if (jo::as_unit(m)->orders->current.kind != 0) joan::orig::as_action(jo::as_unit(m)->orders)->dispatch(joan::orig::arg::mptr(m));  // 0x004bb660
                void* q = jo::as_unit(m)->orders;
                if (q != nullptr) joan::orig::as_ai(q)->destroy(1u);  // 0x00415220 scalar dtor
                jo::as_unit(m)->orders = nullptr;
                joan::orig::as_squad(m)->clear();  // 0x00422f10
            }
            if (jo::self<jo::UnitType>(info)->kind == 0xb && (jo::view<u8*>(jo::as_unit_type(info)->buffers[2])[i] & 2) != 0 &&
                joan::orig::arg::as<i8>(joan::orig::as_unit(m)->dead()) == 0) {  // 0x0042d0e0
                jo::as_unit(m)->state_120 = 0;
                joan::orig::as_unit(m)->set_value_135(0);  // 0x004fe510
                joan::orig::as_unit(m)->on_removed();     // 0x0064f320 (recursion)
            } else if (jo::as_unit(m)->modifiers->hand_mode != 0) {
                if (jo::as_unit(m)->modifiers->hand_mode <= 1) {
                    i32 id = effect_of(joan::orig::arg::as<u8*>(joan::orig::as_modifiers(jo::as_unit(m)->modifiers)->slot(0)));  // 0x004974f0
                    if (id != 0) member_effect(m, id, 0);
                    id = effect_of(joan::orig::arg::as<u8*>(joan::orig::as_modifiers(jo::as_unit(m)->modifiers)->slot(1)));  // 0x004974f0
                    if (id != 0) {
                        void* fx = joan::orig::as_units(obj(kEffects))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
                        virtual_call<void>(fx, 0x1c);
                        member_effect(m, id, 1);
                    }
                } else {
                    const i32 id = effect_of(joan::orig::arg::as<u8*>(joan::orig::as_unit_data(jo::as_unit(m)->modifiers)->slot_a()));  // 0x00497560
                    if (id != 0) member_effect(m, id, 1);
                }
            }
            jo::view<i32*>(jo::self<jo::Unit>(this)->mask_slots_b)[i] = 0;
        }

        // Release the carried unit.
        if (joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->rider_ref)) == 0) {  // 0x00428b70
            u8* t = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(jo::self<jo::Unit>(this)->rider_ref)));  // 0x0041b3c0
            if (jo::as_unit(t)->mount_ref == static_cast<i32>(jo::self<jo::Unit>(this)->id)) {
                jo::as_unit(t)->mount_ref = 0;
                jo::as_unit(t)->state_120 = 0;
                joan::orig::as_unit(t)->set_value_135(0);  // 0x004fe510
                joan::orig::as_unit(t)->on_removed();     // 0x0064f320 (recursion)
                jo::self<jo::Unit>(this)->rider_ref = 0;
            }
        }

        if (void* side = joan::orig::arg::as<void*>(joan::orig::as_unit(this)->nation()); side != nullptr) {  // 0x00509400
            const u32 a = static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hit_points_max()));                         // 0x00523320
            const u32 b = static_cast<u32>(joan::orig::arg::as<i32>(joan::orig::unit_class(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id)))));  // 0x00527dd0
            joan::orig::as_nation(side)->add_1389(static_cast<i32>((0u - a) * b));  // 0x004b5520
        }

        // Morale loss of the units of the same side.
        const i32 value = joan::orig::arg::as<i32>(joan::orig::unit_class(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id))));  // 0x00527dd0
        for (i32 i = joan::orig::as_dynarray(obj(kUnits))->size(); i > 0; --i) {  // 0x00417ad0
            u8* u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (u == nullptr) continue;
            if (jo::self<jo::Unit>(this)->side == 0 || jo::as_unit(u)->side != jo::self<jo::Unit>(this)->side) continue;
            const float k = (jo::self<jo::Unit>(this)->group_id != 0 && jo::as_unit(u)->group_id == jo::self<jo::Unit>(this)->group_id) ? global<float>(d::g_a3cd08)
                                                                                   : global<float>(d::g_a3cc80);
            const Fpu fpu{true};
            const double r = fpu.sub(static_cast<double>(jo::as_unit(u)->stats[7]), fpu.mul(static_cast<double>(value), k));
            joan::orig::as_unit(u)->roll_morale(joan::orig::arg::i32(ftol(r)));  // 0x004fe8c0
        }

        u32 angle = 0;  // 1-byte angle object (ebp-0x5c)
        joan::orig::as_angle(&angle)->construct();        // 0x00576120
        joan::orig::as_angle(&angle)->set_byte(0.0f);  // 0x005762c0
        const i32 dist = 3000;
        const i32 height = 500;
        joan::orig::as_unit(this)->spawn_projectile();  // 0x0050afe0
        if (static_cast<i32>(jo::self<jo::Unit>(this)->id) == global<i32>(d::g_player_unit_recno)) {
            const i32 type = jo::self<jo::Unit>(this)->type_id;
            if (type != joan::orig::arg::as<i32>(joan::orig::as_unit_types(obj(kTypes))->find(joan::orig::arg::str(&img::global<const char>(d::s_JOAN_OF_ARC)))))  // 0x005270b0
                joan::orig::as_player(obj(d::g_player_unit_recno))->select_hero(0);  // 0x00432300
        }
        joan::orig::as_unit(this)->release_slot();  // 0x00528300
        if (jo::self<jo::Unit>(this)->markers == 1) {
            jo::self<jo::Unit>(this)->hit_value = 0x7fffffff;
            joan::orig::as_unit(this)->set_value_135(1);  // 0x004fe510
            joan::orig::as_group(this)->release();     // 0x0042dd70
            u8 cmd[0x1c] = {};
            joan::orig::as_unit_task(cmd)->set(7u, static_cast<std::uint8_t>(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->task.priority) + 1)), 0u, 0u, 0u, 0u);  // 0x004e4520
            joan::orig::as_unit(this)->set_task(joan::orig::arg::mptr(cmd));                                          // 0x00527fb0
            jo::self<jo::Unit>(this)->markers = -1;
            return;
        }
        if (jo::self<jo::UnitType>(info)->unit_class == 8) {
            const i32 kind = jo::self<jo::UnitType>(info)->kind;
            if (kind == 4 || kind == 5 || kind == 6 || kind == 9 || kind == 10 || kind == 11 || kind == 12)
                joan::orig::as_unit(this)->set_anim_set(1);  // 0x00511060
        }

        Vec3 pos;
        const float fdist = static_cast<float>(dist);
        pos.x = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->cos()) * fdist;  // 0x00576ac0
        const float fdist2 = static_cast<float>(dist);
        pos.y = joan::orig::arg::as<float>(joan::orig::as_angle(&angle)->sin()) * fdist2;  // 0x005768d0
        pos.z = static_cast<float>(height);
        if (jo::self<jo::Unit>(this)->anim_kind == 0x4e) joan::orig::vec_scale_in_place(&pos.x, 3.0f);  // 0x0060ce80
        if (jo::self<jo::Unit>(this)->actor != nullptr && jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->record != 0 && jo::self<jo::Unit>(this)->anim_kind != 0x3b) {
            void* snd = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<i32>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->record)));  // 0x006af310
            joan::orig::as_body(snd)->init_target(&pos, joan::orig::arg::u32(global<u32>(d::g_now)), 0x64u);                         // 0x006b11a0
        }
        if (jo::self<jo::Unit>(this)->anim_kind != 0x3b) joan::orig::as_unit(this)->set_action(0x2a, 0, 1);  // 0x00501190

        if (jo::self<jo::Unit>(this)->markers != -2) {
            const bool skip = joan::orig::arg::as<i32>(joan::orig::as_scenario(obj(d::g_mission))->mode()) == 2 && jo::self<jo::Unit>(this)->formation_slot == 1;  // 0x004cd510
            if (!skip && jo::self<jo::Unit>(this)->actor != nullptr) {
                jo::view<i32>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->aim_mode) = 0;
                if (static_cast<i32>(jo::self<jo::Unit>(this)->id) != global<i32>(d::g_player_unit_recno)) {
                    jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->fade) = 0xbdcccccdu;
                    jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->fade_38 = global<u32>(d::g_now);
                    fade_alpha(jo::view<u8*>(jo::self<jo::Unit>(this)->actor));
                    if (jo::self<jo::Unit>(this)->model != nullptr) {
                        jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->model))->fade) = 0xbdcccccdu;
                        jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->model))->fade_38 = global<u32>(d::g_now);
                        fade_alpha(jo::view<u8*>(jo::self<jo::Unit>(this)->model));
                    }
                }
            }
            fade_effect(&jo::Unit::carried_a);
            fade_effect(&jo::Unit::carried_b);
        }

        if (jo::self<jo::Unit>(this)->draw != nullptr) {
            void* q = jo::self<jo::Unit>(this)->draw;
            if (q != nullptr) joan::orig::as_unit_draw(q)->destroy(1u);  // 0x00416640 scalar dtor
            jo::self<jo::Unit>(this)->draw = nullptr;
        }
        if (jo::self<jo::Unit>(this)->model != nullptr) {
            if (jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->model))->scene != nullptr)
                joan::orig::as_scene(jo::view<void*>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->model))->scene))->detach(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->model));  // 0x005efdb0
            void* q = jo::self<jo::Unit>(this)->model;
            if (q != nullptr) joan::orig::as_scene_object(q)->destroy(1u);  // 0x00420840 scalar dtor
            jo::self<jo::Unit>(this)->model = nullptr;
        }
        if (jo::self<jo::Unit>(this)->actor != nullptr && jo::self<jo::Unit>(this)->layer != 0) joan::orig::as_actor(jo::self<jo::Unit>(this)->actor)->set_byte_fd(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x005ad000
        if (jo::self<jo::UnitType>(info)->unit_class == 8 && jo::self<jo::UnitType>(info)->kind == 8 && jo::self<jo::Unit>(this)->siege_tower_state == 2)
            joan::orig::as_unit(this)->emit_6afb10(static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x00510220
        if (jo::self<jo::UnitType>(info)->unit_class == 0 && joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hostile_to(joan::orig::arg::i16(global<i16>(d::g_player_unit_recno)))) == 0) {  // 0x005094d0
            global<u32>(d::g_get_a5918d) += 1;
            if (jo::self<jo::UnitType>(info)->gender == 0x46) global<u32>(d::g_get_a59199) += 1;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b110_09", name, addr, fn)
B("Unit::on_removed", 0x0064f320, &H::on_removed);
#undef B

}  // namespace game_b110_09
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

// 0x00650db0
std::int32_t JOAN_CDECL set_c6(std::int32_t id, std::uint32_t value) {
    if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)) == 0) {  // 0x00425d50
        void* const p = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
        void* const r = virtual_call<void*>(p, 0x58);
        jo::as_wall(r)->u32_c6 = value;  // slot 0x58: the Wall
    }
    return 0;
}

struct H {
    // 0x006509a0
    void* plus20() { return &jo::view<std::uint8_t>(jo::self<jo::Script>(this)->name_[0]); }

    // 0x006509c0
    jo::DlgRes* get64() { return jo::self<jo::Script>(this)->strings; }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("Obj::plus20", 0x006509a0, &H::plus20);
B94("Obj::get64", 0x006509c0, &H::get64);
B94("Script::set_c6", 0x00650db0, &set_c6);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b101_13.cpp (game/b101_13)
namespace {
namespace game_b101_13 {

JOAN_COUNTER(c_cond_cmd, "ScriptNode::cond_cmd");

struct H {
    // ------------------------------------------------------------------ 0x006509e0
    u32 cond_cmd(u32) {
        c_cond_cmd.hit();
        const i32 code = static_cast<i32>(jo::self<jo::ScriptFuncCall>(this)->func_id);
        u32 r;
        if (code == 0x12d) r = joan::orig::arg::as<u32>(joan::orig::script_reset_first_unit());       // 0x00653a90
        else if (code == 0x130) r = joan::orig::arg::as<u32>(joan::orig::script_not_a4fc98_flag());  // 0x00653c20
        else if (code == 0x149) r = joan::orig::arg::as<u32>(joan::orig::script_a48a38_byte());  // 0x0065c470
        else if (code == 0x14a) r = joan::orig::arg::as<u32>(joan::orig::script_a6a254_below());  // 0x0065c4d0
        else if (code == 0x181) r = joan::orig::arg::as<u32>(joan::orig::script_not_a53cb0());  // 0x0065eea0
        else if (code == 0x128 || code == 0x129) r = joan::orig::arg::as<u32>(joan::orig::script_state_busy());  // 0x0065af50
        else if (code == 0x12e) r = joan::orig::arg::as<u32>(joan::orig::script_state_busy());  // 0x0065af50
        else if (code == 0x186) r = joan::orig::arg::as<u32>(joan::orig::script_flag(0));  // 0x0065f600
        else if (code == 0x12f) r = joan::orig::arg::as<u32>(joan::orig::game_state_is_7());  // 0x0065b0e0
        else if (code == 0x18e) r = joan::orig::arg::as<u32>(joan::orig::script_not_a46040());  // 0x0065f950
        else return (static_cast<u32>(code) & 0xffffff00u) | 1u;  // mov al, 1
        return r != 0 ? 1u : 0u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_13", name, addr, fn)
B("ScriptNode::cond_cmd", 0x006509e0, &H::cond_cmd);
#undef B

}  // namespace game_b101_13
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
namespace {
namespace game_b97_06 {

struct Vec3 {  // float members: the callee takes floats, and on System V x86-64 three floats go in XMM registers while three dwords go in integer registers (List43::add 0x004eef00)
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_unit_target, "unit_target");
JOAN_COUNTER(c_unit_say, "unit_say");

// 0x00650b90
std::uint32_t JOAN_CDECL unit_target(std::uint32_t id) {
    c_unit_target.hit();
    if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)) == 0) {  // 0x00425d50
        void* u = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
        if (jo::as_map_object(u)->class_id == 1) {
            void* u2 = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
            void* d = virtual_call<void*>(u2, 0x4c);
            return jo::view<std::uint32_t>(jo::as_gate(d)->hp);  // slot 0x4c returns the Gate
        }
    }
    return 0;
}

// 0x00650e70
std::int32_t JOAN_CDECL unit_say(std::uint32_t id, std::uint32_t sound) {
    c_unit_say.hit();
    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(sound)) != 0) return 0;  // 0x00428b70
    if (joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(id)) == 0) {  // 0x00425d50
        void* u = joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
        const std::uint32_t g = global<std::uint32_t>(d::g_now);
        Vec3 pos;
        std::memcpy(&pos, jo::as_map_object(u)->pos, sizeof pos);
        joan::orig::as_triggers(obj(d::g_a5ccd0))->add(0x40u, 2u, joan::orig::arg::i16(static_cast<std::uint32_t>(id & 0xffffu)), 1u, joan::orig::arg::i16(static_cast<std::uint32_t>(sound & 0xffffu)), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), pos, 0x447a0000u, static_cast<std::int8_t>(joan::orig::arg::i32(0u)), joan::orig::arg::u32(g), joan::orig::arg::u32(g), static_cast<std::int8_t>(joan::orig::arg::i32(1u)), 0u, joan::orig::arg::i16(0u));  // 0x004eef00
    }
    return 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("unit_target", 0x00650b90, &unit_target);
B("unit_say", 0x00650e70, &unit_say);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

// Virtual call through vtable slot `offset` (thiscall, no args).
template <class R>
R vcall0(void* o, u32 offset) {
    using V = R (Obj::*)();
    JOAN_MEMBER_POINTER_CHECK(V);
    V method{};
    std::memcpy(&method, &joan::call::vtable_of(o)[joan::call::x86_vslot(offset)], sizeof(void*));  // the address word (JOAN_MEMBER_POINTER_CHECK)  // slot offset / 4 (x86 byte offset)
    return (static_cast<Obj*>(o)->*method)();
}

void* const kUnits = obj(d::g_items);

u32 JOAN_CDECL unit_value_c6(u32 id) {  // 0x00650c10
    if (joan::orig::as_units(kUnits)->list.empty_at(joan::orig::arg::i32(id)) != 0) return 0;  // 0x00425d50
    if (jo::as_map_object(joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id))))->class_id != 1) return 0;  // 0x00425d20
    u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_units(kUnits)->list.ptr_at(joan::orig::arg::i32(id)));  // 0x00425d20
    u8* const r = vcall0<u8*>(unit, 0x4c);
    return jo::view<u32>(jo::as_gate(r)->max_hp);  // slot 0x4c returns the Gate
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("unit_value_c6", 0x00650c10, &unit_value_c6);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

void* const kChars = obj(d::g_items);  // script character table

// ---------------------------------------------------------------------------- 0x00650c90
i32 JOAN_CDECL script_set_c2(i32 id, u32 v) {
    if (joan::orig::as_units(kChars)->list.empty_at(joan::orig::arg::i32(id)) == 0) {  // 0x00425d50
        void* const o = joan::orig::as_units(kChars)->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
        u8* const r = virtual_call<u8*>(o, 0x4c);
        jo::view<u32>(jo::as_gate(r)->hp) = v;  // slot 0x4c returns the Gate
    }
    return 0;
}

// ---------------------------------------------------------------------------- 0x00650f40
i32 JOAN_CDECL script_set_e6(i32 id, u32 c) {
    if (joan::orig::as_units(kChars)->list.empty_at(joan::orig::arg::i32(id)) != 0) return -1;  // 0x00425d50
    void* const o = joan::orig::as_units(kChars)->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
    u8* const r = virtual_call<u8*>(o, 0x58);
    jo::as_wall(r)->invincible = static_cast<u8>(c);  // slot 0x58: the Wall
    return static_cast<i32>(static_cast<i8>(c));
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("script_set_c2", 0x00650c90, &script_set_c2);
B96("script_set_e6", 0x00650f40, &script_set_e6);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b99_08.cpp (game/b99_08)
namespace {
namespace game_b99_08 {

// Virtual thiscall: vtable slot at byte offset `off`.
template <class R = void, class... A>
R vcall(void* o, std::int32_t off, A... args) {
    return thiscall<R>(reinterpret_cast<std::uintptr_t>(static_cast<void* const*>(jo::as_map_object(o)->vtable)[joan::call::x86_vslot(off)]), o, args...);  // slot off / 4 (x86 byte offset)
}

// ---------------------------------------------------------------------------- 0x00650cf0
static i32 JOAN_CDECL script_object_set_c6(i32 id, i32 v) {
    if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(id)) == 0) {  // 0x00425d50
        void* const o = joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(id));  // 0x00425d20
        jo::as_gate(vcall<void*>(o, 0x4c))->max_hp = v;  // slot 0x4c returns the Gate
    }
    return 0;
}

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_08", name, addr, fn)
B99("script_object_set_c6", 0x00650cf0, &script_object_set_c6);
#undef B99

}  // namespace game_b99_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Script::set_name(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_06::H::set_name_649860>(this, a0)); }  // 0x649860
auto joan::orig::ScriptStatementCompile::set_error_text(const char* a0) -> void { joan::bind::member<&::game_b91_04::H::set_name_a6a118>(this, a0); }  // 0x6498a0
auto joan::orig::ScriptLexer::rewind() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_12::H::init_6498e0>(this)); }  // 0x6498e0
auto joan::orig::Unit::on_removed() -> void { joan::bind::member<&::game_b110_09::H::on_removed>(this); }  // 0x64f320
auto joan::orig::Script::name() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_06::H::plus20>(this)); }  // 0x6509a0
auto joan::orig::Script::dlg_res() -> DlgRes* { return joan::bind::conv<DlgRes*>(joan::bind::member<&::game_b94_06::H::get64>(this)); }  // 0x6509c0
auto joan::orig::unit_target(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b97_06::unit_target>(a0)); }  // 0x650b90
auto joan::orig::unit_value_c6(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b95_06::unit_value_c6>(a0)); }  // 0x650c10
auto joan::orig::script_set_c2(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_06::script_set_c2>(a0, a1)); }  // 0x650c90
auto joan::orig::script_object_set_c6(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b99_08::script_object_set_c6>(a0, a1)); }  // 0x650cf0
auto joan::orig::script_obj_c6(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b100_12::script_obj_c6>(a0)); }  // 0x650d50
auto joan::orig::script::set_c6(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_06::set_c6>(a0, a1)); }  // 0x650db0
auto joan::orig::object_field_ca(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b98_06::object_field_ca>(a0)); }  // 0x650e10
auto joan::orig::unit_say(std::uint32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b97_06::unit_say>(a0, a1)); }  // 0x650e70
auto joan::orig::script_set_e6(std::int32_t a0, std::uint32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_06::script_set_e6>(a0, a1)); }  // 0x650f40
