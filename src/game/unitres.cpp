// game/unitres.cpp: functions of the original d:\ww\unitres.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>

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
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wvolatile"  // GCC 13: the original's volatile counter in a for header
#endif

namespace {

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b102_05.cpp (game/b102_05)
//
//    - UnitRes::release_524d00 0x00524d00 (ret 0, unitres.cpp): when byte +6 == 1:
//      0x00524dc0, free +0 (0x58ed70 on 0xa48838, line [0xa07bc0] + 7), byte +6 = 0.
namespace {
namespace game_b102_05 {

struct H {

    // ------------------------------------------------------------------ 0x00524d00
    void release_524d00() {
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::UnitRes>(this)->loaded)) != 1) return;
        joan::orig::as_unit_types(this)->reset_all();  // 0x00524dc0
        if (jo::self<jo::UnitRes>(this)->types != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a07bc0)) + 7;
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::UnitRes>(this)->types)), str(d::s_d_ww_unitres_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::UnitRes>(this)->types = nullptr;
            jo::view<u16>(jo::self<jo::UnitRes>(this)->count) = 0;
        }
        jo::self<jo::UnitRes>(this)->loaded = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_05", name, addr, fn)
B("UnitRes::release_524d00", 0x00524d00, &H::release_524d00);
#undef B

}  // namespace game_b102_05
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
//
//    - UnitRes::load_and_mark_524d90 0x00524d90 (ret 0): 0x00524e30, +6 = 1; returns this.
namespace {
namespace game_b108_04 {

struct H {
    // ------------------------------------------------------------------ 0x00524d90
    void* load_and_mark_524d90() {
        joan::orig::as_unit_types(this)->load();  // 0x00524e30
        jo::self<jo::UnitRes>(this)->loaded = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("UnitRes::load_and_mark_524d90", 0x00524d90, &H::load_and_mark_524d90);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b101_07.cpp (game/b101_07)
//
//    - UnitRes::reset_all 0x00524dc0 (ret 0): 0x5273b0 on 0x42d0a0(i), i = 1..word +4.
//
//  - Return values: callers of 0x4ec3d0, 0x50aca0, 0x50c680 (0x43ab80 overwrites ax and
//    returns a short), 0x50c8a0, 0x518890, 0x524dc0 ignore eax: void here. 0x4ea9c0,
//    0x510120, 0x5101c0 return the callee's eax as the originals do.
namespace {
namespace game_b101_07 {

JOAN_COUNTER(c_unitres_reset, "UnitRes::reset_all");

struct H {

    // ------------------------------------------------------------------ 0x00524dc0
    void unitres_reset_all() {
        c_unitres_reset.hit();
        for (i16 i = 1; static_cast<i32>(i) <= static_cast<i32>(jo::self<jo::UnitRes>(this)->count); i = static_cast<i16>(i + 1)) {
            void* const r = joan::orig::arg::as<void*>(joan::orig::as_records_203(this)->get(joan::orig::arg::i16(static_cast<i32>(i))));  // 0x0042d0a0
            joan::orig::as_unit_type(r)->free_buffers();                                          // 0x005273b0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_07", name, addr, fn)
B("UnitRes::reset_all", 0x00524dc0, &H::unitres_reset_all);
#undef B

}  // namespace game_b101_07
}  // namespace

// ---------------------------------------- from modules/game_b107_10.cpp (game/b107_10)
//
//    - UnitRes::load 0x00524e30 (ret 0, unitres.cpp): stack resource object (ctor 0x004179f0)
//      loads "UNIT\\" "unit.res" (0x005a1170 / 0x005a1440 / 0x005a1550 / 0x00417d90 /
//      0x0059b920(path, 1, 0)); word +4 = word res+0x121; +0 = alloc(+4 * 0x203, unitres.cpp,
//      [0xa07bc4] + 0x10) (0x0058e7b0 on 0xa48838), memset 0; for i = 1..res+0x121: record word
//      = i, stack item 0x00589780(0x0059bf70(i), 0x0059bdd0(i)), 0x00525080(item, rec, 0),
//      strncpy-like 0x007b1db0(rec + 4, 0x0059c3b0(i), 0x20), item dtor 0x00589890; finally
//      res dtor 0x00417a30 (its eax returned). SEH frame: normal path only.
//
//  - 0x00524e30: stack slots sized as in the original frame (res 0x138, path 0x194, item 0x19c).
namespace {
namespace game_b107_10 {

JOAN_COUNTER(c_unitres_load, "UnitRes::load_524e30");

struct H {
    // ------------------------------------------------------------------ 0x00524e30
    std::uintptr_t unitres_load() {
        c_unitres_load.hit();
        alignas(alignof(void*)) char res[((sizeof(jo::ResourceIdxL) > 0x138 ? sizeof(jo::ResourceIdxL) : 0x138) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger ResourceIdxL
        alignas(4) char path[0x194];
        alignas(alignof(void*)) char item[((sizeof(jo::FileTxt) > 0x19c ? sizeof(jo::FileTxt) : 0x19c) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // x64: the larger FileTxt
        joan::orig::as_res_idx(res)->construct();                           // 0x004179f0
        joan::orig::as_string(path)->construct();                          // 0x005a1170
        joan::orig::as_string(path)->assign(str(d::s_UNIT_9d5748));         // 0x005a1440 "UNIT\\"
        joan::orig::as_string(path)->append(str(d::s_unit_res));         // 0x005a1550 "unit.res"
        void* const name = joan::orig::arg::as<void*>(joan::orig::as_string(path)->c_str());       // 0x00417d90
        joan::orig::as_res_idx_l(res)->init(joan::orig::arg::str(name), static_cast<char>(i32{1}), static_cast<char>(i32{0}));  // 0x0059b920
        jo::view<u16>(jo::self<jo::UnitRes>(this)->count) = *reinterpret_cast<const u16*>(&jo::as_res_idx_l(res)->rec_count);  // low word
        const i32 line = static_cast<i32>(global<i16>(d::g_a07bc4)) + 0x10;
        const i32 bytes = static_cast<i32>(jo::self<jo::UnitRes>(this)->count) * static_cast<i32>(sizeof(jo::UnitType));
        jo::self<jo::UnitRes>(this)->types = joan::orig::arg::as<jo::UnitType*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(bytes), str(d::s_d_ww_unitres_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        std::memset(joan::orig::arg::mptr(jo::self<jo::UnitRes>(this)->types), i32{0}, joan::orig::arg::u32(static_cast<i32>(jo::self<jo::UnitRes>(this)->count) * static_cast<i32>(sizeof(jo::UnitType))));  // memset
        for (i32 i = 1; i <= jo::as_res_idx_l(res)->rec_count; ++i) {
            jo::UnitType* const rec = jo::self<jo::UnitRes>(this)->types + (i - 1);
            rec->type_id = static_cast<u16>(i);
            const u32 a = joan::orig::arg::as<u32>(joan::orig::as_res_idx_l(res)->get_data_size(joan::orig::arg::i32(i)));  // 0x0059bdd0
            const char* const b = joan::orig::arg::as<const char*>(joan::orig::as_res_idx_l(res)->read_rec(joan::orig::arg::i32(i)));  // 0x0059bf70
            joan::orig::as_file_txt(item)->construct_mem(joan::orig::arg::str(b), joan::orig::arg::i32(a));  // 0x00589780
            joan::orig::as_unit_types(this)->parse(joan::orig::arg::mptr(static_cast<void*>(item)), joan::orig::arg::mptr(rec), joan::orig::arg::u32(i32{0}));  // 0x00525080
            const void* const s = joan::orig::arg::as<const void*>(joan::orig::as_res_idx_l(res)->get_index_rec(joan::orig::arg::i32(i)));  // 0x0059c3b0
            // strncpy without a terminator into the fixed-size field, as the original does
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
            std::strncpy(static_cast<char*>(joan::orig::arg::mptr(rec->name)), joan::orig::arg::str(s), joan::orig::arg::u32(i32{0x20}));
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
            joan::orig::as_file_txt(item)->destruct();  // 0x00589890
        }
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_archive(res)->dtor());  // 0x00417a30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_10", name, addr, fn)
B("UnitRes::load_524e30", 0x00524e30, &H::unitres_load);
#undef B

}  // namespace game_b107_10
}  // namespace

// ---------------------------------------- from modules/game_b106_04.cpp (game/b106_04)
//
//      0x00525080 UnitRes::parse_525080(reader, out, unused)  thiscall, ret 0xc (called from
//                 0x00524fa2, eax ignored). Long key/value parse of one unit type record.
//
//  - 0x00525080: the local frame of the original is reproduced for the read_line buffers
//    (0x00589f10 writes its NUL at buf[n]): the 0x100-byte line buffer at ebp-0x110 is
//    directly followed by the loop counter at ebp-0x10, so a 256-char line clears the low
//    byte of the counter exactly as in the original (Frame525 below, counter read volatile).
//    The 8 / 0x28 / 0x20-byte buffers spill their NUL into unused padding (kept). The first
//    atoi result (ebp-0xc) and the fst to ebp-0x3c4 are dead stores (calls kept).
//    Tokens from 0x00589c70 are at most 128 chars, so the '.'-split copy into the 256-byte
//    buffer at ebp-0x270 cannot overflow. Review fix: after the optional "EArmorCode" block
//    the 0x171 armor value is read with ONE token call (not key + value like 0x173..0x177).
namespace {
namespace game_b106_04 {

// strncpy without a terminator into the fixed-size field, as the original does
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
char* crt_strncpy(char* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
char* crt_strchr(const char* s, i32 c) { return joan::orig::arg::as<char*>(joan::orig::libc::strchr(joan::orig::arg::str(s), joan::orig::arg::i32(c))); }
using joan::orig::helpers::crt_atoi;
double crt_atof(const char* s) { return joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s))); }  // st0 (qword)
i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_unitres_parse, "UnitRes::parse_525080");

struct H {
    // -----------------------------------------------------------------------------------
    // 0x00525080
    struct Frame525 {         // ebp-0x16c .. ebp-0xc of the original
        char b16c[0x20];      // ebp-0x16c
        u8 pad14c[4];         // ebp-0x14c (NUL of the 0x20 read)
        char b148[0x28];      // ebp-0x148
        u8 pad120[4];         // ebp-0x120 (NUL of the 0x28 read)
        char b11c[8];         // ebp-0x11c
        u8 pad114[4];         // ebp-0x114 (NUL of the 8 read)
        char b110[0x100];     // ebp-0x110
        i32 i;                // ebp-0x10 (NUL of the 0x100 read lands in its low byte)
    };
    static_assert(sizeof(Frame525) == 0x160 && offsetof(Frame525, i) == 0x15c, "frame");

    void unitres_parse(void* rd, u8* out, u32 /*unused*/) {
        c_unitres_parse.hit();
        void* const self = this;
        jo::UnitType* const t = jo::as_unit_type(out);
        auto T = [rd]() { return joan::orig::as_file_txt(rd)->get_token(1); };  // 0x00589c70
        auto read_line = [rd](char* buf, i32 n) { joan::orig::as_file_txt(rd)->read_line(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::i32(n)); };  // 0x00589f10
        auto unit_id = [](const char* s) { return joan::orig::arg::as<u32>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->index_of(joan::orig::arg::str(s))); };  // 0x00418430
        auto res = [](const char* dir, const char* name, u32 n) {
            return joan::orig::arg::as<u32>(joan::orig::as_model_res(obj(d::g_model_res_mgr))->add(joan::orig::arg::str(dir), joan::orig::arg::str(name), joan::orig::arg::i32(n)));  // 0x004b18d0
        };
        auto armor = [](const char* s) { return joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_item_res))->find_4a36c0(joan::orig::arg::str(s))); };  // 0x004a36c0
        Frame525 fr;
        char* const frp = reinterpret_cast<char*>(&fr);
        volatile i32* const counter = reinterpret_cast<volatile i32*>(frp + offsetof(Frame525, i));
        char* v8 = nullptr;

        v8 = T();
        v8 = T();
        crt_atoi(v8);  // dead store to ebp-0xc
        v8 = T();
        t->unit_class = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_unit_types(self)->class_of_name(joan::orig::arg::str(T()))));  // 0x00527220
        v8 = T();
        crt_strncpy(t->name, T(), 0x20);
        v8 = T();
        v8 = T();
        crt_strncpy(t->anim_name, v8, 0x20);
        v8 = T();
        jo::view<u32>(t->model_flags[0]) = unit_id(T());
        jo::view<u32>(t->model_ids[0]) = res(str(d::s_UNIT_9d5748), t->anim_name, jo::view<u32>(t->model_flags[0]));  // "UNIT\\"
        v8 = T();
        v8 = T();
        crt_strncpy(t->anim_name_b, v8, 0x20);
        v8 = T();
        if (crt_stricmp(t->anim_name_b, str(d::s_none)) != 0)  // "none"
            t->anim_id_b = unit_id(T());
        else
            T();
        if (crt_stricmp(t->anim_name_b, str(d::s_none)) != 0)
            jo::view<u32>(t->model_b) = res(str(d::s_UNIT_9d5748), t->anim_name_b, t->anim_id_b);
        else
            jo::view<u32>(t->model_b) = 0;
        v8 = T();
        v8 = T();
        if (crt_stricmp(v8, str(d::s_none)) != 0)
            jo::view<u32>(t->model_a) = res(str(d::s_MODEL_9d3750), v8, 0);  // "MODEL\\"
        else
            jo::view<u32>(t->model_a) = 0;
        v8 = T();
        v8 = T();
        t->extra_model_count = crt_atoi(v8);
        for (*counter = 1; *counter <= t->extra_model_count; *counter = *counter + 1) {
            v8 = T();
            read_line(frp + offsetof(Frame525, b110), 0x100);
            v8 = T();
            const u32 id = unit_id(T());
            jo::view<u32>(t->model_flags[*counter]) = id;
            const u32 r = res(str(d::s_UNIT_9d5748), frp + offsetof(Frame525, b110), jo::view<u32>(t->model_flags[*counter]));
            jo::view<u32>(t->model_ids[*counter]) = r;
        }
        v8 = T();
        read_line(frp + offsetof(Frame525, b11c), 8);
        crt_strncpy(t->portrait, frp + offsetof(Frame525, b11c), 8);
        v8 = T();
        read_line(frp + offsetof(Frame525, b148), 0x28);
        crt_strncpy(t->title_b4, frp + offsetof(Frame525, b148), 0x28);
        v8 = T();
        read_line(frp + offsetof(Frame525, b16c), 0x20);
        crt_strncpy(t->text_de, frp + offsetof(Frame525, b16c), 0x20);
        v8 = T();
        v8 = T();
        t->height = static_cast<i16>(crt_atoi(v8));
        v8 = T();
        v8 = T();
        if (crt_stricmp(v8, str(d::s_FAT)) == 0)  // "FAT"
            t->body_type = 0;
        else if (crt_stricmp(v8, str(d::s_NORMAL)) == 0)  // "NORMAL"
            t->body_type = 1;
        else if (crt_stricmp(v8, str(d::s_SHORT)) == 0)  // "SHORT"
            t->body_type = 2;
        v8 = T();
        v8 = T();
        {
            struct Kind {
                img::Addr name;
                i16 value;
            };
            static const Kind kKinds[] = {
                {d::s_NORMAL, 0x0},  // NORMAL
                {d::s_SPECIAL, 0xe},  // SPECIAL
                {d::s_MAIN, 0x1},  // MAIN
                {d::s_HORSE, 0x2},  // HORSE
                {d::s_ELEPHANT, 0x3},  // ELEPHANT
                {d::s_CANNON, 0x4},  // CANNON
                {d::s_AUTOBOW, 0xc},  // AUTOBOW
                {d::s_FAUCONNEAU, 0x5},  // FAUCONNEAU
                {d::s_CROSSBOW, 0x6},  // CROSSBOW
                {d::s_ANIMAL, 0x7},  // ANIMAL
                {d::s_SEIGE_TOWER, 0x8},  // SEIGE_TOWER
                {d::s_BOMBARD, 0x9},  // BOMBARD
                {d::s_TREBUCHET, 0xa},  // TREBUCHET
                {d::s_BATTERING_RAM, 0xb},  // BATTERING_RAM
                {d::s_CART, 0xd},  // CART
            };
            for (const Kind& k : kKinds) {
                if (crt_stricmp(v8, str(k.name)) == 0) {
                    t->kind = k.value;
                    break;
                }
            }
        }
        v8 = T();
        v8 = T();
        crt_strncpy(t->str_10d, v8, 8);
        v8 = T();
        v8 = T();
        jo::view<u8>(t->gender) = static_cast<u8>(*v8);
        v8 = T();
        v8 = T();
        while (crt_stricmp(v8, str(d::s_TRIGGER_END)) != 0) {  // "TRIGGER_END"
            struct Flag {
                img::Addr name;
                u32 bit;
            };
            static const Flag kFlags[] = {
                {d::s_TRIGGER_ATTACK, 0x0001},  // TRIGGER_ATTACK
                {d::s_TRIGGER_DOWN_ATTACK, 0x0400},  // TRIGGER_DOWN_ATTACK
                {d::s_TRIGGER_ARROW_ATTACK, 0x0002},  // TRIGGER_ARROW_ATTACK
                {d::s_TRIGGER_STONE_ATTACK, 0x0040},  // TRIGGER_STONE_ATTACK
                {d::s_TRIGGER_EXPLOSIVE_ARROW_ATTA, 0x2000},  // TRIGGER_EXPLOSIVE_ARROW_ATTACK
                {d::s_TRIGGER_EXPLOSIVE_KEG_ATTACK, 0x4000},  // TRIGGER_EXPLOSIVE_KEG_ATTACK
                {d::s_TRIGGER_THROW, 0x0010},  // TRIGGER_THROW
                {d::s_TRIGGER_INTERACT, 0x0200},  // TRIGGER_INTERACT
            };
            for (const Flag& f : kFlags)
                if (crt_stricmp(v8, str(f.name)) == 0) t->trigger_flags = t->trigger_flags | f.bit;
            v8 = T();
        }
        v8 = T();
        v8 = T();
        if (crt_stricmp(v8, str(d::s_NONE_9d4350)) != 0) {  // "NONE"
            char* const dot = crt_strchr(v8, 0x2e);
            if (dot != nullptr) {
                char name[0x100];  // ebp-0x270 (token <= 128 chars)
                crt_strncpy(name, v8, static_cast<u32>(dot - v8));
                name[dot - v8] = 0;
                jo::view<u32>(t->script_id) = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_uscr_res))->find_5464a0(joan::orig::arg::str(name)));  // 0x005464a0
            } else {
                jo::view<u32>(t->script_id) = 0;
            }
        } else {
            jo::view<u32>(t->script_id) = 0;
        }
        v8 = T();
        v8 = T();
        t->idle_delay = crt_atoi(v8);
        v8 = T();
        v8 = T();
        t->rank = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_unit(self)->rank_of_name(joan::orig::arg::str(v8))));  // 0x00527130
        v8 = T();
        v8 = T();
        crt_strncpy(t->str_129, v8, 0x20);
        v8 = T();
        v8 = T();
        t->kingdom_id = static_cast<i16>(crt_atoi(v8));
        v8 = T();
        v8 = T();
        t->race = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_race_res(obj(d::g_race_res))->find(joan::orig::arg::str(v8))));  // 0x004ca400
        if (t->race == 0)
            joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Unknown_Race_s_in_s), v8, rd);  // 0x00586cb0 "Unknown Race %s in %s"
        float* const speeds = &t->speed_walk;  // speed_walk .. speed_back: 5 consecutive floats
        for (i32 k = 0; k < 5; ++k) {
            v8 = T();
            v8 = T();
            speeds[k] = static_cast<float>(crt_atof(v8));
        }
        i32* const stats = &t->hit_points;  // hit_points, u32_155, u32_159, xp_value, cost: 5 consecutive dwords
        for (i32 k = 0; k < 5; ++k) {
            v8 = T();
            v8 = T();
            stats[k] = crt_atoi(v8);
        }
        if (t->unit_class == 8) {
            v8 = T();
            v8 = T();
            t->siege_165 = crt_atoi(v8);
        }
        v8 = T();
        v8 = T();
        t->upgrade_id = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_upgrade_res))->find_531df0(joan::orig::arg::str(v8)));  // 0x00531df0
        v8 = T();
        v8 = T();
        t->i16_16d = static_cast<i16>(crt_atoi(v8));
        v8 = T();
        if (crt_stricmp(v8, str(d::s_EArmorCode)) == 0) {  // "EArmorCode"
            v8 = T();
            t->armor_code = static_cast<i16>(armor(v8));
            v8 = T();
        }
        // 0x171: a single token read after the EArmorCode block (both paths).
        v8 = T();
        t->item_weapon = static_cast<i16>(armor(v8));
        i16* const items = &t->item_weapon_b;  // item_weapon_b, item_shield, item_stack
        for (i32 k = 0; k < 3; ++k) {
            v8 = T();
            v8 = T();
            items[k] = static_cast<i16>(armor(v8));
        }
        v8 = T();
        v8 = T();
        t->treasure_list = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_names(obj(d::g_randList))->find_463ed0(joan::orig::arg::str(v8))));  // 0x00463ed0
        i16* const attrs = &t->strength;  // strength .. i16_1ab: 25 consecutive words
        for (i32 k = 0; k < 25; ++k) {
            if (k == 16) continue;  // attrs[16] = i16_19b: not read by the original
            v8 = T();
            v8 = T();
            attrs[k] = static_cast<i16>(crt_atoi(v8));
        }
        v8 = T();
        v8 = T();
        t->shape_kind = joan::orig::arg::as<u32>(joan::orig::as_unit_types(self)->shape_kind_of_name(joan::orig::arg::str(v8)));  // 0x00527d20
        {
            float* shape = nullptr;
            i32 n = 0;
            switch (jo::view<i32>(t->shape_kind)) {
                case 1: shape = t->shape_sphere, n = 4; break;
                case 2: shape = t->shape_box, n = 6; break;
                case 3: shape = t->shape_cylinder, n = 5; break;
                default: break;
            }
            for (i32 k = 0; k < n; ++k) {
                v8 = T();
                v8 = T();
                shape[k] = static_cast<float>(crt_atoi(v8));
            }
        }
        v8 = T();
        v8 = T();
        t->slot_count = crt_atoi(v8);
        t->buffers[0] = nullptr;
        t->buffers[1] = nullptr;
        if (t->slot_count > 0) {
            auto alloc = [](u32 size, i32 line_add) {
                const i32 line = static_cast<i32>(global<i16>(d::g_a07bc8)) + line_add;
                return joan::orig::arg::as<void*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), str(d::s_d_ww_unitres_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
            };
            t->buffers[0] = alloc(jo::view<u32>(t->slot_count) * 0xc, 0x289);
            t->buffers[1] = alloc(jo::view<u32>(t->slot_count) << 2, 0x28a);
            t->buffers[2] = alloc(jo::view<u32>(t->slot_count), 0x28b);
            t->buffers[3] = alloc(jo::view<u32>(t->slot_count) << 1, 0x28c);
            char name[0x100];  // ebp-0x374, read_line n = 0xff
            for (i32 i = 0; i < t->slot_count; ++i) {
                for (i32 k = 0; k < 3; ++k) {
                    v8 = T();
                    v8 = T();
                    const float f = static_cast<float>(crt_atoi(v8));
                    static_cast<float*>(t->buffers[0])[i * 3 + k] = f;
                }
                v8 = T();
                v8 = T();
                const float sc = static_cast<float>(crt_atof(v8));
                static_cast<float*>(t->buffers[1])[i] = sc;
                v8 = T();
                v8 = T();
                const i8 c = static_cast<i8>(*v8);
                if (c == 0x4d)  // 'M'
                    static_cast<u8*>(t->buffers[2])[i] = 1;
                else if (c == 0x46)  // 'F'
                    static_cast<u8*>(t->buffers[2])[i] = 2;
                else if (c == 0x42)  // 'B'
                    static_cast<u8*>(t->buffers[2])[i] = 3;
                v8 = T();
                read_line(name, 0xff);
                const i16 a = static_cast<i16>(armor(name));
                static_cast<i16*>(t->buffers[3])[i] = a;
            }
        }
        jo::view<u8>(t->is_hero) = static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::is_hero_name(joan::orig::arg::str(t->name))));  // 0x00430050
        joan::orig::as_unit_type(out)->finish_parse(0u);                                       // 0x00527770
    }
};

