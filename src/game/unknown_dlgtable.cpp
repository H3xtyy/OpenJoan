// game/unknown_dlgtable.cpp: functions whose original source file is unknown (formerly game/_unplaced/dlgtable.cpp;
// re/attribution.csv: between ww/uti3d/dlgres.cpp and ww/uti3d/esstate2.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/cxx_eh.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../ilib/sound_archives.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using joan::call::thiscall;

using joan::call::stdcall_call;

}  // namespace

// ---------------------------------------- from modules/game_b115_12.cpp (fix/uninit)
namespace {
namespace game_b115_12 {

using i8 = std::int8_t;

using joan::orig::helpers::at;
using joan::orig::helpers::wrap_sub;
using joan::orig::helpers::wrap_mul;

constexpr img::Addr kUtil(d::g_misc);

JOAN_COUNTER(c_exres, "ExRes::write_638aa0");

// ------------------------------------------------------------------ common helpers
struct alignas(4) Text {  // 0x005a1170 / 0x005a11a0 string, 0x194 bytes, no destructor
    u8 b[0x194];
};
void text_ctor(Text& t) { joan::orig::as_string(&t)->construct(); }               // 0x005a1170
void text_set(Text& t, const char* s) { joan::orig::as_string(&t)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void text_add(Text& t, const char* s) { joan::orig::as_string(&t)->append(joan::orig::arg::str(s)); }  // 0x005a1550
const char* c_str(Text& t) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&t)->c_str()); }  // 0x00417d90

struct H {
    // -------------------------------------------------------------- 0x00638aa0
    void ex_res_write(const char* dir, const char* name) {
        c_exres.hit();
        if (jo::self<jo::ExRes>(this)->count_ == 0) return;
        Text path;
        text_ctor(path);
        text_set(path, dir);
        text_add(path, name);
        text_add(path, str(d::s_EX_res));  // "EX.res"
        void* const fp = joan::orig::arg::as<void*>(joan::orig::libc::fopen(joan::orig::arg::str(c_str(path)), str(d::s_w)));  // fopen "w"
        char pad[8];
        std::memset(joan::orig::arg::mptr(pad), i32{0}, u32{5});  // memset
        for (i32 i = 1; i <= jo::self<jo::ExRes>(this)->count_; ++i) {
            u8* const e = jo::view<u8*>(jo::self<jo::ExRes>(this)->entries) + wrap_sub(wrap_mul(i, static_cast<i32>(sizeof(jo::DlgResEntry))), static_cast<i32>(sizeof(jo::DlgResEntry)));  // 16 on x86
            const char* const s =
                joan::orig::arg::as<const char*>(joan::orig::as_misc(img::obj(kUtil))->format_int_58fca0(joan::orig::arg::i32(static_cast<u32>(jo::as_dlg_res_entry(e)->id)), i32{1}));  // 0x0058fca0 (ret 8: the old third arg 5 was never read)
            char num[8];
            std::strncpy(static_cast<char*>(joan::orig::arg::mptr(num)), joan::orig::arg::str(s), u32{5});  // strncpy
            char unpadded[6];
            const char* shown = pad;
            if (num[0] != 0 && num[1] != 0 && num[2] != 0 && num[3] != 0 && num[4] != 0) {
                // fix/uninit: unterminated 5-character number (see the header): written unpadded.
                std::memcpy(unpadded, num, 5);
                unpadded[5] = 0;
                shown = unpadded;
            } else {
                for (u32 j = 0; j < 4u - static_cast<u32>(std::strlen(joan::orig::arg::str(num))); ++j) pad[j] = '0';
                i32 j = 4 - static_cast<i32>(std::strlen(joan::orig::arg::str(num)));
                for (i32 k = 0; j < 4; ++j, ++k) pad[j] = num[k];
            }
            joan::orig::libc::fprintf(joan::orig::arg::mptr(fp), str(d::s_s_c), shown, static_cast<i32>(jo::view<i8>(jo::as_dlg_res_entry(e)->kind)));  // "[%s%c]\n"
            joan::orig::libc::fprintf(joan::orig::arg::mptr(fp), str(d::s_s), jo::view<const char*>(jo::as_dlg_res_entry(e)->text));               // "%s\n"
            joan::orig::libc::fprintf(joan::orig::arg::mptr(fp), str(d::blob_9da858));                                      // "~\n"
        }
        joan::orig::libc::fclose(joan::orig::arg::mptr(fp));  // fclose
    }

