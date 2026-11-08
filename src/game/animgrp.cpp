// game/animgrp.cpp: functions of the original d:\ww\animgrp.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstddef>
#include <cstdint>
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

using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
//
//    - AnimGroup::free_buffer 0x004149c0: when +0x39 set, frees +0x30 (0x0058ed70, animgrp.cpp
//      line word 0x9f6564 + 7), clears +0x30 and +0x39.
namespace {
namespace game_b92_01 {

using joan::orig::helpers::s16;
using joan::orig::helpers::s8;

void* const kMemory = obj(d::g_mem);  // allocator (0x0058ed70 free)

JOAN_COUNTER(c_free_buffer, "AnimGroup::free_buffer");

struct H {
    // ------------------------------------------------------------ 0x004149c0
    void* free_buffer() {  // eax = this on every path
        c_free_buffer.hit();
        if (s8(jo::self<jo::AnimSet>(this)->owns_groups) == 0) return this;
        if (jo::view<void*>(jo::self<jo::AnimSet>(this)->groups) != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::AnimSet>(this)->groups)), str(d::s_d_ww_animgrp_cpp), joan::orig::arg::i32(s16(global<std::uint16_t>(d::g_9f6564)) + 7));  // 0x0058ed70
            jo::view<void*>(jo::self<jo::AnimSet>(this)->groups) = nullptr;
        }
        jo::self<jo::AnimSet>(this)->owns_groups = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("AnimGroup::free_buffer", 0x004149c0, &H::free_buffer);
#undef B

}  // namespace game_b92_01
}  // namespace

// ---------------------------------------- from modules/game_b107_16.cpp (game/b107_16)
//
//    - AnimGroup::load_414a30 0x00414a30 (id), ret 4 (animgrp.cpp): resource 0x0058b180(id) on
//      0xa56b18, count 0x00415280 -> +0x2c, allocates count * 0x99 bytes (0x0058e7b0 on
//      0xa48838, "d:\ww\animgrp.cpp" line [0x9f6568]+9) -> +0x30, memset 0, then for each
//      record i parses the fixed-width source row 0x00580c70(i + 1) through the string copier
//      0x00590210 / trimmer 0x00590170 (on 0xa45fa8), atoi 0x007b15cb and lookups 0x004157c0,
//      0x00415820, 0x00416910, 0x00416aa0, 0x004184b0 / 0x00417470 / 0x004174f0 / 0x00418550
//      (on 0xa58cb8, "ANIM\"), 0x0041a0e0 (0xa5d0f8), 0x004d1470 / 0x004d12f0 (0xa59a72).
//      Returns the final loop index (eax the original leaves; its only caller 0x00414995
//      ignores it).
//
//  - 0x00414a30: the 0x80-byte stack scratch buffer is uninitialised in the original; it is
//    always written by 0x00590210 (copy + NUL) before each read, so the zero-initialised
//    buffer here only differs past the terminator.
namespace {
namespace game_b107_16 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
JOAN_COUNTER(c_load_414a30, "AnimGroup::load_414a30");

// ---- helpers for 0x00414a30 ----
// 0x00590210 on 0xa45fa8: copy up to n chars of src to dst and NUL-terminate.
void copy_field(void* dst, const void* src, i32 n) {
    joan::orig::as_misc(obj(d::g_misc))->copy_field_590210(joan::orig::arg::str(dst), joan::orig::arg::str(src), joan::orig::arg::i32(n));  // 0x00590210
}
// 0x00590170 on 0xa45fa8: copy n chars into its buffer, trim trailing blanks, return buffer.
char* trimmed(const void* src, i32 n) {
    return joan::orig::arg::as<char*>(joan::orig::as_misc(obj(d::g_misc))->rtrim_field_590170(joan::orig::arg::str(src), joan::orig::arg::i32(n)));  // 0x00590170
}
i32 atoi_(const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); }