JOAN_REPLACE("game/b106_04", "UnitRes::parse_525080", 0x00525080, &H::unitres_parse);

}  // namespace game_b106_04
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (game/b82_17)
//
//      0x005270b0 UnitTypes::find(name) (ret 4)  - _stricmp over (short)+4 entries of 0x203
namespace {
namespace en3d_b82_17 {

std::int32_t stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

JOAN_COUNTER(c_types_find, "UnitTypes::find");

struct H {

    // 0x005270b0
    std::int32_t types_find(const char* name) {
        c_types_find.hit();
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::UnitRes>(this)->count); ++i)
            if (stricmp_(jo::self<jo::UnitRes>(this)->types[i].name, name) == 0) return i + 1;
        return 0;
    }
};

#define B82_17G(name, addr, fn) JOAN_REPLACE("game/b82_17", name, addr, fn)
B82_17G("UnitTypes::find", 0x005270b0, &H::types_find);
#undef B82_17G

}  // namespace en3d_b82_17
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
//
//      0x00527130 Unit::rank_of_name   (thiscall(name), ret 4) - KING 5 .. CAVALRY 2.
namespace {
namespace game_b92_04 {

JOAN_COUNTER(c_rank_of_name, "Unit::rank_of_name");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x00527130
    i32 rank_of_name(const char* name) {
        c_rank_of_name.hit();
        if (stricmp_(name, str(d::s_KING)) == 0) return 5;  // "KING"
        if (stricmp_(name, str(d::s_GENERAL)) == 0) return 4;  // "GENERAL"
        if (stricmp_(name, str(d::s_CAPTAIN)) == 0) return 3;  // "CAPTAIN"
        if (stricmp_(name, str(d::s_ARCHER_9d7bc0)) == 0) return 1;  // "ARCHER"
        if (stricmp_(name, str(d::s_INFANTRY)) == 0) return 0;  // "INFANTRY"
        if (stricmp_(name, str(d::s_CAVALRY)) == 0) return 2;  // "CAVALRY"
        return 0;
    }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("Unit::rank_of_name", 0x00527130, &H::rank_of_name);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
//
//      0x00527220 UnitRes::class_of_name  (name), ret 4 - _stricmp table, default 1.
namespace {
namespace game_b97_04 {

JOAN_COUNTER(c_class_of_name, "UnitRes::class_of_name");

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

struct H {
    // ------------------------------------------------------------------------ 0x00527220
    i32 class_of_name(const char* s) {
        c_class_of_name.hit();
        if (stricmp_(s, str(d::s_CITIZEN)) == 0) return 0;  // "CITIZEN"
        if (stricmp_(s, str(d::s_LIGHT_INFANTRY)) == 0) return 1;  // "LIGHT_INFANTRY"
        if (stricmp_(s, str(d::s_HEAVY_INFANTRY)) == 0) return 2;  // "HEAVY_INFANTRY"
        if (stricmp_(s, str(d::s_POLEMAN)) == 0) return 3;  // "POLEMAN"
        if (stricmp_(s, str(d::s_MACEMAN)) == 0) return 4;  // "MACEMAN"
        if (stricmp_(s, str(d::s_ARCHER_9d7bc0)) == 0) return 5;  // "ARCHER"
        if (stricmp_(s, str(d::s_CROSSBOW_MAN)) == 0) return 6;  // "CROSSBOW_MAN"
        if (stricmp_(s, str(d::s_CAVALRY)) == 0) return 7;  // "CAVALRY"
        if (stricmp_(s, str(d::s_SIEGE_WEAPON)) == 0) return 8;  // "SIEGE_WEAPON"
        if (stricmp_(s, str(d::s_CART)) == 0) return 9;  // "CART"
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("UnitRes::class_of_name", 0x00527220, &H::class_of_name);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
//
//    - UnitRes::free_buffers 0x005273b0 (ret 0): frees +0x1b1 / +0x1b5 / +0x1b9 / +0x1bd
//      (0x58ed70 on 0xa48838, "unitres.cpp", line [0xa07bcc] + 9 / 0xf / 0x15 / 0x1b).
namespace {
namespace game_b100_08 {

void* const kMemory = obj(d::g_mem);

JOAN_COUNTER(c_free_buffers, "UnitRes::free_buffers");

// ------------------------------------------------------------------------------- thiscall
struct H {

    // 0x005273b0
    void free_buffers() {
        c_free_buffers.hit();
        const i32 lines[4] = {9, 0xf, 0x15, 0x1b};
        for (int i = 0; i < 4; ++i) {
            if (jo::self<jo::UnitType>(this)->buffers[i] != nullptr) {
                joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::self<jo::UnitType>(this)->buffers[i]), str(d::s_d_ww_unitres_cpp), joan::orig::arg::i32(// 0x0058ed70
                              static_cast<i32>(global<i16>(d::g_a07bcc)) + lines[i]));
                jo::self<jo::UnitType>(this)->buffers[i] = nullptr;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("UnitRes::free_buffers", 0x005273b0, &H::free_buffers);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b105_07.cpp (game/b105_07)
//
//    - empty thiscall stubs (never write eax): 0x00527770 (ret 4), 0x005311a0 (ret 0),
//      0x00546ca0 (ret 0x10), 0x00546d60 (ret 0), 0x0054dc80 (ret 0xc), 0x0054dca0 (ret 0).
//
//  - Empty stubs: eax is the caller's. Callers checked: 0x00527770 from 0x00526a2c (tail of
//    0x00525080, whose only caller 0x00524fa2 ignores eax); 0x00546d60 from 0x00470eb5 and
namespace {
namespace game_b105_07 {

using u32 = std::uint32_t;

struct H {
    // empty stubs (eax untouched in the original; no caller reads it)
    void nop_527770(u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_07", name, addr, fn)
B("Obj::nop_527770", 0x00527770, &H::nop_527770);
#undef B

}  // namespace game_b105_07
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
//
//      0x00527d20 Shape::kind_from_name (thiscall(name), ret 4) - strcmp: "None" 0, "Box3d" 2,
//                 "Cylinder" 3, "Sphere" 1, else 0.
namespace {
namespace game_b98_04 {

using joan::img::str;

using i32 = std::int32_t;

using joan::orig::helpers::strcmp_;  // strcmp

struct H {
    // ------------------------------------------------------------------------ 0x00527d20
    i32 shape_kind(const char* s) {
        if (strcmp_(s, str(d::s_None)) == 0) return 0;  // "None"
        if (strcmp_(s, str(d::s_Box3d)) == 0) return 2;  // "Box3d"
        if (strcmp_(s, str(d::s_Cylinder)) == 0) return 3;  // "Cylinder"
        if (strcmp_(s, str(d::s_Sphere)) == 0) return 1;  // "Sphere"
        return 0;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("Shape::kind_from_name", 0x00527d20, &H::shape_kind);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b103_06.cpp (game/b103_06)
//
//    - Obj::~Obj_524c90 0x00524c90 (ret 0): 0x00524d00 on this, 0x00417a30 on this + 7 (SEH frame
//      only marks the base destructor state: normal path).
#define M "game/b103_06"

namespace {
namespace game_b103_06 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // 0x00524c90
    std::uintptr_t dtor_524c90() {
        joan::orig::as_unit_types(this)->release();                  // 0x00524d00
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_archive(self() + 7)->dtor());     // 0x00417a30
    }
};

JOAN_REPLACE(M, "Obj::~Obj_524c90", 0x00524c90, &H::dtor_524c90);

}  // namespace game_b103_06
}  // namespace
#undef M

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("Obj::nop_527500", 0x00527500, &H::nop);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/en3d_b82_17.cpp (game/b82_17)
namespace {
namespace en3d_b82_17_2 {

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_panel_open, "Panel::open");

struct H {

    // 0x00527520
    std::int32_t panel_open(std::int32_t i) {
        c_panel_open.hit();
        void* const kSprites = obj(d::g_model_res_mgr);
        std::int32_t* const count = &jo::self<jo::UnitType>(this)->model_refs[i];
        *count = *count + 1;
        if (*count == 1) {
            joan::orig::as_model_res_mgr(kSprites)->set_params(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::UnitType>(this)->model_ids[i])), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(jo::self<jo::UnitType>(this)->model_flags[i] != 0))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(1))), joan::orig::arg::i16(static_cast<std::int16_t>(0)));  // 0x004b0ec0
            joan::orig::as_model_res(kSprites)->acquire(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::UnitType>(this)->model_ids[i])));            // 0x004b0f40
        }
        jo::self<jo::UnitType>(this)->refcount = jo::self<jo::UnitType>(this)->refcount + 1;
        if (jo::self<jo::UnitType>(this)->refcount == 1) {
            if (jo::self<jo::UnitType>(this)->model_a != 0) {
                joan::orig::as_model_res_mgr(kSprites)->set_params(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::UnitType>(this)->model_a)), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(0))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(0))), joan::orig::arg::i16(static_cast<std::int16_t>(0)));            // 0x004b0ec0
                joan::orig::as_model_res(kSprites)->acquire(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::UnitType>(this)->model_a)));  // 0x004b0f40
            }
            if (jo::self<jo::UnitType>(this)->model_b != 0) {
                // neg/sbb/neg on the ADDRESS this + 0x90 (not its contents).
                const std::uint8_t flag = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(this) + 0x90u != 0u);
                joan::orig::as_model_res_mgr(kSprites)->set_params(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::UnitType>(this)->model_b)), static_cast<std::int8_t>(joan::orig::arg::i32(flag)), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<std::uint8_t>(1))), joan::orig::arg::i16(static_cast<std::int16_t>(0)));            // 0x004b0ec0
                joan::orig::as_model_res(kSprites)->acquire(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::UnitType>(this)->model_b)));  // 0x004b0f40
            }
        }
        return jo::self<jo::UnitType>(this)->refcount;
    }
};