    // -------------------------------------------------------------- 0x006a9570
    struct FV {
        float x, y, z;
    };
    static_assert(sizeof(FV) == 12, "sizes");
};

// 0x00638aa0: numbers >= 1000 leave the strncpy buffer unterminated (strlen of frame garbage, pad
// loop runs off the stack); they are written unpadded.
JOAN_REPLACE("fix/uninit", "ExRes::write_638aa0", 0x00638aa0, &H::ex_res_write);

}  // namespace game_b115_12
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

JOAN_COUNTER(c_strtab_size, "StringTable::size");

struct H {
    // 0x00638d50 StringTable::size
    std::int32_t strtab_size() {
        c_strtab_size.hit();
        std::int32_t total = jo::view<std::int32_t>(jo::self<jo::DlgRes>(this)->data_size) + 0xf;
        for (std::int32_t i = jo::self<jo::DlgRes>(this)->count_; i > 0; --i) {
            const char* const s = jo::self<jo::DlgRes>(this)->entries[i - 1].text;
            if (s == nullptr) continue;
            const std::uintptr_t p = reinterpret_cast<std::uintptr_t>(s);
            if (jo::view<std::uintptr_t>(jo::self<jo::DlgRes>(this)->data) == 0 || p < jo::view<std::uintptr_t>(jo::self<jo::DlgRes>(this)->data) || p > jo::view<std::uintptr_t>(jo::self<jo::DlgRes>(this)->data) + jo::self<jo::DlgRes>(this)->data_size)
                total = total + joan::orig::arg::as<std::int32_t>(std::strlen(joan::orig::arg::str(s))) + 1;  // strlen
        }
        return total;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("StringTable::size", 0x00638d50, &H::strtab_size);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- from modules/game_b103_13.cpp (game/b103_13)
namespace {
namespace game_b103_13 {


struct H {
    H* ctor_638e10() {  // 0x00638e10
        joan::orig::as_wave_res(this)->construct();  // 0x004d08f0
        jo::self<jo::ResObj>(this)->vtable = img::obj(d::vt_ResObj);
        std::memset(joan::orig::arg::mptr(&jo::self<jo::WaveRes>(this)->priority), 0, 4u);  // memset
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_13", name, addr, fn)
B("Node::ctor_638e10", 0x00638e10, &H::ctor_638e10);
#undef B

}  // namespace game_b103_13
}  // namespace

// ---------------------------------------- from modules/game_b111_05.cpp (fix/uninit)
namespace {
namespace game_b111_05 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_res, "ResObj::load_638e60");

struct H {
    // ------------------------------------------------------------------ 0x00638e60 (fix/uninit)
    // The original strncpy's the 0x30-byte name at +6 into a 0x34-byte stack buffer [ebp-0x13c] and
    // _strupr's it. For a name of exactly 48 chars (reachable: 0x00590210 copies up to 0x30 bytes and
    // NULs at +0x36) the copy is unterminated and _strupr / 0x0059ef00 read the 4 uninitialised stack
    // bytes [ebp-0x10c..-0x109] and then run on into the path buffer. Intended value: those 4 bytes are
    // 0 (the name terminated at 48 chars). Every other byte and call is as in the original.
    std::uintptr_t res_load() {
        c_res.hit();
        jo::Sound3DBuffer* const r = joan::orig::arg::as<jo::Sound3DBuffer*>(joan::orig::as_sound3d(obj(d::g_sound3d_system))->create_buffer_48d120());  // 0x0048d120
        jo::self<jo::ResObj>(this)->buffer = r;
        if (jo::self<jo::ResObj>(this)->buffer == nullptr) return joan::orig::arg::as<u32>(r);
        char path[0x104];  // [ebp-0x108..-0x4): "RESOURCE\\" + zero fill
        std::memcpy(path, str(d::s_RESOURCE), 10);
        std::memset(path + 10, 0, sizeof(path) - 10);
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(path)), joan::orig::arg::str(jo::self<jo::ResObj>(this)->res_name));  // strcat
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(path)), str(d::s_RES));      // strcat ".RES"
        char name[0x34];
        name[0x30] = name[0x31] = name[0x32] = name[0x33] = 0;  // fix: originally uninitialised
// strncpy of 0x30 bytes into the buffer whose last 4 bytes were cleared above, as the original does.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
        std::strncpy(static_cast<char*>(joan::orig::arg::mptr(name)), joan::orig::arg::str(jo::self<jo::ResObj>(this)->name), 0x30u);  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        char* const up = joan::orig::arg::as<char*>(joan::orig::libc::_strupr(joan::orig::arg::str(name)));                                // _strupr
        return joan::orig::arg::as<u32>(joan::orig::as_sound3dbuffer(jo::self<jo::ResObj>(this)->buffer)->load(joan::orig::arg::str(path), up, 1u));                   // 0x0059ef00, ret 0xc
    }
};