struct H {
    // ------------------------------------------------------------------ 0x00414a30
    i32 load_414a30(const char* id) {
        c_load_414a30.hit();
        // The table name for [Fixes] MissingSounds below. `id` points into 0x00590170's shared buffer (the caller
        // trims the ANIMATE entry with it), which trimmed() overwrites with every field read here.
        char table[16] = {};
        std::strncpy(table, id, sizeof table - 1);
        void* const res = joan::orig::arg::as<void*>(joan::orig::as_game_set(obj(d::g_game_set))->open_db_58b180(id));  // 0x0058b180
        jo::self<jo::AnimSet>(this)->group_count = joan::orig::arg::as<i32>(joan::orig::as_database(res)->rec_count());                      // 0x00415280
        const i32 line = static_cast<i32>(global<i16>(d::g_9f6568)) + 9;
        jo::view<u8*>(jo::self<jo::AnimSet>(this)->groups) = joan::orig::arg::as<u8*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<u32>(jo::self<jo::AnimSet>(this)->group_count) * 0x99u), str(d::s_d_ww_animgrp_cpp), joan::orig::arg::i32(line)));  // 0x0058e7b0
        std::memset(static_cast<void*>(jo::view<u8*>(jo::self<jo::AnimSet>(this)->groups)), 0, joan::orig::arg::u32(static_cast<u32>(jo::self<jo::AnimSet>(this)->group_count) * 0x99u));  // memset

        char buf[0x80] = {};
        i32 i = 0;
        for (; i < jo::self<jo::AnimSet>(this)->group_count; ++i) {
            u8* const src = joan::orig::arg::as<u8*>(joan::orig::as_database(res)->read(joan::orig::arg::i32(i + 1)));  // 0x00580c70
            u8* const rec = jo::view<u8*>(jo::self<jo::AnimSet>(this)->groups) + static_cast<u32>(i) * 0x99u;

            copy_field(buf, src, 0x20);
            copy_field(buf, src + 0x20, 8);
            jo::as_anim_group_rec(rec)->is_jump = joan::orig::arg::as<u32>(joan::orig::as_anim(this)->is_jump(joan::orig::arg::str(buf)));  // 0x004157c0
            copy_field(buf, src + 0x28, 8);
            jo::view<u32>(jo::as_anim_group_rec(rec)->kind) = joan::orig::arg::as<u32>(joan::orig::as_anim(this)->id_from_name(joan::orig::arg::str(buf)));  // 0x00415820
            copy_field(jo::as_anim_group_rec(rec)->name_8, src + 0x30, 6);
            jo::as_anim_group_rec(rec)->u8_e = src[0x36];
            jo::as_anim_group_rec(rec)->u8_f = src[0x37];
            copy_field(buf, src + 0x38, 8);
            jo::view<u32>(jo::as_anim_group_rec(rec)->weapon_kind) = joan::orig::arg::as<u32>(joan::orig::as_anim_set(this)->weapon_kind_from_name(joan::orig::arg::str(buf)));  // 0x00416910
            copy_field(buf, src + 0x40, 8);
            jo::view<u32>(jo::as_anim_group_rec(rec)->direction) = joan::orig::arg::as<u32>(joan::orig::anim_kind_from_name(joan::orig::arg::str(static_cast<char*>(buf))));  // 0x00416aa0
            copy_field(buf, src + 0x48, 3);
            jo::as_anim_group_rec(rec)->min_mana = atoi_(buf);
            copy_field(buf, src + 0x4b, 3);
            jo::as_anim_group_rec(rec)->i32_1c = atoi_(buf);
            copy_field(buf, src + 0x4e, 4);
            jo::as_anim_group_rec(rec)->duration = atoi_(buf);
            copy_field(buf, src + 0x52, 4);
            jo::as_anim_group_rec(rec)->combo_min = atoi_(buf);
            copy_field(buf, src + 0x56, 4);
            jo::as_anim_group_rec(rec)->combo_max = atoi_(buf);
            copy_field(jo::as_anim_group_rec(rec)->anim_name, src + 0x5a, 0x20);
            if (static_cast<i8>(jo::as_anim_group_rec(rec)->anim_name[0]) != 0) {
                jo::view<u32>(jo::as_anim_group_rec(rec)->anim_id) =
                    joan::orig::arg::as<u32>(joan::orig::as_path(obj(d::g_anim_res_mgr))->find(str(d::s_ANIM), joan::orig::arg::str(jo::as_anim_group_rec(rec)->anim_name)));  // 0x004184b0
                if (jo::view<u32>(jo::as_anim_group_rec(rec)->anim_id) == 0) {
                    jo::view<u32>(jo::as_anim_group_rec(rec)->anim_id) = joan::orig::arg::as<u32>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->add_slot(0u));  // 0x00417470
                    void* const e =
                        joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_anim_group_rec(rec)->anim_id))));  // 0x004174f0
                    joan::orig::as_path(e)->init(str(d::s_ANIM), joan::orig::arg::str(jo::as_anim_group_rec(rec)->anim_name));  // 0x00418550
                }
            }
            copy_field(jo::as_anim_group_rec(rec)->bip_name, src + 0x7a, 0x20);
            {
                char* const t = trimmed(src + 0x9a, 0x20);
                jo::view<u32>(jo::as_anim_group_rec(rec)->model_ref) = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_model_refs))->find_41a0e0(joan::orig::arg::str(t)));  // 0x0041a0e0
            }
            if (joan::orig::arg::as<u8>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->is_ts_prefix(joan::orig::arg::str(trimmed(src + 0xba, 8)))) != 0) {  // 0x004d1470
                jo::as_anim_group_rec(rec)->sound_a = -99;
            } else {
                jo::as_anim_group_rec(rec)->sound_a = static_cast<i32>(
                    joan::orig::arg::as<i16>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(joan::orig::arg::str(trimmed(src + 0xba, 8)))));  // 0x004d12f0
            }
            copy_field(jo::as_anim_group_rec(rec)->sound_a_name, src + 0xba, 8);
            i32 swing_delay = -1;  // [Fixes] MissingSounds below
            // [Fixes] MissingSounds: the Bastard of Orleans' (ORLEAN) and Richemont's (RICHEMON) tables leave SOUND
            // empty in their STAND IDLE2 row, where Joan, La Hire, Jean de Metz and Alencon name their idle voice line
            // (JOIDLE2, ...). Theirs, ORIDLE2 / RIIDLE2 (RTS_OTHER_ORLEANS_009 / RTS_OTHER_RICHEMONT_009), are in the
            // SOUND table but were never played.
            // The same three tables (and Charles VII's, CHARLIE7) have no sound on any attack row: they were made for
            // AI-controlled heroes. With [Game] AllHeroes they can be played, and their swings were silent. They get
            // the sword swing every other hero's attacks use (A / AA / AAA = attack_sw_a_v1, MAIN_CHAR: only for the
            // controlled hero, so as AI allies they stay as they were), with Joan's delays (0 / 100 / 200 / 300 ms by
            // the combo step).
            if (joan::hook::settings().fix_missing_sounds && jo::as_anim_group_rec(rec)->sound_a == 0 &&
                (_stricmp(table, "ORLEAN") == 0 || _stricmp(table, "RICHEMON") == 0 || _stricmp(table, "CHARLIE7") == 0)) {
                const u32 k = jo::view<u32>(jo::as_anim_group_rec(rec)->kind);
                // ATTACK, JATTACKB, ENGATK..ENGATK3, DOWNATK, H_ATTACK
                if (k == 28 || k == 29 || k == 30 || k == 31 || k == 32 || k == 34 || k == 73) {
                    static constexpr const char* kSwing[3] = {"A", "AA", "AAA"};
                    i32 step = 0;  // F_ATKA = 0, F_ATKAA = 1, ...: the combo letters after "F_ATK", all A
                    const char* const upper = jo::as_anim_group_rec(rec)->bip_name;
                    if (_strnicmp(upper, "F_ATK", 5) == 0) {
                        const char* p = upper + 5;
                        while (*p == 'A' || *p == 'a') ++p;
                        if (*p == '\0') step = static_cast<i32>(p - (upper + 5)) - 1;
                    }
                    if (step < 0) step = 0;
                    const char* const swing = kSwing[step % 3];
                    jo::as_anim_group_rec(rec)->sound_a = static_cast<i32>(
                        joan::orig::arg::as<i16>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(joan::orig::arg::str(swing))));  // 0x004d12f0
                    if (jo::as_anim_group_rec(rec)->sound_a != 0) {
                        std::strcpy(jo::as_anim_group_rec(rec)->sound_a_name, swing);
                        swing_delay = (step < 3 ? step : 3) * 100;  // set after SOUND_TIME is read below
                    }
                }
            }
            if (joan::hook::settings().fix_missing_sounds && jo::as_anim_group_rec(rec)->sound_a == 0 && jo::as_anim_group_rec(rec)->is_jump == 0 &&
                jo::view<u32>(jo::as_anim_group_rec(rec)->kind) == 2) {  // STAND IDLE2
                static constexpr struct { const char* table; const char* sound; } kIdleVoices[] = {{"ORLEAN", "ORIDLE2"}, {"RICHEMON", "RIIDLE2"}};
                for (const auto& v : kIdleVoices) {
                    if (_stricmp(table, v.table) != 0) continue;
                    jo::as_anim_group_rec(rec)->sound_a = static_cast<i32>(
                        joan::orig::arg::as<i16>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(joan::orig::arg::str(v.sound))));  // 0x004d12f0
                    if (jo::as_anim_group_rec(rec)->sound_a != 0) std::strcpy(jo::as_anim_group_rec(rec)->sound_a_name, v.sound);
                    joan::hook::log_line("[Fixes] MissingSounds: %s IDLE2 plays %s (sound %d)", v.table, v.sound, jo::as_anim_group_rec(rec)->sound_a);
                }
            }
            copy_field(buf, src + 0xc2, 4);
            jo::as_anim_group_rec(rec)->sound_a_delay = atoi_(buf);
            if (swing_delay >= 0) jo::as_anim_group_rec(rec)->sound_a_delay = swing_delay;
            if (joan::orig::arg::as<u8>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->is_ts_prefix(joan::orig::arg::str(trimmed(src + 0xc6, 8)))) != 0) {  // 0x004d1470
                jo::as_anim_group_rec(rec)->sound_b = -99;
            } else {
                jo::as_anim_group_rec(rec)->sound_b = static_cast<i32>(
                    joan::orig::arg::as<i16>(joan::orig::as_sorted_names(obj(d::g_sound_def_table))->find(joan::orig::arg::str(trimmed(src + 0xc6, 8)))));  // 0x004d12f0
            }
            copy_field(jo::as_anim_group_rec(rec)->sound_b_name, src + 0xc6, 8);
            copy_field(buf, src + 0xce, 4);
            jo::as_anim_group_rec(rec)->sound_b_delay = atoi_(buf);
            const i32 c = static_cast<i8>(src[0xd2]);
            jo::as_anim_group_rec(rec)->flag_98 = (c != 0x20 && c != 0x30) ? 1 : 0;
        }
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_16", name, addr, fn)
B("AnimGroup::load_414a30", 0x00414a30, &H::load_414a30);
#undef B

}  // namespace game_b107_16
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
//
//    - Obj::release_39_414930 0x00414930 (ret 0): when byte +0x39: 0x004149c0, byte +0x39 = 0
//      (eax = 0x004149c0's result), else this.
namespace {
namespace game_b103_01 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;

using joan::call::thiscall;
struct H {
    // ------------------------------------------------------------------ 0x00414930
    std::uintptr_t release_39_414930() {
        if (static_cast<i8>(jo::self<jo::AnimSet>(this)->owns_groups) != 0) {
            const std::uintptr_t r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_anim_set(this)->free_buffer());  // 0x004149c0
            jo::self<jo::AnimSet>(this)->owns_groups = 0;
            return r;
        }
        return joan::call::register_low_bits(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::release_39_414930", 0x00414930, &H::release_39_414930);
#undef B

}  // namespace game_b103_01
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
//
//    - AnimEntry::init_414970 0x00414970 (ret 4): 0x004149c0, +0x34 = 0, 0x00414a30(arg), byte +0x39 = 1;
//      eax = 0x00414a30's.
//
//    - AnimRes::load_4172a0 0x004172a0 (ret 0, animres.cpp): opens "RESOURCE\\ANIM_AM2.RES" (+0xc) and
//      "RESOURCE\\ANIM_AM3.RES" (+0x141) (0x0059b920(path, 0, 0)), section "ANIMATE" (0x0058b180 on
//      0xa56b18), count +0 = 0x00415280, +4 = alloc(count * 0x3a, animres.cpp, word [0x9f6570] + 0xc)
//      (0x0058e7b0 on 0xa48838), memset 0; entry i: word = i + 1, 0x00590210 copies (0x20 / 8 bytes) from
//      0x00580c70(i + 1); then 0x00414a30-entry init 0x00414970(0x00590170(entry + 0x23, 8)). eax = this.
namespace {
namespace game_b108_20 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
struct H {
    // -------------------------------------------------------------- 0x00414970
    u32 anim_entry_init(const char* arg) {
        joan::orig::as_anim_set(this)->free_buffer();  // 0x004149c0
        jo::view<u32>(jo::self<jo::AnimSet>(this)->ref_count) = 0;
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_anim_set(this)->load(arg));  // 0x00414a30
        jo::self<jo::AnimSet>(this)->owns_groups = 1;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("AnimEntry::init_414970", 0x00414970, &H::anim_entry_init);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b97_01.cpp (game/b97_01)
//
//      0x004157c0 Anim::is_jump(const char*)              (thiscall, ret 4) - strcmp "STAND" / "JUMP".
namespace {
namespace game_b97_01 {

using joan::img::str;

using i32 = std::int32_t;

using joan::orig::helpers::strcmp_;

struct H {
    // ------------------------------------------------------------------ 0x004157c0
    i32 is_jump(const char* s) {
        if (strcmp_(s, str(d::s_STAND)) == 0) return 0;  // "STAND"
        if (strcmp_(s, str(d::s_JUMP)) == 0) return 1;  // "JUMP"
        return 0;
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_01", name, addr, fn)
B97("Anim::is_jump", 0x004157c0, &H::is_jump);
#undef B97

}  // namespace game_b97_01
}  // namespace

// ---------------------------------------- from modules/game_b106_10.cpp (game/b106_10)
//
//      0x00415820 Anim::id_from_name(name)        (thiscall, ret 4) - 108 strcmp (0x7b16f0)
//                 tests in the original order ("DEFAULT" -> 0, "IDLE" -> 1, ...); otherwise
//                 _strupr(name) (0x7b1800, in place) + strstr(.., "EXTRA") (0x7b1780):
//                 found -> atoi(name + 5) + 0x6b, else -1. `this` unused.
namespace {
namespace game_b106_10 {

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::cdecl_call;
using joan::img::str;

JOAN_COUNTER(c_anim_id, "Anim::id_from_name_415820");

// CRT (called directly, as the original does)
using joan::orig::helpers::crt_strcmp;

// ---------------------------------------------------------------- 0x00415820
struct NameId {
    img::Addr name;
    i32 id;
};
const NameId kAnimNames[] = {
    {d::s_DEFAULT, 0}, {d::s_IDLE, 1}, {d::s_IDLE2, 2}, {d::s_IDLE3, 3}, {d::s_JATTBI, 8},
    {d::s_IDLE_BAT, 4}, {d::s_IDLEUP, 5}, {d::s_IDLEDN, 6}, {d::s_IDLE_T, 7}, {d::s_TIPTOE, 9},
    {d::s_WALK, 10}, {d::s_RUN, 11}, {d::s_T_LEFT, 12}, {d::s_T_RIGHT, 13}, {d::s_MOVE_L, 14},
    {d::s_MOVE_R, 15}, {d::s_MOVE_B, 16}, {d::s_MOVE_FL, 17}, {d::s_MOVE_FR, 18}, {d::s_MOVE_BL, 19},
    {d::s_MOVE_BR, 20}, {d::s_JUMP_B, 21}, {d::s_JUMP_R, 22}, {d::s_JUMP_L, 23}, {d::s_STOP_B, 63},
    {d::s_STOP, 62}, {d::s_DOWN, 24}, {d::s_JATTBD, 25}, {d::s_UP, 26}, {d::s_CHAIN, 27},
    {d::s_ATTACK_9cf380, 28}, {d::s_JATTACKB, 29}, {d::s_ENGATK, 30}, {d::s_ENGATK2, 31}, {d::s_ENGATK3, 32},
    {d::s_ENGATK4, 33}, {d::s_DOWNATK, 34}, {d::s_ATTACKED, 35}, {d::s_ATTKED2, 36}, {d::s_ATTKEDR, 37},
    {d::s_BATTKED, 38}, {d::s_LATTKED, 39}, {d::s_RATTKED, 40}, {d::s_DNATKED, 41}, {d::s_DIE, 42},
    {d::s_GUARD1, 43}, {d::s_GUARD2, 44}, {d::s_GUARDR, 45}, {d::s_BACK, 46}, {d::s_CLIMB, 47},
    {d::s_SHTLOAD, 48}, {d::s_SHTLOADC, 49}, {d::s_SHTLOAD2, 50}, {d::s_SHTOUT, 51}, {d::s_SHTOUT2, 52},
    {d::s_SHTOUTC, 53}, {d::s_SHTIDLE, 54}, {d::s_SHTIDLEC, 55}, {d::s_SHTOUTH, 56}, {d::s_SHTIDLEH, 57},
    {d::s_THROW, 58}, {d::s_THROWN, 59}, {d::s_STANDUP, 60}, {d::s_DIZZY, 61}, {d::s_KNOCKDN, 64},
    {d::s_KNOCKDN2, 65}, {d::s_KNOCKDN3, 66}, {d::s_H_MNT_L, 79}, {d::s_H_MNT_R, 80}, {d::s_H_DMNT_L, 81},
    {d::s_H_DMNT_R, 82}, {d::s_H_IDLE, 67}, {d::s_H_WALK, 68}, {d::s_H_BACK, 69}, {d::s_H_RUN, 70},
    {d::s_H_STOP_B, 71}, {d::s_H_STOP, 72}, {d::s_H_ATTACK, 73}, {d::s_H_HITED, 74}, {d::s_H_BHITED, 75},
    {d::s_H_LHITED, 76}, {d::s_H_RHITED, 77}, {d::s_H_DROP, 78}, {d::s_RUSH, 83}, {d::s_PUSH, 84},
    {d::s_PUSH2, 85}, {d::s_PUSH_L, 86}, {d::s_PUSH_R, 87}, {d::s_BATTER_C, 88}, {d::s_C_IDLE_A, 89},
    {d::s_C_IDLE_B, 90}, {d::s_C_DN_AB, 91}, {d::s_C_DN_AS, 92}, {d::s_C_DN_BA, 93}, {d::s_C_DN_CA, 94},
    {d::s_C_DN_CB, 95}, {d::s_C_DN_SC, 96}, {d::s_C_UP_AB, 97}, {d::s_C_UP_AC, 98}, {d::s_C_UP_BA, 99},
    {d::s_C_UP_BC, 100}, {d::s_C_UP_BC, 100}, {d::s_C_UP_CS, 101}, {d::s_C_UP_SA, 102}, {d::s_C_DROP, 103},
    {d::s_CA_IDLE, 104}, {d::s_CA_RUN, 105}, {d::s_CA_WALK, 106},
};

struct H {
    // 0x00415820
    i32 anim_id(const char* name) {
        c_anim_id.hit();
        for (const NameId& e : kAnimNames)
            if (crt_strcmp(name, str(e.name)) == 0) return e.id;
        const char* const up = joan::orig::arg::as<const char*>(joan::orig::libc::_strupr(joan::orig::arg::str(name)));                   // _strupr
        if (joan::orig::arg::as<u32>(joan::orig::libc::strstr(joan::orig::arg::str(up), str(d::s_EXTRA))) != 0)  // strstr(.., "EXTRA")
            return joan::crt::atoi(joan::orig::arg::str(name + 5)) + 0x6b;    // atoi
        return -1;
    }

