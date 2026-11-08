// game/unknown_diplo_text.cpp: functions whose original source file is unknown (formerly game/_unplaced/diplo_text.cpp;
// re/attribution.csv: between ww/uti3d/t_scrres.cpp and ww/trackres.cpp). They move to their original file when
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

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"
#include "../original/records.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::hook::global;

using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using u16 = std::uint16_t;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b107_18.cpp (game/b107_18)
namespace {
namespace game_b107_18 {

using joan::call::cdecl_call;

// ---- static initialisers ----
u32 JOAN_CDECL init_4dfff0() { return joan::orig::arg::as<u32>(joan::orig::init_4e0010()); }

#define B(name, addr, fn) JOAN_REPLACE("game/b107_18", name, addr, fn)
B("init_4dfff0", 0x004dfff0, &init_4dfff0);
#undef B

}  // namespace game_b107_18
}  // namespace

// ---------------------------------------- from modules/game_b105_06.cpp (game/b105_06)
namespace {
namespace game_b105_06 {

using joan::img::obj;

std::uintptr_t JOAN_CDECL init_4e0010() { return joan::orig::arg::as<u32>(joan::orig::as_string(obj(d::g_msg_a3a348))->construct()); }  // 0x005a1170

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_06", name, addr, fn)
B("init_4e0010", 0x004e0010, &init_4e0010);
B("Obj::nop_4e1ea0", 0x004e1ea0, &H::nop);
#undef B

}  // namespace game_b105_06
}  // namespace

