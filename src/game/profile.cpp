// game/profile.cpp: functions of the original d:\ww\profile.cpp (re/attribution.csv).
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
#include <cstdarg>
#include "../original/records.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../save/s5.hpp"
#include "../save/gen/records_s1.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

void* const kMemory = obj(d::g_mem);

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
//
//      0x004c7e70 Profile::clear()                    ret 0   (profile.cpp)
namespace {
namespace game_b92_03 {

using joan::orig::helpers::line_base;
void mem_free(void* p, const char* file, std::int32_t line) {
    joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(p), joan::orig::arg::str(file), joan::orig::arg::i32(line));  // 0x0058ed70
}

JOAN_COUNTER(c_profile_clear, "Profile::clear");

struct H {
    // 0x004c7e70
    void profile_clear() {
        c_profile_clear.hit();
        if (jo::view<std::uint8_t*>(jo::self<jo::Profile>(this)->units) != nullptr) {
            for (std::int32_t i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
                if (jo::view<void*>((jo::as_profile_unit(&jo::self<jo::Profile>(this)->units[i]))->leader) != nullptr) {
                    void* const p = jo::view<void*>((jo::as_profile_unit(&jo::self<jo::Profile>(this)->units[i]))->leader);
                    if (p != nullptr) joan::orig::as_unit(p)->destroy(1u);  // 0x00471530
                    jo::view<void*>((jo::as_profile_unit(&jo::self<jo::Profile>(this)->units[i]))->leader) = nullptr;
                }
                for (std::int32_t j = 0; j < 0x14; ++j) {
                    if (jo::view<void*>((jo::as_profile_unit(&jo::self<jo::Profile>(this)->units[i]))->members[j]) != nullptr) {
                        void* const p = jo::view<void*>((jo::as_profile_unit(&jo::self<jo::Profile>(this)->units[i]))->members[j]);
                        if (p != nullptr) joan::orig::as_unit(p)->destroy(1u);  // 0x00471530
                        jo::view<void*>((jo::as_profile_unit(&jo::self<jo::Profile>(this)->units[i]))->members[j]) = nullptr;
                    }
                }
            }
            mem_free(jo::view<void*>(jo::self<jo::Profile>(this)->units), str(d::s_d_ww_profile_cpp), line_base(d::g_a02438) + 0x16);
            jo::view<void*>(jo::self<jo::Profile>(this)->units) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Profile>(this)->list_a) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::Profile>(this)->list_a), str(d::s_d_ww_profile_cpp), line_base(d::g_a02438) + 0x1c);
            jo::view<void*>(jo::self<jo::Profile>(this)->list_a) = nullptr;
        }
        jo::self<jo::Profile>(this)->list_a_count = 0;
        if (jo::view<void*>(jo::self<jo::Profile>(this)->list_b) != nullptr) {
            mem_free(jo::view<void*>(jo::self<jo::Profile>(this)->list_b), str(d::s_d_ww_profile_cpp), line_base(d::g_a02438) + 0x23);
            jo::view<void*>(jo::self<jo::Profile>(this)->list_b) = nullptr;
        }
        jo::self<jo::Profile>(this)->list_b_count = 0;
        jo::self<jo::Profile>(this)->i32_40 = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Profile::clear", 0x004c7e70, &H::profile_clear);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