  private:
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_10", name, addr, fn)
B("Anim::id_from_name", 0x00415820, &H::anim_id);
#undef B

}  // namespace game_b106_10
}  // namespace

// ---------------------------------------- from modules/game_b100_01.cpp (game/b100_01)
//
//    - weapon_kind_from_name 0x00416910 (thiscall, name), ret 4: strcmp against DEFAULT /
//      1SWORD / 2SWORD / 2HAMMER / 2SPEAR / 2STAFF / 2FLAIL / BOW / CROSSBOW / SHIELD -> 0..9,
//      else -1.
namespace {
namespace game_b100_01 {

using joan::img::str;

using joan::orig::helpers::strcmp_;

JOAN_COUNTER(c_weapon_kind, "weapon_kind_from_name");

struct H {
    // 0x00416910
    std::int32_t weapon_kind(const char* name) {
        c_weapon_kind.hit();
        if (strcmp_(name, str(d::s_DEFAULT)) == 0) return 0;  // DEFAULT
        if (strcmp_(name, str(d::s_1SWORD)) == 0) return 1;  // 1SWORD
        if (strcmp_(name, str(d::s_2SWORD)) == 0) return 2;  // 2SWORD
        if (strcmp_(name, str(d::s_2HAMMER)) == 0) return 3;  // 2HAMMER
        if (strcmp_(name, str(d::s_2SPEAR)) == 0) return 4;  // 2SPEAR
        if (strcmp_(name, str(d::s_2STAFF)) == 0) return 5;  // 2STAFF
        if (strcmp_(name, str(d::s_2FLAIL)) == 0) return 6;  // 2FLAIL
        if (strcmp_(name, str(d::s_BOW)) == 0) return 7;  // BOW
        if (strcmp_(name, str(d::s_CROSSBOW)) == 0) return 8;  // CROSSBOW
        if (strcmp_(name, str(d::s_SHIELD)) == 0) return 9;  // SHIELD
        return -1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_01", name, addr, fn)
B("weapon_kind_from_name", 0x00416910, &H::weapon_kind);
#undef B

}  // namespace game_b100_01
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
//
//    - anim_kind_from_name 0x00416aa0 (cdecl, const char*): strcmp chain DEFAULT..BATTER_R ->
//      0..15, else -1.
namespace {
namespace game_b92_01_2 {

using joan::img::str;

using joan::orig::helpers::strcmp_;

JOAN_COUNTER(c_kind_name, "anim_kind_from_name");

// ------------------------------------------------------------ 0x00416aa0 (cdecl)
std::int32_t JOAN_CDECL anim_kind_from_name(const char* name) {
    c_kind_name.hit();
    if (strcmp_(name, str(d::s_DEFAULT)) == 0) return 0;    // DEFAULT
    if (strcmp_(name, str(d::s_TIRED)) == 0) return 1;    // TIRED
    if (strcmp_(name, str(d::s_HAPPY)) == 0) return 2;    // HAPPY
    if (strcmp_(name, str(d::s_POISON)) == 0) return 3;    // POISON
    if (strcmp_(name, str(d::s_CAMP)) == 0) return 4;    // CAMP
    if (strcmp_(name, str(d::s_CANNON)) == 0) return 5;    // CANNON
    if (strcmp_(name, str(d::s_FAUCONN)) == 0) return 6;    // FAUCONN
    if (strcmp_(name, str(d::s_BOMBARD)) == 0) return 7;    // BOMBARD
    if (strcmp_(name, str(d::s_CROSSBOW)) == 0) return 8;    // CROSSBOW
    if (strcmp_(name, str(d::s_SIEGET)) == 0) return 9;    // SIEGET
    if (strcmp_(name, str(d::s_TREB_L)) == 0) return 0xa;  // TREB_L
    if (strcmp_(name, str(d::s_TREB_R)) == 0) return 0xb;  // TREB_R
    if (strcmp_(name, str(d::s_TREB_F)) == 0) return 0xc;  // TREB_F
    if (strcmp_(name, str(d::s_BATTER)) == 0) return 0xd;  // BATTER
    if (strcmp_(name, str(d::s_BATTER_L)) == 0) return 0xe;  // BATTER_L
    if (strcmp_(name, str(d::s_BATTER_R)) == 0) return 0xf;  // BATTER_R
    return -1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("anim_kind_from_name", 0x00416aa0, &anim_kind_from_name);
#undef B

}  // namespace game_b92_01_2
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
//
//    - Obj::clear_39_414900 0x00414900 (ret 0): byte +0x39 = 0, returns this.
namespace {
namespace game_b103_01_2 {

using u8 = std::uint8_t;

struct H {
    // ------------------------------------------------------------------ 0x00414900
    void* clear_39_414900() {
        jo::self<jo::AnimSet>(this)->owns_groups = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::clear_39_414900", 0x00414900, &H::clear_39_414900);
#undef B

}  // namespace game_b103_01_2
}  // namespace

// ---------------------------------------- from modules/game_b94_01.cpp (game/b94_01)
namespace {
namespace game_b94_01 {

using i16 = std::int16_t;
using i32 = std::int32_t;

struct H {
    // 0x00414f50 Unit::push_message: the queue lives in the Unit (+0x247 count, +0x249 pairs); the Queue view read it at
    // x86 offsets, which on x64 lie 8 bytes before the real fields (group / modifiers pointers precede them).
    i32 push3(i32 a, i16 b, i16 c) {
        if (static_cast<i32>(jo::self<jo::Unit>(this)->think_forced) >= 10) return 0;
        jo::QueuePair* const p = reinterpret_cast<jo::QueuePair*>(jo::self<jo::Unit>(this)->gap_249) + jo::self<jo::Unit>(this)->think_forced;
        p->key = a;
        p->b = b;
        p->c = c;
        jo::self<jo::Unit>(this)->think_forced = static_cast<i16>(jo::self<jo::Unit>(this)->think_forced + 1);
        return 1;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_01", name, addr, fn)
B94("Queue::push3", 0x00414f50, &H::push3);
#undef B94

}  // namespace game_b94_01
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using i16 = std::int16_t;
using i32 = std::int32_t;

using u8 = std::uint8_t;

struct H {
    // ------------------------------------------------------------------ 0x00414fe0
    void* find_pair(i32 key) {
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Unit>(this)->think_forced); ++i) {
            jo::QueuePair* const p = reinterpret_cast<jo::QueuePair*>(jo::self<jo::Unit>(this)->gap_249) + i;
            if (p->key == key) return p;
        }
        return nullptr;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::find_pair", 0x00414fe0, &H::find_pair);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b96_01.cpp (game/b96_01)
namespace {
namespace game_b96_01 {

using joan::hook::global;

using joan::call::cdecl_call;

// ---------------------------------------------------------------------------- static inits
using joan::orig::helpers::pi_from_atan;
void JOAN_CDECL init_a3cda0() { global<float>(d::g_a3cda0) = pi_from_atan(); }

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_01", name, addr, fn)
B96("init_a3cda0", 0x00415040, &init_a3cda0);
#undef B96

}  // namespace game_b96_01
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

using joan::call::thiscall;

using joan::hook::global;

using joan::call::cdecl_call;

using joan::img::obj;

using joan::call::stdcall_call;
JOAN_COUNTER(c_set_stream, "AnimSet::stream");
JOAN_COUNTER(c_set_acquire, "AnimSet::acquire");

[[maybe_unused]] constexpr img::Addr kAnimRes = d::g_anim_res_mgr, kModelRes = d::g_model_res_mgr, kModelRefs = d::g_model_refs;
[[maybe_unused]] constexpr img::Addr kFiles = d::g_misc, kCanvas = d::g_vga_back;

void sleep_5() { stdcall_call<std::uint32_t>(global<std::uintptr_t>(d::iat_KERNEL32_SleepEx), 5u, 1u); }  // SleepEx(5, TRUE)
using joan::orig::helpers::game_delete;                      // operator delete

// AnimSet entry kinds that also register their sound set (0x00417f20).
bool sound_kind(std::int32_t k) {
    return k == 0x1c || k == 0x1d || k == 0x1b || k == 0x23 || k == 0x24 || k == 0x25 || k == 0x1e || k == 0x1f ||
           k == 0x20 || k == 0x21 || k == 0x3a || k == 0x3b || k == 0x3c || k == 0x53 || k == 0x22 || k == 0x15 ||
           k == 0x16 || k == 0x17;
}

struct H {
    // ------------------------------------------------------------------ 0x00415300
    void set_stream() {
        c_set_stream.hit();
        if (jo::self<jo::AnimSet>(this)->stream_state == 0) return;
        if (jo::self<jo::AnimSet>(this)->stream_state == 1) {
            for (std::int32_t i = 1; i <= jo::self<jo::AnimSet>(this)->group_count; ++i) {
                std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(this)->entry(joan::orig::arg::u32(i)));  // 0x004168d0
                const std::int32_t id = jo::as_anim_group_rec(e)->anim_id;
                joan::orig::as_anim_res_mgr(obj(kAnimRes))->add_ref(joan::orig::arg::i16(static_cast<std::int16_t>(id)));  // 0x00417af0
                if (jo::as_anim_group_rec(e)->model_ref != 0) {
                    void* const m = joan::orig::arg::as<void*>(joan::orig::as_model_refs(obj(kModelRefs))->get(joan::orig::arg::i16(reinterpret_cast<const std::int16_t&>(jo::as_anim_group_rec(e)->model_ref))));  // 0x00416f20
                    joan::orig::as_model_slot(m)->add_ref();  // 0x00419cf0
                }
            }
            jo::self<jo::AnimSet>(this)->stream_state = 2;
            return;
        }
        if (jo::self<jo::AnimSet>(this)->stream_state != 2) return;
        std::int32_t pending_anims = 0, pending_models = 0;
        const std::uint32_t start = joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(obj(kFiles))->get_time_591050());  // 0x00591050
        const std::uint32_t limit = 5;
        std::int32_t timed_out = 0;
        for (std::int32_t i = 1; i <= jo::self<jo::AnimSet>(this)->group_count; ++i) {
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(this)->entry(joan::orig::arg::u32(i)));
            const std::int32_t id = jo::as_anim_group_rec(e)->anim_id;
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_res_mgr(obj(kAnimRes))->idle(joan::orig::arg::i16(static_cast<std::int16_t>(id)))) == 0) {  // 0x00417db0
                if (timed_out == 0) {
                    joan::orig::as_anim_res_mgr(obj(kAnimRes))->stream(joan::orig::arg::i16(static_cast<std::int16_t>(id)));  // 0x00417b60
                    if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(obj(kFiles))->get_time_591050()) - start > limit) timed_out = 1;
                }
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_res_mgr(obj(kAnimRes))->idle(joan::orig::arg::i16(static_cast<std::int16_t>(id)))) != 0) {
                    if (sound_kind(jo::as_anim_group_rec(e)->kind))
                        joan::orig::as_anim_res_mgr(obj(kAnimRes))->bind_bip(joan::orig::arg::i16(static_cast<std::int16_t>(id)), joan::orig::arg::str(jo::as_anim_group_rec(e)->bip_name));  // 0x00417f20
                } else {
                    ++pending_anims;
                }
            }
            if (jo::as_anim_group_rec(e)->model_ref != 0) {
                void* m = joan::orig::arg::as<void*>(joan::orig::as_model_refs(obj(kModelRefs))->get(joan::orig::arg::i16(reinterpret_cast<const std::int16_t&>(jo::as_anim_group_rec(e)->model_ref))));
                if (joan::orig::arg::as<std::int32_t>(joan::orig::as_model_slot(m)->idle()) == 0) {  // 0x00419ff0
                    if (timed_out == 0) {
                        m = joan::orig::arg::as<void*>(joan::orig::as_model_refs(obj(kModelRefs))->get(joan::orig::arg::i16(reinterpret_cast<const std::int16_t&>(jo::as_anim_group_rec(e)->model_ref))));
                        joan::orig::as_model_slot(m)->stream();  // 0x00419d50
                        if (joan::orig::arg::as<std::uint32_t>(joan::orig::as_misc(obj(kFiles))->get_time_591050()) - start > limit) timed_out = 1;
                    }
                    m = joan::orig::arg::as<void*>(joan::orig::as_model_refs(obj(kModelRefs))->get(joan::orig::arg::i16(reinterpret_cast<const std::int16_t&>(jo::as_anim_group_rec(e)->model_ref))));
                    if (joan::orig::arg::as<std::int32_t>(joan::orig::as_model_slot(m)->idle()) == 0) ++pending_models;
                }
            }
        }
        if (pending_anims == 0 && pending_models == 0) jo::self<jo::AnimSet>(this)->stream_state = 0;
    }

