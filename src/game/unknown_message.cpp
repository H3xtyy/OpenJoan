// game/unknown_message.cpp: functions whose original source file is unknown (formerly game/_unplaced/message.cpp;
// re/attribution.csv: between ww/lightsys.cpp and ww/modelres.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cmath>
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../canvas_api.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::call::cdecl_call;

using joan::img::obj;

using joan::hook::global;

using i16 = std::int16_t;

using joan::img::str;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;
using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b94_02.cpp (game/b94_02)
namespace {
namespace game_b94_02 {


using joan::orig::helpers::memset_;  // memset

struct H {

    H* clear_5b() {
        memset_(this, 0, static_cast<u32>(sizeof(jo::Message)));  // 0x5b bytes on i686
        return this;
    }

    // ------------------------------------------------------------------------ 0x004af5d0
    u8 byte0() { return jo::self<jo::MessageIndicator>(this)->active; }  // this: the MessageIndicator 0x00a5ccf0
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_02", name, addr, fn)
B94("Block5b::clear", 0x004aa2e0, &H::clear_5b);
B94("Obj::byte0", 0x004af5d0, &H::byte0);
#undef B94

}  // namespace game_b94_02
}  // namespace

// ---------------------------------------- from modules/game_b87_04.cpp (game/b87_04)
namespace {
namespace game_b87_04 {

struct H {
    void wrap_4aa310() { joan::orig::as_message(this)->destruct(); }  // 0x004aa340
};

#define B(name, addr, fn) JOAN_REPLACE("game/b87_04", name, addr, fn)
B("Wrap::f_4aa310", 0x004aa310, &H::wrap_4aa310);
#undef B

}  // namespace game_b87_04
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {


void* const kTable5c = obj(d::g_sfx_player);

JOAN_COUNTER(c_release, "Obj4a::release");

struct H {
    // 0x004aa340
    void release() {
        c_release.hit();
        if (jo::view<void*>(jo::self<jo::Message>(this)->text) != nullptr) joan::crt::operator_delete(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Message>(this)->text)));  // operator delete
        if (jo::view<std::int32_t>(jo::self<jo::Message>(this)->sound_id) == 0) return;
        if (joan::orig::arg::as<std::int8_t>(joan::orig::as_sounds(kTable5c)->has_kind(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Message>(this)->sound_id)))) == 0) return;  // 0x004cfee0
        joan::orig::as_scenario(kTable5c)->trigger_slot(joan::orig::arg::i32(jo::view<std::int32_t>(jo::self<jo::Message>(this)->sound_id)));                                 // 0x004cecd0
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj4a::release", 0x004aa340, &H::release);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b106_05.cpp (game/b106_05)
namespace {
namespace game_b106_05 {


using joan::orig::helpers::game_new;  // operator new
i32 crt_strcmp_7c07b0(const void* a, const void* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
void* crt_strncpy(void* d, const void* s, u32 n) { return joan::orig::arg::as<void*>(std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n))); }
void* crt_memset(void* d, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

// Fixed 0x191-byte string class (0x005a1170 ctor, 0x005a1440 assign, 0x005a1550 append,
// 0x00417d90 c_str); no destructor.
struct alignas(4) Str {
    char b[0x194];
};

JOAN_COUNTER(c_msg, "Message::init");

// ============================================================================ 0x004aa3c0
struct Armoured {
    img::Addr key;           // name for 0x005270b0
    img::Addr lv[3], pic[3];  // armour names / portraits
};
struct Simple {
    img::Addr name, pic;
};
constexpr Armoured kAlencon{d::s_ALENCON_9cfb30, {d::s_ALENCON_ARMOR_LV1, d::s_ALENCON_ARMOR_LV2, d::s_ALENCON_ARMOR_LV3}, {d::s_ALENCON1, d::s_ALENCON2, d::s_ALENCON3}};
constexpr Armoured kLaHire{d::s_LaHire, {d::s_LAHIRE_ARMOR_LV1, d::s_LAHIRE_ARMOR_LV2, d::s_LAHIRE_ARMOR_LV3}, {d::s_LAHIRE1, d::s_LAHIRE2, d::s_LAHIRE3}};
constexpr Armoured kMetz{d::s_JEAN_DE_METZ, {d::s_JEAN_ARMOR_LV1, d::s_JEAN_ARMOR_LV2, d::s_JEAN_ARMOR_LV3}, {d::s_METZ1, d::s_METZ2, d::s_METZ3}};
// After "Duke of Alencon" (0x9d4c28), up to "Dunois".
constexpr Simple kGroup1[] = {
    {d::s_Pierre_Ossier, d::s_CITI3A}, {d::s_Renaud_Encrier, d::s_CITI3A}, {d::s_Renier_Chanevacier, d::s_CITI3A}, {d::s_Eudes_Masuier, d::s_CITI3A},
    {d::s_Capellarius, d::s_CITI1}, {d::s_The_Bastard_of_Orl_ans, d::s_BASTARD}, {d::s_Guillaume_de_Flavy, d::s_FLAVY}, {d::s_French_Archer_Captain, d::s_F_ARCCAP},
    {d::s_French_CrossbowMan_Captain, d::s_FCROSSC}, {d::s_French_Heavy_Infantry_Captai, d::s_FHINFCAP}, {d::s_French_Poleman_Captain, d::s_FPOLECAP}, {d::s_Sir_William_Glasdale, d::s_GLASDALE},
    {d::s_d_Illiers, d::s_FHINFCAP}, {d::s_Dunois, d::s_FHINFCAP},
};
// After "Jean de Metz" (0x9d49a4).
constexpr Simple kGroup2[] = {
    {d::s_Arthur_de_Richemont, d::s_RICHEMON}, {d::s_Regent_Bedford, d::s_Bedford}, {d::s_Lord_Talbot, d::s_TALBOT}, {d::s_William_de_la_Pole, d::s_WILLIAM},
    {d::s_Alexander_de_la_Pole, d::s_ALEXDLP}, {d::s_John_de_la_Pole, d::s_JOHNDLP}, {d::s_Marcel_Parcheminier, d::s_F_ARCCAP}, {d::s_Radulfus_Bradcroft, d::s_EHINFCAP},
    {d::s_Jacobus_Sprode, d::s_EHINFCAP}, {d::s_Nicholas_Weston, d::s_ELINFCAP}, {d::s_Triston_Walsoken, d::s_EMACECAP}, {d::s_Tower_Guard, d::s_F_ARCCAP},
    {d::s_Female_Citizen, d::s_CITI4}, {d::s_Renier_Balancier, d::s_flinfcap}, {d::s_Charles_VII, d::s_CHARLES_9d4790}, {d::s_Reiner_Mille, d::s_EARCCAP},
    {d::s_Luxembourg, d::s_LUX}, {d::s_WANDOMME, d::s_WANDOMME}, {d::s_HUDLEG, d::s_WANDOMME}, {d::s_MATTHEWGOUGH, d::s_GOUGH},
    {d::s_Perrinet, d::s_GRESSART},
};
constexpr img::Addr kCiti1 = d::s_CITI1;

void set_pic(void* self, img::Addr pic) { crt_strncpy(jo::as_message(self)->pic, str(pic), 8); }

void armoured_pic(void* self, const Armoured& a) {
    const i32 id = joan::orig::arg::as<i32>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(str(a.key))));  // 0x005270b0
    const i32 h = joan::orig::arg::as<i32>(joan::orig::as_units(obj(d::g_units))->find_by_3b(joan::orig::arg::i32(id)));            // 0x0051fd50
    if (h == 0) {
        set_pic(self, kCiti1);
        return;
    }
    const i16 armour = jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(h))))->stats[5];  // 0x0041b3c0
    for (int l = 0; l < 3; ++l) {
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(armour)));  // 0x00425ce0
        if (crt_strcmp_7c07b0(jo::view<u8*>(jo::as_item_def(rec)->name), str(a.lv[l])) == 0) {
            set_pic(self, a.pic[l]);
            return;
        }
    }
}

bool simple_group(void* self, const char* name, const Simple* g, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        if (crt_strcmp_7c07b0(name, str(g[i].name)) == 0) {
            set_pic(self, g[i].pic);
            return true;
        }
    }
    return false;
}