//
//      0x004c80c0 Profile::add             (thiscall(src), ret 4) - ++count, realloc count*0x77,
//                 memset + strncpy(src, 0x21), returns the new entry.
namespace {
namespace game_b95_03 {

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_profile_add, "Profile::add");

using joan::orig::helpers::memset_;  // memset
char* strncpy_(char* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }  // strncpy
using joan::orig::helpers::line_of;

struct H {
    // ------------------------------------------------------------------------ 0x004c80c0
    u8* profile_add(const char* src) {
        c_profile_add.hit();
        jo::view<u32>(jo::self<jo::Profile>(this)->unit_count) += 1;
        const i32 line = line_of(d::g_a0243c, 2);
        jo::view<void*>(jo::self<jo::Profile>(this)->units) = joan::orig::as_mem(kMem)->resize(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Profile>(this)->units)), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Profile>(this)->unit_count) * static_cast<u32>(sizeof(jo::ProfileUnit))), str(d::s_d_ww_profile_cpp), joan::orig::arg::i32(line));  // 0x0058eb30 "d:\ww\profile.cpp"
        memset_(&jo::self<jo::Profile>(this)->units[jo::self<jo::Profile>(this)->unit_count - 1], 0, sizeof(jo::ProfileUnit));
        strncpy_(reinterpret_cast<char*>(&jo::self<jo::Profile>(this)->units[jo::self<jo::Profile>(this)->unit_count - 1]), src, 0x21);
        return reinterpret_cast<u8*>(&jo::self<jo::Profile>(this)->units[jo::self<jo::Profile>(this)->unit_count - 1]);
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("Profile::add", 0x004c80c0, &H::profile_add);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
//
//    - Table::find_name 0x004c81a0 (name), ret 4: 0x77-byte entries +0x24 / count +0x20,
//      first one with stricmp == 0, else 0.
namespace {
namespace game_b89_06 {

using joan::call::cdecl_call;

std::int32_t stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_find_name, "Table::find_name");

struct H {
    // 0x004c81a0
    void* find_name(const char* name) {
        c_find_name.hit();
        for (std::int32_t i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
            void* const e = static_cast<void*>(&jo::self<jo::Profile>(this)->units[i]);
            if (stricmp_(e, name) == 0) return static_cast<void*>(&jo::self<jo::Profile>(this)->units[i]);
        }
        return nullptr;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("Table::find_name", 0x004c81a0, &H::find_name);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b104_04.cpp (game/b104_04)
namespace {
namespace game_b104_04 {

using u16 = std::uint16_t;

JOAN_COUNTER(c_4c8810, "Profile::save_4c8810");

struct H {
    // ---------------------------------------------------------------- 0x004c8810
    i32 save_4c8810() {
        c_4c8810.hit();
        alignas(4) jo::File file_obj;  // the 0x110-byte frame slot of the original; a real File (x64: 0x112 bytes)
        void* const file = &file_obj;
        alignas(4) u8 path[0x194];
        joan::orig::as_file(file)->construct();                               // 0x004179c0 File::File
        joan::orig::as_string(path)->construct();                        // 0x005a1170
        joan::orig::as_string(path)->assign(str(d::s_SAVE));                        // 0x005a1440 'SAVE\'
        joan::orig::as_string(path)->append(joan::orig::arg::str(static_cast<const void*>(this)));  // 0x005a1550
        joan::orig::as_string(path)->append(str(d::s_dat_9d56f0));                            // 0x005a1550 '.dat'
        const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_string(path)->c_str());  // 0x00417d90
        joan::orig::as_file(file)->file_create(joan::orig::arg::str(name), 1, static_cast<char>(0));  // 0x00586f00
        auto fail = [&]() {
            joan::orig::as_file(file)->file_close();   // 0x00587260 close
            joan::orig::as_file(file)->destruct();  // 0x005872b0 ~File
            return 0;
        };
        // Profile 0x48 + name 0x20 field by field (save::s5::write_profile_head; [Debug] SaveFieldwise=0: raw writes)
        if (joan::save::s5::write_profile_head(file_obj, *jo::self<jo::Profile>(this)) == 0) return fail();  // 0x005872e0 x2
        joan::orig::as_file(file)->file_put_long(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Profile>(this)->unit_count)));  // 0x00587da0
        for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i)
            if (joan::orig::arg::as<i32>(joan::orig::as_key(&jo::self<jo::Profile>(this)->units[i])->read(joan::orig::arg::mptr(static_cast<void*>(file)))) == 0) return fail();  // 0x004c93e0
        for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i)
            if (joan::orig::arg::as<i32>(joan::orig::as_profile_unit(&jo::self<jo::Profile>(this)->units[i])->write(joan::orig::arg::mptr(static_cast<void*>(file)))) == 0) return fail();  // 0x004c9460
        // the low words of the counts (+0x28 / +0x30 / +0x40 on x86), by member: the x64 offsets differ
        joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(jo::self<jo::Profile>(this)->list_a_count))));  // 0x00587ba0 (writes the word)
        joan::orig::as_file(file)->file_write_array(joan::orig::arg::ptr(jo::self<jo::Profile>(this)->list_a), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Profile>(this)->list_a_count)), 2u);  // 0x00587510
        joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(jo::self<jo::Profile>(this)->list_b_count))));
        joan::orig::as_file(file)->file_write_array(joan::orig::arg::ptr(jo::self<jo::Profile>(this)->list_b), joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Profile>(this)->list_b_count)), 2u);
        joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(static_cast<u32>(static_cast<u16>(jo::self<jo::Profile>(this)->i32_40))));
        joan::orig::as_file(file)->file_close();
        joan::orig::as_file(file)->destruct();
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_04", name, addr, fn)
B("Profile::save_4c8810", 0x004c8810, &H::save_4c8810);
#undef B

}  // namespace game_b104_04
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
//
//    - Profile::load 0x004c8bb0, ret 0 -> 0 / 1: "SAVE\<name>.dat", 0x48 + 0x20 byte header,
//      records (0x77 bytes, 0x004c9420 / 0x004c9530), two short arrays, +0x40 (SEH frame: normal
//      path only; the file object is destroyed on every return like the unwind state does).
namespace {
namespace game_b103_04 {

using joan::orig::helpers::mul;

JOAN_COUNTER(c_profile_load, "Profile::load");

struct H {
    // ------------------------------------------------------------------------ 0x004c8bb0
    void* alloc(i32 size, i32 line_add) {  // 0x0058e7b0 (size, file, line)
        const i32 line = static_cast<i32>(global<i16>(d::g_a02444)) + line_add;
        return joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(size), str(d::s_d_ww_profile_cpp), joan::orig::arg::i32(line));
    }
    i32 profile_load() {
        c_profile_load.hit();
        alignas(4) jo::File file_obj;  // [ebp-0x120] (0x110 bytes); a real File (x64: 0x112 bytes)
        void* const file = &file_obj;
        alignas(4) u8 path[0x194];  // [ebp-0x2b8], 0x191 used
        joan::orig::as_profile(this)->clear();  // 0x004c7e70
        joan::orig::as_file(file)->construct();  // 0x004179c0 ctor
        void* const fp = file;
        auto finish = [&](i32 r) {
            joan::orig::as_file(file)->destruct();  // 0x005872b0 dtor
            return r;
        };
        joan::orig::as_string(path)->construct();                // 0x005a1170
        joan::orig::as_string(path)->assign(str(d::s_SAVE));                // 0x005a1440 'SAVE\'
        joan::orig::as_string(path)->append(joan::orig::arg::str(this));         // 0x005a1550 append
        joan::orig::as_string(path)->append(str(d::s_dat_9d56f0));                    // '.dat'
        if (joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str()))) == 0)  // 0x00590b80, 0x00417d90
            return finish(0);
        joan::orig::as_file(file)->file_open(joan::orig::arg::str(joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str())), 1, static_cast<char>(0));  // 0x00586da0 open
        // Profile 0x48 + name 0x20 field by field (save::s5::read_profile_head; [Debug] SaveFieldwise=0: raw reads)
        if (joan::save::s5::read_profile_head(file_obj, *jo::self<jo::Profile>(this)) == 0) {  // 0x005873a0 x2
            joan::orig::as_file(fp)->file_close();   // 0x00587260 close
            return finish(0);
        }
        jo::self<jo::Profile>(this)->unit_count = joan::orig::as_file(fp)->file_get_long();   // 0x00587e20
        if (jo::self<jo::Profile>(this)->unit_count != 0) {
            jo::view<void*>(jo::self<jo::Profile>(this)->units) = alloc(mul(jo::self<jo::Profile>(this)->unit_count, static_cast<i32>(sizeof(jo::ProfileUnit))), 0x24);
            std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Profile>(this)->units)), 0, joan::orig::arg::u32(mul(jo::self<jo::Profile>(this)->unit_count, static_cast<i32>(sizeof(jo::ProfileUnit)))));  // memset
            for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
                void* const rec = &jo::self<jo::Profile>(this)->units[i];
                if (joan::orig::arg::as<i32>(joan::orig::as_key(rec)->write(joan::orig::arg::mptr(fp))) == 0) {  // 0x004c9420
                    joan::orig::as_file(fp)->file_close();
                    return finish(0);
                }
            }
            for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
                void* const rec = &jo::self<jo::Profile>(this)->units[i];
                if (joan::orig::arg::as<i32>(joan::orig::as_profile_unit(rec)->read(joan::orig::arg::mptr(fp))) == 0) {  // 0x004c9530
                    joan::orig::as_file(fp)->file_close();
                    return finish(0);
                }
            }
        } else {
            jo::view<void*>(jo::self<jo::Profile>(this)->units) = nullptr;
        }
        jo::self<jo::Profile>(this)->list_a_count = joan::orig::arg::as<i16>(joan::orig::as_file(fp)->file_get_short());  // 0x00587c20 read short
        if (jo::self<jo::Profile>(this)->list_a_count != 0) {
            jo::view<void*>(jo::self<jo::Profile>(this)->list_a) = alloc(jo::self<jo::Profile>(this)->list_a_count << 1, 0x43);
            joan::orig::as_file(fp)->file_read_array(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Profile>(this)->list_a)), joan::orig::arg::u32(jo::self<jo::Profile>(this)->list_a_count), 2u);  // 0x00587650
        }
        jo::self<jo::Profile>(this)->list_b_count = joan::orig::arg::as<i16>(joan::orig::as_file(fp)->file_get_short());
        if (jo::self<jo::Profile>(this)->list_b_count != 0) {
            jo::view<void*>(jo::self<jo::Profile>(this)->list_b) = alloc(jo::self<jo::Profile>(this)->list_b_count << 1, 0x4a);
            joan::orig::as_file(fp)->file_read_array(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Profile>(this)->list_b)), joan::orig::arg::u32(jo::self<jo::Profile>(this)->list_b_count), 2u);
        }
        jo::self<jo::Profile>(this)->i32_40 = joan::orig::arg::as<i16>(joan::orig::as_file(fp)->file_get_short());
        joan::orig::as_file(fp)->file_close();
        return finish(1);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("Profile::load", 0x004c8bb0, &H::profile_load);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