// ---------------------------------------- from modules/game_b106_02.cpp (game/b106_02)
namespace {
namespace game_b106_02 {

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_dispatch, "Diplomacy::dispatch_4e0030");

struct H {
    const char* dispatch_4e0030(u32 a, u32 b, u32 c) {  // 0x004e0030
        c_dispatch.hit();
        global<u16>(d::g_me) = static_cast<u16>(a);
        global<u8>(d::g_a3a343) = static_cast<u8>(b);
        global<u8>(d::g_a3a342) = static_cast<u8>(c);
        const u32 index = static_cast<u32>(static_cast<i32>(jo::self<jo::Diplomacy>(this)->kind) - 1);
        switch (index) {
            case 0: joan::orig::as_treaty(this)->propose_text(str(d::s_trade)); break;   // 0x004e0250 "trade"
            case 1: joan::orig::as_treaty(this)->propose_text(str(d::s_friendly)); break;   // "friendly"
            case 2: joan::orig::as_treaty(this)->propose_text(str(d::s_alliance)); break;   // "alliance"
            case 3: joan::orig::as_treaty(this)->end_message(str(d::s_trade)); break;   // 0x004e0680
            case 4: joan::orig::as_treaty(this)->end_message(str(d::s_friendly)); break;
            case 5: joan::orig::as_treaty(this)->end_message(str(d::s_alliance)); break;
            case 6: joan::orig::as_treaty(this)->aid_request_text(); break;                     // 0x004e1ec0
            case 7: joan::orig::as_treaty(this)->embargo_text(); break;                     // 0x004e2140
            case 8: joan::orig::as_treaty(this)->ceasefire_text(); break;                     // 0x004e0800
            case 9: joan::orig::as_treaty(this)->war_request_text(); break;                     // 0x004e0aa0
            case 10: joan::orig::as_treaty(this)->food_text(); break;                    // 0x004e0ef0
            case 11: joan::orig::as_treaty(this)->war_text(); break;                    // 0x004e1390
            case 12: joan::orig::as_treaty(this)->gift_text(str(d::s_tribute)); break;  // 0x004e14a0 "tribute"
            case 13: joan::orig::as_treaty(this)->tribute_text(0); break;                 // 0x004e1980
            case 14: joan::orig::as_treaty(this)->gift_text(str(d::s_aid)); break;  // "aid"
            case 15: joan::orig::as_treaty(this)->tribute_text(1); break;
            case 16: joan::orig::as_treaty(this)->nop_text_16(); break;                    // 0x004e1ea0
            case 17: joan::orig::as_treaty(this)->throne_text(); break;                    // 0x004e2570
            case 18: joan::orig::as_treaty(this)->surrender_text(); break;                    // 0x004e28c0
            default: break;
        }
        return joan::orig::as_string(obj(d::g_msg_a3a348))->c_str();  // 0x00417d90
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_02", name, addr, fn)
B("Diplomacy::dispatch_4e0030", 0x004e0030, &H::dispatch_4e0030);
#undef B

}  // namespace game_b106_02
}  // namespace

// ---------------------------------------- from modules/game_b104_05.cpp (game/b104_05)
namespace {
namespace game_b104_05 {

using joan::call::thiscall;
using joan::img::obj;

void* const kMsg = obj(d::g_msg_a3a348);
void* const kPlayers = obj(d::g_nations);

i32 player_out(i32 p) { return joan::orig::arg::as<i32>(joan::orig::as_list(kPlayers)->is_null(joan::orig::arg::i32(p))); }       // 0x00484870

void* const kArrays = obj(d::g_misc);

JOAN_COUNTER(c_propose, "Treaty::propose_text");
JOAN_COUNTER(c_war_req, "Treaty::war_request_text");
JOAN_COUNTER(c_food, "Treaty::food_text");
JOAN_COUNTER(c_gift, "Treaty::gift_text");
JOAN_COUNTER(c_tribute, "Treaty::tribute_text");
JOAN_COUNTER(c_embargo, "Treaty::embargo_text");
JOAN_COUNTER(c_still_valid, "Treaty::still_valid");

// 0x191-byte fixed string (0x005a1170 ctor); frame space as in the original
struct LocalStr {
    char b[0x198];
};

std::uintptr_t msg_set(const void* s) { return joan::orig::arg::as<u32>(joan::orig::as_string(kMsg)->assign(joan::orig::arg::str(s))); }  // 0x005a1440
std::uintptr_t msg_add(const void* s) { return joan::orig::arg::as<u32>(joan::orig::as_string(kMsg)->append(joan::orig::arg::str(s))); }  // 0x005a1550
std::uintptr_t msg_add_str(const LocalStr* s) { return joan::orig::arg::as<u32>(joan::orig::as_string(kMsg)->append_str(joan::orig::as_string(s))); }  // 0x005a14f0
std::uintptr_t msg_add_num(i32 v) { return joan::orig::arg::as<u32>(joan::orig::as_string(kMsg)->append(joan::orig::arg::i32(v))); }  // 0x005a15d0
std::uintptr_t msg_dot() { return msg_add(str(d::s_dot_9d36e8)); }                               // "."
void* player(i32 p) { return joan::orig::as_dynarray(kPlayers)->ptr_at(joan::orig::arg::i32(p)); }  // 0x0047b600
const char* fmt_num(i32 v) {                                                    // 0x004e1360
    return joan::orig::arg::as<const char*>(joan::orig::as_misc(kArrays)->format_int_short(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(v))), 2u));
}

struct H {
    const char* name_e() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->from_name()); }  // 0x004e3550 (+0xe)
    const char* name_f() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->to_name()); }  // 0x004e3590 (+0xf)
    const char* name_of(i32 p) { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->player_name(joan::orig::arg::u32(p))); }  // 0x004e3670
    i32 me() { return global<i16>(d::g_me); }
    bool is_e() { return me() == jo::self<jo::Treaty>(this)->from; }
    bool is_f() { return me() == jo::self<jo::Treaty>(this)->to; }
    bool is_third() { return me() == jo::self<jo::Treaty>(this)->arg; }
    bool asking() { return jo::self<jo::Treaty>(this)->reply == 1 || global<i8>(d::g_a3a343) == 0; }
    bool yes() { return jo::self<jo::Treaty>(this)->reply == 2; }

    // ------------------------------------------------------------------------ 0x004e0250
    std::uintptr_t propose_text(const char* kind) {
        c_propose.hit();
        LocalStr s;
        joan::orig::as_string(&s)->construct();  // 0x005a1170
        const char* const art = static_cast<i8>(kind[0]) == 0x61 ? str(d::s_an)   // "an "
                                                                 : str(d::s_a);  // "a "
        joan::orig::as_string(&s)->assign(joan::orig::arg::str(kind));  // 0x005a1440
        joan::orig::as_string(&s)->append(str(d::s_treaty));                // 0x005a1550 " treaty"
        if (asking()) {
            if (is_e()) {
                msg_set(str(d::s_You_propose));  // "You propose "
                msg_add(art);
                msg_add_str(&s);
                msg_add(str(d::s_to));  // " to "
                msg_add(name_f());
                return msg_dot();
            }
            if (is_f()) {
                msg_set(name_e());
                msg_add(str(d::s_proposes));  // " proposes "
                msg_add(art);
                msg_add_str(&s);
                return msg_add(str(d::s_to_you));  // " to you."
            }
            msg_set(name_e());
            msg_add(str(d::s_proposes));  // " proposes "
            msg_add(art);
            msg_add_str(&s);
            msg_add(str(d::s_to));  // " to "
            msg_add(name_f());
            return msg_dot();
        }
        if (is_e()) {
            msg_set(name_f());
            msg_add(yes() ? str(d::s_accepts_your_proposed) : str(d::s_rejects_your_proposed));  // " accepts/rejects your proposed "
            msg_add_str(&s);
            return msg_dot();
        }
        if (is_f()) {
            msg_set(yes() ? str(d::s_You_accept_the) : str(d::s_You_reject_the));  // "You accept/reject the "
            msg_add_str(&s);
            msg_add(str(d::s_proposed_by));  // " proposed by "
            msg_add(name_e());
            return msg_dot();
        }
        msg_set(name_f());
        msg_add(yes() ? str(d::s_accepts_the) : str(d::s_rejects_the));  // " accepts/rejects the "
        msg_add_str(&s);
        msg_add(str(d::s_proposed_by));  // " proposed by "
        msg_add(name_e());
        return msg_dot();
    }

    // ------------------------------------------------------------------------ 0x004e0aa0
    std::uintptr_t war_request_text() {
        c_war_req.hit();
        if (asking()) {
            if (is_e()) {
                msg_set(str(d::s_You_request));  // "You request "
                msg_add(name_f());
                msg_add(str(d::s_to_declare_war_on));  // " to declare war on "
                msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
                return msg_dot();
            }
            if (is_f()) {
                msg_set(name_e());
                msg_add(str(d::s_requests_that_you_declare_wa));  // " requests that you declare war on "
                msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
                return msg_dot();
            }
            if (is_third()) {
                msg_set(name_e());
                msg_add(str(d::s_requests));  // " requests "
                msg_add(name_f());
                return msg_add(str(d::s_to_declare_war_on_you));  // " to declare war on you"
            }
            msg_set(name_e());
            msg_add(str(d::s_requests));  // " requests "
            msg_add(name_f());
            msg_add(str(d::s_to_declare_war_on));  // " to declare war on "
            return msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
        }
        if (is_e()) {
            msg_set(name_f());
            msg_add(yes() ? str(d::s_agrees_to_declare_war_on) : str(d::s_refuses_to_declare_war_on));  // " agrees/refuses to declare war on "
            msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
            return msg_dot();
        }
        if (is_f()) {
            msg_set(yes() ? str(d::s_You_agree_to_declare_war_on) : str(d::s_You_refuse_to_declare_war_on));  // "You agree/refuse to declare war on "
            msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
            return msg_dot();
        }
        if (is_third()) {
            msg_set(name_f());
            // " agrees/refuses to declare war on you."
            return msg_add(yes() ? str(d::s_agrees_to_declare_war_on_you) : str(d::s_refuses_to_declare_war_on_yo));
        }
        msg_set(name_f());
        msg_add(yes() ? str(d::s_agrees_to_declare_war_on) : str(d::s_refuses_to_declare_war_on));  // " agrees/refuses to declare war on "
        msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
        return msg_dot();
    }

    // ------------------------------------------------------------------------ 0x004e0ef0
    std::uintptr_t food_text() {
        c_food.hit();
        if (global<i8>(d::g_a3a342) != 0) {
            if (is_e()) {
                msg_set(str(d::s_You_offer));  // "You offer "
            } else {
                msg_set(name_e());
                msg_add(str(d::s_offers_9d6a54));  // " offers "
            }
            msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg2));
            return msg_add(str(d::s_for_10_units_of_food));  // " for 10 units of food."
        }
        if (asking()) {
            if (is_e()) {
                msg_set(str(d::s_You_request_to_purchase));  // "You request to purchase "
                msg_add_num(jo::self<jo::Treaty>(this)->arg);
                msg_add(str(d::s_units_of_food_from));  // " units of food from "
                msg_add(name_f());
                return msg_dot();
            }
            if (is_f()) {
                msg_set(name_e());
                msg_add(str(d::s_requests_to_purchase));  // " requests to purchase "
                msg_add_num(jo::self<jo::Treaty>(this)->arg);
                return msg_add(str(d::s_units_of_food_from_you));  // " units of food from you."
            }
            msg_set(name_e());
            msg_add(str(d::s_requests_to_purchase));  // " requests to purchase "
            msg_add_num(jo::self<jo::Treaty>(this)->arg);
            msg_add(str(d::s_units_of_food_from));  // " units of food from "
            msg_add(name_f());
            return msg_dot();
        }
        if (is_e()) {
            msg_set(name_f());
            msg_add(yes() ? str(d::s_agrees) : str(d::s_refuses_9d6794));  // " agrees" / " refuses"
            msg_add(str(d::s_to_sell));                            // " to sell "
            msg_add_num(jo::self<jo::Treaty>(this)->arg);
            return msg_add(str(d::s_units_of_food_to_you));  // " units of food to you."
        }
        if (is_f()) {
            msg_set(yes() ? str(d::s_You_agree) : str(d::s_You_refuse));  // "You agree" / "You refuse"
            msg_add(str(d::s_to_sell));                            // " to sell "
            msg_add_num(jo::self<jo::Treaty>(this)->arg);
            msg_add(str(d::s_units_of_food_to));  // " units of food to "
            msg_add(name_e());
            return msg_dot();
        }
        msg_set(name_f());
        msg_add(yes() ? str(d::s_agrees) : str(d::s_refuses_9d6794));  // " agrees" / " refuses"
        msg_add(str(d::s_to_sell));                            // " to sell "
        msg_add_num(jo::self<jo::Treaty>(this)->arg);
        msg_add(str(d::s_units_of_food_to));  // " units of food to "
        msg_add(name_e());
        return msg_dot();
    }

    // ------------------------------------------------------------------------ 0x004e14a0
    std::uintptr_t gift_text(const char* what) {
        c_gift.hit();
        if (asking()) {
            if (is_e()) {
                msg_set(str(d::s_You_offer));  // "You offer "
                msg_add(name_f());
                msg_add(str(d::s_sp_9d05f0));
                msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
                msg_add(str(d::s_in));  // " in "
                msg_add(what);
                return msg_dot();
            }
            if (is_f()) {
                msg_set(name_e());
                msg_add(str(d::s_offers_you));  // " offers you "
                msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
                msg_add(str(d::s_in));  // " in "
                msg_add(what);
                return msg_dot();
            }
            msg_set(name_e());
            msg_add(str(d::s_offers_9d6a54));  // " offers "
            msg_add(name_f());
            msg_add(str(d::s_sp_9d05f0));
            msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
            msg_add(str(d::s_in));  // " in "
            msg_add(what);
            return msg_dot();
        }
        if (is_e()) {
            msg_set(name_f());
            msg_add(yes() ? str(d::s_accepts_your) : str(d::s_rejects_your));  // " accepts your " / " rejects your "
            msg_add(what);
            msg_add(str(d::s_of));  // " of "
            msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
            return msg_dot();
        }
        if (is_f()) {
            msg_set(yes() ? str(d::s_You_accept_the) : str(d::s_You_reject_the));  // "You accept the " / "You reject the "
            msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
            msg_add(str(d::s_in));  // " in "
            msg_add(what);
            msg_add(str(d::s_from));  // " from "
            msg_add(name_e());
            return msg_dot();
        }
        msg_set(name_f());
        msg_add(yes() ? str(d::s_accepts) : str(d::s_rejects));  // " accepts " / " rejects "
        msg_add(what);
        msg_add(str(d::s_of));  // " of "
        msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
        msg_add(str(d::s_from));  // " from "
        msg_add(name_e());
        return msg_dot();
    }

    // ------------------------------------------------------------------------ 0x004e1980
    std::uintptr_t tribute_text(i32 aid) {
        c_tribute.hit();
        const char* const what = aid != 0 ? str(d::s_aid) : str(d::s_tribute);  // "aid" / "tribute"
        if (asking()) {
            if (is_e()) {
                msg_set(aid != 0 ? str(d::s_You_request) : str(d::s_You_demand));  // "You request " / "You demand "
                msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
                msg_add(str(d::s_in));  // " in "
                msg_add(what);
                msg_add(str(d::s_from));  // " from "
                msg_add(name_f());
                return msg_dot();
            }
            if (is_f()) {
                msg_set(name_e());
                msg_add(aid != 0 ? str(d::s_requests) : str(d::s_demands));  // " requests " / " demands "
                msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
                msg_add(str(d::s_in));  // " in "
                msg_add(what);
                return msg_add(str(d::s_from_you));  // " from you."
            }
            msg_set(name_e());
            msg_add(aid != 0 ? str(d::s_requests) : str(d::s_demands));  // " requests " / " demands "
            msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
            msg_add(str(d::s_in));  // " in "
            msg_add(what);
            msg_add(str(d::s_from));  // " from "
            msg_add(name_f());
            return msg_dot();
        }
        if (is_e()) {
            msg_set(name_f());
            msg_add(yes() ? str(d::s_agrees) : str(d::s_refuses_9d6794));     // " agrees" / " refuses"
            msg_add(aid != 0 ? str(d::s_to_give_you) : str(d::s_to_pay_you));  // " to give you " / " to pay you "
        } else if (is_f()) {
            msg_set(yes() ? str(d::s_You_agree) : str(d::s_You_refuse));     // "You agree" / "You refuse"
            msg_add(aid != 0 ? str(d::s_to_give) : str(d::s_to_pay));  // " to give " / " to pay "
            msg_add(name_e());
            msg_add(str(d::s_sp_9d05f0));
        } else {
            msg_set(name_f());
            msg_add(yes() ? str(d::s_agrees) : str(d::s_refuses_9d6794));     // " agrees" / " refuses"
            msg_add(aid != 0 ? str(d::s_to_give) : str(d::s_to_pay));  // " to give " / " to pay "
            msg_add(name_e());
            msg_add(str(d::s_sp_9d05f0));
        }
        msg_add(fmt_num(jo::self<jo::Treaty>(this)->arg));
        msg_add(str(d::s_in));  // " in "
        msg_add(what);
        return msg_dot();
    }

    // ------------------------------------------------------------------------ 0x004e2140
    std::uintptr_t embargo_text() {
        c_embargo.hit();
        if (asking()) {
            if (is_e()) {
                msg_set(str(d::s_You_request));  // "You request "
                msg_add(name_f());
                msg_add(str(d::s_to_join_an_embargo_on_trade_9d6eac));  // " to join an embargo on trade with "
                msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
                return msg_dot();
            }
            if (is_f()) {
                msg_set(name_e());
                msg_add(str(d::s_requests_you_to_join_an_emba));  // " requests you to join an embargo on trade with "
                msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
                return msg_dot();
            }
            if (is_third()) {
                msg_set(name_e());
                msg_add(str(d::s_requests));  // " requests "
                msg_add(name_f());
                return msg_add(str(d::s_to_join_an_embargo_on_trade_9d6e40));  // " to join an embargo on trade with you."
            }
            msg_set(name_e());
            msg_add(str(d::s_requests));  // " requests "
            msg_add(name_f());
            msg_add(str(d::s_to_join_an_embargo_on_trade_9d6eac));  // " to join an embargo on trade with "
            msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
            return msg_dot();
        }
        if (is_e()) {
            msg_set(name_f());
            msg_add(yes() ? str(d::s_agrees_to_join_an_embargo_on) : str(d::s_refuses_to_join_an_embargo_o));  // " agrees/refuses to join an embargo ..."
            msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
            return msg_dot();
        }
        if (is_f()) {
            msg_set(yes() ? str(d::s_You_agree_to_join_an_embargo) : str(d::s_You_refuse_to_join_an_embarg));  // "You agree/refuse to join an embargo ..."
            msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
            msg_add(str(d::s_as_requested_by));  // " as requested by "
            msg_add(name_e());
            return msg_dot();
        }
        if (!is_third()) return joan::call::register_low_bits(this);
        msg_set(name_f());
        msg_add(yes() ? str(d::s_agrees_to_join_an_embargo_on) : str(d::s_refuses_to_join_an_embargo_o));  // " agrees/refuses to join an embargo ..."
        msg_add(name_of(jo::self<jo::Treaty>(this)->arg));
        return msg_dot();
    }

    // ------------------------------------------------------------------------ 0x004e3180
    i32 still_valid() {
        c_still_valid.hit();
        if (global<i32>(d::g_get_a47bb0) > static_cast<i32>(jo::self<jo::Treaty>(this)->time + 0x1eu)) return 0;
        if (player_out(jo::self<jo::Treaty>(this)->from) != 0) return 0;
        if (player_out(jo::self<jo::Treaty>(this)->to) != 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_talk(obj(d::g_queue))->allows(static_cast<i32>(jo::self<jo::Treaty>(this)->to), joan::orig::arg::i32(// 0x004e37f0
                          static_cast<i32>(jo::self<jo::Treaty>(this)->from)), static_cast<i32>(jo::self<jo::Treaty>(this)->kind))) == 0)
            return 0;
        void* const pf = player(jo::self<jo::Treaty>(this)->to);
        void* const pe = player(jo::self<jo::Treaty>(this)->from);
        switch (jo::self<jo::Treaty>(this)->kind) {
            case 8:
                if (player_out(jo::self<jo::Treaty>(this)->arg) != 0) return 0;
                if (jo::as_relation(joan::orig::arg::as<void*>(joan::orig::as_player(pe)->relation(static_cast<i32>(jo::self<jo::Treaty>(this)->arg))))->status != 0)  // 0x004b42e0
                    return 0;
                if (jo::as_relation(joan::orig::arg::as<void*>(joan::orig::as_player(pf)->relation(static_cast<i32>(jo::self<jo::Treaty>(this)->arg))))->status == 0) return 0;
                return 1;
            case 10:
                if (player_out(jo::self<jo::Treaty>(this)->arg) != 0) return 0;
                if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_table(pe)->byte_58(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Treaty>(this)->arg))))) != 0)  // 0x004b4360
                    return 0;
                if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_table(pf)->byte_58(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Treaty>(this)->arg))))) == 0) return 0;
                return 1;
            case 11: {
                const i32 v = static_cast<i32>(jo::self<jo::Treaty>(this)->arg) * static_cast<i32>(jo::self<jo::Treaty>(this)->arg2) / 10;
                return !(static_cast<double>(v) > static_cast<double>(jo::as_nation(pe)->gold)) ? 1 : 0;
            }
            case 13:
            case 15: {
                const i32 v = jo::self<jo::Treaty>(this)->arg;
                return !(static_cast<double>(v) > static_cast<double>(jo::as_nation(pe)->gold)) ? 1 : 0;
            }
            default:
                return 1;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_05", name, addr, fn)