struct H {
    // ------------------------------------------------------------------------ 0x004aa3c0
    void* message_init(const char* text, const char* name) {
        c_msg.hit();
        jo::self<jo::Message>(this)->state = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->x) = 0x42180000;
        jo::view<u32>(jo::self<jo::Message>(this)->y) = 0x43cf0000;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[4]) = 0x437f0000;
        jo::view<u32>(jo::self<jo::Message>(this)->revealed_chars) = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[0]) = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[1]) = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[2]) = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[3]) = 0;
        if (name == nullptr) {
            jo::view<u8>(jo::self<jo::Message>(this)->has_pic) = 1;
            set_pic(this, d::s_M0000);  // "M0000"
        } else if (crt_strcmp_7c07b0(name, str(d::s_TMESS)) == 0) {  // "TMESS"
            jo::view<u8>(jo::self<jo::Message>(this)->has_pic) = 1;
            set_pic(this, d::s_TMESS);
        } else {
            if (crt_strcmp_7c07b0(name, str(d::s_Duke_of_Alencon)) == 0) {  // "Duke of Alencon"
                armoured_pic(this, kAlencon);
            } else if (simple_group(this, name, kGroup1, sizeof(kGroup1) / sizeof(kGroup1[0]))) {
            } else if (crt_strcmp_7c07b0(name, str(d::s_La_Hire)) == 0) {  // "La Hire"
                armoured_pic(this, kLaHire);
            } else if (crt_strcmp_7c07b0(name, str(d::s_Jean_de_Metz_9d49a4)) == 0) {  // "Jean de Metz"
                armoured_pic(this, kMetz);
            } else if (simple_group(this, name, kGroup2, sizeof(kGroup2) / sizeof(kGroup2[0]))) {
            } else {
                set_pic(this, kCiti1);
            }
            jo::view<u8>(jo::self<jo::Message>(this)->has_pic) = 1;
        }
        Str s;
        joan::orig::as_string(&s)->construct();  // 0x005a1170
        if (jo::self<jo::Message>(this)->has_pic != 0 && name != nullptr && crt_strcmp_7c07b0(jo::self<jo::Message>(this)->pic, str(d::s_TMESS)) != 0) {
            joan::orig::as_string(&s)->assign(str(d::s_us_9cf774));            // 0x005a1440
            joan::orig::as_string(&s)->append(joan::orig::arg::str(name));    // 0x005a1550
            joan::orig::as_string(&s)->append(str(d::s_colon_us_sp));                // ":_ "
            joan::orig::as_string(&s)->append(joan::orig::arg::str(text));
        } else {
            joan::orig::as_string(&s)->assign(joan::orig::arg::str(text));
        }
        const u32 now = global<u32>(d::g_now);
        if (global<u32>(d::g_attention_call_last_key) == now) {
            jo::self<jo::Message>(this)->priority_ = now + 1;
        } else if (global<u32>(d::g_attention_call_last_key) > now) {
            jo::self<jo::Message>(this)->priority_ = global<u32>(d::g_attention_call_last_key) + 1;
        } else {
            jo::self<jo::Message>(this)->priority_ = now;
        }
        global<u32>(d::g_attention_call_last_key) = jo::self<jo::Message>(this)->priority_;
        jo::self<jo::Message>(this)->start_time = global<u32>(d::g_now);
        const u32 len = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str()))));  // strlen(c_str)
        jo::self<jo::Message>(this)->len = static_cast<i16>(len);
        jo::view<void*>(jo::self<jo::Message>(this)->text) = game_new(static_cast<u32>(static_cast<i32>(jo::self<jo::Message>(this)->len) + 1));
        crt_memset(jo::view<void*>(jo::self<jo::Message>(this)->text), 0, static_cast<u32>(static_cast<i32>(jo::self<jo::Message>(this)->len) + 1));
        const i32 n = jo::self<jo::Message>(this)->len;
        const char* const cs = joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str());  // 0x00417d90
        return crt_strncpy(jo::view<void*>(jo::self<jo::Message>(this)->text), cs, static_cast<u32>(n));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_05", name, addr, fn)
B("Message::init", 0x004aa3c0, &H::message_init);
#undef B

}  // namespace game_b106_05
}  // namespace

// ---------------------------------------- from modules/game_b101_04.cpp (game/b101_04)
namespace {
namespace game_b101_04 {

struct Obj {};

// Virtual thiscall through the object's vtable slot (byte offset).
using joan::orig::helpers::virtual_call;

void* const kArmies = obj(d::g_nations);
void* const kIds = obj(d::g_units);
void* const kKinds = obj(d::g_types);

i32 army_missing(i32 id) { return joan::orig::arg::as<i32>(joan::orig::as_list(kArmies)->is_null(joan::orig::arg::i32(id))); }  // 0x00484870
u8* army_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kArmies)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0047b600
// Volatile word read: the original dereferences a possibly-null pointer here.
i32 word_at(const void* p) { return *reinterpret_cast<const volatile i16*>(p); }

JOAN_COUNTER(c_unit_of, "Region::has_unit_of");
JOAN_COUNTER(c_side_ratio, "Army::side_ratio");

// The +0x1ee list tail shared by 0x4a8c20 / 0x4ab020. Returns true with *result when the
// original returns from inside the loop.
bool list_tail(u8* p, i32& cnt) {
    while (p != nullptr) {
        if (static_cast<i32>(jo::as_facility(p)->kind) == 1) {
            if (static_cast<i32>(jo::as_building(virtual_call<u8*>(p, 0x18))->unit_kind) != 0) cnt = static_cast<i32>(static_cast<u32>(cnt) + 1u);  // slot 0x18: the building
            for (i32 j = 0; j < static_cast<i32>(jo::as_building(virtual_call<u8*>(p, 0x18))->queue_n); ++j) {
                u8* const d = virtual_call<u8*>(p, 0x18);
                cnt = static_cast<i32>(static_cast<u32>(cnt) + static_cast<u32>(static_cast<i32>(jo::as_building(d)->queue_count[j])));
                if (cnt != 0) return true;
            }
        }
        p = jo::view<u8*>(jo::as_facility(p)->prev);
    }
    return false;
}

struct H {
    // Box-scan filter shared by 0x4a8c20 / 0x4ab020: kind record of a unit that passes, or null.
    u8* scan_kind(u8* e) {
        if (!(jo::as_unit(e)->hit_points > 0 || jo::view<u32>(jo::as_unit(e)->state_120) != 0)) return nullptr;
        if ((joan::orig::arg::as<u32>(joan::orig::as_aabb(jo::self<jo::Place>(this)->box)->contains(joan::orig::arg::ptr(jo::as_unit(e)->position_))) & 0xffu) == 0) return nullptr;  // 0x0060d5f0
        u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<u32>(jo::view<u16>(jo::as_unit(e)->type_id)))));  // 0x0042d0a0
        const i32 s = jo::as_unit_type(t)->unit_class;
        if (s == 0 || s == 8 || s == 9) return nullptr;
        const i32 k = jo::as_unit_type(t)->kind;
        if (k == 7 || k == 2 || k == 3) return nullptr;
        return t;
    }

    // ------------------------------------------------------------------ 0x004ab020
    i32 has_unit_of(const char* id) {
        c_unit_of.hit();
        if (id == nullptr) return 0;
        const i32 want = joan::orig::arg::as<i32>(joan::orig::as_list(kArmies)->find_by_word2(joan::orig::arg::i32(joan::orig::arg::as<i32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(id)))));  // 0x004b67e0(0x004b91a0)
        if (jo::view<void*>(jo::self<jo::Place>(this)->world) == nullptr) return 0;
        i32 cnt = 0;
        u8* owner = nullptr;
        if (army_missing(jo::self<jo::Place>(this)->nation) == 0) owner = army_get(jo::self<jo::Place>(this)->nation);
        i32 n = 0;
        void** const cells = joan::orig::arg::as<void**>(joan::orig::as_world(jo::view<void*>(jo::self<jo::Place>(this)->world))->cells_in_box(joan::orig::arg::mptr(jo::self<jo::Place>(this)->box), &n));  // 0x00429090
        for (i32 i = 0; i < n; ++i) {
            u8* const first = joan::orig::arg::as<u8*>(joan::orig::as_world_cell(cells[i])->get_8c());  // 0x0054ee10
            if (first == nullptr) continue;
            u8* e = first;
            do {
                if (scan_kind(e) != nullptr && static_cast<i32>(jo::as_unit(e)->side) == want) return 1;
                e = jo::view<u8*>(jo::as_unit(e)->cell_prev);
            } while (e != nullptr && e != first);
        }
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_place(this)->facility_head());  // 0x004c48b0
        if (word_at(owner) != want) return cnt;
        if (list_tail(p, cnt)) return 1;
        return cnt;
    }

    // ------------------------------------------------------------------ 0x004af5f0
    float side_ratio(i32 id) {
        c_side_ratio.hit();
        if (joan::orig::as_dynarray(kIds)->empty_at(joan::orig::arg::i32(id)) != 0) return global<float>(d::flt_1);  // 0x00428b70
        u32 a = 0, b = 0;
        u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_unit(joan::orig::as_dynarray(kIds)->ptr_at(joan::orig::arg::i32(id)))->nation());  // 0x00509400(0x0041b3c0)
        if (q == nullptr) return global<float>(d::flt_1);
        for (i32 i = 1; i <= joan::orig::as_dynarray(kArmies)->size(); ++i) {  // 0x00417ad0
            if (army_missing(i) != 0) continue;
            u8* const u = army_get(i);
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_player(q)->relation(static_cast<i32>(jo::as_nation(u)->id)));  // 0x004b42e0
            if (static_cast<i32>(jo::as_nation(u)->id) == static_cast<i32>(jo::as_nation(q)->id) ||
                joan::orig::arg::as<i32>(joan::orig::as_relation(t)->is_kind_3_or_4()) != 0)  // 0x0042f5d0
                a += jo::view<u32>(jo::as_nation(u)->military_strength);
            else
                b += jo::view<u32>(jo::as_nation(u)->military_strength);
        }
        const double zero = global<float>(d::k_9cf5c4);
        if (static_cast<double>(static_cast<i32>(a)) == zero && static_cast<double>(static_cast<i32>(b)) == zero)
            return global<float>(d::flt_0_5);
        float r = static_cast<float>(Fpu{true}.div(static_cast<double>(static_cast<i32>(b)), static_cast<double>(static_cast<i32>(a + b))));
        if (!(r >= global<float>(d::k_9cf5c4))) r = 0.0f;   // test ah,1: less or unordered
        if (r > global<float>(d::flt_1)) r = 1.0f;       // test ah,0x41 jne skip
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_04", name, addr, fn)
B("Region::has_unit_of", 0x004ab020, &H::has_unit_of);
B("Army::side_ratio", 0x004af5f0, &H::side_ratio);
#undef B

}  // namespace game_b101_04
}  // namespace