//
//      0x004c7df0 X::reset              (thiscall, ret 0) - 0x004c7e70.
namespace {
namespace game_b99_03 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    u32 reset_c7() {  // 0x004c7df0 -> 0x004c7e70
        joan::orig::as_profile(this)->clear();  // 0x004c7e70
        return 0;  // Profile::clear returns nothing; callers (0x004ca00f, profile.cpp) discard the result
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("X::reset_c7", 0x004c7df0, &H::reset_c7);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
//
//      0x004c90a0 X::attach                (thiscall(obj), ret 4) - entry = 0x0042d0a0(word obj+0x3b)
//                 + 4, find (0x004c81a0) or add (0x004c80c0) the record, lazily new(0x4b4) +
//                 ctor 0x004155e0 into record+0x21, returns 0x004c96f0(record; obj, +0x21) != 0.
//
//  - 0x004c90a0: the original pushes ecx with cx = word obj+0x3b (upper half = this's upper
//    half); 0x0042d0a0 only reads the word (movsx), so the upper half is irrelevant. The SEH
//    frame only guards new+ctor (delete on throw); the normal path is reproduced.
namespace {
namespace game_b96_03 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_attach, "X::attach");

using joan::orig::helpers::game_new;                         // operator new

struct H {
    // ------------------------------------------------------------------------ 0x004c90a0
    i32 attach(void* o) {
        c_attach.hit();
        const i16 id = jo::as_unit(o)->type_id;
        u8* const entry = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(id))));  // 0x0042d0a0
        void* rec = joan::orig::arg::as<void*>(joan::orig::as_table(this)->find_name_4c81a0(joan::orig::arg::str(entry + 4)));                             // 0x004c81a0
        if (rec == nullptr) rec = joan::orig::arg::as<void*>(joan::orig::as_profile(this)->add(joan::orig::arg::str(entry + 4)));               // 0x004c80c0
        if (jo::as_profile_unit(rec)->leader == nullptr) {
            void* const mem = game_new(sizeof(jo::Squad));  // 0x4b4 on x86
            void* const made = mem != nullptr ? joan::orig::as_squad(mem)->construct() : nullptr;  // 0x004155e0
            jo::view<void*>(jo::as_profile_unit(rec)->leader) = made;
        }
        return joan::orig::arg::as<i32>(joan::orig::as_unit(rec)->copy_to(joan::orig::arg::mptr(o), joan::orig::arg::mptr(jo::as_profile_unit(rec)->leader))) != 0 ? 1 : 0;  // 0x004c96f0
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("X::attach", 0x004c90a0, &H::attach);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03_2 {

struct H {
    // the name key, field-wise (save::s5::read_unit_key; [Debug] SaveFieldwise=0: the raw read)
    std::int32_t key_write(void* file) { return joan::save::s5::read_unit_key(*jo::as_file(file), *jo::self<jo::ProfileUnit>(this)) != 0 ? 1 : 0; }  // 0x005873a0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Key::write", 0x004c9420, &H::key_write);
#undef B

}  // namespace game_b92_03_2
}  // namespace

// ---------------------------------------- from modules/game_b102_04.cpp (game/b102_04)
//
//    - Obj::load_4c9530 0x004c9530 (stream), ret 4 -> 0 / 1: reads 0x20 bytes (0x5873a0),
//      creates the 0x4b4-byte objects (0x4155e0) at +0x21 and +0x23 + 4 * i (i = 1..+0x25,
//      the count read by 0x587c20) and loads each one (0x4c9930).
//
//  - 0x004c9530 / 0x004d1620: SEH frames only guard `new` + constructor (delete on throw);
//    normal path reproduced, the constructors do not throw.
namespace {
namespace game_b102_04 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::op_new;

JOAN_COUNTER(c_load_4c9530, "Obj::load_4c9530");

struct H {
    // ------------------------------------------------------------------ 0x004c9530
    void* new_4b4() {
        void* const m = op_new(sizeof(jo::Squad));  // 0x4b4 on x86
        if (m == nullptr) return nullptr;
        return joan::orig::as_squad(m)->construct();  // 0x004155e0
    }
    i32 load_4c9530(void* stream) {
        c_load_4c9530.hit();
        if (joan::save::s5::read_unit_key(*jo::as_file(stream), *jo::self<jo::ProfileUnit>(this)) == 0) return 0;  // 0x005873a0 (field-wise)
        if (jo::view<void*>(jo::self<jo::ProfileUnit>(this)->leader) == nullptr) jo::view<void*>(jo::self<jo::ProfileUnit>(this)->leader) = new_4b4();
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(jo::view<void*>(jo::self<jo::ProfileUnit>(this)->leader))->read_profile(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x004c9930
        jo::self<jo::ProfileUnit>(this)->member_count = joan::orig::as_file(stream)->file_get_short();                       // 0x00587c20
        for (i32 i = 1; i <= static_cast<i32>(jo::self<jo::ProfileUnit>(this)->member_count); ++i) {
            if (jo::view<void*>(jo::self<jo::ProfileUnit>(this)->members[i - 1]) == nullptr) jo::view<void*>(jo::self<jo::ProfileUnit>(this)->members[i - 1]) = new_4b4();
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(jo::view<void*>(jo::self<jo::ProfileUnit>(this)->members[i - 1]))->read_profile(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x004c9930
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("Obj::load_4c9530", 0x004c9530, &H::load_4c9530);
#undef B

}  // namespace game_b102_04
}  // namespace

// ---------------------------------------- from modules/game_b106_08.cpp (game/b106_08)
//
//    0x004c9cd0 SaveList::refresh(scan)       (thiscall, ret 4) - clears (0x004c9f00); with
//               scan != 0 lists SAVE\*.dat (Directory 0x00582ad0 / 0x00582b00), adds every
//               file name without ".dat" (0x004ca050 + 0x00485790 -> 0x004c8490); then
//               looks up / adds the current name 0x00a57fcf into word +0x2e.
//
//  - The operator new + ctor pairs of 0x006bc530 and the local Directory of 0x004c9cd0 have
//    SEH unwind entries only (no throw on the normal path); not reproduced.
namespace {
namespace game_b106_08 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::str;

JOAN_COUNTER(c_save_refresh, "SaveList::refresh_4c9cd0");

struct H {
    // 0x004c9cd0
    void save_refresh(i32 scan) {
        c_save_refresh.hit();
        joan::orig::as_unit_list(this)->clear_4c9f00();  // 0x004c9f00
        if (scan != 0) {
            char wildcard[0x11c];
            char name[0x108];
            alignas(4) u8 dir[(sizeof(jo::Directory) + 3u) & ~3u];  // Directory (DynArray: 0x1d bytes on x86, slot 0x20; 0x21 on x64)
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(wildcard)), str(d::s_SAVE));  // strcpy "SAVE\"
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(wildcard)), str(d::s_dat_9d56f8));  // strcat "*.dat"
            joan::orig::as_directory(dir)->construct_582ad0();                           // 0x00582ad0 Directory()
            joan::orig::as_directory(dir)->read_582b00(joan::orig::arg::str(static_cast<const char*>(wildcard)), 0);  // 0x00582b00 read
            for (i32 i = joan::orig::as_dynarray(dir)->size(); i >= 1; --i) {  // 0x00417ad0
                char* const file = joan::orig::arg::as<char*>(joan::orig::as_list(dir)->item_477cc0(joan::orig::arg::i32(i)));  // 0x00477cc0
                char* const dot = joan::orig::arg::as<char*>(joan::orig::libc::strstr(joan::orig::arg::str(file), str(d::s_dat_9d56f0)));  // strstr ".dat"
                const i32 n = static_cast<i32>(dot - file);  // a file name: fits 32 bits
                std::strncpy(static_cast<char*>(joan::orig::arg::mptr(name)), joan::orig::arg::str(static_cast<const char*>(file)), joan::orig::arg::u32(n));  // strncpy
                name[n] = 0;
                const u32 id = joan::orig::arg::as<u32>(joan::orig::as_profile_list(this)->add(static_cast<const char*>(name)));  // 0x004ca050
                void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485790(joan::orig::arg::i32(id)));                            // 0x00485790
                joan::orig::as_profile(e)->load_4c8490();                                                     // 0x004c8490
            }
            joan::orig::as_directory(dir)->destruct_477410();  // 0x00477410 ~Directory
        }
        jo::self<jo::SaveList>(this)->current = joan::orig::arg::as<i16>(joan::orig::as_list(this)->find_name_4ca180(str(d::g_a57fcf)));  // 0x004ca180
        if (static_cast<i32>(jo::self<jo::SaveList>(this)->current) == 0) {
            jo::self<jo::SaveList>(this)->current = joan::orig::arg::as<i16>(joan::orig::as_profile_list(this)->add(str(d::g_a57fcf)));  // 0x004ca050
        } else {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485790(static_cast<i32>(jo::self<jo::SaveList>(this)->current)));  // 0x00485790
            joan::orig::as_profile(e)->load_4c8bb0();                                                        // 0x004c8bb0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_08", name, addr, fn)
B("SaveList::refresh_4c9cd0", 0x004c9cd0, &H::save_refresh);
#undef B

}  // namespace game_b106_08
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
//
//    - UnitList::select_4ca210 0x004ca210 (short), ret 4 -> 1: deselect current, word +0x2e = v,
namespace {
namespace game_b105_06 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_select_4ca210, "UnitList::select_4ca210");

struct H {
    i32 flagged(i32 i) { return joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_empty_at(joan::orig::arg::i32(i))); }          // 0x00485900
    void* entry(i32 i) { return joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485790(joan::orig::arg::i32(i))); }        // 0x00485790
    i32 cur() { return static_cast<i32>(jo::self<jo::SaveList>(this)->current); }
    void deselect_current() {
        if (flagged(cur()) == 0) joan::orig::as_profile(entry(cur()))->save_4c8810();  // 0x004c8810
    }