B("Treaty::propose_text", 0x004e0250, &H::propose_text);
B("Treaty::war_request_text", 0x004e0aa0, &H::war_request_text);
B("Treaty::food_text", 0x004e0ef0, &H::food_text);
B("Treaty::gift_text", 0x004e14a0, &H::gift_text);
B("Treaty::tribute_text", 0x004e1980, &H::tribute_text);
B("Treaty::embargo_text", 0x004e2140, &H::embargo_text);
B("Treaty::still_valid", 0x004e3180, &H::still_valid);
#undef B

}  // namespace game_b104_05
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

using joan::call::thiscall;
using joan::img::obj;

void* const kMsg = obj(d::g_msg_a3a348);
void* const kPlayers = obj(d::g_nations);

i32 player_out(i32 p) { return joan::orig::arg::as<i32>(joan::orig::as_list(kPlayers)->is_null(joan::orig::arg::i32(p))); }       // 0x00484870

JOAN_COUNTER(c_treaty_msg, "Treaty::end_message");
JOAN_COUNTER(c_treaty_valid, "Treaty::valid");

std::uintptr_t msg_set(const char* s) { return joan::orig::arg::as<u32>(joan::orig::as_string(kMsg)->assign(joan::orig::arg::str(s))); }  // 0x005a1440
std::uintptr_t msg_add(const char* s) { return joan::orig::arg::as<u32>(joan::orig::as_string(kMsg)->append(joan::orig::arg::str(s))); }  // 0x005a1550