// fix/uninit: 0x00638e60 reads 4 uninitialised stack bytes for 48-char names (see res_load).
JOAN_REPLACE("fix/uninit", "ResObj::load_638e60", 0x00638e60, &H::res_load);

}  // namespace game_b111_05
}  // namespace

// ---------------------------------------- from modules/game_b105_18.cpp (game/b105_18)
namespace {
namespace game_b105_18 {

// ------------------------------------------------------------------ cdecl helpers
u8* JOAN_CDECL ptr_37_cdecl(u8* p) { return reinterpret_cast<u8*>(jo::as_res_obj(p)->res_name); }  // 0x00638f90

struct H {
    u8* ptr_37() { return reinterpret_cast<u8*>(jo::self<jo::ResObj>(this)->res_name); }    // 0x00638f70
    void nop_639010() {}                                           // 0x00639010 (dead)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_18", name, addr, fn)
B("Obj::ptr_37", 0x00638f70, &H::ptr_37);
B("ptr_37_cdecl", 0x00638f90, &ptr_37_cdecl);
B("Obj::nop_639010", 0x00639010, &H::nop_639010);
#undef B

}  // namespace game_b105_18
}  // namespace

// ---------------------------------------- from modules/game_b100_12.cpp (game/b100_12)
namespace {
namespace game_b100_12 {

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x00638fb0
    H* clear6() {
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::NamedTable)));  // 6 on x86 (the NamedTable 0xa5cd6a)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_12", name, addr, fn)
B("Obj::clear6", 0x00638fb0, &H::clear6);
#undef B

}  // namespace game_b100_12
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