    // ------------------------------------------------------------------------ 0x004ca210
    i32 select_4ca210(u32 v) {
        c_select_4ca210.hit();
        deselect_current();
        jo::view<u16>(jo::self<jo::SaveList>(this)->current) = static_cast<u16>(v);
        joan::orig::as_profile(entry(cur()))->load_4c8bb0();  // 0x004c8bb0
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("UnitList::select_4ca210", 0x004ca210, &H::select_4ca210);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
//
//    - Profile::save 0x004c8220 (ret 0) -> int: path "SAVE\" + name(this) + ".dat", File
//      0x004179c0 + open-for-write 0x00586f00(path, 1, 0); writes 0x48 bytes of this, then
//      0x20 bytes of this, the count +0x20, and each 0x77-byte record of +0x24 via 0x004c93e0.
//      Dead code (no callers, no data references).
//
//    - Profile::load 0x004c8490 (ret 0) -> int: 0x004c7e70, same path; 0 when 0x00590b80 says
//      the file is missing; reads 0x48 / 0x20 bytes into this, count +0x20, allocates +0x24
//      (count * 0x77, profile.cpp), zeroes it, loads each record via 0x004c9420; clears +0x24
//      (count 0), +0x2c, +0x28, +0x34, +0x30, +0x40.
//
//  - SEH frames in 0x004c8220 / 0x004c8490 / 0x004c9930 / 0x004ca980 / 0x004ce630 only guard
//    the File object or new + ctor: normal path reproduced.
namespace {
namespace game_b101_06 {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

void* const kHeap = obj(d::g_mem);
void* const kFiles = obj(d::g_misc);

using joan::orig::helpers::crt_memset;
i32 line_of(img::Addr g, i32 add_) { return static_cast<i32>(global<i16>(g)) + add_; }

// ---------------------------------------------------------------- path / file helpers
struct alignas(4) Path {  // 0x194 bytes in the frames
    u8 bytes[0x194];
};
static_assert(sizeof(Path) == 0x194, "Path");
struct alignas(4) File {
    u8 bytes[0x210];  // 0x110 in the frames (0x004179c0); room to spare
};
void path_ctor(Path& p) { joan::orig::as_string(&p)->construct(); }               // 0x005a1170
void path_assign(Path& p, const char* s) { joan::orig::as_string(&p)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void path_append(Path& p, const void* s) { joan::orig::as_string(&p)->append(joan::orig::arg::str(s)); }  // 0x005a1550
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
void file_ctor(File& f) { joan::orig::as_file(&f)->construct(); }                      // 0x004179c0
void file_close(File& f) { joan::orig::as_file(&f)->file_close(); }                        // 0x00587260
void file_dtor(File& f) { joan::orig::as_file(&f)->destruct(); }                     // 0x005872b0

void build_save_path(Path& path, const void* name) {
    path_ctor(path);
    path_assign(path, str(d::s_SAVE));  // "SAVE\"
    path_append(path, name);
    path_append(path, str(d::s_dat_9d56f0));  // ".dat"
}

JOAN_COUNTER(c_save, "Profile::save");
JOAN_COUNTER(c_load, "Profile::load");

struct H {
    // ------------------------------------------------------------------------ 0x004c8220
    i32 profile_save() {
        c_save.hit();
        File file;
        file_ctor(file);
        Path path;
        build_save_path(path, this);
        joan::orig::as_file(&file)->file_create(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(0));  // 0x00586f00 open (write)
        i32 result = 1;
        // Profile 0x48 + name 0x20 field by field (save::s5::write_profile_head; [Debug] SaveFieldwise=0: raw writes)
        if (joan::save::s5::write_profile_head(*jo::as_file(&file), *jo::self<jo::Profile>(this)) == 0) {  // 0x005872e0 x2
            result = 0;
        } else {
            joan::orig::as_file(&file)->file_put_long(joan::orig::arg::i32(jo::view<u32>(jo::self<jo::Profile>(this)->unit_count)));  // 0x00587da0
            for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
                void* const rec = &jo::self<jo::Profile>(this)->units[i];
                if (joan::orig::arg::as<i32>(joan::orig::as_key(rec)->read(joan::orig::arg::mptr(static_cast<void*>(&file)))) == 0) {  // 0x004c93e0
                    result = 0;
                    break;
                }
            }
        }
        file_close(file);
        file_dtor(file);
        return result;
    }

    // ------------------------------------------------------------------------ 0x004c8490
    i32 profile_load() {
        c_load.hit();
        joan::orig::as_profile(this)->clear();  // 0x004c7e70
        File file;
        file_ctor(file);
        Path path;
        build_save_path(path, this);
        if (joan::orig::as_misc(kFiles)->is_file_exist(joan::orig::arg::str(c_str(path))) == 0) {  // 0x00590b80
            file_dtor(file);
            return 0;
        }
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(path)), 1, static_cast<char>(0));  // 0x00586da0 open (read)
        auto fail = [&file]() {
            file_close(file);
            file_dtor(file);
            return 0;
        };
        // Profile 0x48 + name 0x20 field by field (save::s5::read_profile_head; [Debug] SaveFieldwise=0: raw reads)
        if (joan::save::s5::read_profile_head(*jo::as_file(&file), *jo::self<jo::Profile>(this)) == 0) return fail();  // 0x005873a0 x2
        jo::self<jo::Profile>(this)->unit_count = joan::orig::as_file(&file)->file_get_long();   // 0x00587e20
        if (jo::self<jo::Profile>(this)->unit_count != 0) {
            const i32 line = line_of(d::g_a02440, 0x24);
            const u32 size = static_cast<u32>(jo::self<jo::Profile>(this)->unit_count) * static_cast<u32>(sizeof(jo::ProfileUnit));
            jo::view<void*>(jo::self<jo::Profile>(this)->units) = joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(size), str(d::s_d_ww_profile_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0
            crt_memset(jo::view<void*>(jo::self<jo::Profile>(this)->units), 0, static_cast<u32>(jo::self<jo::Profile>(this)->unit_count) * static_cast<u32>(sizeof(jo::ProfileUnit)));
            for (i32 i = 0; i < jo::self<jo::Profile>(this)->unit_count; ++i) {
                void* const rec = &jo::self<jo::Profile>(this)->units[i];
                if (joan::orig::arg::as<i32>(joan::orig::as_key(rec)->write(joan::orig::arg::mptr(static_cast<void*>(&file)))) == 0) return fail();  // 0x004c9420
            }
        } else {
            jo::self<jo::Profile>(this)->units = nullptr;
        }
        jo::self<jo::Profile>(this)->list_a = nullptr;
        jo::view<u32>(jo::self<jo::Profile>(this)->list_a_count) = 0;
        jo::self<jo::Profile>(this)->list_b = nullptr;
        jo::view<u32>(jo::self<jo::Profile>(this)->list_b_count) = 0;
        jo::view<u32>(jo::self<jo::Profile>(this)->i32_40) = 0;
        file_close(file);
        file_dtor(file);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("Profile::save", 0x004c8220, &H::profile_save);
B("Profile::load", 0x004c8490, &H::profile_load);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
namespace {
namespace game_b94_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ 0x004c7dc0
    void* zero_48() {
        memset_(this, 0, sizeof(jo::Profile));  // 0x48 on x86
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("Struct48::zero", 0x004c7dc0, &H::zero_48);
#undef B94

}  // namespace game_b94_03
}  // namespace

// ---------------------------------------- from modules/game_b96_03_2.cpp (game/b96_03)
namespace {
namespace game_b96_03_2 {

using u32 = std::uint32_t;

using joan::img::str;

struct H {
    // ------------------------------------------------------------------------ 0x004c7e20
    void set_name32(const char* src) {
        if (src != nullptr) std::strncpy(static_cast<char*>(joan::orig::arg::mptr(reinterpret_cast<char*>(this))), joan::orig::arg::str(src), 0x20u);  // strncpy
        jo::self<jo::Profile>(this)->u32_38 = 0x64;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("Name32::set", 0x004c7e20, &H::set_name32);
#undef B96

}  // namespace game_b96_03_2
}  // namespace

// ---------------------------------------- from modules/game_b88_03.cpp (game/b88_03)
namespace {
namespace game_b88_03 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::img::obj;
std::int32_t sx16(std::int16_t v) { return static_cast<std::int32_t>(v); }

using joan::orig::helpers::game_new;  // operator new

void* const kUnitList = obj(d::g_units);

JOAN_COUNTER(c_load_unit, "MapUnits::load_unit");

struct H {
    // ------------------------------------------------------------------------ 0x004c91c0
    std::int32_t load_unit(const char* def_name, void* unit) {
        c_load_unit.hit();
        joan::orig::dynarray_b(kUnitList)->linkin(&unit);                                 // 0x005857a0
        jo::as_unit(unit)->id = joan::orig::arg::as<std::int16_t>(joan::orig::as_dynarray(kUnitList)->cursor());  // 0x00417520
        std::uint8_t* const def = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_table(this)->find_name_4c81a0(def_name));  // 0x004c81a0
        if (def == nullptr) return 0;
        if (jo::as_profile_unit(def)->leader == nullptr) return 0;
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(def)->copy_to(joan::orig::arg::mptr(jo::as_profile_unit(def)->leader), joan::orig::arg::mptr(unit))) == 0) return 0;  // 0x004c96f0
        joan::orig::as_unit(unit)->sync_shadow();  // 0x00447600
        for (std::int32_t i = 1; i <= sx16(jo::as_profile_unit(def)->member_count); ++i) {
            void* const mem = game_new(sizeof(jo::Squad));  // 0x4b4 on x86
            void* part = mem != nullptr ? joan::orig::as_squad(mem)->construct() : nullptr;  // 0x004155e0
            joan::orig::dynarray_b(kUnitList)->linkin(&part);                                 // 0x005857a0
            jo::as_unit(part)->id = joan::orig::arg::as<std::int16_t>(joan::orig::as_dynarray(kUnitList)->cursor());  // 0x00417520
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(def)->copy_to(joan::orig::arg::mptr(jo::as_profile_unit(def)->members[i - 1]), joan::orig::arg::mptr(part))) == 0) return 0;  // 0x004c96f0
            joan::orig::as_unit(part)->link_to(joan::orig::arg::i32(sx16(jo::as_unit(unit)->id)));  // 0x00438f80
            jo::as_unit(part)->layer = 1;
        }
        return 1;
    }
};

#define B88(name, addr, fn) JOAN_REPLACE("game/b88_03", name, addr, fn)
B88("MapUnits::load_unit", 0x004c91c0, &H::load_unit);
#undef B88

}  // namespace game_b88_03
}  // namespace

// ---------------------------------------- from modules/game_b105_06_2.cpp (game/b105_06)
namespace {
namespace game_b105_06_2 {

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_clear_4c9f00, "UnitList::clear_4c9f00");

struct H {
    u32 get44() { return jo::view<u32>(jo::self<jo::Profile>(this)->i32_44); }