// ---------------------------------------- from modules/game_b106_10.cpp (game/b106_10)
namespace {
namespace game_b106_10 {


JOAN_COUNTER(c_speaker_setup, "Speaker::setup_4ab330");

i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
std::uintptr_t crt_strncpy(void* d, const char* s, i32 n) { return joan::orig::arg::as<u32>(std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n))); }
std::uintptr_t crt_strcpy(void* d, const char* s) { return joan::orig::arg::as<u32>(std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s))); }

// ---------------------------------------------------------------- 0x004ab330
struct Portrait {
    img::Addr name;      // compared with _stricmp against the unit type name
    img::Addr fallback;  // strncpy(8) target (or strcpy for hero entries with copy_fallback)
    img::Addr key;       // hero: name passed to 0x005270b0; img::kNoAddr = plain entry
    img::Addr armour[3];
    img::Addr portrait[3];
    bool copy_fallback;  // hero fallback uses strcpy (0x7b2020) instead of strncpy(.., 8)
};
const Portrait kPortraits[] = {
    {d::s_ALENCON_9cfb30, d::s_ALENCON1, d::s_ALENCON_9cfb30, {d::s_ALENCON_ARMOR_LV1, d::s_ALENCON_ARMOR_LV2, d::s_ALENCON_ARMOR_LV3}, {d::s_ALENCON1, d::s_ALENCON2, d::s_ALENCON3}, true},
    {d::s_ORLEAN, d::s_BASTARD, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FLAVY, d::s_FLAVY, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_BEGLEY, d::s_FLAVY, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_BEDFORD, d::s_BEDFORD, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_POLEMAN_CAPTAIN, d::s_EPOLECAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_LIGHT_INFANTRY_CAPTA, d::s_ELINFCAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_HEAVY_INFANTRY_CAPTA, d::s_EHINFCAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_MACEMAN_CAPTAIN, d::s_EMACECAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_LIGHT_INFANTRY, d::s_ELINF, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_SPY, d::s_ELINF, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_BUR_LIGHTINFANTRY, d::s_BLINF, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_BUR_LIGHTINFANTRY_CAPTAIN, d::s_BLINFC, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_BUR_ARCHER_CAPTAIN, d::s_BLINFC, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_ARCHER_CAPTAIN, d::s_F_ARCCAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_CROSSBOW_MAN_CAPTAIN, d::s_FCROSSC, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_HEAVY_INFANTRY_CAPTAI, d::s_FHINFCAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_DUNOIS, d::s_FHINFCAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_DLLLIER, d::s_FHINFCAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_LIGHT_INFANTRY_CAPTAI, d::s_FLINFCAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_POLEMAN_CAPTAIN, d::s_FPOLECAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_ARCHER, d::s_F_ARCH, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_CROSSBOW_MAN, d::s_FCROSS, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_HEAVY_INFANTRY, d::s_FHINF, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_LIGHT_INFANTRY, d::s_FLINF, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_FRENCH_POLEMAN, d::s_FPOLE, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_WILLIAM_GLASDALE, d::s_GLASDALE, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_LAHIRE_9cfb3c, d::s_LAHIRE1, d::s_LAHIRE_9cfb3c, {d::s_LAHIRE_ARMOR_LV1, d::s_LAHIRE_ARMOR_LV2, d::s_LAHIRE_ARMOR_LV3}, {d::s_LAHIRE1, d::s_LAHIRE2, d::s_LAHIRE3}, false},
    {d::s_JEAN_DE_METZ, d::s_METZ1, d::s_JEAN_DE_METZ, {d::s_JEAN_ARMOR_LV1, d::s_JEAN_ARMOR_LV2, d::s_JEAN_ARMOR_LV3}, {d::s_METZ1, d::s_METZ2, d::s_METZ3}, false},
    {d::s_JEAN_DE_METZ_2, d::s_METZ1, d::s_JEAN_DE_METZ, {d::s_JEAN_ARMOR_LV1, d::s_JEAN_ARMOR_LV2, d::s_JEAN_ARMOR_LV3}, {d::s_METZ1, d::s_METZ2, d::s_METZ3}, false},
    {d::s_ARTHUR_DE_RICHEMONT, d::s_RICHEMON, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_LORD_SCALES, d::s_SCALES, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_JOHN_FASTOLF, d::s_FASTOLF, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_MACEMAN_CAPTAIN, d::s_EMACECAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_MACEMAN, d::s_EMACE, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_LORD_TALBOT, d::s_TALBOT, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_PRIEST_01, d::s_C_PRIEST, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_POTION_SHOPKEEPER, d::s_SHOPPE, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_POTION_SHOPKEEPER_2, d::s_SHOPPE, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_WEAPON_SHOPKEEPER, d::s_BLACKSM, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_WEAPON_SHOPKEEPER_2, d::s_BLACKSM, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_william_de_la_pole, d::s_WILLIAM, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ALEXANDER, d::s_ALEXDLP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_John_de_la_pole, d::s_JOHNDLP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_Luxembourg, d::s_LUX, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_MATTHEWGOUGH, d::s_GOUGH, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_Perrinet, d::s_GRESSART, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_Philip_the_good, d::s_phillp, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_WANDOMME, d::s_WANDOMME, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_01, d::s_CITI1, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_01_2, d::s_CITI1, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_02B, d::s_CITI1A, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_02B_2, d::s_CITI1A, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_02A, d::s_CITI2B, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_02A_2, d::s_CITI2B, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_03, d::s_CITI3A, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_03_2, d::s_CITI3A, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_04A, d::s_CITI4, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_04A_2, d::s_CITI4, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_04B, d::s_CITI4B, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CITIZEN_04B_2, d::s_CITI4B, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_CHARLIE7, d::s_CHARLES_9d4790, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_PRISONER_01, d::s_CITI1, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_COUNCIL_01, d::s_CITI1, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_ARCHER_CAPTAIN, d::s_EARCCAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_ENGLISH_EX_ARCHER_CAPTAIN, d::s_EARCCAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_MER_POLEMAN, d::s_MPOLE, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_MER_POLEMAN_CAPTIAN, d::s_MPOLECAP, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_MER_LIGHT_INFANTRY_CAPTAIN, d::s_MLTINFC, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
    {d::s_MER_LIGHT_INFANTRY, d::s_MLTINF, img::kNoAddr, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, {img::kNoAddr, img::kNoAddr, img::kNoAddr}, false},
};

constexpr img::Addr kUnits = d::g_units;
constexpr img::Addr kTypes = d::g_types;
constexpr img::Addr kArmour = d::g_item_res;

const char* fs_cstr(void* s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(s)->c_str()); }  // 0x00417d90

void speaker_pick_portrait(u8* self, void* name) {
    char* dst = jo::as_message(self)->pic;
    for (const Portrait& e : kPortraits) {
        if (crt_stricmp(fs_cstr(name), str(e.name)) != 0) continue;
        if (e.key == img::kNoAddr) {
            crt_strncpy(dst, str(e.fallback), 8);
            return;
        }
        u32 type = joan::orig::arg::as<u32>(joan::orig::as_unit_types(obj(kTypes))->find(joan::orig::arg::str(str(e.key))));  // 0x005270b0
        u32 unit = joan::orig::arg::as<u32>(joan::orig::as_units(obj(kUnits))->find_by_3b(joan::orig::arg::i32(type)));        // 0x0051fd50
        if (unit != 0) {
            u8* rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(unit)));  // 0x0041b3c0
            i16 level = jo::as_unit(rec)->stats[5];
            for (int k = 0; k < 3; ++k) {
                u8* a = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(kArmour))->get(joan::orig::arg::i16(level)));  // 0x00425ce0
                if (crt_stricmp(jo::view<const char*>(jo::as_item_def(a)->name), str(e.armour[k])) == 0) {
                    crt_strncpy(dst, str(e.portrait[k]), 8);
                    break;
                }
            }
        } else if (e.copy_fallback) {
            crt_strcpy(dst, str(e.fallback));
        } else {
            crt_strncpy(dst, str(e.fallback), 8);
        }
        return;
    }
    crt_strncpy(dst, str(d::s_FLINF), 8);  // "FLINF"
}