struct H {
    // ------------------------------------------------------------------------ 0x004e0680
    std::uintptr_t treaty_end_message(const char* name) {
        c_treaty_msg.hit();
        const i32 me = static_cast<i32>(global<i16>(d::g_me));
        if (me == static_cast<i32>(static_cast<i8>(jo::view<u8>(jo::self<jo::Treaty>(this)->from)))) {
            msg_set(str(d::s_You_terminate_your));  // "You terminate your "
            msg_add(name);
            msg_add(str(d::s_treaty_with));  // " treaty with "
            msg_add(joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->to_name()));  // 0x004e3590
            return msg_add(str(d::s_dot_9d36e8));  // "."
        }
        if (me == static_cast<i32>(static_cast<i8>(jo::view<u8>(jo::self<jo::Treaty>(this)->to)))) {
            msg_set(joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->from_name()));  // 0x004e3550
            msg_add(str(d::s_terminates_its));  // " terminates its "
            msg_add(name);
            return msg_add(str(d::s_treaty_with_you));  // " treaty with you."
        }
        msg_set(joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->from_name()));  // 0x004e3550
        msg_add(str(d::s_terminates_its));  // " terminates its "
        msg_add(name);
        msg_add(str(d::s_treaty_with));  // " treaty with "
        msg_add(joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->to_name()));  // 0x004e3590
        return msg_add(str(d::s_dot_9d36e8));  // "."
    }

    // ------------------------------------------------------------------------ 0x004e3450
    i32 treaty_valid() {
        c_treaty_valid.hit();
        if (player_out(static_cast<i8>(jo::view<u8>(jo::self<jo::Treaty>(this)->from))) != 0) return 0;
        if (player_out(static_cast<i8>(jo::view<u8>(jo::self<jo::Treaty>(this)->to))) != 0) return 0;
        (void)joan::orig::as_dynarray(kPlayers)->ptr_at(static_cast<i32>(static_cast<i8>(jo::view<u8>(jo::self<jo::Treaty>(this)->to))));  // 0x0047b600
        (void)joan::orig::as_dynarray(kPlayers)->ptr_at(static_cast<i32>(static_cast<i8>(jo::view<u8>(jo::self<jo::Treaty>(this)->from))));  // 0x0047b600
        const i32 kind = static_cast<i16>(jo::view<u16>(jo::self<jo::Treaty>(this)->kind));
        if (kind == 8) {
            if (player_out(static_cast<i16>(jo::view<u16>(jo::self<jo::Treaty>(this)->arg))) != 0) return 0;
        } else if (kind == 10) {
            if (player_out(static_cast<i16>(jo::view<u16>(jo::self<jo::Treaty>(this)->arg))) != 0) return 0;
        }
        return 1;
    }
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("Treaty::end_message", 0x004e0680, &H::treaty_end_message);
B98("Treaty::valid", 0x004e3450, &H::treaty_valid);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b100_07.cpp (game/b100_07)
namespace {
namespace game_b100_07 {

using joan::call::thiscall;
using joan::img::obj;

void* const kMsg = obj(d::g_msg_a3a348);

JOAN_COUNTER(c_ceasefire, "Treaty::ceasefire_text");
JOAN_COUNTER(c_surrender, "Treaty::surrender_text");

std::uintptr_t msg_set(const void* s) { return joan::orig::arg::as<u32>(joan::orig::as_string(kMsg)->assign(joan::orig::arg::str(s))); }  // 0x005a1440
std::uintptr_t msg_add(const void* s) { return joan::orig::arg::as<u32>(joan::orig::as_string(kMsg)->append(joan::orig::arg::str(s))); }  // 0x005a1550
i32 player() { return static_cast<i32>(global<i16>(d::g_me)); }

struct H {
    const char* name_e() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->to_name()); }  // 0x004e3590
    const char* name_f() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->from_name()); }  // 0x004e3550

    // ------------------------------------------------------------------------ 0x004e0800
    std::uintptr_t ceasefire_text() {
        c_ceasefire.hit();
        if (static_cast<i32>(jo::self<jo::Treaty>(this)->reply) == 1 || static_cast<i32>(global<i8>(d::g_a3a343)) == 0) {
            if (player() == static_cast<i32>(jo::self<jo::Treaty>(this)->from)) {
                msg_set(str(d::s_You_request_a_cease_fire_wit));  // "You request a cease-fire with "
                msg_add(name_e());
                return msg_add(str(d::s_dot_9d36e8));
            }
            if (player() == static_cast<i32>(jo::self<jo::Treaty>(this)->to)) {
                msg_set(name_f());
                return msg_add(str(d::s_requests_a_cease_fire));  // " requests a cease-fire."
            }
            msg_set(name_f());
            msg_add(str(d::s_requests));  // " requests "
            msg_add(name_e());
            return msg_add(str(d::s_for_a_cease_fire));  // " for a cease-fire"
        }
        if (player() == static_cast<i32>(jo::self<jo::Treaty>(this)->from)) {
            msg_set(name_e());
            if (static_cast<i32>(jo::self<jo::Treaty>(this)->reply) == 2)
                msg_add(str(d::s_agrees_to_9d67a0));  // " agrees to"
            else
                msg_add(str(d::s_refuses_9d6794));  // " refuses"
            return msg_add(str(d::s_a_cease_fire));  // " a cease-fire."
        }
        if (player() == static_cast<i32>(jo::self<jo::Treaty>(this)->to)) {
            if (static_cast<i32>(jo::self<jo::Treaty>(this)->reply) == 2)
                msg_set(str(d::s_You_agree_to));  // "You agree to"
            else
                msg_set(str(d::s_You_refuse));  // "You refuse"
            msg_add(str(d::s_a_cease_fire_with));      // " a cease-fire with "
            return msg_add(name_f());
        }
        msg_set(name_e());
        if (static_cast<i32>(jo::self<jo::Treaty>(this)->reply) == 2)
            msg_add(str(d::s_agrees_to_9d6738));  // " agrees to "
        else
            msg_add(str(d::s_refuses_9d672c));  // " refuses "
        msg_add(str(d::s_a_cease_fire_with));      // " a cease-fire with "
        return msg_add(name_f());
    }

    // ------------------------------------------------------------------------ 0x004e28c0
    std::uintptr_t surrender_text() {
        c_surrender.hit();
        if (player() == static_cast<i32>(jo::self<jo::Treaty>(this)->from)) {
            msg_set(str(d::s_You_have_surrendered_to));  // "You have surrendered to "
            msg_add(name_e());
            return msg_add(str(d::s_dot_9d36e8));
        }
        if (player() == static_cast<i32>(jo::self<jo::Treaty>(this)->to)) {
            msg_set(name_f());
            return msg_add(str(d::s_has_surrendered_to_you));  // " has surrendered to you."
        }
        msg_set(name_f());
        msg_add(str(d::s_has_surrendered_to));  // " has surrendered to "
        msg_add(name_e());
        return msg_add(str(d::s_dot_9d36e8));
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_07", name, addr, fn)
B100("Treaty::ceasefire_text", 0x004e0800, &H::ceasefire_text);
B100("Treaty::surrender_text", 0x004e28c0, &H::surrender_text);
#undef B100

}  // namespace game_b100_07
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