    // ---- unit list helpers ----
    i32 count() { return joan::orig::as_dynarray(this)->size(); }  // 0x00417ad0
    i32 flagged(i32 i) { return joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_empty_at(joan::orig::arg::i32(i))); }          // 0x00485900
    void* entry(i32 i) { return joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485790(joan::orig::arg::i32(i))); }        // 0x00485790
    i32 cur() { return static_cast<i32>(jo::self<jo::SaveList>(this)->current); }
    void deselect_current() {
        if (flagged(cur()) == 0) joan::orig::as_profile(entry(cur()))->save_4c8810();  // 0x004c8810
    }

    // ------------------------------------------------------------------------ 0x004c9f00
    u32 clear_4c9f00() {
        c_clear_4c9f00.hit();
        deselect_current();
        for (i32 i = count(); i >= 1; --i) {
            if (flagged(i) != 0) continue;
            void* e = entry(i);
            if (e != nullptr) joan::orig::as_profile(e)->destroy(1u);  // 0x004ca000 scalar dtor
            joan::orig::dynarray_b(this)->linkout(joan::orig::arg::i32(i));  // 0x005858c0
        }
        joan::orig::as_sound(this)->reset_585d20();  // 0x00585d20
        jo::self<jo::SaveList>(this)->current = 0;
        return 0;  // Sound::reset_585d20 returns nothing; no caller of 0x004c9f00 reads the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::get44_4c9390", 0x004c9390, &H::get44);
B("UnitList::clear_4c9f00", 0x004c9f00, &H::clear_4c9f00);
#undef B

}  // namespace game_b105_06_2
}  // namespace