#define B82_17G(name, addr, fn) JOAN_REPLACE("game/b82_17", name, addr, fn)
B82_17G("Panel::open", 0x00527520, &H::panel_open);
#undef B82_17G

}  // namespace en3d_b82_17_2
}  // namespace

// ---------------------------------------- from modules/game_b104_06.cpp (game/b104_06)
namespace {
namespace game_b104_06 {

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;

using u32 = std::uint32_t;

i32 sx16(i32 v) { return static_cast<i32>(static_cast<i16>(v)); }

void* const kTypes = obj(d::g_types);

JOAN_COUNTER(c_rank, "Unit::rank_527790");

void* type_get(i32 w) { return joan::orig::arg::as<void*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(sx16(w)))); }  // 0x0042d0a0 (word)

// 0x00527790 lookup tables (index = word - 1, 0..7).
const i32 kRankOuter[8][8] = {
    {4, 2, 1, 3, 5, 6, 0, 0}, {6, 3, 4, 5, 2, 1, 0, 0}, {5, 4, 3, 6, 2, 1, 0, 0}, {4, 1, 2, 3, 6, 5, 0, 0},
    {2, 5, 6, 1, 4, 3, 0, 0}, {2, 5, 6, 1, 4, 3, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 2, 3, 4, 5, 6, 0, 7},
};