    // ------------------------------------------------------------------ 0x00415090
    std::int32_t set_acquire() {
        c_set_acquire.hit();
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set(this)->idle()) == 0) {  // 0x004156a0
            sleep_5();
            joan::orig::as_anim_set(this)->stream();  // 0x00415300
        }
        ++jo::self<jo::AnimSet>(this)->ref_count;
        if (jo::self<jo::AnimSet>(this)->ref_count == 1) {
            for (std::int32_t i = 1; i <= jo::self<jo::AnimSet>(this)->group_count; ++i) {
                std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(this)->entry(joan::orig::arg::u32(i)));  // 0x004168d0
                const std::int32_t id = jo::as_anim_group_rec(e)->anim_id;
                joan::orig::as_anim_set(obj(kAnimRes))->acquire_417540(joan::orig::arg::u32(static_cast<std::int16_t>(id)));  // 0x00417540
                if (sound_kind(jo::as_anim_group_rec(e)->kind))
                    joan::orig::as_anim_res_mgr(obj(kAnimRes))->bind_bip(joan::orig::arg::i16(static_cast<std::int16_t>(id)), joan::orig::arg::str(jo::as_anim_group_rec(e)->bip_name));  // 0x00417f20
                if (jo::as_anim_group_rec(e)->model_ref != 0) {
                    void* const m = joan::orig::arg::as<void*>(joan::orig::as_model_refs(obj(kModelRefs))->get(joan::orig::arg::i16(reinterpret_cast<const std::int16_t&>(jo::as_anim_group_rec(e)->model_ref))));  // 0x00416f20
                    joan::orig::as_model_slot(m)->acquire();  // 0x00419ae0
                }
            }
        }
        return jo::self<jo::AnimSet>(this)->ref_count;
    }