struct H {
    // 0x004ab330
    std::uintptr_t speaker_setup(const char* text, u32 id) {
        c_speaker_setup.hit();
        u8* self = reinterpret_cast<u8*>(this);
        jo::self<jo::Message>(this)->state = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->x) = 0x42180000;
        jo::view<u32>(jo::self<jo::Message>(this)->y) = 0x43cf0000;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[4]) = 0x437f0000;
        jo::view<u32>(jo::self<jo::Message>(this)->revealed_chars) = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[0]) = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[1]) = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[2]) = 0;
        jo::view<u32>(jo::self<jo::Message>(this)->colour[3]) = 0;
        if (joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id))) != 0) {  // 0x00428b70
            jo::view<u8>(jo::self<jo::Message>(this)->has_pic) = 1;
            crt_strncpy(jo::self<jo::Message>(this)->pic, str(d::s_M0000), 8);  // "M0000"
        } else {
            u8* rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
            u8* type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(kTypes))->get(joan::orig::arg::i16(jo::as_unit(rec)->type_id)));  // 0x0042d0a0
            alignas(4) char name[0x1b4];
            joan::orig::as_string(name)->construct(joan::orig::arg::str(jo::as_unit_type(type)->name));  // 0x005a11a0 fixed string from char*
            speaker_pick_portrait(self, name);
            jo::view<u8>(jo::self<jo::Message>(this)->has_pic) = 1;
        }
        alignas(4) char s[0x1b4];
        joan::orig::as_string(s)->construct();  // 0x005a1170 empty fixed string
        if (jo::self<jo::Message>(this)->has_pic != 0 && joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id))) == 0) {
            joan::orig::as_string(s)->assign(str(d::s_us_9cf774));                         // 0x005a1440 assign
            u8* rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
            joan::orig::as_string(s)->append(joan::orig::arg::str(jo::as_unit(rec)->name_));  // 0x005a1550
            joan::orig::as_string(s)->append(str(d::s_colon_us_sp));                             // ":_ "
            joan::orig::as_string(s)->append(joan::orig::arg::str(text));
        } else {
            joan::orig::as_string(s)->assign(joan::orig::arg::str(text));  // 0x005a1440
        }
        u32 cur = global<u32>(d::g_now);
        if (global<u32>(d::g_attention_call_last_key) == cur)
            jo::self<jo::Message>(this)->priority_ = cur + 1;
        else if (global<u32>(d::g_attention_call_last_key) > cur)
            jo::self<jo::Message>(this)->priority_ = global<u32>(d::g_attention_call_last_key) + 1;
        else
            jo::self<jo::Message>(this)->priority_ = cur;
        global<u32>(d::g_attention_call_last_key) = jo::self<jo::Message>(this)->priority_;
        jo::self<jo::Message>(this)->start_time = global<u32>(d::g_now);
        i32 len = joan::orig::arg::as<i32>(std::strlen(joan::orig::arg::str(fs_cstr(s))));  // strlen
        jo::self<jo::Message>(this)->len = static_cast<i16>(len);
        jo::self<jo::Message>(this)->text = joan::orig::arg::as<char*>(joan::crt::operator_new(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Message>(this)->len) + 1)));  // malloc
        std::memset(joan::orig::arg::mptr(jo::self<jo::Message>(this)->text), 0, joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Message>(this)->len) + 1));  // memset
        i32 n = jo::self<jo::Message>(this)->len;
        const char* src = fs_cstr(s);
        return crt_strncpy(jo::self<jo::Message>(this)->text, src, n);
    }

  private:
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_10", name, addr, fn)
B("Speaker::setup", 0x004ab330, &H::speaker_setup);
#undef B

}  // namespace game_b106_10
}  // namespace

// ---------------------------------------- from modules/game_b104_03.cpp (game/b104_03)
namespace {
namespace game_b104_03 {


struct FVec3 {
    float x, y, z;
};
static_assert(sizeof(FVec3) == 12, "FVec3");

void* const kTypes = obj(d::g_item_res);
void* const kObjects = obj(d::g_items);
void* const kUnitTypes = obj(d::g_types);
void* const kModels = obj(d::g_physic_sys);

u8* types(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kTypes)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0 (reads the low word)
using joan::orig::helpers::crt_strcmp;

JOAN_COUNTER(c_place_4ac910, "Unit::place_near_4ac910");
JOAN_COUNTER(c_best_4ae710, "Obj::is_best_4ae710");

struct H {
    // ------------------------------------------------------------------ 0x004ac910
    bool hero_is(const char* name) {
        u8* const ty = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kUnitTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id))));  // 0x0042d0a0
        return crt_strcmp(jo::as_unit_type(ty)->name, name) == 0;
    }
    void release_model() {
        if (jo::self<jo::Unit>(this)->actor != nullptr && jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record) != 0)
            joan::orig::as_players(kModels)->set_active(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::self<jo::Unit>(this)->actor)->record)), joan::orig::arg::i16(0));  // 0x006ad980
    }
    FVec3 obj_dir(u8* o) {
        FVec3 d;
        d.x = reinterpret_cast<const float*>(jo::as_map_object(o)->actor->bone->matrix_)[0];
        d.y = reinterpret_cast<const float*>(jo::as_map_object(o)->actor->bone->matrix_)[1];
        d.z = reinterpret_cast<const float*>(jo::as_map_object(o)->actor->bone->matrix_)[2];
        return d;
    }
    void add_scaled(FVec3& pos, FVec3& dir, u32 scale_bits) {
        float s;
        std::memcpy(&s, &scale_bits, sizeof s);
        FVec3 tmp;
        FVec3* const r = joan::orig::arg::as<FVec3*>(joan::orig::vec_scale(&tmp, &dir, joan::orig::arg::f32(s)));  // 0x0060cd00
        FVec3 copy = *r;
        joan::orig::vec_add_in_place(&pos, &copy);  // 0x0060cdc0 (pos += copy)
    }
    void place_near_4ac910(u32 object_id, i16 mode) {
        c_place_4ac910.hit();
        if (joan::orig::as_units(kObjects)->list.empty_at(joan::orig::arg::i32(object_id)) != 0) return;  // 0x00425d50
        u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_units(kObjects)->list.ptr_at(joan::orig::arg::i32(object_id)));  // 0x00425d20
        const Fpu fpu{true};
        FVec3 pos;
        joan::orig::as_map_object(o)->position(joan::orig::arg::mptr(static_cast<void*>(&pos)));  // 0x00499930
        const float heading = joan::orig::arg::as<float>(joan::orig::as_map_object(o)->yaw());     // 0x00499e20 (fstp dword)
        if (static_cast<i32>(mode) != 0) {
            release_model();
            const double h = fpu.sub(static_cast<double>(heading), global<double>(d::dbl_3_141592653589793));  // - pi (qword)
            joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(static_cast<float>(h)));       // 0x004ff870
            const double h2 = fpu.div(fpu.sub(static_cast<double>(heading), global<double>(d::dbl_3_141592653589793)),
                                      global<double>(d::dbl_0_017453292519943295));        // / (pi/180) qword
            joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(static_cast<float>(h2)));      // 0x0050abd0
            FVec3 dir = obj_dir(o);
            u32 scale = 0x41b33d71u;
            if (hero_is(str(d::s_LAHIRE_9cfb3c))) {  // "LAHIRE"
                scale = 0x42070419u;
            } else if (hero_is(str(d::s_ALENCON_9cfb30))) {  // "ALENCON"
                scale = 0x41b61687u;
            } else if (hero_is(str(d::s_JEAN_DE_METZ))) {  // "JEAN_DE_METZ"
                scale = 0x41b61687u;
            }
            add_scaled(pos, dir, scale);
            joan::orig::as_ground_object(this)->set_position(pos);  // 0x004fedf0
            joan::orig::as_unit(this)->set_action(0x66, 0, 1);   // 0x00501190
            std::memcpy(jo::self<jo::Unit>(this)->height_point, &pos, sizeof pos);
        } else {
            release_model();
            jo::self<jo::Unit>(this)->u8_201 = 1;
            u8* const t = types(jo::as_map_object(o)->type_id);
            const bool odd = (static_cast<i32>(jo::view<i16>(jo::as_item_def(t)->stats[1])) / 0x16) % 2 != 0;
            FVec3 pos2;  // second read of the position / heading (the original calls both again)
            joan::orig::as_map_object(o)->position(joan::orig::arg::mptr(static_cast<void*>(&pos2)));  // 0x00499930
            const float heading2 = joan::orig::arg::as<float>(joan::orig::as_map_object(o)->yaw());     // 0x00499e20 (a float load)
            const double h = fpu.sub(static_cast<double>(heading2), global<double>(d::dbl_3_141592653589793));
            joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(static_cast<float>(h)));  // 0x004ff870
            const double h2 = fpu.div(fpu.sub(static_cast<double>(heading2), global<double>(d::dbl_3_141592653589793)),
                                      global<double>(d::dbl_0_017453292519943295));
            joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(static_cast<float>(h2)));  // 0x0050abd0
            FVec3 dir = obj_dir(o);
            u8* const t2 = types(jo::as_map_object(o)->type_id);
            u32 f54 = 0x412ab021u;
            u32 f58 = 0x4285c106u;
            u32 f5c = 0x41b33d71u;
            if (hero_is(str(d::s_LAHIRE_9cfb3c))) {  // "LAHIRE"
                f54 = odd ? 0x40be45a2u : 0x40b24dd3u;
                f58 = 0x429c2a7fu;
                f5c = 0x42070419u;
            } else if (hero_is(str(d::s_ALENCON_9cfb30))) {  // "ALENCON"
                f54 = odd ? 0x403cbc6au : 0x40463540u;
                f58 = 0x42a72d91u;
                f5c = 0x41b61687u;
            } else if (hero_is(str(d::s_JEAN_DE_METZ))) {  // "JEAN_DE_METZ"
                f54 = 0x3fd1a9fcu;
                f58 = 0x42aa0000u;
                f5c = 0x41b61687u;
            }
            add_scaled(pos2, dir, f5c);
            float a58, a54;
            std::memcpy(&a58, &f58, sizeof a58);
            std::memcpy(&a54, &f54, sizeof a54);
            double z = static_cast<double>(static_cast<i32>(jo::view<i16>(jo::as_item_def(t2)->stats[1])));  // fild
            z = fpu.sub(z, static_cast<double>(a58));
            z = fpu.sub(z, static_cast<double>(a54));
            z = fpu.add(z, static_cast<double>(pos2.z));
            pos2.z = static_cast<float>(z);
            joan::orig::as_ground_object(this)->set_position(pos2);  // 0x004fedf0
            std::memcpy(jo::self<jo::Unit>(this)->height_point, &pos2, sizeof pos2);
            joan::orig::as_unit(this)->set_action(0x60, 0, 1);  // 0x00501190
        }
        joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(0)), joan::orig::arg::mptr(0));  // 0x0050a480
        joan::orig::as_cell_object(this)->attach_unit(joan::orig::arg::i16(0), 0, static_cast<std::int8_t>(joan::orig::arg::i32(1)), joan::orig::arg::mptr(0));
    }

    // ------------------------------------------------------------------ 0x004ae710
    u8 is_best_4ae710(i32 index) {
        c_best_4ae710.hit();
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::MessageList>(this)->dirty)) == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(index))) != 0) return 0;  // 0x004af300
        u8* const me = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(index)));     // 0x004af2d0
        for (i32 i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));
            if (e == me) continue;
            const u32 a = joan::orig::arg::as<u32>(joan::orig::as_message(e)->priority());  // 0x004aef10
            if (a < joan::orig::arg::as<u32>(joan::orig::as_message(me)->priority())) return 0;
            const u32 b = joan::orig::arg::as<u32>(joan::orig::as_message(e)->priority());
            if (b == joan::orig::arg::as<u32>(joan::orig::as_message(me)->priority()) && jo::as_message(e)->y > jo::as_message(me)->y) return 0;
        }
        jo::self<jo::MessageList>(this)->dirty = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_03", name, addr, fn)