struct H {

    // ------------------------------------------------------------------------ 0x00527790
    i32 rank_527790(u32 type) {
        c_rank.hit();
        const u32 o = static_cast<u32>(static_cast<i32>(jo::self<jo::Unit>(this)->side) - 1);
        if (o > 7 || o == 6) return 0;
        const u32 n = static_cast<u32>(static_cast<i32>(jo::as_unit_type(type_get(static_cast<i32>(type)))->unit_class) - 1);
        if (n > 7) return 0;
        return kRankOuter[o][n];
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_06", name, addr, fn)
B("Unit::rank_527790", 0x00527790, &H::rank_527790);
#undef B

}  // namespace game_b104_06
}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
namespace {
namespace game_b88_05 {

using joan::img::obj;

using i16 = std::int16_t;

void* const kTypes = obj(d::g_types);      // 0x0042d0a0

// ------------------------------------------------------------ 0x00527dd0 (cdecl)
std::int32_t JOAN_CDECL unit_class(std::int16_t type) {
    std::uint8_t* const t = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(type)));  // 0x0042d0a0 (reads the word)
    switch (static_cast<std::int16_t>(jo::view<std::uint16_t>(jo::as_unit_type(t)->rank))) {
        case 0:
        case 1:
        case 2: return 1;
        case 3: return 2;
        case 4:
        case 5: return 3;
        default: return 0;
    }
}

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("unit_class", 0x00527dd0, &unit_class);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