    // ------------------------------------------------------------------ 0x00416ed0
    std::int32_t is_yes() {
        const std::int32_t c = jo::view<std::int8_t>(jo::self<jo::AnimGroupRec>(this)->u8_e);
        return (c == 0x59 || c == 0x79) ? 1 : 0;
    }

    void* entry_21b(std::int16_t i) { return reinterpret_cast<std::uint8_t*>(jo::self<jo::AnimBuf>(this)->data) + static_cast<std::int32_t>(i) * static_cast<std::int32_t>(sizeof(jo::ModelSlot)) - static_cast<std::int32_t>(sizeof(jo::ModelSlot)); }  // 0x00416f20
    std::int32_t idle() { return jo::self<jo::AnimSet>(this)->stream_state == 0 ? 1 : 0; }                                  // 0x004156a0

    // ------------------------------------------------------------------ scalar deleting destructors
    H* delete_520(std::uint32_t flags) {  // 0x00416640
        joan::orig::as_unit_draw(this)->destruct();  // 0x005201a0
        if ((flags & 1) != 0) game_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("AnimSet::stream", 0x00415300, &H::set_stream);
B("AnimSet::acquire", 0x00415090, &H::set_acquire);
B("Obj::is_yes", 0x00416ed0, &H::is_yes);
B("ModelRefs::get", 0x00416f20, &H::entry_21b);
B("AnimSet::idle", 0x004156a0, &H::idle);
B("Obj520::scalar_delete", 0x00416640, &H::delete_520);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b101_01.cpp (game/b101_01)
namespace {
namespace game_b101_01 {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::orig::helpers::op_delete;

struct H {
    // ------------------------------------------------------------------ 0x00415220
    void* scalar_dtor_415220(u32 flags) {
        joan::orig::as_order(this)->destruct();  // 0x004baba0
        if ((flags & 1) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_01", name, addr, fn)
B("Obj::scalar_dtor_415220", 0x00415220, &H::scalar_dtor_415220);
#undef B

}  // namespace game_b101_01
}  // namespace

// ---------------------------------------- from modules/game_b84_05.cpp (game/b84_05)
namespace {
namespace game_b84_05 {

struct H {
    std::int32_t animset_add_ref() {  // 0x004152b0
        jo::self<jo::AnimSet>(this)->ref_count = jo::self<jo::AnimSet>(this)->ref_count + 1;
        if (jo::self<jo::AnimSet>(this)->ref_count == 1) jo::self<jo::AnimSet>(this)->stream_state = 1;
        return jo::self<jo::AnimSet>(this)->ref_count;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_05", name, addr, fn)
B("AnimSet::add_ref", 0x004152b0, &H::animset_add_ref);
#undef B

}  // namespace game_b84_05
}  // namespace

// ---------------------------------------- from modules/game_b88_05.cpp (game/b88_05)
namespace {
namespace game_b88_05 {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::img::obj;

void* const kArrays = obj(d::g_misc);     // 0x00591070 remove, 0x005911a0 search, 0x00590660 random

JOAN_COUNTER(c_squad_ctor, "Squad::Squad");

struct H {
    // ------------------------------------------------------------ 0x004155e0
    void* squad_ctor() {
        c_squad_ctor.hit();
        jo::Unit* const self = jo::self<jo::Unit>(this);
        joan::orig::as_unit_stats(&self->stats_block)->construct();  // 0x004cea10
        joan::orig::as_upgrade_list(&self->upgrades)->construct();  // 0x00531140
        joan::orig::as_path(&self->path)->construct();  // 0x0066e4b0
        joan::orig::as_path(&self->path_b)->construct();  // 0x0066e4b0
        // memset(this, 0, sizeof Unit) except the two point lists (0x4b4 / 0x41 on x86)
        joan::orig::fill_except(joan::orig::arg::mptr(kArrays), joan::orig::arg::str(static_cast<void*>(self)), 0, static_cast<std::uint32_t>(sizeof(jo::Unit)), 2, // 0x005919a0
                                  static_cast<void*>(&self->path), static_cast<std::int32_t>(sizeof self->path), static_cast<void*>(&self->path_b), static_cast<std::int32_t>(sizeof self->path_b));
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b88_05", name, addr, fn)
B("Squad::Squad", 0x004155e0, &H::squad_ctor);
#undef B

}  // namespace game_b88_05
}  // namespace

// ---------------------------------------- from modules/game_b82_16.cpp (game/b82_16)
namespace {
namespace game_b82_16 {

using joan::call::thiscall;

using joan::hook::global;

using joan::img::obj;

JOAN_COUNTER(c_res_release, "ResLock::release");

void sleep_ex() {
    reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(std::uint32_t, std::int32_t)>(global<std::uintptr_t>(d::iat_KERNEL32_SleepEx))(5u, 1);
}

struct H {
    // ------------------------------------------------------------------ 0x004156d0
    std::int32_t res_release() {
        c_res_release.hit();
        while (joan::orig::arg::as<std::int32_t>(joan::orig::as_anim_set(this)->idle()) == 0) {  // 0x004156a0
            sleep_ex();
            joan::orig::as_anim_set(this)->stream();  // 0x00415300
        }
        jo::self<jo::AnimSet>(this)->ref_count -= 1;
        if (jo::self<jo::AnimSet>(this)->ref_count == 0) {
            for (std::int32_t i = jo::self<jo::AnimSet>(this)->group_count; i > 0; --i) {
                std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(this)->entry(joan::orig::arg::u32(i)));  // 0x004168d0
                if (jo::as_anim_group_rec(e)->model_ref != 0)
                    joan::orig::as_model_slot(joan::orig::arg::as<void*>(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16(reinterpret_cast<const std::int16_t&>(jo::as_anim_group_rec(e)->model_ref)))))->release();  // 0x00416f20 / 0x0041a020
                joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->release(joan::orig::arg::i16(reinterpret_cast<const std::int16_t&>(jo::as_anim_group_rec(e)->anim_id)));  // 0x00417e00
            }
        }
        return jo::self<jo::AnimSet>(this)->ref_count;
    }
};

#define B16(name, addr, fn) JOAN_REPLACE("game/b82_16", name, addr, fn)
B16("ResLock::release", 0x004156d0, &H::res_release);
#undef B16

}  // namespace game_b82_16
}  // namespace

// ---------------------------------------- from modules/game_b103_01.cpp (game/b103_01)
namespace {
namespace game_b103_01_3 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::obj;

// ---- free functions ----
u32 JOAN_CDECL atexit_416ce0() {
    joan::orig::as_image(obj(d::g_a56ef0))->dtor();         // 0x0060fb10
    return joan::orig::arg::as<u32>(joan::orig::as_image(obj(d::g_vga_back))->dtor());  // 0x0060fb10
}

struct H {
    // ------------------------------------------------------------------ 0x00415790
    std::uintptr_t dtor_415790() {
        jo::self<jo::MapObject>(this)->vtable = img::obj(d::vt_Obj_9d3d78);
        return joan::orig::arg::as<u32>(joan::orig::as_unit(this)->release_4251b0());  // 0x004251b0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_01", name, addr, fn)
B("Obj::dtor_415790", 0x00415790, &H::dtor_415790);
B("atexit_416ce0", 0x00416ce0, &atexit_416ce0);
#undef B

}  // namespace game_b103_01_3
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {

using joan::call::thiscall;

struct H {
    // ---- destructors ---------------------------------------------------------------------
    void dtor_416580() {  // 0x00416580
        joan::orig::as_unit(this)->cleanup();                // 0x00461f10
        joan::orig::as_wrap(&jo::self<jo::Unit>(this)->path_b)->f_4f0500();      // 0x004f0500
        joan::orig::as_wrap(&jo::self<jo::Unit>(this)->path)->f_4f0500();      // 0x004f0500
        joan::orig::as_upgrade_list(&jo::self<jo::Unit>(this)->upgrades)->destruct();      // 0x00531170
    }
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj416580::~Obj416580", 0x00416580, &H::dtor_416580);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b98_01.cpp (game/b98_01)
namespace {
namespace game_b98_01 {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::orig::helpers::delete_;       // operator delete

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    H* sdtor(std::uintptr_t dtor, u32 flags) {
        thiscall(dtor, this);
        if ((flags & 1) != 0) delete_(this);
        return this;
    }
    H* sdtor_416600(u32 flags) { return sdtor(fn::code_value(fn::UnitData_dtor_body), flags); }  // 0x00495980
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_01", name, addr, fn)
B98("Obj::scalar_dtor_416600", 0x00416600, &H::sdtor_416600);
#undef B98

}  // namespace game_b98_01
}  // namespace

// ---------------------------------------- from modules/game_b107_07.cpp (game/b107_07)
namespace {
namespace game_b107_07 {

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::hook::global;

using joan::call::cdecl_call;

using joan::img::obj;

using i8 = std::int8_t;

using joan::orig::helpers::at;
using joan::img::str;

JOAN_COUNTER(c_unit_tick, "Unit::tick_status_416680");

struct H {
    // 0x00416680
    void tick_status_416680() {
        c_unit_tick.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->find_target_54b480(str(d::s_E_9cfc2c), 0x320, 0, 0, 0, 0)) != 0) {  // 0x0054b480
            jo::view<u32>(jo::self<jo::Unit>(this)->idle_time) = 2;
        } else {
            jo::view<u32>(jo::self<jo::Unit>(this)->idle_time) = 3;
        }
        if (global<u32>(d::g_control_mode) == 4) return;
        // `sub; cmp; jle`: wrapping 32-bit difference, signed compare
        if (static_cast<i32>(jo::view<u32>(jo::self<jo::Unit>(this)->hp_shown) - jo::view<u32>(jo::self<jo::Unit>(this)->hit_points)) > global<i32>(d::g_a3cc78)) {
            joan::orig::as_critical_hud(obj(d::g_critical_hud))->pick_random_combo();  // 0x00462970
            global<u8>(d::g_a4f838) = 1;
            jo::self<jo::Unit>(this)->hp_shown = jo::self<jo::Unit>(this)->hit_points;
        }
        if (jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry) != 0 && jo::self<jo::Unit>(this)->anim_set != nullptr && jo::self<jo::Unit>(this)->actor != nullptr) {
            u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::self<jo::Unit>(this)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Unit>(this)->anim_entry))));  // 0x004168d0
            const u32 v = jo::view<u32>(jo::as_anim_group_rec(r)->model_ref);
            if (v != 0) {
                // `mov ax, word [ebp-8]; push eax`: high half is still the pointer's
                const u32 arg = (reinterpret_cast<std::uintptr_t>(r) & 0xffff0000u) | (v & 0xffffu);
                u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16(arg)));  // 0x00416f20
                if (static_cast<i32>(jo::as_model_slot(rec)->emitter_flag) != 0 && static_cast<i32>(jo::as_model_slot(rec)->emitter_default) == 1) {
                    if (joan::crt::stricmp(joan::orig::arg::str(jo::as_model_slot(rec)->emitter_names), str(d::s_none)) != 0) {  // strcmp 'none'
                        const i32 id = jo::self<jo::Unit>(this)->id;
                        joan::orig::as_spawner(obj(d::g_particle_effect_array))->try_spawn(joan::orig::arg::i32(v), jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene, 0, joan::orig::arg::i32(id));  // 0x0041b640 (result unused)
                    }
                }
            }
        }
        u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_waters(obj(d::g_water_array))->find_at(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->position_), 0));  // 0x00469d10
        if (q == nullptr) return;
        // `test ah,1; je skip`: continue when [+0x16] < q+0xc or unordered
        if (reinterpret_cast<const float*>(jo::self<jo::Unit>(this)->position_)[2] >= jo::as_water(q)->box[2]) return;  // the region box z
        if (jo::view<i32>(jo::as_water(q)->u32_2c) < 2) return;
        if (global<u32>(d::g_now) - jo::self<jo::Unit>(this)->fx_last_ms <= 0x7d0u) return;
        struct {
            u32 x, y;
            float z;
        } pos;
        static_assert(sizeof(pos) == 12, "vector local");
        pos.x = reinterpret_cast<const u32*>(jo::self<jo::Unit>(this)->position_)[0];
        pos.y = reinterpret_cast<const u32*>(jo::self<jo::Unit>(this)->position_)[1];
        pos.z = jo::as_water(q)->box[2] + global<float>(d::flt_5);  // + 5.0f
        const u32 key = jo::as_water(q)->arr_24[jo::view<i32>(jo::as_water(q)->u32_2c) - 1];  // the last sound id
        u8* const ent = joan::orig::arg::as<u8*>(joan::orig::as_name_list(obj(d::g_pemi_res))->entry(joan::orig::arg::i32(key)));  // 0x0041b440
        void* const e24 = jo::as_pemi_script(ent)->effect;  // +0x24 on x86: the 0x0041b440 entry is a PemiScript (g_a527a8 is the PemiRes)
        jo::Scene* const snd = jo::as_actor(jo::self<jo::Unit>(this)->actor)->scene;
        joan::orig::as_model_list(obj(d::g_particle_effect_array))->add(snd, e24, &pos, img::obj(d::g_pos), static_cast<std::int8_t>(joan::orig::arg::i32(0)), 0, 1, 0x32);  // 0x0041baf0
        jo::self<jo::Unit>(this)->fx_last_ms = global<u32>(d::g_now);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_07", name, addr, fn)
B("Unit::tick_status_416680", 0x00416680, &H::tick_status_416680);
#undef B

}  // namespace game_b107_07
}  // namespace

// ---------------------------------------- from modules/game_b99_01.cpp (game/b99_01)
namespace {
namespace game_b99_01 {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::img::obj;

using joan::orig::helpers::delete_;       // operator delete

// ---------------------------------------------------------------------- 0x004168a0
static std::uintptr_t JOAN_CDECL static_dtor_4168a0() {
    joan::orig::as_image(obj(d::g_vga_back))->construct();          // 0x0060fad0
    return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_image(obj(d::g_a56ef0))->construct());   // 0x0060fad0
}

struct H {
    // ------------------------------------------------------------------ scalar deleting dtors
    H* sdtor(std::uintptr_t dtor, u32 flags) {
        thiscall(dtor, this);
        if ((flags & 1) != 0) delete_(this);
        return this;
    }
    H* sdtor_416ca0(u32 flags) { return sdtor(fn::code_value(fn::Obj_dtor_456ef0), flags); }  // 0x00456ef0
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_01", name, addr, fn)
B99("static_dtor_4168a0", 0x004168a0, &static_dtor_4168a0);
B99("Obj::scalar_dtor_416ca0", 0x00416ca0, &H::sdtor_416ca0);
#undef B99

}  // namespace game_b99_01
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    std::uint8_t* elem_4168d0(std::uint32_t i) {  // 0x004168d0
        return jo::view<std::uint8_t*>(jo::self<jo::AnimSet>(this)->groups) + (i * 0x99u - 0x99u);
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("elem_004168d0", 0x004168d0, &H::elem_4168d0);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00416a50
    void* ctor_416a50() {
        jo::self<jo::MapObject>(this)->vtable = img::obj(d::vt_Obj_9d3d78);
        memset_(&jo::self<jo::MapObject>(this)->index, 0, static_cast<u32>(sizeof(jo::MapObject) - offsetof(jo::MapObject, index)));  // all after the vtable (0xbe on x86)
        return self();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::ctor_416a50", 0x00416a50, &H::ctor_416a50);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using i8 = std::int8_t;

struct H {
    // ---------------------------------------------------------------- 0x00416d10
    u32 key_code(char c) {
        const i32 sx = static_cast<i8>(c);
        const u32 idx = static_cast<u32>(sx - 0x41);
        if (idx > 0x29) return (static_cast<u32>(sx) & 0xffffff00u) | 0xffu;  // or al,0xff on movsx
        switch (idx) {
        case 0x00: case 0x01: case 0x02: case 0x20: case 0x21: case 0x22: return 0x1c;
        case 0x04: case 0x24: return 0x1e;
        case 0x09: case 0x29: return 0x1a;
        default: return 0xff;  // eax = idx (< 0x2a), al = 0xff
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Key::code_416d10", 0x00416d10, &H::key_code);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- from modules/game_b89_06.cpp (game/b89_06)
namespace {
namespace game_b89_06 {

using joan::call::thiscall;

JOAN_COUNTER(c_index_of, "List::index_of");

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x00416dc0
    std::int32_t index_of(std::int32_t id) {
        c_index_of.hit();
        for (std::int32_t i = 1; i <= jo::self<jo::AnimSet>(this)->group_count; ++i) {
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_anim_set(self())->entry(joan::orig::arg::u32(i)));  // 0x004168d0
            if (jo::as_anim_group_rec(e)->kind == id) return i;
        }
        return 0;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b89_06", name, addr, fn)
M("List::index_of", 0x00416dc0, &H::index_of);
#undef M

}  // namespace game_b89_06
}  // namespace

// ---------------------------------------- from modules/game_b92_01.cpp (game/b92_01)
namespace {
namespace game_b92_01_3 {

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::orig::helpers::stricmp_;

JOAN_COUNTER(c_anim_find, "AnimTable::find");

struct H {
    // ------------------------------------------------------------ 0x00416e30 (ret 8)
    std::int32_t anim_find(std::int32_t kind, const char* name) {
        c_anim_find.hit();
        if (name == nullptr) return 0;
        for (std::int32_t i = 1; i <= jo::self<jo::AnimSet>(this)->group_count; ++i) {
            std::uint8_t* const e = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(this)->entry(joan::orig::arg::u32(i)));  // 0x004168d0
            if (jo::as_anim_group_rec(e)->kind != kind) continue;
            if (stricmp_(reinterpret_cast<const char*>(e + 8), name) == 0) return i;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_01", name, addr, fn)
B("AnimTable::find", 0x00416e30, &H::anim_find);
#undef B

}  // namespace game_b92_01_3
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
//
//    - byte setters 0x004d7e70 (+0x3c), 0x0042cf20 (+0x10a); getters 0x00497560 (+0xa),
//      0x004301c0 (byte +0x744), 0x004203c0 (+0), 0x00415280 (+0x112).
namespace {
namespace game_b82_18_2 {

struct H {
    std::uint32_t get_112() { return jo::view<std::uint32_t>(jo::self<jo::Database>(this)->header.rec_count); }                              // 0x00415280
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("get_dword_112", 0x00415280, &H::get_112);
#undef B82

}  // namespace game_b82_18_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::AnimSet::init(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::anim_entry_init>(this, a0)); }  // 0x414970
auto joan::orig::AnimSet::free_buffer() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b92_01::H::free_buffer>(this)); }  // 0x4149c0
auto joan::orig::AnimSet::load(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b107_16::H::load_414a30>(this, a0)); }  // 0x414a30
auto joan::orig::Unit::push_message(std::int32_t a0, std::int16_t a1, std::int16_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_01::H::push3>(this, a0, a1, a2)); }  // 0x414f50
auto joan::orig::Unit::find_message(std::int32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_01::H::find_pair>(this, a0)); }  // 0x414fe0
auto joan::orig::init_a3cda0() -> void { joan::bind::free<&::game_b96_01::init_a3cda0>(); }  // 0x415040
auto joan::orig::AnimSet::acquire() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::set_acquire>(this)); }  // 0x415090
auto joan::orig::Ai::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b101_01::H::scalar_dtor_415220>(this, a0)); }  // 0x415220
auto joan::orig::Database::rec_count() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_18_2::H::get_112>(this)); }  // 0x415280
auto joan::orig::AnimSet::add_ref() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_05::H::animset_add_ref>(this)); }  // 0x4152b0
auto joan::orig::AnimSet::stream() -> void { joan::bind::member<&::game_b83_07::H::set_stream>(this); }  // 0x415300
auto joan::orig::Squad::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b88_05::H::squad_ctor>(this)); }  // 0x4155e0
auto joan::orig::AnimSet::idle() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::idle>(this)); }  // 0x4156a0
auto joan::orig::AnimSet::release() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b82_16::H::res_release>(this)); }  // 0x4156d0
auto joan::orig::MapObject::destruct() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_01_3::H::dtor_415790>(this)); }  // 0x415790
auto joan::orig::Anim::is_jump(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b97_01::H::is_jump>(this, a0)); }  // 0x4157c0
auto joan::orig::Anim::id_from_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b106_10::H::anim_id>(this, a0)); }  // 0x415820
auto joan::orig::Unit::destruct() -> void { joan::bind::member<&::game_b85_06::H::dtor_416580>(this); }  // 0x416580
auto joan::orig::UnitDraw::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::delete_520>(this, a0)); }  // 0x416640
auto joan::orig::Unit::tick_status_416680() -> void { joan::bind::member<&::game_b107_07::H::tick_status_416680>(this); }  // 0x416680
auto joan::orig::static_dtor_4168a0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b99_01::static_dtor_4168a0>()); }  // 0x4168a0
auto joan::orig::AnimSet::entry(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18::H::elem_4168d0>(this, a0)); }  // 0x4168d0
auto joan::orig::AnimSet::weapon_kind_from_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b100_01::H::weapon_kind>(this, a0)); }  // 0x416910
auto joan::orig::MapObject::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b90_03::H::ctor_416a50>(this)); }  // 0x416a50
auto joan::orig::anim_kind_from_name(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_01_2::anim_kind_from_name>(a0)); }  // 0x416aa0
auto joan::orig::WorldItem::destroy(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_01::H::sdtor_416ca0>(this, a0)); }  // 0x416ca0
auto joan::orig::List::index_of_416dc0(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b89_06::H::index_of>(this, a0)); }  // 0x416dc0
auto joan::orig::AnimTable::find(std::int32_t a0, const char* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_01_3::H::anim_find>(this, a0, a1)); }  // 0x416e30
auto joan::orig::AnimSetEntry::is_yes() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b83_07::H::is_yes>(this)); }  // 0x416ed0
auto joan::orig::ModelRefs::get(std::int16_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::entry_21b>(this, a0)); }  // 0x416f20