using joan::call::thiscall;

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x004e1360
    const char* fwd_58fca0(std::int16_t a, std::uint32_t b) {
        return joan::orig::as_misc(self())->format_int_58fca0(static_cast<std::int32_t>(a), joan::orig::arg::i32(b));  // 0x0058fca0
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::fwd_58fca0", 0x004e1360, &H::fwd_58fca0);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

using joan::call::thiscall;
using joan::img::obj;

void* const kText = obj(d::g_msg_a3a348);

JOAN_COUNTER(c_war_msg, "Diplo::war_message");

std::uintptr_t text_set(const void* s) { return joan::orig::arg::as<u32>(joan::orig::as_string(kText)->assign(joan::orig::arg::str(s))); }  // 0x005a1440
std::uintptr_t text_append(const void* s) { return joan::orig::arg::as<u32>(joan::orig::as_string(kText)->append(joan::orig::arg::str(s))); }  // 0x005a1550

struct H {
    // ------------------------------------------------------------------------ 0x004e1390
    std::uintptr_t war_message() {
        c_war_msg.hit();
        const i32 player = global<i16>(d::g_me);
        if (player == static_cast<i32>(jo::self<jo::Diplo>(this)->from)) {
            text_set(str(d::s_You_declare_war_on));                                  // "You declare war on "
            text_append(joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->to_name()));      // 0x004e3590
            return text_append(str(d::s_dot_9d36e8));
        }
        if (player == static_cast<i32>(jo::self<jo::Diplo>(this)->to)) {
            text_set(joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->from_name()));         // 0x004e3550
            return text_append(str(d::s_declares_war_on_you));                        // " declares war on you."
        }
        text_set(joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->from_name()));             // 0x004e3550
        text_append(str(d::s_declares_war_on));                                   // " declares war on "
        text_append(joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->to_name()));          // 0x004e3590
        return text_append(str(d::s_dot_9d36e8));
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("Diplo::war_message", 0x004e1390, &H::war_message);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_aid_text, "AidRequest::format_text");