B("Unit::place_near_4ac910", 0x004ac910, &H::place_near_4ac910);
B("Obj::is_best_4ae710", 0x004ae710, &H::is_best_4ae710);
#undef B

}  // namespace game_b104_03
}  // namespace

// ---------------------------------------- from modules/game_b116_12.cpp (game/b116_12)
namespace {
namespace game_b116_12 {


void* const kSound = obj(d::g_sfx_player);
void* const kMessages = obj(d::g_names);
void* const kPortraits = obj(d::g_portraits);
void* const kFiles = obj(d::g_misc);

u32 now_ms() { return global<u32>(d::g_now); }
double frame_ticks() { return static_cast<double>(global<u32>(d::g_frame_step_ms)); }  // fild qword (u32, 0)

struct alignas(4) Path {  // 0x005a11a0 string object (0x194 bytes in the frame)
    u8 bytes[0x194];
};
const char* c_str(Path& p) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&p)->c_str()); }  // 0x00417d90

i32 strcmp_(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
i32 strncmp_(const char* a, const char* b, u32 n) { return joan::orig::arg::as<i32>(joan::orig::libc::strncmp(joan::orig::arg::str(a), joan::orig::arg::str(b), joan::orig::arg::u32(n))); }

JOAN_COUNTER(c_slide, "Message::update_slide_4ace90");
JOAN_COUNTER(c_post, "MessageList::post_4ad710");
JOAN_COUNTER(c_post2, "MessageList::post_2_4ad9a0");

// The shared body of 0x004ad710 / 0x004ad9a0 (they differ only in the init call and the
// addresses of their size reads).
u32 post_message(void* list, std::uintptr_t a, std::uintptr_t b, u32 c, std::uintptr_t init_thunk, std::uintptr_t read_loop,
                 std::uintptr_t read_pos) {  // a: the message text (a pointer); b: the speaker name (a pointer, Message::init) or the unit id (Speaker::setup)
    void* const mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::Message)));  // operator new (0x5b on i686)
    void* m = nullptr;
    if (mem != nullptr) m = joan::orig::as_message(mem)->construct();  // 0x004aa2e0 ctor
    thiscall<u32>(init_thunk, m, a, b);                        // 0x004aa3c0 / 0x004ab330
    jo::as_message(m)->sound_id = c;
    for (i32 i = joan::orig::as_dynarray(list)->size(); i > 0; --i) {  // 0x00417ad0 count
        if (joan::orig::arg::as<i32>(joan::orig::as_list(list)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;  // 0x004af300
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(list)->at_4af2d0(joan::orig::arg::u32(i)));   // 0x004af2d0
        if (strcmp_(jo::as_message(e)->text, jo::as_message(m)->text) == 0) {
            if (m != nullptr) joan::orig::as_message(m)->destroy(joan::orig::arg::u32(i32{1}));  // 0x004ae160 scalar delete
            return 0;
        }
    }
    jo::as_message_list(list)->message_count = jo::as_message_list(list)->message_count + 1;
    i16 count = 0;
    for (i32 i = joan::orig::as_dynarray(list)->size(); i > 0; --i) {  // 0x00417ad0
        if (joan::orig::arg::as<i32>(joan::orig::as_list(list)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;  // 0x004af300
        void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(list)->at_4af2d0(joan::orig::arg::u32(i)));   // 0x004af2d0
        if (jo::as_message(e)->has_pic != 0) continue;
        if (jo::view<i32>(jo::as_message(e)->state) != 0 && jo::view<i32>(jo::as_message(e)->state) != 1) continue;
        const i32 limit = joan::canvas::size_read(read_loop) - 0x112;
        // fild limit; fcomp [e+0xd]; test ah,1 jnz skip: count when limit >= y (not NaN).
        if (!(static_cast<double>(limit) >= static_cast<double>(jo::as_message(e)->y))) continue;
        count = static_cast<i16>(count + 1);
    }
    if (joan::orig::arg::as<i32>(joan::orig::as_pool(list)->alive_count()) > 0 &&                             // 0x004aede0
        joan::orig::arg::as<i32>(joan::orig::as_pool(list)->alive_count()) == static_cast<i32>(count)) {      // 0x004aede0
        count = static_cast<i16>(count - 1);
    }
    jo::view<u16>(jo::as_message(m)->shown_len) = static_cast<u16>(joan::canvas::size_read(read_pos) - 0x112 - static_cast<i32>(count) * 0x28);
    void* slot = m;
    joan::orig::dynarray_b(list)->linkin(&slot);                    // 0x005857a0 append(&m)
    jo::as_message(slot)->list_index = joan::orig::arg::as<u32>(joan::orig::as_dynarray(list)->cursor());  // 0x00417520
    if (strcmp_(jo::as_message(slot)->pic, str(d::s_M0000)) == 0) {     // 'M0000'
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_message(slot)->priority());        // 0x004aef10
        const u32 key = global<u16>(d::g_mission);              // only the word is read (see header)
        joan::orig::as_msg_log(kPortraits)->add(joan::orig::arg::u32(key), joan::orig::arg::u32(i32{1}), joan::orig::arg::str(jo::as_message(slot)->text), joan::orig::arg::u32(r), str(d::s_M0009));  // 0x004b2d30 'M0009'
    } else {
        const char* const name = jo::as_message(slot)->pic;
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_message(slot)->priority());        // 0x004aef10
        const u32 key = global<u16>(d::g_mission);              // only the word is read (see header)
        joan::orig::as_msg_log(kPortraits)->add(joan::orig::arg::u32(key), joan::orig::arg::u32(i32{1}), joan::orig::arg::str(jo::as_message(slot)->text), joan::orig::arg::u32(r), name);  // 0x004b2d30
    }
    return joan::orig::arg::as<u32>(joan::orig::as_dynarray(list)->cursor());  // 0x00417520
}

