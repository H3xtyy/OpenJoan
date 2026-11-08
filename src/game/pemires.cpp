// game/pemires.cpp: functions of the original d:\ww\pemires.cpp (re/attribution.csv).
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
using u32 = std::uint32_t;

void* const kMem = obj(d::g_mem);

using joan::orig::helpers::line_of;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
//
//      0x004c19e0 PemiRes::clear             (thiscall, ret 0) - deletes the +0x24 objects of the
//                 0x28-byte entries, frees the array (pemires.cpp), clears count / flag.
namespace {
namespace game_b97_03 {

using i8 = std::int8_t;

JOAN_COUNTER(c_pemi_clear, "PemiRes::clear");

struct H {
    // ------------------------------------------------------------------------ 0x004c19e0
    u32 pemi_clear() {
        c_pemi_clear.hit();
        if (jo::view<i8>(jo::self<jo::PemiRes>(this)->loaded) == 0) return joan::call::register_low_bits(this);
        for (i32 i = 1; i <= jo::self<jo::PemiRes>(this)->count; ++i) {
            jo::PemiScript* const e = jo::self<jo::PemiRes>(this)->scripts + (i - 1);
            if (e->effect != nullptr) {
                void* const p = jo::view<void*>(e->effect);
                if (p != nullptr) joan::orig::as_particle_system(p)->destroy(1u);  // 0x004c1af0 deleting dtor
                e->effect = nullptr;
            }
        }
        jo::self<jo::PemiRes>(this)->count = 0;
        u32 r = joan::call::register_low_bits(this);
        if (jo::self<jo::PemiRes>(this)->scripts != nullptr) {
            r = thiscall<u32>(fn::del, kMem, jo::view<void*>(jo::self<jo::PemiRes>(this)->scripts), str(d::s_d_ww_pemires_cpp),  // 0x0058ed70 free
                              line_of(d::g_a01f14, 0x16));                       // "d:\ww\pemires.cpp"
            jo::self<jo::PemiRes>(this)->scripts = nullptr;
        }
        jo::self<jo::PemiRes>(this)->loaded = 0;
        return r;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("PemiRes::clear", 0x004c19e0, &H::pemi_clear);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
//
//      0x004c1af0 PemiRes::scalar_dtor     (thiscall(flags), ret 4) - 0x00675930, delete if flags & 1.
//
//  - 0x004c1230 / 0x004c1af0 / 0x004cc7c0 leave this in eax; returned as such.
namespace {
namespace game_b96_03 {

using joan::orig::helpers::game_delete;                                   // operator delete

struct H {
    // ------------------------------------------------------------------------ 0x004c1af0
    void* scalar_dtor(u32 flags) {
        joan::orig::as_zones(this)->dtor();  // 0x00675930
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("PemiRes::scalar_dtor", 0x004c1af0, &H::scalar_dtor);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
//
//      0x004c1b40 PScripts::scan           (thiscall, ret 0) - two _findfirst/_findnext passes
//                 over "SCRIPT\PSCRIPT\*.txt": count, alloc count*0x28, fill names.
//
//  - 0x004c1b40: the max-size local (ebp-0x130) is dead. strcpy of the file name into the
//    0x28-byte entry is kept as is (same CRT call, same overflow behaviour as the original).
namespace {
namespace game_b95_03 {

using u16 = std::uint16_t;

JOAN_COUNTER(c_scan, "PScripts::scan");

using joan::orig::helpers::memset_;  // memset
using joan::orig::helpers::strcpy_;  // strcpy

struct FindData {  // struct _finddata_t (MSVC 6), 0x118 bytes
    u32 attrib;
    i32 time_create;
    i32 time_access;
    i32 time_write;
    u32 size;
    char name[260];
};
static_assert(sizeof(FindData) == 0x118, "finddata size");
i32 findfirst_(const char* spec, FindData* fd) { return joan::orig::arg::as<i32>(joan::orig::libc::_findfirst(joan::orig::arg::str(spec), joan::orig::arg::mptr(fd))); }  // _findfirst
i32 findnext_(i32 h, FindData* fd) { return joan::orig::arg::as<i32>(joan::orig::libc::_findnext(joan::orig::arg::i32(h), joan::orig::arg::mptr(fd))); }                // _findnext
void findclose_(i32 h) { joan::orig::libc::_findclose(joan::orig::arg::i32(h)); }                                        // _findclose

struct H {
    // ------------------------------------------------------------------------ 0x004c1b40
    void scan() {
        c_scan.hit();
        const char* const spec = str(d::s_SCRIPT_PSCRIPT_txt);  // "SCRIPT\PSCRIPT\*.txt"
        FindData fd;
        i32 h = findfirst_(spec, &fd);
        i32 r = 0;
        i32 count = 0;
        while (r != -1 && h != -1) {
            ++count;
            r = findnext_(h, &fd);
        }
        if (h != -1) findclose_(h);
        if (count > 0) {
            const i32 line = line_of(d::g_a01f18, 0x1e);
            jo::view<void*>(jo::self<jo::PemiRes>(this)->scripts) = joan::orig::as_mem(kMem)->add(joan::orig::arg::u32(static_cast<u32>(count) * static_cast<u32>(sizeof(jo::PemiScript))), str(d::s_d_ww_pemires_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0 "d:\ww\pemires.cpp"
        }
        h = findfirst_(spec, &fd);
        r = 0;
        while (r != -1 && h != -1) {
            jo::view<u32>(jo::self<jo::PemiRes>(this)->count) += 1;
            jo::PemiScript* const entry = jo::self<jo::PemiRes>(this)->scripts + (jo::view<u32>(jo::self<jo::PemiRes>(this)->count) - 1u);
            memset_(entry, 0, static_cast<u32>(sizeof(jo::PemiScript)));  // 0x28 on x86
            entry->id = static_cast<u16>(jo::view<u32>(jo::self<jo::PemiRes>(this)->count));
            strcpy_(entry->name, fd.name);
            r = findnext_(h, &fd);
        }
        if (h != -1) findclose_(h);
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("PScripts::scan", 0x004c1b40, &H::scan);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b84_03.cpp (game/b84_03)
//
//    - PemiList::load 0x004c1d80 (index) ret 4 (pemires.cpp): SCRIPT\PSCRIPT\<name>, 64 KB stack
//      buffer or heap buffer, 0x00677390 parse.
//
//  * 0x004c1d80: SEH frame guards operator new + ctor and the File local; normal path reproduced
//    (0x005872b0 ~File on exit). 0x00677390 catches its own exceptions.
namespace {
namespace game_b84_03 {

struct alignas(4) Path {  // 0x005a1170 string, no destructor
    std::uint8_t b[0x194];
};
struct alignas(4) File {  // 0x004179c0 / 0x005872b0 (frame slot 0x110 bytes on x86; jo::File holds a handle pointer)
    std::uint8_t b[(sizeof(jo::File) + 3u) & ~std::size_t{3}];
};

JOAN_COUNTER(c_pemi_load, "PemiList::load");

constexpr img::Addr kMemory = d::g_mem;

using joan::orig::helpers::game_new;  // operator new
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90
using joan::orig::helpers::line_base;
void mem_free(void* p, img::Addr file, std::int32_t line) {
    joan::orig::as_mem(obj(kMemory))->del(joan::orig::arg::mptr(p), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058ed70
}

struct H {
    // ------------------------------------------------------------------ 0x004c1d80
    std::int32_t pemi_load(std::int32_t index) {
        c_pemi_load.hit();
        jo::PemiScript* const e = jo::self<jo::PemiRes>(this)->scripts + (index - 1);
        if (e->effect != nullptr) return 1;
        void* const mem = game_new(static_cast<u32>(sizeof(jo::EffectState)));  // 0x78 on x86
        void* const o = mem != nullptr ? joan::orig::as_effect_state(mem)->construct() : nullptr;  // 0x006758f0
        jo::view<void*>(e->effect) = o;
        joan::orig::as_effect_state(jo::view<void*>(e->effect))->init();  // 0x006759a0
        Path p;
        joan::orig::as_string(&p)->construct();                               // 0x005a1170
        joan::orig::as_string(&p)->assign(str(d::s_SCRIPT_PSCRIPT));               // 0x005a1440 "SCRIPT\PSCRIPT\"
        joan::orig::as_string(&p)->append(joan::orig::arg::str(e->name));  // 0x005a1550 append
        File file;
        joan::orig::as_file(&file)->construct();                            // 0x004179c0
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(c_str(p)), 1, static_cast<char>(0));  // 0x00586da0 open
        alignas(4) char local[0x10000];
        char* buf = nullptr;
        auto size = [&]() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_file(&file)->file_size()); };  // 0x00587f30
        if (size() + 1u > 0x10000u) {
            buf = joan::orig::arg::as<char*>(joan::orig::as_mem(obj(kMemory))->add(joan::orig::arg::u32(size() + 1u), str(d::s_d_ww_pemires_cpp), joan::orig::arg::i32(line_base(d::g_a01f1c) + 0x1d)));  // 0x0058e7b0
        } else {
            buf = local;
        }
        joan::orig::as_file(&file)->file_read(static_cast<void*>(buf), joan::orig::arg::u32(size()));                         // 0x005873a0 read
        buf[size()] = 0;
        const std::uint8_t ok = joan::orig::arg::as<std::uint8_t>(joan::orig::as_effect_state(jo::view<void*>(e->effect))->compile(joan::orig::arg::str(static_cast<const char*>(buf))));  // 0x00677390
        joan::orig::as_file(&file)->file_close();  // 0x00587260 close
        if (buf != nullptr) {
            if (buf != local) mem_free(buf, d::s_d_ww_pemires_cpp, line_base(d::g_a01f1c) + 0x31);
            buf = nullptr;
        }
        const std::int32_t result = ok != 0 ? 1 : 0;
        joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~File
        return result;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_03", name, addr, fn)
B("ParticleEmitterRes::deinit", 0x004c1d80, &H::pemi_load);
#undef B

}  // namespace game_b84_03
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
//
//      0x004c1970 Obj::forward_4c19e0          (thiscall, ret 0) - returns callee eax.
namespace {
namespace game_b98_03 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------------ 0x004c1970
    u32 forward_4c19e0() { return joan::orig::arg::as<u32>(joan::orig::as_pemi_res(this)->clear()); }  // 0x004c19e0
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("Obj::forward_4c19e0", 0x004c1970, &H::forward_4c19e0);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b100_05.cpp (game/b100_05)
//
//    - Obj::init_4c19a0 0x004c19a0: 0x004c19e0, 0x004c1b40, byte +8 = 1, returns this.
namespace {
namespace game_b100_05 {

using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x004c19a0
    H* init_4c19a0() {
        joan::orig::as_pemi_res(this)->clear();  // 0x004c19e0
        joan::orig::as_pemi_res(this)->scan();  // 0x004c1b40
        jo::self<jo::PemiRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_05", name, addr, fn)
B("Obj::init_4c19a0", 0x004c19a0, &H::init_4c19a0);
#undef B

}  // namespace game_b100_05
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
//
//    - NameList::find 0x004c2060 (name, select) ret 8.
namespace {
namespace game_b83_07 {

using joan::call::thiscall;
using joan::call::cdecl_call;
JOAN_COUNTER(c_name_find, "NameList::find");

using joan::orig::helpers::stricmp_;

struct H {
    // ------------------------------------------------------------------ 0x004c2060
    std::int32_t name_find(const char* name, std::int8_t select) {
        c_name_find.hit();
        for (std::int32_t i = 0; i < jo::self<jo::PemiRes>(this)->count; ++i) {
            if (stricmp_(jo::self<jo::PemiRes>(this)->scripts[i].name, name) != 0) continue;
            if (select != 0) joan::orig::as_pemi_res(this)->load_script(joan::orig::arg::i32(i + 1));  // 0x004c1d80
            return i + 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("NameList::find", 0x004c2060, &H::name_find);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::PemiRes::clear() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_03::H::pemi_clear>(this)); }  // 0x4c19e0
auto joan::orig::ParticleSystem::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_03::H::scalar_dtor>(this, a0)); }  // 0x4c1af0
auto joan::orig::PemiRes::scan() -> void { joan::bind::member<&::game_b95_03::H::scan>(this); }  // 0x4c1b40
auto joan::orig::PemiRes::load_script(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_03::H::pemi_load>(this, a0)); }  // 0x4c1d80
auto joan::orig::NameList::find_4c2060(const char* a0, std::int8_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::name_find>(this, a0, a1)); }  // 0x4c2060