// ---- text buffer 0xa3a348 ----
void text_set(const char* s) { joan::orig::as_string(obj(d::g_msg_a3a348))->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void text_append(const char* s) { joan::orig::as_string(obj(d::g_msg_a3a348))->append(joan::orig::arg::str(s)); }  // 0x005a1550

struct H {
    // ------------------------------------------------------------------------ 0x004e1ec0
    const char* name_to() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->to_name()); }    // 0x004e3590
    const char* name_from() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->from_name()); }  // 0x004e3550
    void aid_text() {
        c_aid_text.hit();
        const i32 player = global<i16>(d::g_me);
        if (jo::self<jo::AidRequest>(this)->reply == 1 || global<i8>(d::g_a3a343) == 0) {
            if (player == jo::self<jo::AidRequest>(this)->from) {
                text_set(str(d::s_You_request_immediate_milita));  // "You request immediate military aid from "
                text_append(name_to());
                text_append(str(d::s_dot_9d36e8));
            } else if (player == jo::self<jo::AidRequest>(this)->to) {
                text_set(name_from());
                text_append(str(d::s_requests_immediate_military_9d6cec));  // " requests immediate military aid from you."
            } else {
                text_set(name_from());
                text_append(str(d::s_requests_immediate_military_9d6cbc));  // " requests immediate military aid from "
                text_append(name_to());
                text_append(str(d::s_dot_9d36e8));
            }
        } else {
            if (player == jo::self<jo::AidRequest>(this)->from) {
                text_set(name_to());
                if (jo::self<jo::AidRequest>(this)->reply == 2)
                    text_append(str(d::s_agrees_to_immediately_send_y));  // " agrees to immediately send your requested military aid."
                else
                    text_append(str(d::s_denies_you_your_requested_mi));  // " denies you your requested military aid."
            } else if (player == jo::self<jo::AidRequest>(this)->to) {
                if (jo::self<jo::AidRequest>(this)->reply == 2)
                    text_set(str(d::s_You_agree_to_immediately_sen));  // "You agree to immediately send military aid to "
                else
                    text_set(str(d::s_You_refuse_to_send_military));  // "You refuse to send military aid to "
                text_append(name_from());
                text_append(str(d::s_dot_9d36e8));
            } else {
                text_set(name_to());
                if (jo::self<jo::AidRequest>(this)->reply == 2)
                    text_append(str(d::s_agrees_to_immediately_send_m));  // " agrees to immediately send military aid to "
                else
                    text_append(str(d::s_denies_to_send_military_aid));  // " denies to send military aid to "
                text_append(name_to());  // the original calls 0x004e3590 again here, not 0x004e3550
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("AidRequest::format_text", 0x004e1ec0, &H::aid_text);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

using joan::img::obj;

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------------ small ones
    void* zero_17() {  // 0x004e29d0
        memset_(this, 0, 0x17);
        return this;
    }
    i32 table_a049cb() {  // 0x004e3150
        return static_cast<i32>(reinterpret_cast<const i8*>(obj(d::g_a049cb))[static_cast<i32>(jo::self<jo::Treaty>(this)->kind)]);  // i8 table
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("Struct17::zero", 0x004e29d0, &H::zero_17);
B94("X::table_a049cb", 0x004e3150, &H::table_a049cb);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b109_01.cpp (game/b109_01)
namespace {
namespace game_b109_01 {

using joan::call::thiscall;
using joan::img::obj;

using joan::hook::x87::Fpu;

JOAN_COUNTER(c_msg, "Msg::apply_4e2a00");

// ---------------------------------------------------------------- 0x004e2a00 helpers
void* const kQueue = obj(d::g_queue);
struct alignas(4) Msg {
    u8 b[0x18];
};
static_assert(sizeof(Msg) == 0x18, "Msg");
void msg_clear(Msg& m) { std::memset(joan::orig::arg::mptr(m.b), 0, joan::orig::arg::u32(u32{0x17})); }  // memset
void unit_timer(void* p, i32 kind, i32 time) { joan::orig::as_table(p)->raise_to(joan::orig::arg::i32(kind), joan::orig::arg::i32(time)); }  // 0x004b8d60

struct H {
    // -------------------------------------------------------------- 0x0045d330
    // Original frame [ebp-0x150]..[ebp-0x10]: the 0x18-byte time buffer is followed by the
    // locals it can overrun (tod, date, local / system FILETIME - dead after the format call)
    // and then the live file object, exactly as in the original.
    struct PerfFrame {
        char tbuf[0x18];  // [ebp-0x150]
        float tod;        // [ebp-0x138]
        u32 date;         // [ebp-0x134]
        u32 lft[2];       // [ebp-0x130]
        u32 ft[2];        // [ebp-0x128]
        u8 file[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];   // [ebp-0x120] (x64: the larger File)
    };
    static_assert(offsetof(PerfFrame, file) == 0x30 && sizeof(PerfFrame) >= 0x140, "PerfFrame: no padding between the frame slots");  // 0x140 bytes on x86

    // -------------------------------------------------------------- 0x004e2a00
    i32 msg_apply() {
        c_msg.hit();
        void* const v08 = joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(static_cast<i32>(jo::self<jo::Msg>(this)->to));  // 0x0047b600
        void* const v0c = joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(static_cast<i32>(jo::self<jo::Msg>(this)->from));
        void* const v10 = joan::orig::arg::as<void*>(joan::orig::as_player(v0c)->relation(static_cast<i32>(jo::self<jo::Msg>(this)->to)));  // 0x004b42e0
        void* const v14 = joan::orig::arg::as<void*>(joan::orig::as_player(v08)->relation(static_cast<i32>(jo::self<jo::Msg>(this)->from)));
        i32 v18 = 0;
        const Fpu fpu{true};
        switch (jo::self<jo::Msg>(this)->type) {
            case 1:
                joan::orig::as_player(v08)->set_relation(static_cast<i32>(jo::self<jo::Msg>(this)->from), static_cast<char>(joan::orig::arg::i32(1)));  // 0x004b4610
                break;
            case 2:
                joan::orig::as_nation(v08)->set_state_3(static_cast<i32>(jo::self<jo::Msg>(this)->from));  // 0x004b4510
                break;
            case 3:
                joan::orig::as_nation(v08)->set_state_4(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Msg>(this)->from)));  // 0x004b4550
                break;
            case 4: {
                joan::orig::as_player(v08)->set_relation(static_cast<i32>(jo::self<jo::Msg>(this)->from), static_cast<char>(joan::orig::arg::i32(0)));  // 0x004b4610
                unit_timer(v10, 1, 0xb4);
                unit_timer(v14, 1, 0xb4);
                unit_timer(v10, 4, 0xb4);
                unit_timer(v14, 4, 0xb4);
                if (jo::as_nation(v08)->mood > 0.0f) {  // test ah,0x41: greater (ordered) only
                    double v = fpu.mul(-static_cast<double>(jo::as_nation(v08)->mood), 5.0);  // [0x9cf878]
                    v = fpu.div(v, 100.0);                                                  // [0x9cf9c4]
                    joan::orig::as_player(v0c)->add_mood(joan::orig::arg::f32(static_cast<float>(v)));                  // 0x004b5420
                }
                break;
            }
            case 5:
            case 6:
                joan::orig::as_army(v0c)->set_morale_494cc0(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Msg>(this)->to)), 2u);  // 0x00494cc0
                unit_timer(v10, 2, 0x12c);
                unit_timer(v14, 2, 0x12c);
                unit_timer(v10, 3, 0x12c);
                unit_timer(v14, 3, 0x12c);
                break;
            case 7:
                v18 = 10;
                break;
            case 8: {
                Msg m{};
                joan::orig::as_treaty(m.b)->zero();  // 0x004e29d0
                msg_clear(m);
                m.b[0xf] = jo::view<u8[2]>(jo::self<jo::Msg>(this)->arg)[0];
                m.b[0xe] = jo::view<u8>(jo::self<jo::Msg>(this)->to);
                jo::view<u16>(jo::as_msg(m.b)->type) = 4;
                joan::orig::as_order(kQueue)->dispatch_4e3bb0(joan::orig::arg::mptr(static_cast<void*>(m.b)), 0);  // 0x004e3bb0
                v18 = 4;
                break;
            }
            case 9:
                joan::orig::as_nation(v08)->set_relation_state(static_cast<i32>(jo::self<jo::Msg>(this)->from), 1, 0);  // 0x00499770 (word arg)
                unit_timer(v10, 0xc, 0xb4);
                unit_timer(v14, 0xc, 0xb4);
                break;
            case 10: {
                // 0x004b4360 reads its argument as a word (pushed via ax).
                const i32 w = static_cast<i32>(jo::view<u16>(jo::self<jo::Msg>(this)->arg));
                if (joan::orig::arg::as<i8>(joan::orig::as_table(v0c)->byte_58(joan::orig::arg::i16(w))) != 0) break;  // 0x004b4360
                Msg m{};
                joan::orig::as_treaty(m.b)->zero();  // 0x004e29d0
                if (joan::orig::arg::as<i8>(joan::orig::as_table(v08)->byte_58(joan::orig::arg::i16(static_cast<i32>(jo::view<u16>(jo::self<jo::Msg>(this)->arg))))) == 4) {
                    send(m, 6);
                } else if (joan::orig::arg::as<i8>(joan::orig::as_table(v08)->byte_58(joan::orig::arg::i16(static_cast<i32>(jo::view<u16>(jo::self<jo::Msg>(this)->arg))))) == 3) {
                    send(m, 5);
                }
                send(m, 0xc);
                v18 = 10;
                break;
            }
            case 11: {
                const i32 v4c = (static_cast<i32>(jo::self<jo::Msg>(this)->arg) * static_cast<i32>(jo::self<jo::Msg>(this)->arg2)) / 10;
                joan::orig::as_nation(v0c)->add_time(joan::orig::arg::f32(static_cast<float>(jo::self<jo::Msg>(this)->arg)));  // 0x004b5340
                joan::orig::as_economy(v0c)->spend(0xd, joan::orig::arg::f32(static_cast<float>(v4c)), 0);         // 0x004b5010
                joan::orig::as_stats(v08)->transfer(joan::orig::arg::f32(static_cast<float>(jo::self<jo::Msg>(this)->arg)));  // 0x004b53b0
                joan::orig::as_player(v08)->add_stat(1, joan::orig::arg::f32(static_cast<float>(v4c)), 0);           // 0x004b4ce0
                break;
            }
            case 12: {
                jo::as_relation(v14)->u16_10 = static_cast<u16>(jo::as_relation(v14)->u16_10 + 1);
                joan::orig::as_nation(v08)->set_relation_state(static_cast<i32>(jo::self<jo::Msg>(this)->from), 0, 0);  // 0x00499770 (word arg)
                unit_timer(v10, 9, 0x96);
                unit_timer(v14, 9, 0x96);
                unit_timer(v10, 0xd, 0x12c);
                unit_timer(v14, 0xd, 0x12c);
                if (jo::as_nation(v08)->mood > 0.0f) {
                    double v = fpu.mul(-static_cast<double>(jo::as_nation(v08)->mood), 20.0);  // [0x9d5334]
                    v = fpu.div(v, 100.0);                                                   // [0x9cf9c4]
                    joan::orig::as_player(v0c)->add_mood(joan::orig::arg::f32(static_cast<float>(v)));                   // 0x004b5420
                }
                break;
            }
            case 13:
            case 15:
                joan::orig::as_player(v0c)->give(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Msg>(this)->to)), static_cast<i32>(jo::self<jo::Msg>(this)->arg));  // 0x004b4790
                break;
            case 14:
            case 16:
                joan::orig::as_player(v08)->give(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Msg>(this)->from)), static_cast<i32>(jo::self<jo::Msg>(this)->arg));  // 0x004b4790
                v18 = static_cast<i32>(jo::self<jo::Msg>(this)->arg) / 200;
                break;
            case 17:
                break;
            case 18: {
                const float v50 = static_cast<float>(jo::self<jo::Msg>(this)->arg) * 10.0f;  // fild word-sized int * [0x9cfc20]
                joan::orig::as_player(v08)->add_stat(5, joan::orig::arg::f32(v50), 0);    // 0x004b4ce0
                joan::orig::as_economy(v0c)->spend(0xe, joan::orig::arg::f32(v50), 0);  // 0x004b5010
                joan::orig::as_slot(v08)->release_6(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Msg>(this)->from)));  // 0x004b5630
                break;
            }
            case 19:
                joan::orig::as_slot(v0c)->release_6(joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Msg>(this)->to)));  // 0x004b5630
                break;
            default:
                break;
        }
        if (v18 != 0) jo::as_relation(v14)->f32_c = static_cast<float>(fpu.sub(static_cast<double>(jo::as_relation(v14)->f32_c), static_cast<double>(v18)));
        Msg key{};
        joan::orig::as_treaty(key.b)->zero();  // 0x004e29d0
        key.b[0xe] = jo::view<u8>(jo::self<jo::Msg>(this)->to);
        key.b[0xf] = jo::view<u8>(jo::self<jo::Msg>(this)->from);
        jo::view<u16>(jo::as_msg(key.b)->type) = jo::view<u16>(jo::self<jo::Msg>(this)->type);
        for (;;) {
            const i32 found = joan::orig::arg::as<i32>(joan::orig::as_talk(kQueue)->find_order(joan::orig::arg::mptr(static_cast<void*>(key.b)), 0));  // 0x0048e7f0
            if (found == 0 || jo::as_msg(key.b)->type == 0xc) return found;
            joan::orig::as_talk(kQueue)->remove_message(joan::orig::arg::i32(found));  // 0x0046ad90
        }
    }
    void send(Msg& m, u16 type) {
        msg_clear(m);
        m.b[0xf] = jo::view<u8[2]>(jo::self<jo::Msg>(this)->arg)[0];
        m.b[0xe] = jo::view<u8>(jo::self<jo::Msg>(this)->to);
        jo::view<u16>(jo::as_msg(m.b)->type) = type;
        joan::orig::as_order(kQueue)->dispatch_4e3bb0(joan::orig::arg::mptr(static_cast<void*>(m.b)), 0);  // 0x004e3bb0
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_01", name, addr, fn)
B("Msg::apply_4e2a00", 0x004e2a00, &H::msg_apply);
#undef B

}  // namespace game_b109_01
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::call::thiscall;
using joan::img::obj;