struct H {
    // 0x004ace90
    void update_slide() {
        c_slide.hit();
        const Fpu fpu{true};
        const u32 now = now_ms();
        if (jo::self<jo::Message>(this)->sound_id != 0) {
            const float v = global<float>(d::g_game_speed);
            // fcomp 0.0; test ah,0x40; je: not equal (ordered) -> 0x004ced90, else 0x00470c90.
            if (!(v < 0.0f) && !(v > 0.0f))
                joan::orig::as_sounds(kSound)->resume(joan::orig::arg::i32(jo::self<jo::Message>(this)->sound_id));  // 0x00470c90
            else
                joan::orig::as_sounds(kSound)->pause(joan::orig::arg::i32(jo::self<jo::Message>(this)->sound_id));  // 0x004ced90
        }
        switch (jo::self<jo::Message>(this)->state) {
            case 0:
                jo::self<jo::Message>(this)->state = 1;
                jo::self<jo::Message>(this)->start_time = now;
                return;
            case 1: {
                const i32 h = joan::canvas::size_read(0x004acf25) - 0xc2;
                // test ah,0x41 je: h > y -> go on; else (<=, unordered) arrived.
                if (!(static_cast<double>(h) > static_cast<double>(jo::self<jo::Message>(this)->y))) {
                    jo::self<jo::Message>(this)->state = 2;
                    jo::self<jo::Message>(this)->start_time = now;
                    return;
                }
                const i32 w = jo::self<jo::Message>(this)->shown_len;
                if (!(static_cast<double>(w) > static_cast<double>(jo::self<jo::Message>(this)->y))) {
                    if (joan::orig::arg::as<i8>(joan::orig::as_message_list(kMessages)->flag_44()) == 0) {  // 0x004af2b0
                        jo::self<jo::Message>(this)->start_time = now;
                        return;
                    }
                    if (joan::orig::arg::as<i8>(joan::orig::as_message_list(kMessages)->is_best(joan::orig::arg::i32(jo::self<jo::Message>(this)->list_index))) == 0) {  // 0x004ae710
                        jo::self<jo::Message>(this)->start_time = now;
                        return;
                    }
                    jo::view<u16>(jo::self<jo::Message>(this)->shown_len) = static_cast<u16>(joan::canvas::size_read(0x004acfa8) - 0xc2);
                }
                const double u = frame_ticks();
                if (jo::self<jo::Message>(this)->has_pic != 0) {
                    const double t = fpu.mul(fpu.mul(u, global<float>(d::flt_0_3)), global<float>(d::flt_10));  // *0.3 *10
                    jo::self<jo::Message>(this)->y = static_cast<float>(fpu.add(t, jo::self<jo::Message>(this)->y));
                } else {
                    const double t = fpu.mul(u, global<float>(d::flt_0_3));  // *0.3
                    jo::self<jo::Message>(this)->y = static_cast<float>(fpu.add(t, jo::self<jo::Message>(this)->y));
                }
                // fild word +0x15; fcomp y; test ah,1 je: set when w < y or unordered.
                const i32 w2 = jo::self<jo::Message>(this)->shown_len;
                if (!(static_cast<double>(w2) >= static_cast<double>(jo::self<jo::Message>(this)->y)))
                    jo::self<jo::Message>(this)->y = static_cast<float>(static_cast<i32>(jo::self<jo::Message>(this)->shown_len));
                const i32 h2 = joan::canvas::size_read(0x004ad081) - 0xc2;
                if (!(static_cast<double>(h2) >= static_cast<double>(jo::self<jo::Message>(this)->y)))
                    jo::self<jo::Message>(this)->y = static_cast<float>(joan::canvas::size_read(0x004ad0a6) - 0xc2);
                jo::self<jo::Message>(this)->start_time = now;
                return;
            }
            case 2:
                jo::self<jo::Message>(this)->state = 3;
                joan::orig::as_sound(kSound)->play_def(str(d::s_ATTENTION_CALL));  // 0x004ceef0 'ATTENTION_CALL'
                return;
            case 3: {
                const float k255 = global<float>(d::flt_255);
                // fld +0x4b; fcomp 255; test ah,1 jne: < 255 or unordered -> fade; else state 4.
                if (jo::self<jo::Message>(this)->colour[1] >= k255) {
                    jo::self<jo::Message>(this)->state = 4;
                    return;
                }
                jo::self<jo::Message>(this)->colour[2] = static_cast<float>(
                    fpu.add(fpu.mul(frame_ticks(), global<float>(d::flt_0_5)), jo::self<jo::Message>(this)->colour[2]));  // *0.5
                if (jo::self<jo::Message>(this)->colour[2] > k255) jo::view<u32>(jo::self<jo::Message>(this)->colour[2]) = 0x437f0000u;  // test ah,0x41 jne skip
                if (jo::self<jo::Message>(this)->colour[2] >= global<float>(d::flt_200)) {  // >= 200
                    jo::self<jo::Message>(this)->colour[3] = static_cast<float>(
                        fpu.add(fpu.mul(frame_ticks(), global<float>(d::flt_0_7)), jo::self<jo::Message>(this)->colour[3]));  // *0.7
                    if (jo::self<jo::Message>(this)->colour[3] > k255) jo::view<u32>(jo::self<jo::Message>(this)->colour[3]) = 0x437f0000u;
                }
                if (jo::self<jo::Message>(this)->colour[3] >= global<float>(d::flt_230)) {  // >= 230
                    jo::self<jo::Message>(this)->colour[0] = static_cast<float>(
                        fpu.add(fpu.mul(frame_ticks(), global<float>(d::flt_1_3)), jo::self<jo::Message>(this)->colour[0]));  // *1.3
                    if (jo::self<jo::Message>(this)->colour[0] > k255) jo::view<u32>(jo::self<jo::Message>(this)->colour[0]) = 0x437f0000u;
                }
                if (jo::self<jo::Message>(this)->colour[0] >= k255) {  // >= 255
                    jo::self<jo::Message>(this)->colour[1] = static_cast<float>(
                        fpu.add(fpu.mul(frame_ticks(), global<float>(d::flt_1_3)), jo::self<jo::Message>(this)->colour[1]));  // *1.3
                    if (jo::self<jo::Message>(this)->colour[1] > k255) jo::view<u32>(jo::self<jo::Message>(this)->colour[1]) = 0x437f0000u;
                }
                char* const name = jo::self<jo::Message>(this)->pic;
                if (strcmp_(name, str(d::s_M0009)) == 0) return;  // 'M0009'
                if (jo::self<jo::Message>(this)->has_pic != 0 && strncmp_(jo::self<jo::Message>(this)->pic, str(d::s_M000), 4u) != 0) return;  // 'M000'
                if (jo::self<jo::Message>(this)->start_time + 0x1e < now) {
                    Path p1;
                    joan::orig::as_string(&p1)->construct(joan::orig::arg::str(jo::self<jo::Message>(this)->pic));  // 0x005a11a0
                    char* digits = joan::orig::arg::as<char*>(joan::orig::libc::strrchr(joan::orig::arg::str(c_str(p1)), i32{0x4d}));  // strrchr 'M'
                    digits = digits + 1;
                    const i32 n = joan::orig::arg::as<i32>(std::strlen(joan::orig::arg::str(digits)));  // atoi
                    i16 frame = joan::orig::arg::as<i16>(joan::orig::as_misc(kFiles)->atoi_field_590270(joan::orig::arg::str(digits), joan::orig::arg::u32(n)));  // 0x00590270
                    frame = static_cast<i16>(frame + 1);
                    Path p2;
                    joan::orig::as_string(&p2)->construct(str(d::s_M000));      // 0x005a11a0 'M000'
                    joan::orig::as_string(&p2)->append(static_cast<i32>(frame));           // 0x005a15d0
#if defined(__GNUC__) && !defined(__clang__)
// copies 8 bytes of the picture name into the 8-byte pic field without terminator, as the original strncpy does
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
                    std::strncpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::Message>(this)->pic)), joan::orig::arg::str(c_str(p2)), u32{8});  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
                    jo::self<jo::Message>(this)->start_time = now;
                }
                return;
            }
            case 4: {
                if (jo::self<jo::Message>(this)->sound_id != 0 && jo::self<jo::Message>(this)->sound_started == 0) {
                    joan::orig::as_unit(kSound)->play_sound(joan::orig::arg::i32(jo::self<jo::Message>(this)->sound_id));  // 0x004cf0f0
                    jo::self<jo::Message>(this)->sound_started = 1;
                }
                // fild word +0x17; fcomp +0x3b; test ah,0x41 jne: <=, unordered -> wait branch.
                if (static_cast<double>(jo::self<jo::Message>(this)->len) > static_cast<double>(jo::self<jo::Message>(this)->revealed_chars)) {
                    jo::self<jo::Message>(this)->start_time = now;
                    jo::self<jo::Message>(this)->revealed_chars = static_cast<float>(
                        fpu.add(fpu.mul(frame_ticks(), global<float>(d::flt_0_04)), jo::self<jo::Message>(this)->revealed_chars));  // *0.04
                    if (!(static_cast<double>(jo::self<jo::Message>(this)->len) > static_cast<double>(jo::self<jo::Message>(this)->revealed_chars)))
                        jo::self<jo::Message>(this)->revealed_chars = static_cast<float>(static_cast<i32>(jo::self<jo::Message>(this)->len));
                } else {
                    const u32 end = static_cast<u32>(static_cast<i32>(jo::self<jo::Message>(this)->len) * 0x21) + jo::self<jo::Message>(this)->start_time;
                    if (end < now) {
                        if (joan::orig::arg::as<i8>(joan::orig::as_sounds(kSound)->has_kind(joan::orig::arg::i32(jo::self<jo::Message>(this)->sound_id))) == 0)  // 0x004cfee0
                            jo::self<jo::Message>(this)->state = 5;
                    }
                }
                return;
            }
            case 5: {
                // fld +0x57; fcomp 0; test ah,0x41 je: > 0 -> keep; else state 6.
                if (!(jo::self<jo::Message>(this)->colour[4] > 0.0f)) {
                    jo::self<jo::Message>(this)->state = 6;
                    joan::orig::as_timers(kMessages)->bump_oldest();  // 0x00499e40
                }
                const double t = fpu.mul(frame_ticks(), global<float>(d::flt_1));  // *1.0
                jo::self<jo::Message>(this)->colour[4] = static_cast<float>(fpu.sub(jo::self<jo::Message>(this)->colour[4], t));  // fsubr
                // fcomp 0; test ah,1 je skip: < 0 or unordered -> 0.
                if (!(jo::self<jo::Message>(this)->colour[4] >= 0.0f)) jo::view<u32>(jo::self<jo::Message>(this)->colour[4]) = 0;
                return;
            }
            default:
                return;
        }
    }

    // 0x004ad710
    u32 post(const char* a, const char* b, u32 c) {
        c_post.hit();
        return post_message(this, reinterpret_cast<std::uintptr_t>(a), reinterpret_cast<std::uintptr_t>(b), c, fn::code_value(fn::Message_init), 0x004ad886, 0x004ad8dd);  // init 0x004aa3c0
    }
    // 0x004ad9a0
    u32 post_2(const char* a, u32 b, u32 c) {
        c_post2.hit();
        return post_message(this, reinterpret_cast<std::uintptr_t>(a), b, c, fn::code_value(fn::Speaker_setup), 0x004adb16, 0x004adb6d);  // init 0x004ab330
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b116_12", name, addr, fn)
B("Message::update_slide_4ace90", 0x004ace90, &H::update_slide);
B("MessageList::post_4ad710", 0x004ad710, &H::post);
B("MessageList::post_2_4ad9a0", 0x004ad9a0, &H::post_2);
#undef B

}  // namespace game_b116_12
}  // namespace