// ---------------------------------------- from modules/game_b103_04_2.cpp (game/b103_04)
namespace {
namespace game_b103_04_2 {

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_record_write, "Record::write");

struct H {
    // ------------------------------------------------------------------------ 0x004c9460
    i32 record_write(void* file) {
        c_record_write.hit();
        if (joan::save::s5::write_unit_key(*jo::as_file(file), *jo::self<jo::ProfileUnit>(this)) == 0) return 0;  // 0x005872e0 (field-wise)
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(jo::view<void*>(jo::self<jo::ProfileUnit>(this)->leader))->write_profile(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x004c9880
        joan::orig::as_file(file)->file_put_short(joan::orig::arg::i16(jo::self<jo::ProfileUnit>(this)->member_count));  // 0x00587ba0 write short
        for (i32 i = 1; i <= static_cast<i32>(jo::self<jo::ProfileUnit>(this)->member_count); ++i) {
            void* const p = jo::view<void*>(jo::self<jo::ProfileUnit>(this)->members[i - 1]);
            if (p == nullptr) continue;
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(p)->write_profile(joan::orig::arg::mptr(file))) == 0) return 0;
        }
        return 1;
    }

    // ------------------------------------------------------------------------ 0x004ca110
    u32 remove_4ca110(i32 i) {
        void* const p = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_485790(joan::orig::arg::i32(i)));  // 0x00485790
        if (p != nullptr) joan::orig::as_profile(p)->destroy(1u);     // 0x004ca000 scalar dtor
        return thiscall<u32>(fn::linkout_5858c0, this, i);             // 0x005858c0
    }

    i32 set_44(i32 v) {  // 0x004c93b0
        jo::self<jo::Profile>(this)->i32_44 = v;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("Obj::set_44", 0x004c93b0, &H::set_44);
B("Record::write", 0x004c9460, &H::record_write);
B("List::remove_4ca110", 0x004ca110, &H::remove_4ca110);
#undef B

}  // namespace game_b103_04_2
}  // namespace