using joan::call::thiscall;
using joan::img::obj;

void* const kSides = obj(d::g_nations);

struct H {
    // 0x004e3550 / 0x004e3590
    void* side_data_e() {
        return joan::orig::arg::as<void*>(joan::orig::as_nation(joan::orig::as_dynarray(kSides)->ptr_at(static_cast<std::int32_t>(jo::self<jo::Treaty>(this)->from)))->name());  // 0x0047b600, 0x004b4280
    }
    void* side_data_f() {
        return joan::orig::arg::as<void*>(joan::orig::as_nation(joan::orig::as_dynarray(kSides)->ptr_at(static_cast<std::int32_t>(jo::self<jo::Treaty>(this)->to)))->name());
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("Ref::side_data_e", 0x004e3550, &H::side_data_e);
B("Ref::side_data_f", 0x004e3590, &H::side_data_f);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

using joan::call::thiscall;
using joan::img::obj;

struct H {
    // ------------------------------------------------------------------------ 0x004e35d0
    void* slot_3d() {
        u8* const a = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(static_cast<i32>(jo::self<jo::Throne>(this)->from)));  // 0x0047b600
        void* const b = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(static_cast<i32>(jo::as_nation(a)->king_unit));  // 0x0041b3c0
        return joan::orig::arg::as<void*>(joan::orig::as_unit(b)->name());  // 0x004fe4f0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Obj::slot_3d", 0x004e35d0, &H::slot_3d);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

using joan::call::thiscall;
using joan::img::obj;

void* const kUnits = obj(d::g_units);

struct H {
    // ------------------------------------------------------------------------ 0x004e3620
    std::intptr_t lookup_4e3620() {
        void* const o = joan::orig::as_dynarray(obj(d::g_nations))->ptr_at(static_cast<i32>(jo::self<jo::Throne>(this)->from));  // 0x0047b600
        void* const u = joan::orig::as_dynarray(kUnits)->ptr_at(static_cast<i32>(jo::as_nation(o)->king_unit));  // 0x0041b3c0
        return joan::orig::arg::as<std::intptr_t>(joan::orig::as_unit(u)->name());                                                          // 0x004fe4f0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Obj::lookup_4e3620", 0x004e3620, &H::lookup_4e3620);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

using joan::call::thiscall;
using joan::img::obj;

void* const kPlayers = obj(d::g_nations);

struct H {
    // ------------------------------------------------------------------------ 0x004e3670
    std::uintptr_t player_query(u32 id) {
        void* const p = joan::orig::as_dynarray(kPlayers)->ptr_at(joan::orig::arg::i32(id));  // 0x0047b600
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_nation(p)->name());                        // 0x004b4280
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("X::player_query", 0x004e3670, &H::player_query);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b101_06.cpp (game/b101_06)
namespace {
namespace game_b101_06 {

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::hook::global;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

void* const kMsg = obj(d::g_msg_a3a348);  // global message string

// ---------------------------------------------------------------- string helpers (0xa3a348)
void* msg_assign(const void* s) { return joan::orig::arg::as<void*>(joan::orig::as_string(kMsg)->assign(joan::orig::arg::str(s))); }  // 0x005a1440
void* msg_append(const void* s) { return joan::orig::arg::as<void*>(joan::orig::as_string(kMsg)->append(joan::orig::arg::str(s))); }  // 0x005a1550
void* msg_append_int(i32 v) { return joan::orig::arg::as<void*>(joan::orig::as_string(kMsg)->append(joan::orig::arg::i32(v))); }  // 0x005a15d0

JOAN_COUNTER(c_describe, "Throne::describe");

struct H {
    // ------------------------------------------------------------------------ 0x004e2570
    void* throne_describe() {
        c_describe.hit();
        auto g = []() { return static_cast<i32>(global<i16>(d::g_me)); };
        auto amount = [this]() { return static_cast<i32>(jo::self<jo::Throne>(this)->arg) * 10; };
        auto name_3590 = [this]() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->to_name()); };  // 0x004e3590
        auto name_35d0 = [this]() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->sender_leader_pos()); };  // 0x004e35d0
        auto name_3550 = [this]() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->from_name()); };  // 0x004e3550
        auto name_3620 = [this]() { return joan::orig::arg::as<const char*>(joan::orig::as_treaty(this)->sender_leader_pos_4e3620()); };  // 0x004e3620
        if (jo::self<jo::Throne>(this)->reply == 1 || global<i8>(d::g_a3a343) == 0) {
            if (g() == static_cast<i32>(jo::self<jo::Throne>(this)->from)) {
                msg_assign(str(d::s_You_offer));  // "You offer "
                msg_append_int(amount());
                msg_append(str(d::s_for_the_throne_of));  // " for the throne of "
                msg_append(name_3590());
                return msg_append(str(d::s_dot_9d36e8));  // "."
            }
            if (g() == static_cast<i32>(jo::self<jo::Throne>(this)->to)) {
                msg_assign(str(d::s_To_unite_our_two_Kingdoms_un));  // "To unite our two Kingdoms under his rule, "
                msg_append(name_35d0());
                msg_append(str(d::s_offers_9d6a54));  // " offers "
                msg_append_int(amount());
                return msg_append(str(d::s_for_your_throne));  // " for your throne."
            }
            msg_assign(name_3550());
            msg_append(str(d::s_offers_9d7054));  // "offers "
            msg_append_int(amount());
            msg_append(str(d::s_for_the_throne_of));  // " for the throne of "
            msg_append(name_3590());
            return msg_append(str(d::s_dot_9d36e8));  // "."
        }
        if (g() == static_cast<i32>(jo::self<jo::Throne>(this)->from)) {
            msg_assign(name_3620());
            if (jo::self<jo::Throne>(this)->reply == 2)
                return msg_append(str(d::s_agrees_to_take_your_money_in));  // " agrees to take your money in exchange for his throne."
            return msg_append(str(d::s_refuses_to_dishonor_himself_9d6fd0));      // " refuses to dishonor himself by selling his throne!"
        }
        if (g() == static_cast<i32>(jo::self<jo::Throne>(this)->to)) {
            if (jo::self<jo::Throne>(this)->reply == 2)
                return msg_assign(str(d::s_You_agree_to_take_the_money));  // "You agree to take the money in exchange for your throne."
            msg_assign(str(d::s_You_refuse_to_dishonor_yours));  // "You refuse to dishonor yourself by selling your throne to "
            msg_append(name_3550());
            return msg_append(str(d::s_dot_9d36e8));  // "."
        }
        msg_assign(name_3620());
        if (jo::self<jo::Throne>(this)->reply == 2) {
            msg_append(str(d::s_agrees_to_sell_his_throne_to));  // " agrees to sell his throne to "
            return msg_append(name_3550());
        }
        msg_append(str(d::s_refuses_to_dishonor_himself_9d6ed8));  // " refuses to dishonor himself by selling his throne to "
        msg_append(name_3550());
        return msg_append(str(d::s_dot_9d36e8));  // "."
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_06", name, addr, fn)
B("Throne::describe", 0x004e2570, &H::throne_describe);
#undef B

}  // namespace game_b101_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::init_4e0010() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b105_06::init_4e0010>()); }  // 0x4e0010
auto joan::orig::Treaty::format_text(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b106_02::H::dispatch_4e0030>(this, a0, a1, a2)); }  // 0x4e0030
auto joan::orig::Treaty::propose_text(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_05::H::propose_text>(this, a0)); }  // 0x4e0250
auto joan::orig::Treaty::end_message(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_04::H::treaty_end_message>(this, a0)); }  // 0x4e0680
auto joan::orig::Treaty::ceasefire_text() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_07::H::ceasefire_text>(this)); }  // 0x4e0800
auto joan::orig::Treaty::war_request_text() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_05::H::war_request_text>(this)); }  // 0x4e0aa0
auto joan::orig::Treaty::food_text() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_05::H::food_text>(this)); }  // 0x4e0ef0
auto joan::orig::Misc::format_int_short(std::int16_t a0, std::uint32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b90_03::H::fwd_58fca0>(this, a0, a1)); }  // 0x4e1360
auto joan::orig::Treaty::war_text() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_04::H::war_message>(this)); }  // 0x4e1390
auto joan::orig::Treaty::gift_text(const char* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_05::H::gift_text>(this, a0)); }  // 0x4e14a0
auto joan::orig::Treaty::tribute_text(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_05::H::tribute_text>(this, a0)); }  // 0x4e1980
auto joan::orig::Treaty::nop_text_16() -> void { joan::bind::member<&::game_b105_06::H::nop>(this); }  // 0x4e1ea0
auto joan::orig::Treaty::aid_request_text() -> void { joan::bind::member<&::game_b96_04::H::aid_text>(this); }  // 0x4e1ec0
auto joan::orig::Treaty::embargo_text() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_05::H::embargo_text>(this)); }  // 0x4e2140
auto joan::orig::Treaty::throne_text() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b101_06::H::throne_describe>(this)); }  // 0x4e2570
auto joan::orig::Treaty::surrender_text() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_07::H::surrender_text>(this)); }  // 0x4e28c0
auto joan::orig::Treaty::zero() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b94_04::H::zero_17>(this)); }  // 0x4e29d0
auto joan::orig::Treaty::apply() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b109_01::H::msg_apply>(this)); }  // 0x4e2a00
auto joan::orig::Treaty::needs_reply() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_04::H::table_a049cb>(this)); }  // 0x4e3150
auto joan::orig::Treaty::still_valid() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_05::H::still_valid>(this)); }  // 0x4e3180
auto joan::orig::Treaty::valid() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b98_04::H::treaty_valid>(this)); }  // 0x4e3450
auto joan::orig::Treaty::from_name() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::side_data_e>(this)); }  // 0x4e3550
auto joan::orig::Treaty::to_name() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_02::H::side_data_f>(this)); }  // 0x4e3590
auto joan::orig::Treaty::sender_leader_pos() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b97_04::H::slot_3d>(this)); }  // 0x4e35d0
auto joan::orig::Treaty::sender_leader_pos_4e3620() -> std::intptr_t { return joan::bind::conv<std::intptr_t>(joan::bind::member<&::game_b99_05::H::lookup_4e3620>(this)); }  // 0x4e3620
auto joan::orig::Treaty::player_name(std::uint32_t a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b93_02::H::player_query>(this, a0)); }  // 0x4e3670