// ---------------------------------------- from modules/game_b95_02.cpp (game/b95_02)
namespace {
namespace game_b95_02 {

struct H {
    // ------------------------------------------------------------------ 0x004ad6e0
    void* ctor_4ad6e0() {
        joan::orig::dynarray_b(this)->construct(static_cast<std::int32_t>(sizeof(void*)), 5, 0, 0);  // 0x005856a0 (pointers: 4 on x86)
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_02", name, addr, fn)
B95("Obj::ctor_4ad6e0", 0x004ad6e0, &H::ctor_4ad6e0);
#undef B95

}  // namespace game_b95_02
}  // namespace

// ---------------------------------------- from modules/game_b86_06.cpp (game/b86_06)
namespace {
namespace game_b86_06 {

struct H {
    // ---- 0x004ae160 scalar deleting destructor(flags) ret 4 --------------------------------
    void* scalar_delete(std::uint32_t flags) {
        joan::orig::as_wrap(this)->f_4aa310();  // 0x004aa310
        if ((flags & 1u) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b86_06", name, addr, fn)
B("Obj4aa310::scalar_delete", 0x004ae160, &H::scalar_delete);
#undef B

}  // namespace game_b86_06
}  // namespace

// ---------------------------------------- from modules/game_leaves82b.cpp (game/leaves82b)
namespace {
namespace game_leaves82b {


JOAN_COUNTER(c_messages_update, "Messages::update");

struct H {
    // 0x004ae390 Messages::update: unless paused (+0x3a), entries count..1 that are not done
    // (0x004af300) are updated (0x004d8c10).
    void messages_update() {
        c_messages_update.hit();
        if (jo::self<jo::MessageList>(this)->paused != 0) return;
        for (std::int32_t i = joan::orig::as_dynarray(this)->size(); i > 0; --i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;            // 0x004af300
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));                      // 0x004af2d0
            joan::orig::as_message(e)->draw();                                                   // 0x004d8c10
        }
    }
    // 0x004af530
    void messages_tick() {
        if (static_cast<std::int8_t>(joan::orig::arg::as<std::uint8_t>(joan::orig::as_game(obj(d::g_game_state))->flag_744())) == 0)  // 0x004301c0
            joan::orig::as_shop(obj(d::g_game_a5d380))->active();                                                // 0x0042ced0
    }
};

#define L82(name, addr, fn) JOAN_REPLACE("game/leaves82b", name, addr, fn)
L82("Messages::update", 0x004ae390, &H::messages_update);
L82("Messages::tick", 0x004af530, &H::messages_tick);
#undef L82

}  // namespace game_leaves82b
}  // namespace

// ---------------------------------------- from modules/game_b84_06.cpp (game/b84_06)
namespace {
namespace game_b84_06 {


JOAN_COUNTER(c_stop, "Group::stop_sounds");

void* const kSounds = obj(d::g_sfx_player);

using joan::orig::helpers::count_of;  // 0x00417ad0

struct H {
    // ------------------------------------------------------------------ 0x004aec10
    std::int32_t stop_sounds() {
        c_stop.hit();
        std::int32_t i = count_of(this);
        for (; i > 0; --i) {
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) != 0) continue;  // 0x004af300
            std::uint8_t* const m = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));  // 0x004af2d0
            if (jo::view<std::int32_t>(jo::as_message(m)->sound_id) != 0)
                joan::orig::as_scenario(kSounds)->trigger_slot(joan::orig::arg::i32(jo::view<std::int32_t>(jo::as_message(m)->sound_id)));  // 0x004cecd0
        }
        return i;  // eax at exit = final counter
    }

    // ------------------------------------------------------------------ 0x004ae6c0
    std::uint32_t wrap_4ae6c0() {
        joan::orig::as_list(this)->clear_4734a0();         // 0x004734a0
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_messages(this)->init());  // 0x0046c360
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b84_06", name, addr, fn)
B("Group::stop_sounds", 0x004aec10, &H::stop_sounds);
B("wrap_4ae6c0", 0x004ae6c0, &H::wrap_4ae6c0);
#undef B

}  // namespace game_b84_06
}  // namespace

// ---------------------------------------- from modules/game_b102_03.cpp (game/b102_03)
namespace {
namespace game_b102_03 {

JOAN_COUNTER(c_finish_4aeac0, "List::finish_4aeac0");

struct H {
    // ------------------------------------------------------------------ 0x004aeac0
    u32 finish_4aeac0() {
        c_finish_4aeac0.hit();
        for (i32 i = joan::orig::as_dynarray(this)->size(); ; ) {  // 0x00417ad0
            if (i <= 0) return static_cast<u32>(i);
            if (joan::orig::arg::as<i32>(joan::orig::as_list(this)->is_null_at_4af300(joan::orig::arg::u32(i))) == 0) {  // 0x004af300
                u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_list(this)->at_4af2d0(joan::orig::arg::u32(i)));  // 0x004af2d0
                if (jo::view<i32>(jo::as_message(e)->state) == 3 || jo::view<i32>(jo::as_message(e)->state) == 4 || jo::view<i32>(jo::as_message(e)->state) == 5) {
                    joan::orig::as_message(e)->set_3b(joan::orig::arg::bits(static_cast<float>(jo::as_message(e)->len)));  // 0x004af140
                    jo::as_message(e)->state = 6;
                    joan::orig::as_timers(obj(d::g_names))->bump_oldest();  // 0x00499e40
                    return 0;  // 0x00499e40 returns nothing; no caller reads the result
                }
            }
            i = i - 1;
        }
    }