using joan::img::obj;

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;
using joan::img::str;

using u8 = std::uint8_t;

using u32 = std::uint32_t;

i32 stricmp_(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

// 0x00527e60
i32 JOAN_CDECL character_class(i16 id) {
    u8* const c = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(id))));  // 0x0042d0a0
    switch (static_cast<u32>(static_cast<i32>(jo::as_unit_type(c)->rank))) {
        case 0:
        case 1:
        case 2: return 1;
        case 3: return 2;
        case 4: return stricmp_(jo::as_unit_type(c)->name, str(d::s_JOAN_OF_ARC)) == 0 ? 4 : 3;  // "JOAN_OF_ARC"
        default: return 0;
    }
}

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("character_class", 0x00527e60, &character_class);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

using joan::call::thiscall;
using joan::img::obj;

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_ref_release, "ModelRef::release");

[[maybe_unused]] constexpr img::Addr kAnimRes = d::g_anim_res_mgr, kModelRes = d::g_model_res_mgr, kModelRefs = d::g_model_refs;

struct H {

    // ------------------------------------------------------------------ 0x00527690
    std::int32_t ref_release_slot(std::int32_t slot) {
        c_ref_release.hit();
        std::int32_t* const refs = &jo::self<jo::UnitType>(this)->model_refs[slot];  // ModelRef is the UnitType (0x00527520 panel_open is the acquire side)
        --*refs;
        if (*refs == 0) joan::orig::as_model_res_mgr(obj(kModelRes))->release(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::UnitType>(this)->model_ids[slot])));  // 0x004b16a0
        --jo::self<jo::UnitType>(this)->refcount;
        if (jo::self<jo::UnitType>(this)->refcount == 0) {
            if (jo::self<jo::UnitType>(this)->model_a != 0) joan::orig::as_model_res_mgr(obj(kModelRes))->release(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::UnitType>(this)->model_a)));
            if (jo::self<jo::UnitType>(this)->model_b != 0) joan::orig::as_model_res_mgr(obj(kModelRes))->release(joan::orig::arg::i16(*reinterpret_cast<const std::int16_t*>(&jo::self<jo::UnitType>(this)->model_b)));
        }
        return jo::self<jo::UnitType>(this)->refcount;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("ModelRef::release_slot", 0x00527690, &H::ref_release_slot);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::UnitTypes::release() -> void { joan::bind::member<&::game_b102_05::H::release_524d00>(this); }  // 0x524d00
