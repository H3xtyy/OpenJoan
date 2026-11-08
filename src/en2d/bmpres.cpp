// en2d/bmpres.cpp: functions of the original d:\ww\en2d\bmpres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cmath>
#include <limits>

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

using joan::orig::helpers::at;

constexpr img::Addr kMemory(d::g_mem);
constexpr img::Addr kBmpFile(d::s_d_ww_en2d_bmpres_cpp);  // "d:\ww\en2d\bmpres.cpp"

}  // namespace

// ---------------------------------------- from modules/game_ui2.cpp (game/ui2)
//
//      - BmpRes::free 0x00620ab0 (en2d/bmpres.cpp): frees the pixel block +5. ret 0.
namespace {
namespace game_ui2 {

struct H {
    // 0x00620ab0 BmpRes::free. ret 0.
    void bmp_free() {
        if (jo::self<jo::BmpRes>(this)->entries != nullptr) {
            joan::orig::as_mem(img::obj(kMemory))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::BmpRes>(this)->entries)), joan::orig::arg::str(img::str(kBmpFile)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a19744)) + 3));  // 0x0058ed70
            jo::self<jo::BmpRes>(this)->entries = nullptr;
        }
        jo::self<jo::BmpRes>(this)->loaded = 0;
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/ui2", name, addr, fn)
U2("BmpRes::free", 0x00620ab0, &H::bmp_free);
#undef U2

}  // namespace game_ui2
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
//
//    - BmpRes::entry 0x00620b10 (i), ret 4: [+5] + (i - 1) * 0x38.
namespace {
namespace game_b103_12 {

using u32 = std::uint32_t;

struct H {
    std::uint8_t* bmp_entry(u32 i) { return reinterpret_cast<std::uint8_t*>(jo::self<jo::BmpRes>(this)->entries) + i * 0x38u - 0x38u; }  // 0x00620b10
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("BmpRes::entry", 0x00620b10, &H::bmp_entry);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_ui8.cpp (game/ui8)
//
//      - BmpRes::find 0x00620b40 (name) -> 0x38-byte record or null (CRT 0x007c07b0 compare).
//        ret 4.
//
//      - BmpRes::read 0x00620bc0 (name) -> 1 / 0: switches the archive 0x00a56b18 to
//        'INTERFAC', reads the record table into a 0x38 * count block +5, restores. ret 4.
namespace {
namespace game_ui8 {

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;

constexpr fn::Code kMemset = fn::crt_memset;
constexpr img::Addr kTimer(d::g_misc);
constexpr img::Addr kArchive(d::g_game_set);
constexpr img::Addr kInterfac(d::s_INTERFAC);     // "INTERFAC"

JOAN_COUNTER(c_bmp_find, "BmpRes::find");
JOAN_COUNTER(c_bmp_read, "BmpRes::read");

void archive_restore(std::int16_t previous) {
    joan::orig::as_game_set(img::obj(kArchive))->close_set_58b140();                                     // 0x0058b140
    joan::orig::as_game_set(img::obj(kArchive))->open_set_58b080(static_cast<std::int32_t>(previous));  // 0x0058b080
}

struct H {
    // 0x00620b40. ret 4.
    std::uint8_t* bmp_find(const char* name) {
        c_bmp_find.hit();
        for (std::int32_t i = 0; i < jo::self<jo::BmpRes>(this)->count; ++i) {
            if (joan::crt::stricmp(joan::orig::arg::str(static_cast<void*>(reinterpret_cast<std::uint8_t*>(&jo::self<jo::BmpRes>(this)->entries[i]))), joan::orig::arg::str(name)) == 0) {
                return reinterpret_cast<std::uint8_t*>(&jo::self<jo::BmpRes>(this)->entries[i]);
            }
        }
        return nullptr;
    }