struct H {
    // ------------------------------------------------------------------------ 0x00638fe0
    u32 wrap_639030() {
        joan::orig::as_sound_res_table(this)->clear();  // 0x00639030
        return 0;  // void callee; the only caller is the atexit stub obj_stub_56a510
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Obj638::wrap_639030", 0x00638fe0, &H::wrap_639030);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {

JOAN_COUNTER(c_table_clear, "NamedTable::clear");

struct H {
    // ------------------------------------------------------------------ 0x00639030
    void table_clear() {
        c_table_clear.hit();
        if (jo::view<void*>(jo::self<jo::NamedTable>(this)->table) == nullptr) return;
        for (std::int16_t i = 0x4001;
             static_cast<std::int32_t>(i) <= static_cast<std::int32_t>(jo::self<jo::NamedTable>(this)->count) + 0x4000;
             i = static_cast<std::int16_t>(i + 1)) {
            void* e = joan::orig::arg::as<void*>(joan::orig::as_sound_res_table(this)->at(joan::orig::arg::i16(i)));  // 0x004d0860
            joan::orig::as_wave_res(e)->release_if();                          // 0x004d0990
        }
        void* t = jo::view<void*>(jo::self<jo::NamedTable>(this)->table);
        if (t != nullptr) joan::orig::as_wave_res(t)->vector_delete(3u);  // 0x006390f0 scalar deleting dtor
        jo::view<void*>(jo::self<jo::NamedTable>(this)->table) = nullptr;
        jo::view<std::uint16_t>(jo::self<jo::NamedTable>(this)->count) = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("NamedTable::clear", 0x00639030, &H::table_clear);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b86_01.cpp (game/b86_01)
namespace {
namespace game_b86_01 {

using joan::orig::helpers::at;

struct H {
    // 0x006390f0 vector deleting destructor (elements 0x93, dtor thunk 0x00403918 ->
    // 0x00639190). ret 4.
    void* vector_delete(std::uint32_t flags) {
        if ((flags & 2) != 0) {
            joan::orig::libc::eh_vector_destructor_iterator(joan::orig::arg::mptr(static_cast<void*>(this)), static_cast<std::uint32_t>(sizeof(jo::WaveRes)), joan::orig::arg::i32(at<std::int32_t>(this, -4))  /* x64: the new[] count cookie before the array */, fn::code_value(fn::Object_wrap_639190));  // eh vector destructor iterator
            void* const block = reinterpret_cast<std::uint8_t*>(this) - 4;
            if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(block));  // operator delete
            return block;
        }
        joan::orig::as_wave_res(this)->destruct_639190();  // 0x00639190
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));
        return this;
    }
};

#define GAME86(name, addr, fn) JOAN_REPLACE("game/b86_01", name, addr, fn)
GAME86("Array::vector_delete", 0x006390f0, &H::vector_delete);
#undef GAME86

}  // namespace game_b86_01
}  // namespace

// ---------------------------------------- from modules/en3d_b87_02.cpp (game/b87_02)
namespace {
namespace en3d_b87_02 {

struct H {
    // 0x00639190
    void wrap_639190() { joan::orig::as_wave_res(this)->destruct(); }  // 0x004d0940
};

#define GAME(name, addr, fn) JOAN_REPLACE("game/b87_02", name, addr, fn)
GAME("Object::wrap_639190", 0x00639190, &H::wrap_639190);
#undef GAME

}  // namespace en3d_b87_02
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_find_name, "Table::find_name");

using joan::orig::helpers::stricmp_;

struct H {
    // ------------------------------------------------------------------ 0x006391c0
    u32 find_name(const char* name, i32 key, i32 lo_arg, i32 hi_arg) {
        c_find_name.hit();
        const i16 lo = static_cast<i16>(lo_arg), hi = static_cast<i16>(hi_arg);
        if (lo > hi) return static_cast<u32>(static_cast<i32>(lo)) & 0xffff0000u;
        const i16 mid = static_cast<i16>((static_cast<i32>(lo) + static_cast<i32>(hi)) >> 1);
        u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_sound_res_table(this)->at(joan::orig::arg::i16(static_cast<i32>(mid))));  // 0x004d0860
        const i32 c = stricmp_(name, jo::as_wave_res(e)->file);
        if (c == 0) {
            if (key > jo::as_wave_res(e)->priority)
                return joan::orig::arg::as<u32>(joan::orig::as_table(this)->find_name_6391c0(joan::orig::arg::str(name), joan::orig::arg::i32(key), joan::orig::arg::i32(static_cast<i32>(mid) + 1), static_cast<i32>(hi)));
            if (key < jo::as_wave_res(e)->priority)
                return joan::orig::arg::as<u32>(joan::orig::as_table(this)->find_name_6391c0(joan::orig::arg::str(name), joan::orig::arg::i32(key), static_cast<i32>(lo), joan::orig::arg::i32(static_cast<i32>(mid) - 1)));
            return static_cast<u32>(static_cast<u16>(mid));
        }
        if (c > 0) return joan::orig::arg::as<u32>(joan::orig::as_table(this)->find_name_6391c0(joan::orig::arg::str(name), joan::orig::arg::i32(key), joan::orig::arg::i32(static_cast<i32>(mid) + 1), static_cast<i32>(hi)));
        return joan::orig::arg::as<u32>(joan::orig::as_table(this)->find_name_6391c0(joan::orig::arg::str(name), joan::orig::arg::i32(key), static_cast<i32>(lo), joan::orig::arg::i32(static_cast<i32>(mid) - 1)));
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Table::find_name", 0x006391c0, &H::find_name);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

struct H {
    // 0x00639310 (dead)
    u32 parse_639310(const char* a, const char* s) {
        const i32 n = joan::crt::atoi(joan::orig::arg::str(s));  // atoi
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_table(this)->find_name_6391c0(joan::orig::arg::str(a), joan::orig::arg::i32(n), 0x4001, joan::orig::arg::i32(static_cast<i32>(jo::self<jo::NamedTable>(this)->count) + 0x4000)));  // 0x006391c0
        if (static_cast<i16>(r) > 0) return r;
        return r & 0xffff0000u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Obj::parse_639310", 0x00639310, &H::parse_639310);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

struct H {
    // ------------------------------------------------------------------ 0x00639380
    u32 find_positive(const char* a, u32 b) {
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_table(this)->find_name_6391c0(joan::orig::arg::str(a), joan::orig::arg::i32(b), i32{0x4001}, joan::orig::arg::i32(static_cast<i32>(jo::self<jo::NamedTable>(this)->count) + 0x4000)));  // 0x006391c0
        const i16 v = static_cast<i16>(r);
        return (r & 0xffff0000u) | (v > 0 ? static_cast<u16>(v) : u16{0});
    }
};

JOAN_REPLACE("game/b108_07", "Obj::find_positive", 0x00639380, &H::find_positive);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b102_12.cpp (game/b102_12)
namespace {
namespace game_b102_12 {

using joan::hook::global;

using joan::orig::helpers::addr_of;

JOAN_COUNTER(c_pool_sweep, "Pool::sweep");
JOAN_COUNTER(c_cmp_name, "cmp_name_prio");

// ------------------------------------------------------------------ 0x006394b0
i32 JOAN_CDECL cmp_name_prio(u8* a, u8* b) {
    c_cmp_name.hit();
    const i32 r = joan::crt::stricmp(joan::orig::arg::str(jo::as_wave_res(a)->file), joan::orig::arg::str(jo::as_wave_res(b)->file));  // _stricmp
    if (r > 0) return 1;
    if (r < 0) return -1;
    if (jo::as_wave_res(a)->priority > jo::as_wave_res(b)->priority) return 1;
    if (jo::as_wave_res(b)->priority > jo::as_wave_res(a)->priority) return -1;
    return 0;
}

struct H {
    // ------------------------------------------------------------------ 0x006393e0
    std::uintptr_t pool_sweep() {
        const i32 c = global<i32>(d::g_a69f34) + 1;
        global<i32>(d::g_a69f34) = c;
        if (!(global<i32>(d::g_a69f34) > 0x190)) return static_cast<u32>(c);
        c_pool_sweep.hit();
        global<i32>(d::g_a69f34) = 0;
        std::uintptr_t eax = static_cast<u32>(c);
        i16 i = static_cast<i16>(static_cast<i32>(jo::self<jo::SortedNames>(this)->count) + 0x4000);
        while (static_cast<i32>(i) >= 0x4001) {
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->get(joan::orig::arg::i16(static_cast<u16>(i))));  // 0x004d0820
            eax = addr_of(p);
            u8* const q = jo::view<u8*>(jo::as_wave_res(p)->buffer_);
            if (q != nullptr) {
                const i32 v = jo::as_sound3dbuffer(q)->refs;
                eax = static_cast<u32>(v);
                if (v <= 0) eax = joan::orig::arg::as<u32>(joan::orig::as_wave_res(p)->release());  // 0x004d0a00
            }
            i = static_cast<i16>(i - 1);
            eax = (eax & ~std::uintptr_t{0xffff}) | static_cast<u16>(i);
        }
        return eax;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_12", name, addr, fn)
B("Pool::sweep", 0x006393e0, &H::pool_sweep);
B("cmp_name_prio", 0x006394b0, &cmp_name_prio);
#undef B

}  // namespace game_b102_12
}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

using joan::orig::helpers::at;

// A ResourceIdxL (0x47cdf0 constructs the archive): sorted_index (+0x11a, 0x26-byte name records), rec_count (+0x121).
struct alignas(8) ResReader {  // [ebp-0x24c] in 0x00639560 (0x138 bytes up to the path)
    u8 bytes[((sizeof(jo::ResourceIdxL) > 0x138 ? sizeof(jo::ResourceIdxL) : 0x138) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger ResourceIdxL
};

JOAN_COUNTER(c_res_load, "ResTable::load_639560");

struct H {
    // ------------------------------------------------------------------ 0x00639560
    u32 res_load(const char* name) {
        c_res_load.hit();
        char path[0x104];
        std::memcpy(path, str(d::s_RESOURCE), 10);  // 'RESOURCE\'
        std::memset(path + 10, 0, sizeof path - 10);
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(path)), joan::orig::arg::str(name));           // strcat
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(path)), str(d::s_RES));  // '.RES'
        const u32 ok = joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->is_file_exist(joan::orig::arg::str(path)));  // 0x00590b80
        if (ok == 0) return ok;
        // [Audio] SoundsInMemory: this archive's sounds read into memory on a thread of their own; an empty table
        // is a new mission, which drops the previous mission's archives.
        joan::sound_archives::mission_archive(path, jo::self<jo::ResTable>(this)->count == 0);
        ResReader res;
        joan::orig::as_archive(&res)->construct(path, 0u, 0u);  // 0x0047cdf0
        const i32 count = static_cast<i32>(jo::self<jo::ResTable>(this)->count) + jo::as_res_idx_l(&res)->rec_count;
        u8* const mem = joan::orig::arg::as<u8*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<u32>(static_cast<u32>(count) * sizeof(jo::WaveRes) + 4))));  // operator new
        u8* arr = nullptr;
        if (mem != nullptr) {
            at<i32>(mem, 0) = count;  // x64: the new[] element count cookie, not an object member
            joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(mem + 4)), static_cast<u32>(sizeof(jo::WaveRes)), joan::orig::arg::i32(count), fn::code_value(fn::Node_ctor_638e10), fn::code_value(fn::Object_wrap_639190));  // eh vector ctor (0x00638e10 / 0x00639190; 0x93-byte elements on x86)
            arr = mem + 4;
        }
        if (jo::view<void*>(jo::self<jo::ResTable>(this)->table) != nullptr) {
            for (i32 i = 0; i < static_cast<i32>(jo::self<jo::ResTable>(this)->count); ++i) {
                // memcpy of everything after the vtable (+4, 0x8f bytes on x86)
                std::memcpy(joan::orig::arg::mptr(&jo::as_wave_res(arr + i * static_cast<i32>(sizeof(jo::WaveRes)))->index), joan::orig::arg::ptr(&jo::as_wave_res(jo::view<u8*>(jo::self<jo::ResTable>(this)->table) + i * static_cast<i32>(sizeof(jo::WaveRes)))->index), sizeof(jo::WaveRes) - offsetof(jo::WaveRes, index));
                joan::orig::as_wave_res(jo::view<u8*>(jo::self<jo::ResTable>(this)->table) + i * static_cast<i32>(sizeof(jo::WaveRes)))->release_if();  // 0x004d0990
            }
            void* const old = jo::view<void*>(jo::self<jo::ResTable>(this)->table);
            if (old != nullptr) joan::orig::as_wave_res(old)->vector_delete(3u);  // 0x006390f0
        }
        jo::view<u8*>(jo::self<jo::ResTable>(this)->table) = arr;
        for (i32 j = jo::self<jo::ResTable>(this)->count; j < jo::as_res_idx_l(&res)->rec_count + static_cast<i32>(jo::self<jo::ResTable>(this)->count); ++j) {
            u8* const e = jo::view<u8*>(jo::self<jo::ResTable>(this)->table) + j * static_cast<i32>(sizeof(jo::WaveRes));
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_wave_res(e)->name)), joan::orig::arg::str(jo::as_res_idx_l(&res)->sorted_index[j - static_cast<i32>(jo::self<jo::ResTable>(this)->count)].name));  // strcpy
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_wave_res(e)->file)), joan::orig::arg::str(name));
            jo::as_wave_res(e)->priority = joan::crt::atoi(joan::orig::arg::str(jo::as_wave_res(e)->name));  // atoi
            jo::as_wave_res(e)->max_play = 1;
            jo::view<u32>(jo::as_wave_res(e)->volume) = 0x3f800000u;
            jo::view<u32>(jo::as_wave_res(e)->min_pitch) = 0x3f800000u;
            jo::view<u32>(jo::as_wave_res(e)->max_pitch) = 0x40000000u;
            jo::view<u32>(jo::as_wave_res(e)->min_dist) = 0x461c4000u;
            jo::view<u32>(jo::as_wave_res(e)->max_dist) = 0x466a6000u;
            jo::view<u32>(jo::as_wave_res(e)->rolloff) = 0x3f800000u;
            jo::view<u32>(jo::as_wave_res(e)->sound_time_ms) = 0u;
            jo::as_wave_res(e)->flags = static_cast<u8>(jo::as_wave_res(e)->flags & 0xfc);
            jo::as_wave_res(e)->flags = static_cast<u8>(jo::as_wave_res(e)->flags & 0xf3);
            jo::as_wave_res(e)->delay_begin = 0;
            jo::as_wave_res(e)->delay_end = 0;
            jo::as_wave_res(e)->stream = 0;
            joan::orig::as_wave_res(e)->nop_4d0970();  // 0x004d0970
        }
        jo::view<u16>(jo::self<jo::ResTable>(this)->count) = static_cast<u16>(jo::view<u16>(jo::self<jo::ResTable>(this)->count) + static_cast<u16>(jo::as_res_idx_l(&res)->rec_count));  // the original adds the low word
        joan::orig::libc::qsort(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::ResTable>(this)->table)), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::ResTable>(this)->count)), static_cast<u32>(sizeof(jo::WaveRes)), fn::code_ptr(fn::cmp_name_prio));  // qsort (0x006394b0)
        for (i16 k = 0x4001; static_cast<i32>(k) <= static_cast<i32>(jo::self<jo::ResTable>(this)->count) + 0x4000; k = static_cast<i16>(k + 1)) {
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_sound_res_table(this)->at(joan::orig::arg::i16(static_cast<i32>(k))));  // 0x004d0860
            jo::view<i16>(jo::as_wave_res(r)->index) = k;
        }
        joan::orig::as_archive(&res)->dtor();  // 0x00417a30
        return 0;  // void callee; the one caller (dlgres.cpp, 0x00639560) discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("ResTable::load_639560", 0x00639560, &H::res_load);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::DlgRes::byte_size() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_06::H::strtab_size>(this)); }  // 0x638d50
auto joan::orig::SoundResTable::clear() -> void { joan::bind::member<&::game_b85_05::H::table_clear>(this); }  // 0x639030
auto joan::orig::WaveRes::vector_delete(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b86_01::H::vector_delete>(this, a0)); }  // 0x6390f0
auto joan::orig::WaveRes::destruct_639190() -> void { joan::bind::member<&::en3d_b87_02::H::wrap_639190>(this); }  // 0x639190
auto joan::orig::Table::find_name_6391c0(const char* a0, std::int32_t a1, std::int32_t a2, std::int32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::find_name>(this, a0, a1, a2, a3)); }  // 0x6391c0
auto joan::orig::SoundResTable::find_positive(const char* a0, std::uint32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_07::H::find_positive>(this, a0, a1)); }  // 0x639380
auto joan::orig::SoundResTable::load(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_11::H::res_load>(this, a0)); }  // 0x639560