auto joan::orig::UnitTypes::reset_all() -> void { joan::bind::member<&::game_b101_07::H::unitres_reset_all>(this); }  // 0x524dc0
auto joan::orig::UnitTypes::parse(void* a0, void* a1, std::uint32_t a2) -> void { joan::bind::member<&::game_b106_04::H::unitres_parse>(this, a0, a1, a2); }  // 0x525080
auto joan::orig::UnitTypes::find(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b82_17::H::types_find>(this, a0)); }  // 0x5270b0
auto joan::orig::Unit::rank_of_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_04::H::rank_of_name>(this, a0)); }  // 0x527130
auto joan::orig::UnitTypes::class_of_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_04::H::class_of_name>(this, a0)); }  // 0x527220
auto joan::orig::UnitType::free_buffers() -> void { joan::bind::member<&::game_b100_08::H::free_buffers>(this); }  // 0x5273b0
auto joan::orig::Panel::open(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::en3d_b82_17_2::H::panel_open>(this, a0)); }  // 0x527520
auto joan::orig::UnitType::release_model_slot(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::ref_release_slot>(this, a0)); }  // 0x527690
auto joan::orig::UnitType::finish_parse(std::uint32_t a0) -> void { joan::bind::member<&::game_b105_07::H::nop_527770>(this, a0); }  // 0x527770
auto joan::orig::Unit::rank_527790(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_06::H::rank_527790>(this, a0)); }  // 0x527790
auto joan::orig::UnitTypes::shape_kind_of_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_04::H::shape_kind>(this, a0)); }  // 0x527d20
auto joan::orig::unit_class(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b88_05::unit_class>(a0)); }  // 0x527dd0
auto joan::orig::character_class(std::int16_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_04::character_class>(a0)); }  // 0x527e60