    // 0x00620bc0. ret 4.
    char bmp_read(const char* name) {
        c_bmp_read.hit();
        if (static_cast<std::int32_t>(global<std::int16_t>(d::g_interfac_set_id)) == 0) {
            global<std::int16_t>(d::g_interfac_set_id) = joan::orig::arg::as<std::int16_t>(joan::orig::as_game_set(img::obj(kArchive))->find_set_58b250(joan::orig::arg::str(img::str(kInterfac))));  // 0x0058b250
        }
        const std::int16_t previous = global<std::int16_t>(d::g_a56b19);
        if (static_cast<std::int32_t>(global<std::int16_t>(d::g_interfac_set_id)) != static_cast<std::int32_t>(previous)) {
            joan::orig::as_game_set(img::obj(kArchive))->close_set_58b140();                                                         // 0x0058b140
            joan::orig::as_game_set(img::obj(kArchive))->open_set_58b080(static_cast<std::int32_t>(global<std::int16_t>(d::g_interfac_set_id)));  // 0x0058b080
        }
        if (joan::orig::as_res_idx(obj(d::g_a56b21))->read(joan::orig::arg::str(name)) == nullptr) {  // a pointer result: test it whole (x64)  // 0x0059c950
            if (static_cast<std::int32_t>(global<std::int16_t>(d::g_interfac_set_id)) != static_cast<std::int32_t>(previous)) archive_restore(previous);
            return 0;
        }
        void* const res = joan::orig::arg::as<void*>(joan::orig::as_game_set(img::obj(kArchive))->open_db_58b180(joan::orig::arg::str(name)));  // 0x0058b180
        jo::view<std::uint32_t>(jo::self<jo::BmpRes>(this)->count) = joan::orig::arg::as<std::uint32_t>(joan::orig::as_database(res)->rec_count());  // 0x00415280
        jo::view<void*>(jo::self<jo::BmpRes>(this)->entries) = joan::orig::as_mem(img::obj(kMemory))->add(joan::orig::arg::u32(jo::view<std::uint32_t>(jo::self<jo::BmpRes>(this)->count) * static_cast<std::uint32_t>(sizeof(jo::BmpEntry))), joan::orig::arg::str(img::str(kBmpFile)), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a19748)) + 0x21));  // 0x0058e7b0
        // memset(void*, int, size_t) (lib/crt/string.cpp): the count is a size_t argument, as the callee reads it on x64
        cdecl_call<void*>(kMemset, jo::view<void*>(jo::self<jo::BmpRes>(this)->entries), 0, static_cast<std::size_t>(jo::view<std::uint32_t>(jo::self<jo::BmpRes>(this)->count) * static_cast<std::uint32_t>(sizeof(jo::BmpEntry))));
        for (std::int32_t i = 0; i < jo::self<jo::BmpRes>(this)->count; ++i) {
            std::uint8_t* const rec = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_database(res)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            std::uint8_t* const dst = reinterpret_cast<std::uint8_t*>(&jo::self<jo::BmpRes>(this)->entries[i]);
            joan::orig::as_misc(img::obj(kTimer))->copy_field_590210(joan::orig::arg::str(static_cast<void*>(dst)), joan::orig::arg::str(static_cast<void*>(rec)), 8);               // 0x00590210
            joan::orig::as_misc(img::obj(kTimer))->copy_field_590210(joan::orig::arg::str(static_cast<void*>(dst + 9)), joan::orig::arg::str(static_cast<void*>(rec + 8)), 0x20);
            jo::as_bmp_entry(dst)->x0 = joan::orig::arg::as<std::uint16_t>(joan::orig::as_misc(img::obj(kTimer))->atoi_field_590270(joan::orig::arg::str(static_cast<void*>(rec + 0x28)), 4u));  // 0x00590270
            jo::as_bmp_entry(dst)->y0 = joan::orig::arg::as<std::uint16_t>(joan::orig::as_misc(img::obj(kTimer))->atoi_field_590270(joan::orig::arg::str(static_cast<void*>(rec + 0x2c)), 4u));
            jo::as_bmp_entry(dst)->x1 = joan::orig::arg::as<std::uint16_t>(joan::orig::as_misc(img::obj(kTimer))->atoi_field_590270(joan::orig::arg::str(static_cast<void*>(rec + 0x30)), 4u));
            jo::as_bmp_entry(dst)->y1 = joan::orig::arg::as<std::uint16_t>(joan::orig::as_misc(img::obj(kTimer))->atoi_field_590270(joan::orig::arg::str(static_cast<void*>(rec + 0x34)), 4u));
            jo::as_bmp_entry(dst)->u16_32 = joan::orig::arg::as<std::uint16_t>(joan::orig::as_misc(img::obj(kTimer))->atoi_field_590270(joan::orig::arg::str(static_cast<void*>(rec + 0x38)), 1u));
            jo::as_bmp_entry(dst)->width = joan::orig::arg::as<std::uint16_t>(joan::orig::as_misc(img::obj(kTimer))->atoi_field_590270(joan::orig::arg::str(static_cast<void*>(rec + 0x39)), 4u));
            jo::as_bmp_entry(dst)->height = joan::orig::arg::as<std::uint16_t>(joan::orig::as_misc(img::obj(kTimer))->atoi_field_590270(joan::orig::arg::str(static_cast<void*>(rec + 0x3d)), 4u));
        }
        joan::orig::as_database(res)->deinit();  // 0x00580da0
        if (static_cast<std::int32_t>(global<std::int16_t>(d::g_interfac_set_id)) != static_cast<std::int32_t>(previous)) archive_restore(previous);
        return 1;
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("BmpRes::find", 0x00620b40, &H::bmp_find);
U8("BmpRes::read", 0x00620bc0, &H::bmp_read);
#undef U8

}  // namespace game_ui8
}  // namespace

// ---------------------------------------- from modules/game_ui2.cpp (game/ui2)
//
//      - BmpRes::load 0x00620a50 (en2d/bmpres.cpp, name) -> loaded: frees, loads (0x00620bc0),
//        sets +0 loaded. ret 4.
namespace {
namespace game_ui2_2 {

using joan::call::thiscall;
using joan::orig::helpers::at;

struct H {
    // 0x00620a50 BmpRes::load. ret 4.
    char bmp_load(const char* name) {
        if (static_cast<std::int8_t>(jo::self<jo::BmpRes>(this)->loaded) != 0) joan::orig::as_bmp_res(this)->free();  // 0x00620ab0
        if (static_cast<std::int8_t>(joan::orig::arg::as<char>(joan::orig::as_bmp_res(this)->read(joan::orig::arg::str(name)))) != 0) {         // 0x00620bc0
            jo::self<jo::BmpRes>(this)->loaded = 1;
            return 1;
        }
        return 0;
    }
};

#define U2(name, addr, fn) JOAN_REPLACE("game/ui2", name, addr, fn)
U2("BmpRes::load", 0x00620a50, &H::bmp_load);
#undef U2

}  // namespace game_ui2_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::BmpRes::load(const char* a0) -> char { return joan::bind::conv<char>(joan::bind::member<&::game_ui2_2::H::bmp_load>(this, a0)); }  // 0x620a50
auto joan::orig::BmpRes::free() -> void { joan::bind::member<&::game_ui2::H::bmp_free>(this); }  // 0x620ab0
auto joan::orig::BmpRes::find(const char* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_ui8::H::bmp_find>(this, a0)); }  // 0x620b40
auto joan::orig::BmpRes::read(const char* a0) -> char { return joan::bind::conv<char>(joan::bind::member<&::game_ui8::H::bmp_read>(this, a0)); }  // 0x620bc0