// ---------------------------------------- from modules/game_b92_03_3.cpp (game/b92_03)
namespace {
namespace game_b92_03_3 {

struct H {
    // 0x004c93e0 / 0x004c9420
    // the name key, field-wise (save::s5::write_unit_key; [Debug] SaveFieldwise=0: the raw write)
    std::int32_t key_read(void* file) { return joan::save::s5::write_unit_key(*jo::as_file(file), *jo::self<jo::ProfileUnit>(this)) != 0 ? 1 : 0; }  // 0x005872e0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Key::read", 0x004c93e0, &H::key_read);
#undef B

}  // namespace game_b92_03_3
}  // namespace

// ---------------------------------------- from modules/game_b89_05.cpp (game/b89_05)
namespace {
namespace game_b89_05 {

using joan::call::thiscall;

using joan::orig::helpers::s16;

JOAN_COUNTER(c_copy, "Unit::copy_to");

struct H {
    // ------------------------------------------------------------ 0x004c96f0 (ret 8)
    std::int32_t copy_to(std::uint8_t* src, std::uint8_t* dst) {
        c_copy.hit();
        joan::orig::as_unit(dst)->cleanup();  // 0x00461f10
        joan::orig::as_unit(dst)->init(joan::orig::arg::i32(s16(jo::view<std::uint16_t>(jo::as_unit(src)->type_id))));  // 0x004f8550
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_unit(dst)->name_)), joan::orig::arg::str(jo::as_unit(src)->name_), 0x42u);  // strncpy
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_unit(dst)->name_c)), joan::orig::arg::str(jo::as_unit(src)->name_c), 0x41u);  // strncpy
        jo::view<std::uint32_t>(jo::as_unit(dst)->rank) = jo::view<std::uint32_t>(jo::as_unit(src)->rank);
        jo::view<std::uint16_t>(jo::as_unit(dst)->kingdom_id) = jo::view<std::uint16_t>(jo::as_unit(src)->kingdom_id);
        jo::view<std::uint16_t>(jo::as_unit(dst)->race_id) = jo::view<std::uint16_t>(jo::as_unit(src)->race_id);
        jo::view<std::uint16_t>(jo::as_unit(dst)->group_id) = jo::view<std::uint16_t>(jo::as_unit(src)->group_id);
        std::memcpy(static_cast<void*>(&jo::as_unit(dst)->lives_max), static_cast<const void*>(&jo::as_unit(src)->lives_max), offsetof(jo::Unit, stats_block) - offsetof(jo::Unit, lives_max));  // memcpy +0x11c .. +0x16b end (0x53)
        std::memmove(&jo::as_unit(dst)->stats_block, &jo::as_unit(src)->stats_block, sizeof(jo::as_unit(src)->stats_block));  // rep movsd x14 + movsw + movsb
        joan::orig::as_upgrade_list(&jo::as_unit(dst)->upgrades)->assign(joan::orig::arg::mptr(&jo::as_unit(src)->upgrades));  // 0x00531230
        joan::orig::as_unit_data(jo::as_unit(dst)->modifiers)->copy(joan::orig::arg::mptr(jo::as_unit(src)->modifiers));  // 0x00497a10
        jo::view<std::uint32_t>(jo::as_unit(dst)->collision_id) = jo::view<std::uint32_t>(jo::as_unit(src)->collision_id);
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b89_05", name, addr, fn)
B("Unit::copy_to", 0x004c96f0, &H::copy_to);
#undef B

}  // namespace game_b89_05
}  // namespace