    // ------------------------------------------------------------------ 0x004af820
    // MessageIndicator texture (+0x5).
    jo::Texture*& texture() { return jo::self<jo::MessageIndicator>(this)->texture; }

    H* free_4af820() {
        if (texture() != nullptr) {
            joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(texture()));  // 0x0060a550
            texture() = nullptr;
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_03", name, addr, fn)
B("List::finish_4aeac0", 0x004aeac0, &H::finish_4aeac0);
B("Obj::free_4af820", 0x004af820, &H::free_4af820);
#undef B

}  // namespace game_b102_03
}  // namespace

// ---------------------------------------- from modules/game_fx.cpp (game/fx1)
namespace {
namespace game_fx {


// Thunks.
constexpr fn::Code kListCount = fn::List_count_417ad0;   // 0x00417ad0

std::int32_t list_count(const void* list) { return thiscall<std::int32_t>(kListCount, list); }

struct H {
    std::int32_t count_minus_25() { return list_count(this) - jo::self<jo::DynArrayB>(this)->empty_room_count; }  // 0x004aede0
};

#define FX1(name, addr, fn) JOAN_REPLACE("game/fx1", name, addr, fn)
FX1("Pool::alive_count", 0x004aede0, &H::count_minus_25);
#undef FX1

}  // namespace game_fx
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {


struct H {
    std::int32_t get_3f() { return jo::view<std::int32_t>(jo::self<jo::Message>(this)->priority_); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("get_3f", 0x004aef10, &H::get_3f);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b85_06.cpp (game/b85_06)
namespace {
namespace game_b85_06 {


struct H {
    void* set_43(std::uint32_t v) { jo::self<jo::Message>(this)->start_time = v; return this; }  // 0x004af070
    std::uint32_t get_43() { return jo::self<jo::Message>(this)->start_time; }                    // 0x004af040
    void nop1(std::uint32_t) {}                                                  // 0x004af8a0
};

#define B85(name, addr, fn) JOAN_REPLACE("game/b85_06", name, addr, fn)
B85("Obj4af070::set_43", 0x004af070, &H::set_43);
B85("Obj4af040::get_43", 0x004af040, &H::get_43);
B85("Obj4af8a0::nop", 0x004af8a0, &H::nop1);
#undef B85

}  // namespace game_b85_06
}  // namespace

// ---------------------------------------- from modules/game_b92_03.cpp (game/b92_03)
namespace {
namespace game_b92_03 {


struct H {

    // 0x004af140
    void set_3b(std::int32_t v) { jo::view<std::int32_t>(jo::self<jo::Message>(this)->revealed_chars) = v; }
    // 0x004af410
    std::uint8_t byte_9ec() { return jo::view<std::uint8_t>(jo::self<jo::GameOver>(this)->is_open); }  // this: the GameOver screen 0x00a53ff8
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_03", name, addr, fn)
B("Obj::set_3b", 0x004af140, &H::set_3b);
B("Obj::byte_9ec", 0x004af410, &H::byte_9ec);
#undef B

}  // namespace game_b92_03
}  // namespace

// ---------------------------------------- from modules/game_b105_05.cpp (game/b105_05)
namespace {
namespace game_b105_05 {


struct H {
    // ------------------------------------------------------------------ trivial
    H* self() { return this; }
    void nop0() {}
    void nop1(u32) {}
    bool false_4af570() { return false; }

    // ------------------------------------------------------------------ 0x004af2b0
    std::uintptr_t flag_4af2b0() {
        return (reinterpret_cast<std::uintptr_t>(this) & ~std::uintptr_t{0xffu}) | jo::self<jo::MessageList>(this)->dirty;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_05", name, addr, fn)
B("Obj::self_4af4a0", 0x004af4a0, &H::self);
B("Obj::nop_4af5b0", 0x004af5b0, &H::nop0);
B("Obj::nop_4af590", 0x004af590, &H::nop1);
B("Obj::false_4af570", 0x004af570, &H::false_4af570);
B("Obj::flag_4af2b0", 0x004af2b0, &H::flag_4af2b0);
#undef B

}  // namespace game_b105_05
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

struct H {
    std::uint32_t is_null_at(std::uint32_t i) {  // 0x004af300
        return joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i)) == nullptr ? 1u : 0u;  // 0x0041b3f0
    }
    void* list_at(std::uint32_t i) {  // 0x004af2d0
        return joan::orig::as_dynarray(this)->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
    }
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("List::at_4af2d0", 0x004af2d0, &H::list_at);
B82("List::is_null_at_4af300", 0x004af300, &H::is_null_at);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b99_03.cpp (game/b99_03)
namespace {
namespace game_b99_03 {


struct H {

    // ------------------------------------------------------------------------ small ones
    void* init_af4c0() {  // 0x004af4c0
        jo::self<jo::MessageIndicator>(this)->sprites = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_milibal));
        jo::self<jo::MessageIndicator>(this)->active = 1;
        return this;
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_03", name, addr, fn)
B99("X::init_af4c0", 0x004af4c0, &H::init_af4c0);
#undef B99

}  // namespace game_b99_03
}  // namespace

// ---------------------------------------- from modules/game_b93_01.cpp (game/b93_01)
namespace {
namespace game_b93_01 {

struct H {
    // ------------------------------------------------------------------------ 0x004af4f0
    void release_05() {
        joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::self<jo::MessageIndicator>(this)->texture));  // 0x0060a550
        jo::self<jo::MessageIndicator>(this)->texture = nullptr;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_01", name, addr, fn)
B93("Obj::release_05", 0x004af4f0, &H::release_05);
#undef B93

}  // namespace game_b93_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Message::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_02::H::clear_5b>(this)); }  // 0x4aa2e0
auto joan::orig::Wrap::f_4aa310() -> void { joan::bind::member<&::game_b87_04::H::wrap_4aa310>(this); }  // 0x4aa310
auto joan::orig::Message::destruct() -> void { joan::bind::member<&::game_b88_06::H::release>(this); }  // 0x4aa340
auto joan::orig::Place::has_unit_of(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_04::H::has_unit_of>(this, a0)); }  // 0x4ab020
auto joan::orig::Unit::place_near_4ac910(std::uint32_t a0, std::int16_t a1) -> void { joan::bind::member<&::game_b104_03::H::place_near_4ac910>(this, a0, a1); }  // 0x4ac910
auto joan::orig::Messages::update() -> void { joan::bind::member<&::game_leaves82b::H::messages_update>(this); }  // 0x4ae390
auto joan::orig::MessageList::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b84_06::H::wrap_4ae6c0>(this)); }  // 0x4ae6c0
auto joan::orig::MessageList::is_best(std::int32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b104_03::H::is_best_4ae710>(this, a0)); }  // 0x4ae710
auto joan::orig::Group::stop_sounds() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b84_06::H::stop_sounds>(this)); }  // 0x4aec10
auto joan::orig::Pool::alive_count() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_fx::H::count_minus_25>(this)); }  // 0x4aede0
auto joan::orig::Message::priority() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_02::H::get_3f>(this)); }  // 0x4aef10
auto joan::orig::Message::time() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b85_06::H::get_43>(this)); }  // 0x4af040
auto joan::orig::Message::set_time(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b85_06::H::set_43>(this, a0)); }  // 0x4af070
auto joan::orig::Message::set_3b(std::int32_t a0) -> void { joan::bind::member<&::game_b92_03::H::set_3b>(this, a0); }  // 0x4af140
auto joan::orig::MessageList::flag_44() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_05::H::flag_4af2b0>(this)); }  // 0x4af2b0
auto joan::orig::List::at_4af2d0(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b82_18::H::list_at>(this, a0)); }  // 0x4af2d0
auto joan::orig::List::is_null_at_4af300(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b82_18::H::is_null_at>(this, a0)); }  // 0x4af300
auto joan::orig::Screen::flag_9ec() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b92_03::H::byte_9ec>(this)); }  // 0x4af410
auto joan::orig::MessageIndicator::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b105_05::H::self>(this)); }  // 0x4af4a0
auto joan::orig::MessageIndicator::release_texture() -> void { joan::bind::member<&::game_b93_01::H::release_05>(this); }  // 0x4af4f0
auto joan::orig::Messages::tick() -> void { joan::bind::member<&::game_leaves82b::H::messages_tick>(this); }  // 0x4af530
auto joan::orig::MessageIndicator::on_event(std::uint32_t a0) -> void { joan::bind::member<&::game_b105_05::H::nop1>(this, a0); }  // 0x4af590
auto joan::orig::MessageIndicator::set_mode(std::uint32_t a0) -> void { joan::bind::member<&::game_b85_06::H::nop1>(this, a0); }  // 0x4af8a0