// ---------------------------------------- from modules/game_b102_04_2.cpp (game/b102_04)
namespace {
namespace game_b102_04_2 {

using joan::call::thiscall;

using i32 = std::int32_t;

using u8 = std::uint8_t;


JOAN_COUNTER(c_load_4c9880, "Obj::load_4c9880");

struct H {
    // ------------------------------------------------------------------ 0x004c9880
    i32 load_4c9880(void* stream) {
        c_load_4c9880.hit();
        {  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::put_ProfileHero; 0 = the raw write
            i32 ok;
            if (joan::save::fieldwise()) {
                joan::save::Out o(*joan::orig::as_file(stream), joan::save::kProfileHero, this);
                joan::save::put_ProfileHero(o, *jo::self<jo::Unit>(this));
                ok = o.end();  // File::file_write 0x005872e0
            } else {
                ok = joan::orig::as_file(stream)->file_write(static_cast<void*>(this), 0x432u);  // 0x005872e0
            }
            if (ok == 0) return 0;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit_stats(&jo::self<jo::Unit>(this)->stats_block)->save(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x0047e5c0 (+0x16f)
        if (joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->save(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x0047e640 (+0x1aa)
        if (jo::self<jo::Unit>(this)->modifiers != nullptr) {
            if (joan::orig::arg::as<i32>(joan::orig::as_unit_data(jo::self<jo::Unit>(this)->modifiers)->save(joan::orig::arg::mptr(stream))) == 0) return 0;  // 0x0047e850
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_04", name, addr, fn)
B("Obj::load_4c9880", 0x004c9880, &H::load_4c9880);
#undef B

}  // namespace game_b102_04_2
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
namespace {
namespace game_b100_05 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u32 = std::uint32_t;

using joan::orig::helpers::op_delete;

struct H {
    // ------------------------------------------------------------------ 0x004c9c30
    H* ctor_4c9c30() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 0x14, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        return this;
    }

    // ------------------------------------------------------------------ 0x004ca000
    H* scalar_dtor_4ca000(u32 flags) {
        joan::orig::as_profile(this)->destruct();  // 0x004c7df0
        if ((flags & 1u) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::ctor_4c9c30", 0x004c9c30, &H::ctor_4c9c30);
B("Obj::scalar_dtor_4ca000", 0x004ca000, &H::scalar_dtor_4ca000);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b106_02.cpp (game/b106_02)
namespace {
namespace game_b106_02 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    u32 dtor_4c9c60() {  // 0x004c9c60
        joan::orig::as_unit_list(this)->clear_4c9f00();          // 0x004c9f00
        return thiscall<u32>(fn::dtor_DynArrayB, this);   // 0x00585710
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_02", name, addr, fn)
B("Obj::dtor_4c9c60", 0x004c9c60, &H::dtor_4c9c60);
#undef B

}  // namespace game_b106_02
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
namespace {
namespace game_b97_03 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using u32 = std::uint32_t;

JOAN_COUNTER(c_list48_add, "List48::add");

struct H {
    // ------------------------------------------------------------------------ 0x004ca050
    u32 list48_add(const char* a) {
        c_list48_add.hit();
        void* const mem = joan::crt::operator_new(sizeof(jo::Profile));  // operator new (0x48 on x86)
        void* p = nullptr;
        if (mem != nullptr) p = joan::orig::as_profile(mem)->construct();  // 0x004c7dc0
        joan::orig::as_profile(p)->set_name(a);                            // 0x004c7e20
        joan::orig::dynarray_b(this)->linkin(&p);                          // 0x005857a0
        return joan::orig::arg::as<u32>(joan::orig::as_dynarray(this)->cursor());  // 0x00417520
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("List48::add", 0x004ca050, &H::list48_add);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b94_03_2.cpp (game/b94_03)
namespace {
namespace game_b94_03_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using i32 = std::int32_t;

JOAN_COUNTER(c_find_name, "List::find_name");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x004ca180
    i32 find_name(const char* name) {
        c_find_name.hit();
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;   // 0x00485900
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_list(this)->at_485790(joan::orig::arg::i32(i)));  // 0x00485790
            if (stricmp_(s, name) == 0) return i;
        }
        return 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("List::find_name", 0x004ca180, &H::find_name);
#undef B94

}  // namespace game_b94_03_2
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {

using joan::call::thiscall;

struct H {
    // 0x004ca290
    void* lookup_4ca290() {
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_empty_at(static_cast<std::int32_t>(jo::self<jo::SaveList>(this)->current))) != 0)  // 0x00485900
            return nullptr;
        return joan::orig::as_list(this)->at_485790(static_cast<std::int32_t>(jo::self<jo::SaveList>(this)->current));  // 0x00485790
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj4ca::lookup", 0x004ca290, &H::lookup_4ca290);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
namespace {
namespace game_b101_06_2 {

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::op_new;

JOAN_COUNTER(c_hero_load, "Hero::load");

struct H {
    // ------------------------------------------------------------------------ 0x004c9930
    i32 hero_load(void* file) {
        c_hero_load.hit();
        {  // [Debug] SaveFieldwise=1 (default, re/a6/SAVES_PLAN.md S1): save::get_ProfileHero; 0 = the raw read
            i32 ok;
            if (joan::save::fieldwise()) {
                joan::save::In in(*joan::orig::as_file(file), joan::save::kProfileHero, this);  // File::file_read 0x005873a0
                joan::save::get_ProfileHero(in, *jo::self<jo::Hero>(this));
                ok = in.end();
            } else {
                ok = joan::orig::as_file(file)->file_read(static_cast<void*>(this), 0x432u);  // 0x005873a0
            }
            if (ok == 0) return 0;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_unit_stats(&jo::self<jo::Hero>(this)->stats_block)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047e600 (+0x16f)
        if (joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(&jo::self<jo::Hero>(this)->upgrades)->load(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047e700 (+0x1aa)
        if (jo::self<jo::Hero>(this)->modifiers != nullptr) {
            void* p = op_new(sizeof(jo::UnitData));  // 0x1e on x86
            if (p != nullptr) p = joan::orig::as_unit_data(p)->construct();  // 0x00495950 ctor
            jo::self<jo::Hero>(this)->modifiers = static_cast<jo::Modifiers*>(p);
            if (joan::orig::arg::as<i32>(joan::orig::as_sub(jo::self<jo::Hero>(this)->modifiers)->load_parts(joan::orig::arg::mptr(file))) == 0) return 0;  // 0x0047e9f0
        }
        joan::orig::as_unit(this)->reset_links();  // 0x0047e300
        jo::view<u16>(jo::self<jo::Hero>(this)->side) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->place_index) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->place_slot) = 0;
        jo::view<u32>(jo::self<jo::Hero>(this)->anim_entry) = 0;
        jo::view<u8>(jo::self<jo::Hero>(this)->frozen) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->action) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->anim_kind) = 1;
        jo::view<u16>(jo::self<jo::Hero>(this)->anim_item_using) = 1;
        jo::view<u16>(jo::self<jo::Hero>(this)->anim_special) = 0;
        jo::self<jo::Hero>(this)->armour_flags[0] = 0xff;
        jo::view<u8>(jo::self<jo::Hero>(this)->combo_state) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->combo_queued) = 0;
        jo::self<jo::Hero>(this)->anim_done = 0;
        jo::self<jo::Hero>(this)->mount_busy = 0;
        jo::self<jo::Hero>(this)->view = 0;
        joan::orig::as_squad(this)->clear();  // 0x00422f10
        jo::self<jo::Hero>(this)->wait_limit = global<u32>(d::g_unit_init_w_c);
        jo::self<jo::Hero>(this)->wait_count = global<u32>(d::g_unit_init_w_c);
        jo::view<u32>(jo::self<jo::Hero>(this)->mount_id) = 0;
        jo::view<u32>(jo::self<jo::Hero>(this)->rider_id) = 0;
        jo::self<jo::Hero>(this)->u8_3e5 = 1;  // +0x3e5
        jo::view<u16>(jo::self<jo::Hero>(this)->carried_a) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->carried_b) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->target_building) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->target_object_id) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->look_a) = 0;
        jo::view<u16>(jo::self<jo::Hero>(this)->look_b) = 0;
        jo::self<jo::Hero>(this)->mask_slots = nullptr;
        jo::self<jo::Hero>(this)->mask_slots_b = nullptr;
        jo::self<jo::Hero>(this)->orders = nullptr;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("Hero::load", 0x004c9930, &H::hero_load);
#undef B

}  // namespace game_b101_06_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Profile::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_03::H::zero_48>(this)); }  // 0x4c7dc0
auto joan::orig::Profile::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_03::H::reset_c7>(this)); }  // 0x4c7df0
auto joan::orig::Profile::set_name(const char* a0) -> void { joan::bind::member<&::game_b96_03_2::H::set_name32>(this, a0); }  // 0x4c7e20
auto joan::orig::Profile::clear() -> void { joan::bind::member<&::game_b92_03::H::profile_clear>(this); }  // 0x4c7e70
auto joan::orig::Profile::add(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_03::H::profile_add>(this, a0)); }  // 0x4c80c0
auto joan::orig::Table::find_name_4c81a0(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b89_06::H::find_name>(this, a0)); }  // 0x4c81a0
auto joan::orig::Profile::load_4c8490() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_06::H::profile_load>(this)); }  // 0x4c8490
auto joan::orig::Profile::save_4c8810() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_04::H::save_4c8810>(this)); }  // 0x4c8810
auto joan::orig::Profile::load_4c8bb0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_04::H::profile_load>(this)); }  // 0x4c8bb0
auto joan::orig::Profile::attach_unit(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_03::H::attach>(this, a0)); }  // 0x4c90a0
auto joan::orig::Profile::load_unit(const char* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b88_03::H::load_unit>(this, a0, a1)); }  // 0x4c91c0
auto joan::orig::Key::read(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_03_3::H::key_read>(this, a0)); }  // 0x4c93e0
auto joan::orig::Key::write(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_03_2::H::key_write>(this, a0)); }  // 0x4c9420
auto joan::orig::ProfileUnit::write(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b103_04_2::H::record_write>(this, a0)); }  // 0x4c9460
auto joan::orig::ProfileUnit::read(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_04::H::load_4c9530>(this, a0)); }  // 0x4c9530
auto joan::orig::Unit::copy_to(void* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_05::H::copy_to>(this, a0, a1)); }  // 0x4c96f0
auto joan::orig::Unit::write_profile(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_04_2::H::load_4c9880>(this, a0)); }  // 0x4c9880
auto joan::orig::Unit::read_profile(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_06_2::H::hero_load>(this, a0)); }  // 0x4c9930
auto joan::orig::ProfileList::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_02::H::dtor_4c9c60>(this)); }  // 0x4c9c60
auto joan::orig::ProfileList::refresh(std::int32_t a0) -> void { joan::bind::member<&::game_b106_08::H::save_refresh>(this, a0); }  // 0x4c9cd0
auto joan::orig::UnitList::clear_4c9f00() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_06_2::H::clear_4c9f00>(this)); }  // 0x4c9f00
auto joan::orig::Profile::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_05::H::scalar_dtor_4ca000>(this, a0)); }  // 0x4ca000
auto joan::orig::ProfileList::add(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_03::H::list48_add>(this, a0)); }  // 0x4ca050
auto joan::orig::List::find_name_4ca180(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_03_2::H::find_name>(this, a0)); }  // 0x4ca180
auto joan::orig::ProfileList::current() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b88_06::H::lookup_4ca290>(this)); }  // 0x4ca290
