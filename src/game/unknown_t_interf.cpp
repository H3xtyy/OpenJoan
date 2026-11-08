// game/unknown_t_interf.cpp: functions whose original source file is unknown (formerly game/_unplaced/t_interf.cpp;
// re/attribution.csv: between ww/speech_s.cpp and ww/t_scrres.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/records.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b100_06.cpp (game/b100_06)
namespace {
namespace game_b100_06 {

// TxtRes text lookup, varargs cdecl 0x005a3090(self, section, n, ...), via thunk 0x00408675.
char* text(void* self, const char* section, i32 n) { return joan::orig::arg::as<char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(section), joan::orig::arg::i32(n))); }
// One variable argument: a number, or the key name of IGameMnu_open_2 (a pointer: 8 bytes on x64, where the
// format's %s reads the whole vararg slot).
template <class A>
char* text1(void* self, const char* section, i32 n, A a) {
    return joan::orig::arg::as<char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(section), joan::orig::arg::i32(n), a));
}

const char* const kBuy = str(d::s_BUY);       // "BUY"
const char* const kGmsg = str(d::s_GMSG);      // "GMSG"
const char* const kIgamemnu = str(d::s_IGAMEMNU);  // "IGAMEMNU"
const char* const kSysdet = str(d::s_SYSDET);    // "SYSDET"
const char* const kOptmenu = str(d::s_OPTMENU);   // "OPTMENU"
const char* const kVict = str(d::s_VICT);      // "VICT"
const char* const kObjmenu = str(d::s_OBJMENU);   // "OBJMENU"
const char* const kSavemenu = str(d::s_SAVEMENU);  // "SAVEMENU"

struct H {
    char* buy_1() { return text(this, kBuy, 0x1); }               // 0x004da650
    char* buy_10() { return text(this, kBuy, 0xa); }              // 0x004da840
    char* gmsg_8() { return text(this, kGmsg, 0x8); }             // 0x004daa00
    char* gmsg_9() { return text(this, kGmsg, 0x9); }             // 0x004daa30
    char* igamemnu_2(const char* a) { return text1(this, kIgamemnu, 0x2, a); }  // 0x004dac10
    char* igamemnu_11() { return text(this, kIgamemnu, 0xb); }    // 0x004dadf0
    char* sysdet_1() { return text(this, kSysdet, 0x1); }         // 0x004daef0
    char* optmenu_33() { return text(this, kOptmenu, 0x33); }     // 0x004db0a0
    char* optmenu_0f() { return text(this, kOptmenu, 0xf); }      // 0x004db1f0
    char* optmenu_16() { return text(this, kOptmenu, 0x16); }     // 0x004db340
    char* optmenu_17() { return text(this, kOptmenu, 0x17); }     // 0x004db370
    char* optmenu_1e() { return text(this, kOptmenu, 0x1e); }     // 0x004db4c0
    char* optmenu_2a() { return text(this, kOptmenu, 0x2a); }     // 0x004db700
    char* optmenu_2f() { return text(this, kOptmenu, 0x2f); }     // 0x004db7f0
    char* optmenu_35() { return text(this, kOptmenu, 0x35); }     // 0x004db8e0
    char* vict_3() { return text(this, kVict, 0x3); }             // 0x004dba00
    char* objmenu_0c() { return text(this, kObjmenu, 0xc); }      // 0x004dbca0
    char* objmenu_12() { return text(this, kObjmenu, 0x12); }     // 0x004dbdc0
    char* objmenu_16() { return text(this, kObjmenu, 0x16); }     // 0x004dbe80
    char* objmenu_1e() { return text(this, kObjmenu, 0x1e); }     // 0x004dc010
    char* objmenu_22() { return text(this, kObjmenu, 0x22); }     // 0x004dc0d0
    char* objmenu_29() { return text(this, kObjmenu, 0x29); }     // 0x004dc230
    char* objmenu_44(u32 a) { return text1(this, kObjmenu, 0x44, a); }  // 0x004dc420
    char* objmenu_4a() { return text(this, kObjmenu, 0x4a); }     // 0x004dc550
    char* savemenu_01() { return text(this, kSavemenu, 0x1); }    // 0x004dc610
    char* savemenu_05() { return text(this, kSavemenu, 0x5); }    // 0x004dc6d0
    char* savemenu_0e() { return text(this, kSavemenu, 0xe); }    // 0x004dc880
    char* savemenu_11() { return text(this, kSavemenu, 0x11); }   // 0x004dc910
    char* savemenu_18() { return text(this, kSavemenu, 0x18); }   // 0x004dca70
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_06", name, addr, fn)
B("TxtRes::buy_1", 0x004da650, &H::buy_1);
B("TxtRes::buy_10", 0x004da840, &H::buy_10);
B("TxtRes::gmsg_8", 0x004daa00, &H::gmsg_8);
B("TxtRes::gmsg_9", 0x004daa30, &H::gmsg_9);
B("TxtRes::igamemnu_2", 0x004dac10, &H::igamemnu_2);
B("TxtRes::igamemnu_11", 0x004dadf0, &H::igamemnu_11);
B("TxtRes::sysdet_1", 0x004daef0, &H::sysdet_1);
B("TxtRes::optmenu_33", 0x004db0a0, &H::optmenu_33);
B("TxtRes::optmenu_0f", 0x004db1f0, &H::optmenu_0f);
B("TxtRes::optmenu_16", 0x004db340, &H::optmenu_16);
B("TxtRes::optmenu_17", 0x004db370, &H::optmenu_17);
B("TxtRes::optmenu_1e", 0x004db4c0, &H::optmenu_1e);
B("TxtRes::optmenu_2a", 0x004db700, &H::optmenu_2a);
B("TxtRes::optmenu_2f", 0x004db7f0, &H::optmenu_2f);
B("TxtRes::optmenu_35", 0x004db8e0, &H::optmenu_35);
B("TxtRes::vict_3", 0x004dba00, &H::vict_3);
B("TxtRes::objmenu_0c", 0x004dbca0, &H::objmenu_0c);
B("TxtRes::objmenu_12", 0x004dbdc0, &H::objmenu_12);
B("TxtRes::objmenu_16", 0x004dbe80, &H::objmenu_16);
B("TxtRes::objmenu_1e", 0x004dc010, &H::objmenu_1e);
B("TxtRes::objmenu_22", 0x004dc0d0, &H::objmenu_22);
B("TxtRes::objmenu_29", 0x004dc230, &H::objmenu_29);
B("TxtRes::objmenu_44", 0x004dc420, &H::objmenu_44);
B("TxtRes::objmenu_4a", 0x004dc550, &H::objmenu_4a);
B("TxtRes::savemenu_01", 0x004dc610, &H::savemenu_01);
B("TxtRes::savemenu_05", 0x004dc6d0, &H::savemenu_05);
B("TxtRes::savemenu_0e", 0x004dc880, &H::savemenu_0e);
B("TxtRes::savemenu_11", 0x004dc910, &H::savemenu_11);
B("TxtRes::savemenu_18", 0x004dca70, &H::savemenu_18);
#undef B

}  // namespace game_b100_06
}  // namespace

// ---------------------------------------- from modules/game_b95_03.cpp (game/b95_03)
namespace {
namespace game_b95_03 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* buy_2(const char* a) { return open_menu(this, str(d::s_BUY), 2, a); }                       // 0x004da680 "BUY"
    const char* gmsg_3(const char* a, u32 b, u32 c) { return open_menu(this, str(d::s_GMSG), 3, a, b, c); }  // 0x004da8e0 "GMSG"
    const char* gmsg_10() { return menu(d::s_GMSG, 0xa); }                                           // 0x004daa60
    const char* igm_3(const char* a) { return open_menu(this, str(d::s_IGAMEMNU), 3, a); }   // 0x004dac50 "IGAMEMNU"
    const char* igm_13(const char* a) { return open_menu(this, str(d::s_IGAMEMNU), 0xd, a); }  // 0x004dae50
    const char* opt_07() { return open_text(this, str(d::s_OPTMENU), 0x07); }   // 0x004db040 "OPTMENU"
    const char* opt_10() { return menu(d::s_OPTMENU, 0x10); }   // 0x004db220
    const char* opt_18() { return menu(d::s_OPTMENU, 0x18); }   // 0x004db3a0
    const char* opt_1f() { return menu(d::s_OPTMENU, 0x1f); }   // 0x004db4f0
    const char* opt_2c() { return menu(d::s_OPTMENU, 0x2c); }   // 0x004db760
    const char* opt_34() { return menu(d::s_OPTMENU, 0x34); }   // 0x004db8b0
    const char* vict_4() { return menu(d::s_VICT, 0x04); }   // 0x004dba30 "VICT"
    const char* obj_0e() { return menu(d::s_OBJMENU, 0x0e); }   // 0x004dbd00 "OBJMENU"
    const char* obj_17() { return menu(d::s_OBJMENU, 0x17); }   // 0x004dbeb0
    const char* obj_1f() { return menu(d::s_OBJMENU, 0x1f); }   // 0x004dc040
    const char* obj_27() { return open_text(this, str(d::s_OBJMENU), 0x27); }   // 0x004dc1c0
    const char* obj_41() { return menu(d::s_OBJMENU, 0x41); }   // 0x004dc390
    const char* obj_49() { return open_text(this, str(d::s_OBJMENU), 0x49); }   // 0x004dc520
    const char* save_03() { return menu(d::s_SAVEMENU, 0x03); }  // 0x004dc670 "SAVEMENU"
    const char* save_0d() { return menu(d::s_SAVEMENU, 0x0d); }  // 0x004dc850
    const char* save_13() { return menu(d::s_SAVEMENU, 0x13); }  // 0x004dc970
    const char* gameover_2() { return menu(d::s_GAMEOVER, 0x02); }  // 0x004dcc80 "GAMEOVER"
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_03", name, addr, fn)
B95("Buy::open_2", 0x004da680, &H::buy_2);
B95("Gmsg::open_3", 0x004da8e0, &H::gmsg_3);
B95("Gmsg::open_10", 0x004daa60, &H::gmsg_10);
B95("IGameMnu::open_3", 0x004dac50, &H::igm_3);
B95("IGameMnu::open_13", 0x004dae50, &H::igm_13);
B95("OptMenu::open_07", 0x004db040, &H::opt_07);
B95("OptMenu::open_10", 0x004db220, &H::opt_10);
B95("OptMenu::open_18", 0x004db3a0, &H::opt_18);
B95("OptMenu::open_1f", 0x004db4f0, &H::opt_1f);
B95("OptMenu::open_2c", 0x004db760, &H::opt_2c);
B95("OptMenu::open_34", 0x004db8b0, &H::opt_34);
B95("Vict::open_4", 0x004dba30, &H::vict_4);
B95("ObjMenu::open_0e", 0x004dbd00, &H::obj_0e);
B95("ObjMenu::open_17", 0x004dbeb0, &H::obj_17);
B95("ObjMenu::open_1f", 0x004dc040, &H::obj_1f);
B95("ObjMenu::open_27", 0x004dc1c0, &H::obj_27);
B95("ObjMenu::open_41", 0x004dc390, &H::obj_41);
B95("ObjMenu::open_49", 0x004dc520, &H::obj_49);
B95("SaveMenu::open_03", 0x004dc670, &H::save_03);
B95("SaveMenu::open_0d", 0x004dc850, &H::save_0d);
B95("SaveMenu::open_13", 0x004dc970, &H::save_13);
B95("GameOver::open_2", 0x004dcc80, &H::gameover_2);
#undef B95

}  // namespace game_b95_03
}  // namespace

// ---------------------------------------- from modules/game_b98_03.cpp (game/b98_03)
namespace {
namespace game_b98_03 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* buy_3(u32 a) { return open_text(this, str(d::s_BUY), 3, a); }               // 0x004da6c0 "BUY"
    const char* gmsg_5(const char* a, u32 b) { return open_text(this, str(d::s_GMSG), 5, a, b); }    // 0x004da960 "GMSG"
    const char* attcomb_1(u32 a) { return open_text(this, str(d::s_ATTCOMB), 1, a); }           // 0x004dab60 "ATTCOMB"
    const char* igm_5(const char* a) { return open_menu(this, str(d::s_IGAMEMNU), 5, a); }               // 0x004dacc0 "IGAMEMNU"
    const char* opt_03() { return menu(d::s_OPTMENU, 0x03); }  // 0x004daf80 "OPTMENU"
    const char* opt_0b() { return menu(d::s_OPTMENU, 0x0b); }  // 0x004db130
    const char* opt_14() { return menu(d::s_OPTMENU, 0x14); }  // 0x004db2e0
    const char* opt_1c() { return menu(d::s_OPTMENU, 0x1c); }  // 0x004db460
    const char* opt_28() { return menu(d::s_OPTMENU, 0x28); }  // 0x004db6a0
    const char* opt_31() { return menu(d::s_OPTMENU, 0x31); }  // 0x004db850
    const char* vict_2() { return menu(d::s_VICT, 0x02); }  // 0x004db9d0 "VICT"
    const char* obj_0b() { return menu(d::s_OBJMENU, 0x0b); }  // 0x004dbc70 "OBJMENU"
    const char* obj_15() { return menu(d::s_OBJMENU, 0x15); }  // 0x004dbe50
    const char* obj_1d() { return menu(d::s_OBJMENU, 0x1d); }  // 0x004dbfe0
    const char* obj_25() { return menu(d::s_OBJMENU, 0x25); }  // 0x004dc160
    const char* obj_40() { return menu(d::s_OBJMENU, 0x40); }  // 0x004dc360
    const char* obj_47() { return open_text(this, str(d::s_OBJMENU), 0x47); }  // 0x004dc4c0
    const char* save_04() { return menu(d::s_SAVEMENU, 0x04); }  // 0x004dc6a0 "SAVEMENU"
    const char* save_0a() { return menu(d::s_SAVEMENU, 0x0a); }  // 0x004dc7c0
    const char* save_14() { return open_text(this, str(d::s_SAVEMENU), 0x14); }  // 0x004dc9a0
    const char* gameover_3() { return menu(d::s_GAMEOVER, 0x03); }  // 0x004dccb0 "GAMEOVER"
    const char* invent_04() { return menu(d::s_INVENT, 0x04); }   // 0x004dcea0 "INVENT"
    const char* invent_0c() { return menu(d::s_INVENT, 0x0c); }   // 0x004dd020
    const char* unitatt_4() { return open_text(this, str(d::s_UNITATT), 0x04); }   // 0x004dd1f0 "UNITATT"
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_03", name, addr, fn)
B98("Menu::buy_3", 0x004da6c0, &H::buy_3);
B98("Menu::gmsg_5", 0x004da960, &H::gmsg_5);
B98("Menu::attcomb_1", 0x004dab60, &H::attcomb_1);
B98("Menu::igamemnu_5", 0x004dacc0, &H::igm_5);
B98("Menu::optmenu_03", 0x004daf80, &H::opt_03);
B98("Menu::optmenu_0b", 0x004db130, &H::opt_0b);
B98("Menu::optmenu_14", 0x004db2e0, &H::opt_14);
B98("Menu::optmenu_1c", 0x004db460, &H::opt_1c);
B98("Menu::optmenu_28", 0x004db6a0, &H::opt_28);
B98("Menu::optmenu_31", 0x004db850, &H::opt_31);
B98("Menu::vict_2", 0x004db9d0, &H::vict_2);
B98("Menu::objmenu_0b", 0x004dbc70, &H::obj_0b);
B98("Menu::objmenu_15", 0x004dbe50, &H::obj_15);
B98("Menu::objmenu_1d", 0x004dbfe0, &H::obj_1d);
B98("Menu::objmenu_25", 0x004dc160, &H::obj_25);
B98("Menu::objmenu_40", 0x004dc360, &H::obj_40);
B98("Menu::objmenu_47", 0x004dc4c0, &H::obj_47);
B98("Menu::savemenu_04", 0x004dc6a0, &H::save_04);
B98("Menu::savemenu_0a", 0x004dc7c0, &H::save_0a);
B98("Menu::savemenu_14", 0x004dc9a0, &H::save_14);
B98("Menu::gameover_3", 0x004dccb0, &H::gameover_3);
B98("Menu::invent_04", 0x004dcea0, &H::invent_04);
B98("Menu::invent_0c", 0x004dd020, &H::invent_0c);
B98("Menu::unitatt_4", 0x004dd1f0, &H::unitatt_4);
#undef B98

}  // namespace game_b98_03
}  // namespace

// ---------------------------------------- from modules/game_b99_04.cpp (game/b99_04)
namespace {
namespace game_b99_04 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* buy_4() { return open_text(this, str(d::s_BUY), 0x04); }       // 0x004da700 "BUY"
    const char* gmsg_04() { return open_text(this, str(d::s_GMSG), 0x04); }     // 0x004da930 "GMSG"
    const char* gmsg_0b() { return open_text(this, str(d::s_GMSG), 0x0b); }     // 0x004daa90
    const char* igm_04() { return menu(d::s_IGAMEMNU, 0x04); }      // 0x004dac90 "IGAMEMNU"
    const char* igm_0c() { return menu(d::s_IGAMEMNU, 0x0c); }      // 0x004dae20
    const char* opt_06() { return open_text(this, str(d::s_OPTMENU), 0x06); }      // 0x004db010 "OPTMENU"
    const char* opt_0e() { return menu(d::s_OPTMENU, 0x0e); }      // 0x004db1c0
    const char* opt_11() { return menu(d::s_OPTMENU, 0x11); }      // 0x004db250
    const char* opt_19() { return menu(d::s_OPTMENU, 0x19); }      // 0x004db3d0
    const char* opt_20() { return menu(d::s_OPTMENU, 0x20); }      // 0x004db520
    const char* opt_2b() { return menu(d::s_OPTMENU, 0x2b); }      // 0x004db730
    const char* opt_32() { return menu(d::s_OPTMENU, 0x32); }      // 0x004db880
    const char* opt_37() { return menu(d::s_OPTMENU, 0x37); }      // 0x004db940
    const char* obj_09() { return menu(d::s_OBJMENU, 0x09); }      // 0x004dbc10 "OBJMENU"
    const char* obj_13() { return menu(d::s_OBJMENU, 0x13); }      // 0x004dbdf0
    const char* obj_1b() { return open_text(this, str(d::s_OBJMENU), 0x1b); }      // 0x004dbf70
    const char* obj_21() { return menu(d::s_OBJMENU, 0x21); }      // 0x004dc0a0
    const char* obj_28(u32 a) { return open_text(this, str(d::s_OBJMENU), 0x28, a); }  // 0x004dc1f0
    const char* obj_2b() { return menu(d::s_OBJMENU, 0x2b); }      // 0x004dc290
    const char* obj_45() { return open_text(this, str(d::s_OBJMENU), 0x45); }      // 0x004dc460
    const char* obj_4b() { return menu(d::s_OBJMENU, 0x4b); }      // 0x004dc580
    const char* save_06() { return open_text(this, str(d::s_SAVEMENU), 0x06); }     // 0x004dc700 "SAVEMENU"
    const char* save_0f() { return menu(d::s_SAVEMENU, 0x0f); }     // 0x004dc8b0
    const char* save_17() { return open_text(this, str(d::s_SAVEMENU), 0x17); }     // 0x004dca40
    const char* gameover_1() { return menu(d::s_GAMEOVER, 0x01); }  // 0x004dcc50 "GAMEOVER"
    const char* qsave_6() { return open_text(this, str(d::s_QSAVE), 0x06); }     // 0x004dcdd0 "QSAVE"
    const char* invent_06() { return menu(d::s_INVENT, 0x06); }   // 0x004dcf00 "INVENT"
    const char* unitatt_01() { return open_text(this, str(d::s_UNITATT), 0x01); }  // 0x004dd160 "UNITATT"
    const char* unitatt_06() { return open_text(this, str(d::s_UNITATT), 0x06); }  // 0x004dd250
    const char* unitatt_0c() { return open_text(this, str(d::s_UNITATT), 0x0c); }  // 0x004dd370
    const char* unitatt_11() { return menu(d::s_UNITATT, 0x11); }  // 0x004dd460
    const char* message_3() { return open_text(this, str(d::s_MESSAGE), 0x03); }   // 0x004dd5b0 "MESSAGE"
    const char* sms_9(const char* a) { return open_text(this, str(d::s_SMS), 0x09, a); }  // 0x004dd820 "SMS"
    const char* barrack_09() { return menu(d::s_BARRACK, 0x09); }  // 0x004ddb90 "BARRACK"
    const char* barrack_0f() { return open_text(this, str(d::s_BARRACK), 0x0f); }  // 0x004ddcc0
    const char* workshop_c() { return open_text(this, str(d::s_WORKSHOP), 0x0c); }  // 0x004ddf00 "WORKSHOP"
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_04", name, addr, fn)
B99("Menu::buy_4", 0x004da700, &H::buy_4);
B99("Menu::gmsg_04", 0x004da930, &H::gmsg_04);
B99("Menu::gmsg_0b", 0x004daa90, &H::gmsg_0b);
B99("Menu::igamemnu_04", 0x004dac90, &H::igm_04);
B99("Menu::igamemnu_0c", 0x004dae20, &H::igm_0c);
B99("Menu::optmenu_06", 0x004db010, &H::opt_06);
B99("Menu::optmenu_0e", 0x004db1c0, &H::opt_0e);
B99("Menu::optmenu_11", 0x004db250, &H::opt_11);
B99("Menu::optmenu_19", 0x004db3d0, &H::opt_19);
B99("Menu::optmenu_20", 0x004db520, &H::opt_20);
B99("Menu::optmenu_2b", 0x004db730, &H::opt_2b);
B99("Menu::optmenu_32", 0x004db880, &H::opt_32);
B99("Menu::optmenu_37", 0x004db940, &H::opt_37);
B99("Menu::objmenu_09", 0x004dbc10, &H::obj_09);
B99("Menu::objmenu_13", 0x004dbdf0, &H::obj_13);
B99("Menu::objmenu_1b", 0x004dbf70, &H::obj_1b);
B99("Menu::objmenu_21", 0x004dc0a0, &H::obj_21);
B99("Menu::objmenu_28", 0x004dc1f0, &H::obj_28);
B99("Menu::objmenu_2b", 0x004dc290, &H::obj_2b);
B99("Menu::objmenu_45", 0x004dc460, &H::obj_45);
B99("Menu::objmenu_4b", 0x004dc580, &H::obj_4b);
B99("Menu::savemenu_06", 0x004dc700, &H::save_06);
B99("Menu::savemenu_0f", 0x004dc8b0, &H::save_0f);
B99("Menu::savemenu_17", 0x004dca40, &H::save_17);
B99("Menu::gameover_1", 0x004dcc50, &H::gameover_1);
B99("Menu::qsave_6", 0x004dcdd0, &H::qsave_6);
B99("Menu::invent_06", 0x004dcf00, &H::invent_06);
B99("Menu::unitatt_01", 0x004dd160, &H::unitatt_01);
B99("Menu::unitatt_06", 0x004dd250, &H::unitatt_06);
B99("Menu::unitatt_0c", 0x004dd370, &H::unitatt_0c);
B99("Menu::unitatt_11", 0x004dd460, &H::unitatt_11);
B99("Menu::message_3", 0x004dd5b0, &H::message_3);
B99("Menu::sms_9", 0x004dd820, &H::sms_9);
B99("Menu::barrack_09", 0x004ddb90, &H::barrack_09);
B99("Menu::barrack_0f", 0x004ddcc0, &H::barrack_0f);
B99("Menu::workshop_c", 0x004ddf00, &H::workshop_c);
#undef B99

}  // namespace game_b99_04
}  // namespace

// ---------------------------------------- from modules/game_b96_03.cpp (game/b96_03)
namespace {
namespace game_b96_03 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* buy_5(u32 a) { return open_menu(this, str(d::s_BUY), 5, a); }  // 0x004da730 "BUY"
    const char* gmsg_07() { return menu(d::s_GMSG, 0x07); }     // 0x004da9d0 "GMSG"
    const char* attcomb_2() { return menu(d::s_ATTCOMB, 0x02); }   // 0x004daba0 "ATTCOMB"
    const char* igm_06() { return menu(d::s_IGAMEMNU, 0x06); }      // 0x004dad00 "IGAMEMNU"
    const char* opt_04() { return menu(d::s_OPTMENU, 0x04); }      // 0x004dafb0 "OPTMENU"
    const char* opt_0c() { return menu(d::s_OPTMENU, 0x0c); }      // 0x004db160
    const char* opt_12() { return menu(d::s_OPTMENU, 0x12); }      // 0x004db280
    const char* opt_1a() { return menu(d::s_OPTMENU, 0x1a); }      // 0x004db400
    const char* opt_22() { return menu(d::s_OPTMENU, 0x22); }      // 0x004db580
    const char* opt_2e() { return menu(d::s_OPTMENU, 0x2e); }      // 0x004db7c0
    const char* opt_36() { return menu(d::s_OPTMENU, 0x36); }      // 0x004db910
    const char* vict_05() { return menu(d::s_VICT, 0x05); }     // 0x004dba60 "VICT"
    const char* obj_11() { return menu(d::s_OBJMENU, 0x11); }      // 0x004dbd90 "OBJMENU"
    const char* obj_18() { return menu(d::s_OBJMENU, 0x18); }      // 0x004dbee0
    const char* obj_20() { return menu(d::s_OBJMENU, 0x20); }      // 0x004dc070
    const char* obj_26() { return menu(d::s_OBJMENU, 0x26); }      // 0x004dc190
    const char* obj_43() { return open_text(this, str(d::s_OBJMENU), 0x43); }      // 0x004dc3f0
    const char* obj_48() { return open_text(this, str(d::s_OBJMENU), 0x48); }      // 0x004dc4f0
    const char* save_02() { return menu(d::s_SAVEMENU, 0x02); }     // 0x004dc640 "SAVEMENU"
    const char* save_0c() { return menu(d::s_SAVEMENU, 0x0c); }     // 0x004dc820
    const char* save_12() { return menu(d::s_SAVEMENU, 0x12); }     // 0x004dc940
    const char* maingame_7() { return open_text(this, str(d::s_MAINGAME), 0x07); }  // 0x004dcc20 "MAINGAME"
    const char* inv_02() { return menu(d::s_INVENT, 0x02); }      // 0x004dce40 "INVENT"
    const char* inv_08() { return menu(d::s_INVENT, 0x08); }      // 0x004dcf60
    const char* inv_0f() { return menu(d::s_INVENT, 0x0f); }      // 0x004dd0b0
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_03", name, addr, fn)
B96("Buy::open_5", 0x004da730, &H::buy_5);
B96("Gmsg::open_7", 0x004da9d0, &H::gmsg_07);
B96("AttComb::open_2", 0x004daba0, &H::attcomb_2);
B96("IGameMnu::open_6", 0x004dad00, &H::igm_06);
B96("OptMenu::open_04", 0x004dafb0, &H::opt_04);
B96("OptMenu::open_0c", 0x004db160, &H::opt_0c);
B96("OptMenu::open_12", 0x004db280, &H::opt_12);
B96("OptMenu::open_1a", 0x004db400, &H::opt_1a);
B96("OptMenu::open_22", 0x004db580, &H::opt_22);
B96("OptMenu::open_2e", 0x004db7c0, &H::opt_2e);
B96("OptMenu::open_36", 0x004db910, &H::opt_36);
B96("Vict::open_5", 0x004dba60, &H::vict_05);
B96("ObjMenu::open_11", 0x004dbd90, &H::obj_11);
B96("ObjMenu::open_18", 0x004dbee0, &H::obj_18);
B96("ObjMenu::open_20", 0x004dc070, &H::obj_20);
B96("ObjMenu::open_26", 0x004dc190, &H::obj_26);
B96("ObjMenu::open_43", 0x004dc3f0, &H::obj_43);
B96("ObjMenu::open_48", 0x004dc4f0, &H::obj_48);
B96("SaveMenu::open_02", 0x004dc640, &H::save_02);
B96("SaveMenu::open_0c", 0x004dc820, &H::save_0c);
B96("SaveMenu::open_12", 0x004dc940, &H::save_12);
B96("MainGame::open_7", 0x004dcc20, &H::maingame_7);
B96("Invent::open_02", 0x004dce40, &H::inv_02);
B96("Invent::open_08", 0x004dcf60, &H::inv_08);
B96("Invent::open_0f", 0x004dd0b0, &H::inv_0f);
#undef B96

}  // namespace game_b96_03
}  // namespace

// ---------------------------------------- from modules/game_b97_03.cpp (game/b97_03)
namespace {
namespace game_b97_03 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* buy_6(u32 a) { return open_menu(this, str(d::s_BUY), 6, a); }        // 0x004da770 "BUY"
    const char* gmsg_6() { return menu(d::s_GMSG, 6); }                               // 0x004da9a0 "GMSG"
    const char* igm_1(const char* a) { return open_menu(this, str(d::s_IGAMEMNU), 1, a); }        // 0x004dabd0 "IGAMEMNU"
    const char* igm_7() { return menu(d::s_IGAMEMNU, 7); }                                // 0x004dad30
    const char* opt_05() { return menu(d::s_OPTMENU, 0x05); }                            // 0x004dafe0 "OPTMENU"
    const char* opt_0d() { return menu(d::s_OPTMENU, 0x0d); }                            // 0x004db190
    const char* opt_15() { return menu(d::s_OPTMENU, 0x15); }                            // 0x004db310
    const char* opt_1b() { return menu(d::s_OPTMENU, 0x1b); }                            // 0x004db430
    const char* opt_29() { return menu(d::s_OPTMENU, 0x29); }                            // 0x004db6d0
    const char* opt_30() { return menu(d::s_OPTMENU, 0x30); }                            // 0x004db820
    const char* vict_1() { return menu(d::s_VICT, 1); }                               // 0x004db9a0 "VICT"
    const char* obj_0a() { return menu(d::s_OBJMENU, 0x0a); }                            // 0x004dbc40 "OBJMENU"
    const char* obj_14() { return menu(d::s_OBJMENU, 0x14); }                            // 0x004dbe20
    const char* obj_1c(u32 a) { return open_text(this, str(d::s_OBJMENU), 0x1c, a); }    // 0x004dbfa0
    const char* obj_23() { return menu(d::s_OBJMENU, 0x23); }                            // 0x004dc100
    const char* obj_2a() { return menu(d::s_OBJMENU, 0x2a); }                            // 0x004dc260
    const char* obj_46() { return open_text(this, str(d::s_OBJMENU), 0x46); }                            // 0x004dc490
    const char* obj_4d() { return menu(d::s_OBJMENU, 0x4d); }                            // 0x004dc5e0
    const char* save_09() { return menu(d::s_SAVEMENU, 0x09); }                           // 0x004dc790 "SAVEMENU"
    const char* save_10() { return menu(d::s_SAVEMENU, 0x10); }                           // 0x004dc8e0
    const char* save_19() { return menu(d::s_SAVEMENU, 0x19); }                           // 0x004dcaa0
    const char* qsave_4() { return menu(d::s_QSAVE, 4); }                              // 0x004dcd70 "QSAVE"
    const char* invent_07() { return menu(d::s_INVENT, 0x07); }                         // 0x004dcf30 "INVENT"
    const char* invent_0e() { return menu(d::s_INVENT, 0x0e); }                         // 0x004dd080
    const char* unitatt_7() { return menu(d::s_UNITATT, 7); }                            // 0x004dd280 "UNITATT"
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_03", name, addr, fn)
B97("Buy::open_6", 0x004da770, &H::buy_6);
B97("Gmsg::open_6", 0x004da9a0, &H::gmsg_6);
B97("IGameMnu::open_1", 0x004dabd0, &H::igm_1);
B97("IGameMnu::open_7", 0x004dad30, &H::igm_7);
B97("OptMenu::open_05", 0x004dafe0, &H::opt_05);
B97("OptMenu::open_0d", 0x004db190, &H::opt_0d);
B97("OptMenu::open_15", 0x004db310, &H::opt_15);
B97("OptMenu::open_1b", 0x004db430, &H::opt_1b);
B97("OptMenu::open_29", 0x004db6d0, &H::opt_29);
B97("OptMenu::open_30", 0x004db820, &H::opt_30);
B97("Vict::open_1", 0x004db9a0, &H::vict_1);
B97("ObjMenu::open_0a", 0x004dbc40, &H::obj_0a);
B97("ObjMenu::open_14", 0x004dbe20, &H::obj_14);
B97("ObjMenu::open_1c", 0x004dbfa0, &H::obj_1c);
B97("ObjMenu::open_23", 0x004dc100, &H::obj_23);
B97("ObjMenu::open_2a", 0x004dc260, &H::obj_2a);
B97("ObjMenu::open_46", 0x004dc490, &H::obj_46);
B97("ObjMenu::open_4d", 0x004dc5e0, &H::obj_4d);
B97("SaveMenu::open_09", 0x004dc790, &H::save_09);
B97("SaveMenu::open_10", 0x004dc8e0, &H::save_10);
B97("SaveMenu::open_19", 0x004dcaa0, &H::save_19);
B97("QSave::open_4", 0x004dcd70, &H::qsave_4);
B97("Invent::open_07", 0x004dcf30, &H::invent_07);
B97("Invent::open_0e", 0x004dd080, &H::invent_0e);
B97("UnitAtt::open_7", 0x004dd280, &H::unitatt_7);
#undef B97

}  // namespace game_b97_03
}  // namespace

// ---------------------------------------- from modules/game_b103_04.cpp (game/b103_04)
namespace {
namespace game_b103_04 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* buy_7() { return menu(d::s_BUY, 7); }     // 0x004da7b0
    const char* gmsg_1() { return menu(d::s_GMSG, 1); }    // 0x004da870
    const char* igm_8() { return menu(d::s_IGAMEMNU, 8); }     // 0x004dad60
    const char* igm_9() { return menu(d::s_IGAMEMNU, 9); }     // 0x004dad90
    const char* opt_9() { return menu(d::s_OPTMENU, 9); }     // 0x004db0d0
    const char* opt_a() { return menu(d::s_OPTMENU, 0xa); }   // 0x004db100
    const char* obj_1() { return menu(d::s_OBJMENU, 1); }     // 0x004dba90
    const char* obj_2() { return menu(d::s_OBJMENU, 2); }     // 0x004dbac0
    const char* obj_3() { return menu(d::s_OBJMENU, 3); }     // 0x004dbaf0
    const char* save_7() { return menu(d::s_SAVEMENU, 7); }    // 0x004dc730
    const char* save_8() { return menu(d::s_SAVEMENU, 8); }    // 0x004dc760
    const char* qsave_2() { return menu(d::s_QSAVE, 2); }   // 0x004dcd10
    const char* qsave_3() { return menu(d::s_QSAVE, 3); }   // 0x004dcd40
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_04", name, addr, fn)
B("Buy::open_7", 0x004da7b0, &H::buy_7);
B("Gmsg::open_1", 0x004da870, &H::gmsg_1);
B("IGameMnu::open_8", 0x004dad60, &H::igm_8);
B("IGameMnu::open_9", 0x004dad90, &H::igm_9);
B("OptMenu::open_09", 0x004db0d0, &H::opt_9);
B("OptMenu::open_0a", 0x004db100, &H::opt_a);
B("ObjMenu::open_01", 0x004dba90, &H::obj_1);
B("ObjMenu::open_02", 0x004dbac0, &H::obj_2);
B("ObjMenu::open_03", 0x004dbaf0, &H::obj_3);
B("SaveMenu::open_7", 0x004dc730, &H::save_7);
B("SaveMenu::open_8", 0x004dc760, &H::save_8);
B("QSave::open_2", 0x004dcd10, &H::qsave_2);
B("QSave::open_3", 0x004dcd40, &H::qsave_3);
#undef B

}  // namespace game_b103_04
}  // namespace

// ---------------------------------------- from modules/game_b94_03.cpp (game/b94_03)
namespace {
namespace game_b94_03 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* buy_8() { return menu(d::s_BUY, 8); }          // 0x004da7e0 "BUY"
    const char* buy_9() { return menu(d::s_BUY, 9); }          // 0x004da810
    const char* gmsg_2(const char* a) { return open_menu(this, str(d::s_GMSG), 2, a); }  // 0x004da8a0 "GMSG"
    const char* gmsg_12() { return menu(d::s_GMSG, 0xc); }      // 0x004daac0
    const char* igm_10() { return menu(d::s_IGAMEMNU, 0xa); }       // 0x004dadc0 "IGAMEMNU"
    const char* igm_14() { return menu(d::s_IGAMEMNU, 0xe); }       // 0x004dae90
    const char* igm_15() { return menu(d::s_IGAMEMNU, 0xf); }       // 0x004daec0
    const char* opt_08() { return open_text(this, str(d::s_OPTMENU), 0x08); }      // 0x004db070 "OPTMENU"
    const char* opt_13() { return menu(d::s_OPTMENU, 0x13); }      // 0x004db2b0
    const char* opt_1d() { return menu(d::s_OPTMENU, 0x1d); }      // 0x004db490
    const char* opt_23() { return menu(d::s_OPTMENU, 0x23); }      // 0x004db5b0
    const char* opt_24() { return menu(d::s_OPTMENU, 0x24); }      // 0x004db5e0
    const char* opt_25() { return menu(d::s_OPTMENU, 0x25); }      // 0x004db610
    const char* opt_26() { return menu(d::s_OPTMENU, 0x26); }      // 0x004db640
    const char* opt_2d() { return open_text(this, str(d::s_OPTMENU), 0x2d); }      // 0x004db790
    const char* opt_38() { return menu(d::s_OPTMENU, 0x38); }      // 0x004db970
    const char* obj_05() { return menu(d::s_OBJMENU, 0x05); }      // 0x004dbb50 "OBJMENU"
    const char* obj_06() { return menu(d::s_OBJMENU, 0x06); }      // 0x004dbb80
    const char* obj_07() { return menu(d::s_OBJMENU, 0x07); }      // 0x004dbbb0
    const char* obj_08() { return menu(d::s_OBJMENU, 0x08); }      // 0x004dbbe0
    const char* obj_0d() { return menu(d::s_OBJMENU, 0x0d); }      // 0x004dbcd0
    const char* obj_0f() { return open_text(this, str(d::s_OBJMENU), 0x0f); }      // 0x004dbd30
    const char* obj_10() { return menu(d::s_OBJMENU, 0x10); }      // 0x004dbd60
    const char* obj_19() { return menu(d::s_OBJMENU, 0x19); }      // 0x004dbf10
    const char* obj_1a() { return open_text(this, str(d::s_OBJMENU), 0x1a); }      // 0x004dbf40
    const char* obj_24() { return menu(d::s_OBJMENU, 0x24); }      // 0x004dc130
    const char* obj_2c() { return menu(d::s_OBJMENU, 0x2c); }      // 0x004dc2c0
    const char* obj_3f() { return menu(d::s_OBJMENU, 0x3f); }      // 0x004dc330
    const char* obj_42() { return menu(d::s_OBJMENU, 0x42); }      // 0x004dc3c0
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_03", name, addr, fn)
B94("Buy::open_8", 0x004da7e0, &H::buy_8);
B94("Buy::open_9", 0x004da810, &H::buy_9);
B94("Gmsg::open_2", 0x004da8a0, &H::gmsg_2);
B94("Gmsg::open_12", 0x004daac0, &H::gmsg_12);
B94("IGameMnu::open_10", 0x004dadc0, &H::igm_10);
B94("IGameMnu::open_14", 0x004dae90, &H::igm_14);
B94("IGameMnu::open_15", 0x004daec0, &H::igm_15);
B94("OptMenu::open_08", 0x004db070, &H::opt_08);
B94("OptMenu::open_13", 0x004db2b0, &H::opt_13);
B94("OptMenu::open_1d", 0x004db490, &H::opt_1d);
B94("OptMenu::open_23", 0x004db5b0, &H::opt_23);
B94("OptMenu::open_24", 0x004db5e0, &H::opt_24);
B94("OptMenu::open_25", 0x004db610, &H::opt_25);
B94("OptMenu::open_26", 0x004db640, &H::opt_26);
B94("OptMenu::open_2d", 0x004db790, &H::opt_2d);
B94("OptMenu::open_38", 0x004db970, &H::opt_38);
B94("ObjMenu::open_05", 0x004dbb50, &H::obj_05);
B94("ObjMenu::open_06", 0x004dbb80, &H::obj_06);
B94("ObjMenu::open_07", 0x004dbbb0, &H::obj_07);
B94("ObjMenu::open_08", 0x004dbbe0, &H::obj_08);
B94("ObjMenu::open_0d", 0x004dbcd0, &H::obj_0d);
B94("ObjMenu::open_0f", 0x004dbd30, &H::obj_0f);
B94("ObjMenu::open_10", 0x004dbd60, &H::obj_10);
B94("ObjMenu::open_19", 0x004dbf10, &H::obj_19);
B94("ObjMenu::open_1a", 0x004dbf40, &H::obj_1a);
B94("ObjMenu::open_24", 0x004dc130, &H::obj_24);
B94("ObjMenu::open_2c", 0x004dc2c0, &H::obj_2c);
B94("ObjMenu::open_3f", 0x004dc330, &H::obj_3f);
B94("ObjMenu::open_42", 0x004dc3c0, &H::obj_42);
#undef B94

}  // namespace game_b94_03
}  // namespace

// ---------------------------------------- from modules/game_b82_18.cpp (game/b82_18)
namespace {
namespace game_b82_18 {

// Screen state setter: cdecl (self, name, index[, arg]).
const char* state3(void* self, img::Addr name, std::int32_t n) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(str(name)), joan::orig::arg::i32(n)));  // 0x005a3090
}
const char* state4(void* self, img::Addr name, std::int32_t n, std::uint32_t arg) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(str(name)), joan::orig::arg::i32(n), arg));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    const char* barrack6(std::uint32_t a) { return open_text(this, str(d::s_BARRACK), 6, a); }   // 0x004ddaf0
    const char* barrack5() { return open_text(this, str(d::s_BARRACK), 5); }                     // 0x004ddac0
    const char* barrack4() { return open_text(this, str(d::s_BARRACK), 4); }                     // 0x004dda90
    const char* barrack3() { return open_text(this, str(d::s_BARRACK), 3); }                     // 0x004dda60
    const char* autosave2(std::uint32_t a) { return state4(this, d::s_AUTOSAVE, 2, a); }  // 0x004dd120
    const char* autosave1(std::uint32_t a) { return state4(this, d::s_AUTOSAVE, 1, a); }  // 0x004dd0e0
    const char* maingame4(std::uint32_t a) { return open_text(this, str(d::s_MAINGAME), 4, a); }  // 0x004dcb60
    const char* upgrade1(std::uint32_t a) { return state4(this, d::s_UPGRADE, 1, a); }   // 0x004daaf0
    const char* upgrade2() { return state3(this, d::s_UPGRADE, 2); }                     // 0x004dab30
};

#define B82(name, addr, fn) JOAN_REPLACE("game/b82_18", name, addr, fn)
B82("Upgrade::state1", 0x004daaf0, &H::upgrade1);
B82("Upgrade::state2", 0x004dab30, &H::upgrade2);
B82("MainGame::state4", 0x004dcb60, &H::maingame4);
B82("Autosave::state1", 0x004dd0e0, &H::autosave1);
B82("Autosave::state2", 0x004dd120, &H::autosave2);
B82("Barrack::state3", 0x004dda60, &H::barrack3);
B82("Barrack::state4", 0x004dda90, &H::barrack4);
B82("Barrack::state5", 0x004ddac0, &H::barrack5);
B82("Barrack::state6", 0x004ddaf0, &H::barrack6);
#undef B82

}  // namespace game_b82_18
}  // namespace

// ---------------------------------------- from modules/game_b92_04.cpp (game/b92_04)
namespace {
namespace game_b92_04 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // 0x004daf20 / 0x004daf50 / 0x004db550 / 0x004dbb20 / 0x004dcb30
    const char* optmenu_1() { return open_text(this, str(d::s_OPTMENU), 1); }
    const char* optmenu_2() { return open_text(this, str(d::s_OPTMENU), 2); }
    const char* optmenu_33() { return open_menu(this, str(d::s_OPTMENU), 0x21); }
    const char* objmenu_4() { return open_menu(this, str(d::s_OBJMENU), 4); }
    const char* maingame_3() { return open_text(this, str(d::s_MAINGAME), 3); }
};

#define B92(name, addr, fn) JOAN_REPLACE("game/b92_04", name, addr, fn)
B92("OptMenu::open_1", 0x004daf20, &H::optmenu_1);
B92("OptMenu::open_2", 0x004daf50, &H::optmenu_2);
B92("OptMenu::open_33", 0x004db550, &H::optmenu_33);
B92("ObjMenu::open_4", 0x004dbb20, &H::objmenu_4);
B92("MainGame::open_3", 0x004dcb30, &H::maingame_3);
#undef B92

}  // namespace game_b92_04
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

struct H {
    void* self() { return static_cast<void*>(this); }

    // 0x004db670
    const char* load_optmenu() {
        return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self()), str(d::s_OPTMENU), 0x27));  // 0x005a3090
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Menu::load_optmenu", 0x004db670, &H::load_optmenu);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b93_02.cpp (game/b93_02)
namespace {
namespace game_b93_02 {

template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* objmenu_n(i32 n) { return open_text(this, str(d::s_OBJMENU), static_cast<i32>(static_cast<u32>(n) + 0x2du)); }         // 0x004dc2f0
    const char* savemenu_22(u32 a, u32 b) { return open_menu(this, str(d::s_SAVEMENU), 0x16, a, b); }  // 0x004dca00
    const char* maingame_5(u32 a) { return open_text(this, str(d::s_MAINGAME), 5, a); }               // 0x004dcba0
    const char* maingame_6(u32 a, u32 b) { return open_text(this, str(d::s_MAINGAME), 6, a, b); }      // 0x004dcbe0
    const char* invent_1(u32 a) { return open_menu(this, str(d::s_INVENT), 1, a); }                 // 0x004dce00
    const char* sms_4(const char* a) { return open_menu(this, str(d::s_SMS), 4, a); }                    // 0x004dd6e0
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_02", name, addr, fn)
B93("ObjMenu::open_n", 0x004dc2f0, &H::objmenu_n);
B93("SaveMenu::open_22", 0x004dca00, &H::savemenu_22);
B93("MainGame::open_5", 0x004dcba0, &H::maingame_5);
B93("MainGame::open_6", 0x004dcbe0, &H::maingame_6);
B93("Invent::open_1", 0x004dce00, &H::invent_1);
B93("Sms::open_4", 0x004dd6e0, &H::sms_4);
#undef B93

}  // namespace game_b93_02
}  // namespace

// ---------------------------------------- from modules/game_b94_04.cpp (game/b94_04)
namespace {
namespace game_b94_04 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* objmenu_76() { return open_menu(this, str(d::s_OBJMENU), 0x4c); }    // 0x004dc5b0 "OBJMENU"
    const char* savemenu_11() { return open_menu(this, str(d::s_SAVEMENU), 0xb); }    // 0x004dc7f0 "SAVEMENU"
    const char* savemenu_21() { return open_menu(this, str(d::s_SAVEMENU), 0x15); }   // 0x004dc9d0
    const char* maingame_1() { return open_text(this, str(d::s_MAINGAME), 1); }       // 0x004dcad0 "MAINGAME"
    const char* maingame_2() { return open_text(this, str(d::s_MAINGAME), 2); }       // 0x004dcb00
    const char* qsave_5() { return open_menu(this, str(d::s_QSAVE), 5); }          // 0x004dcda0 "QSAVE"
    const char* invent_10() { return open_menu(this, str(d::s_INVENT), 0xa); }      // 0x004dcfc0 "INVENT"
    const char* unitatt_5() { return open_menu(this, str(d::s_UNITATT), 5); }        // 0x004dd220 "UNITATT"
    const char* unitatt_15() { return open_menu(this, str(d::s_UNITATT), 0xf); }     // 0x004dd400
    const char* sms_3(const char* a) { return open_menu(this, str(d::s_SMS), 3, a); }    // 0x004dd6a0 "SMS"
    const char* victs_n(i32 n) { return open_menu(this, str(d::s_VICTS), n); }     // 0x004dd9c0 "VICTS"
    const char* barrack_1() { return open_text(this, str(d::s_BARRACK), 1); }        // 0x004dda00 "BARRACK"
    const char* barrack_2() { return open_text(this, str(d::s_BARRACK), 2); }        // 0x004dda30
    const char* workshop_1() { return open_menu(this, str(d::s_WORKSHOP), 1); }       // 0x004ddcf0 "WORKSHOP"
    const char* workshop_2() { return open_menu(this, str(d::s_WORKSHOP), 2); }       // 0x004ddd20
    const char* workshop_8() { return open_menu(this, str(d::s_WORKSHOP), 8); }       // 0x004dde40
    const char* workshop_19() { return open_menu(this, str(d::s_WORKSHOP), 0x13); }   // 0x004de050
    const char* mainmenu_5() { return open_menu(this, str(d::s_MAINMENU), 5); }       // 0x004de260 "MAINMENU"
    const char* armory_4() { return open_menu(this, str(d::s_ARMORY), 4); }         // 0x004de440 "ARMORY"
    const char* ending_4() { return open_menu(this, str(d::s_ENDING), 4); }         // 0x004de6c0 "ENDING"
    const char* ending_15() { return open_menu(this, str(d::s_ENDING), 0xf); }      // 0x004de8d0
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_04", name, addr, fn)
B94("ObjMenu::open_76", 0x004dc5b0, &H::objmenu_76);
B94("SaveMenu::open_11", 0x004dc7f0, &H::savemenu_11);
B94("SaveMenu::open_21", 0x004dc9d0, &H::savemenu_21);
B94("MainGame::open_1", 0x004dcad0, &H::maingame_1);
B94("MainGame::open_2", 0x004dcb00, &H::maingame_2);
B94("QSave::open_5", 0x004dcda0, &H::qsave_5);
B94("Invent::open_10", 0x004dcfc0, &H::invent_10);
B94("UnitAtt::open_5", 0x004dd220, &H::unitatt_5);
B94("UnitAtt::open_15", 0x004dd400, &H::unitatt_15);
B94("Sms::open_3", 0x004dd6a0, &H::sms_3);
B94("Victs::open_n", 0x004dd9c0, &H::victs_n);
B94("Barrack::open_1", 0x004dda00, &H::barrack_1);
B94("Barrack::open_2", 0x004dda30, &H::barrack_2);
B94("Workshop::open_1", 0x004ddcf0, &H::workshop_1);
B94("Workshop::open_2", 0x004ddd20, &H::workshop_2);
B94("Workshop::open_8", 0x004dde40, &H::workshop_8);
B94("Workshop::open_19", 0x004de050, &H::workshop_19);
B94("MainMenu::open_5", 0x004de260, &H::mainmenu_5);
B94("Armory::open_4", 0x004de440, &H::armory_4);
B94("Ending::open_4", 0x004de6c0, &H::ending_4);
B94("Ending::open_15", 0x004de8d0, &H::ending_15);
#undef B94

}  // namespace game_b94_04
}  // namespace

// ---------------------------------------- from modules/game_b100_07.cpp (game/b100_07)
namespace {
namespace game_b100_07 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* qsave_1() { return menu(d::s_QSAVE, 0x1); }      // 0x004dcce0 "QSAVE"
    const char* invent_5() { return menu(d::s_INVENT, 0x5); }     // 0x004dced0 "INVENT"
    const char* invent_9() { return menu(d::s_INVENT, 0x9); }     // 0x004dcf90
    const char* invent_d() { return menu(d::s_INVENT, 0xd); }     // 0x004dd050
    const char* unitatt_03() { return menu(d::s_UNITATT, 0x03); }  // 0x004dd1c0 "UNITATT"
    const char* unitatt_09() { return menu(d::s_UNITATT, 0x09); }  // 0x004dd2e0
    const char* unitatt_10() { return menu(d::s_UNITATT, 0x10); }  // 0x004dd430
    const char* unitatt_14() { return menu(d::s_UNITATT, 0x14); }  // 0x004dd4f0
    const char* sms_06(const char* a) { return open_menu(this, str(d::s_SMS), 0x06, a); }  // 0x004dd760 "SMS"
    const char* sms_0a(const char* a) { return open_menu(this, str(d::s_SMS), 0x0a, a); }  // 0x004dd860
    const char* sms_0f(const char* a) { return open_menu(this, str(d::s_SMS), 0x0f, a); }  // 0x004dd980
    const char* barrack_0b() { return open_text(this, str(d::s_BARRACK), 0x0b); }  // 0x004ddbf0 "BARRACK"
    const char* workshop_09() { return menu(d::s_WORKSHOP, 0x09); } // 0x004dde70 "WORKSHOP"
    const char* workshop_10() { return menu(d::s_WORKSHOP, 0x10); } // 0x004ddfc0
    const char* msglog_1(u32 a, u32 b) { return open_text(this, str(d::s_MSGLOG), 1, a, b); }  // 0x004de120 "MSGLOG"
    const char* mainmenu_2() { return menu(d::s_MAINMENU, 0x2); }   // 0x004de1d0 "MAINMENU"
    const char* mainmenu_9() { return menu(d::s_MAINMENU, 0x9); }   // 0x004de320
    const char* armory_2() { return menu(d::s_ARMORY, 0x2); }     // 0x004de3e0 "ARMORY"
    const char* popup_1() { return menu(d::s_G_POPUP, 0x1); }      // 0x004de4a0 "G_POPUP"
    const char* ending_02() { return menu(d::s_ENDING, 0x02); }   // 0x004de660 "ENDING"
    const char* ending_09() { return menu(d::s_ENDING, 0x09); }   // 0x004de7b0
    const char* ending_10() { return menu(d::s_ENDING, 0x10); }   // 0x004de900
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_07", name, addr, fn)
B100("QSave::open_1", 0x004dcce0, &H::qsave_1);
B100("Invent::open_5", 0x004dced0, &H::invent_5);
B100("Invent::open_9", 0x004dcf90, &H::invent_9);
B100("Invent::open_d", 0x004dd050, &H::invent_d);
B100("UnitAtt::open_03", 0x004dd1c0, &H::unitatt_03);
B100("UnitAtt::open_09", 0x004dd2e0, &H::unitatt_09);
B100("UnitAtt::open_10", 0x004dd430, &H::unitatt_10);
B100("UnitAtt::open_14", 0x004dd4f0, &H::unitatt_14);
B100("Sms::open_06", 0x004dd760, &H::sms_06);
B100("Sms::open_0a", 0x004dd860, &H::sms_0a);
B100("Sms::open_0f", 0x004dd980, &H::sms_0f);
B100("Barrack::open_0b", 0x004ddbf0, &H::barrack_0b);
B100("Workshop::open_09", 0x004dde70, &H::workshop_09);
B100("Workshop::open_10", 0x004ddfc0, &H::workshop_10);
B100("MsgLog::open_1", 0x004de120, &H::msglog_1);
B100("MainMenu::open_2", 0x004de1d0, &H::mainmenu_2);
B100("MainMenu::open_9", 0x004de320, &H::mainmenu_9);
B100("Armory::open_2", 0x004de3e0, &H::armory_2);
B100("GPopup::open_1", 0x004de4a0, &H::popup_1);
B100("Ending::open_02", 0x004de660, &H::ending_02);
B100("Ending::open_09", 0x004de7b0, &H::ending_09);
B100("Ending::open_10", 0x004de900, &H::ending_10);
#undef B100

}  // namespace game_b100_07
}  // namespace

// ---------------------------------------- from modules/game_b95_04.cpp (game/b95_04)
namespace {
namespace game_b95_04 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* invent_3() { return open_menu(this, str(d::s_INVENT), 3); }         // 0x004dce70 "INVENT"
    const char* invent_11() { return open_menu(this, str(d::s_INVENT), 0xb); }      // 0x004dcff0
    const char* unitatt_2() { return open_menu(this, str(d::s_UNITATT), 2); }        // 0x004dd190 "UNITATT"
    const char* unitatt_10() { return open_menu(this, str(d::s_UNITATT), 0xa); }     // 0x004dd310
    const char* unitatt_18() { return open_menu(this, str(d::s_UNITATT), 0x12); }    // 0x004dd490
    const char* message_4(const char* a) { return open_menu(this, str(d::s_MESSAGE), 4, a); }  // 0x004dd5e0 "MESSAGE"
    const char* sms_11(u32 a) { return open_menu(this, str(d::s_SMS), 0xb, a); }   // 0x004dd8a0 "SMS"
    const char* barrack_10() { return open_menu(this, str(d::s_BARRACK), 0xa); }     // 0x004ddbc0 "BARRACK"
    const char* workshop_10() { return open_menu(this, str(d::s_WORKSHOP), 0xa); }    // 0x004ddea0 "WORKSHOP"
    const char* workshop_17() { return open_menu(this, str(d::s_WORKSHOP), 0x11); }   // 0x004ddff0
    const char* mainmenu_1() { return open_menu(this, str(d::s_MAINMENU), 1); }       // 0x004de1a0 "MAINMENU"
    const char* mainmenu_8() { return open_menu(this, str(d::s_MAINMENU), 8); }       // 0x004de2f0
    const char* sceload_1() { return open_menu(this, str(d::s_SCELOAD), 1); }        // 0x004de470 "SCELOAD"
    const char* ending_1() { return open_menu(this, str(d::s_ENDING), 1); }         // 0x004de630 "ENDING"
    const char* ending_10() { return open_menu(this, str(d::s_ENDING), 0xa); }      // 0x004de7e0
    const char* ending_17() { return open_menu(this, str(d::s_ENDING), 0x11); }     // 0x004de930
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_04", name, addr, fn)
B95("Invent::open_3", 0x004dce70, &H::invent_3);
B95("Invent::open_11", 0x004dcff0, &H::invent_11);
B95("UnitAtt::open_2", 0x004dd190, &H::unitatt_2);
B95("UnitAtt::open_10", 0x004dd310, &H::unitatt_10);
B95("UnitAtt::open_18", 0x004dd490, &H::unitatt_18);
B95("Message::open_4", 0x004dd5e0, &H::message_4);
B95("Sms::open_11", 0x004dd8a0, &H::sms_11);
B95("Barrack::open_10", 0x004ddbc0, &H::barrack_10);
B95("Workshop::open_10", 0x004ddea0, &H::workshop_10);
B95("Workshop::open_17", 0x004ddff0, &H::workshop_17);
B95("MainMenu::open_1", 0x004de1a0, &H::mainmenu_1);
B95("MainMenu::open_8", 0x004de2f0, &H::mainmenu_8);
B95("SceLoad::open_1", 0x004de470, &H::sceload_1);
B95("Ending::open_1", 0x004de630, &H::ending_1);
B95("Ending::open_10", 0x004de7e0, &H::ending_10);
B95("Ending::open_17", 0x004de930, &H::ending_17);
#undef B95

}  // namespace game_b95_04
}  // namespace

// ---------------------------------------- from modules/game_b96_04.cpp (game/b96_04)
namespace {
namespace game_b96_04 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* unitatt_08() { return menu(d::s_UNITATT, 0x08); }   // 0x004dd2b0 "UNITATT"
    const char* unitatt_0e() { return menu(d::s_UNITATT, 0x0e); }   // 0x004dd3d0
    const char* unitatt_15() { return menu(d::s_UNITATT, 0x15); }   // 0x004dd520
    const char* sms_08(const char* a) { return open_menu(this, str(d::s_SMS), 8, a); }       // 0x004dd7e0 "SMS"
    const char* barrack_07() { return menu(d::s_BARRACK, 0x07); }   // 0x004ddb30 "BARRACK"
    const char* barrack_0d(u32 a) { return open_text(this, str(d::s_BARRACK), 0xd, a); }  // 0x004ddc50
    const char* workshop_0d() { return menu(d::s_WORKSHOP, 0x0d); }  // 0x004ddf30 "WORKSHOP"
    const char* quit_01() { return menu(d::s_QUIT, 0x01); }      // 0x004de0c0 "QUIT"
    const char* mainmenu_03() { return menu(d::s_MAINMENU, 0x03); }  // 0x004de200 "MAINMENU"
    const char* mainmenu_0b() { return menu(d::s_MAINMENU, 0x0b); }  // 0x004de380
    const char* gpopup_02() { return menu(d::s_G_POPUP, 0x02); }    // 0x004de4d0 "G_POPUP"
    const char* ending_03() { return menu(d::s_ENDING, 0x03); }    // 0x004de690 "ENDING"
    const char* ending_0c() { return menu(d::s_ENDING, 0x0c); }    // 0x004de840
    const char* credits_02() { return menu(d::s_CREDITS, 0x02); }   // 0x004de990 "CREDITS"
};

#define B(name, addr, fn) JOAN_REPLACE("game/b96_04", name, addr, fn)
B("UnitAtt::open_08", 0x004dd2b0, &H::unitatt_08);
B("UnitAtt::open_0e", 0x004dd3d0, &H::unitatt_0e);
B("UnitAtt::open_15", 0x004dd520, &H::unitatt_15);
B("Sms::open_08", 0x004dd7e0, &H::sms_08);
B("Barrack::open_07", 0x004ddb30, &H::barrack_07);
B("Barrack::open_0d", 0x004ddc50, &H::barrack_0d);
B("Workshop::open_0d", 0x004ddf30, &H::workshop_0d);
B("Quit::open_01", 0x004de0c0, &H::quit_01);
B("MainMenu::open_03", 0x004de200, &H::mainmenu_03);
B("MainMenu::open_0b", 0x004de380, &H::mainmenu_0b);
B("GPopup::open_02", 0x004de4d0, &H::gpopup_02);
B("Ending::open_03", 0x004de690, &H::ending_03);
B("Ending::open_0c", 0x004de840, &H::ending_0c);
B("Credits::open_02", 0x004de990, &H::credits_02);
#undef B

}  // namespace game_b96_04
}  // namespace

// ---------------------------------------- from modules/game_b98_04.cpp (game/b98_04)
namespace {
namespace game_b98_04 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }
    const char* unitatt_0b() { return menu(d::s_UNITATT, 0x0b); }   // 0x004dd340 "UNITATT"
    const char* unitatt_13() { return menu(d::s_UNITATT, 0x13); }   // 0x004dd4c0
    const char* sms_05(const char* a) { return open_menu(this, str(d::s_SMS), 5, a); }    // 0x004dd720 "SMS"
    const char* sms_0c(u32 a) { return open_menu(this, str(d::s_SMS), 0xc, a); }  // 0x004dd8e0
    const char* barrack_0c() { return open_text(this, str(d::s_BARRACK), 0x0c); }   // 0x004ddc20 "BARRACK"
    const char* workshop_0b() { return menu(d::s_WORKSHOP, 0x0b); }  // 0x004dded0 "WORKSHOP"
    const char* rts_1(u32 a, const char* b) { return open_menu(this, str(d::s_RTS), 1, a, b); }  // 0x004de080 "RTS"
    const char* mainmenu_04() { return menu(d::s_MAINMENU, 0x04); }  // 0x004de230 "MAINMENU"
    const char* mainmenu_0a() { return menu(d::s_MAINMENU, 0x0a); }  // 0x004de350
    const char* gpopup_03() { return menu(d::s_G_POPUP, 0x03); }    // 0x004de500 "G_POPUP"
    const char* ending_05() { return menu(d::s_ENDING, 0x05); }    // 0x004de6f0 "ENDING"
    const char* ending_0d() { return menu(d::s_ENDING, 0x0d); }    // 0x004de870
};

#define B98(name, addr, fn) JOAN_REPLACE("game/b98_04", name, addr, fn)
B98("UnitAtt::open_0b", 0x004dd340, &H::unitatt_0b);
B98("UnitAtt::open_13", 0x004dd4c0, &H::unitatt_13);
B98("Sms::open_05", 0x004dd720, &H::sms_05);
B98("Sms::open_0c", 0x004dd8e0, &H::sms_0c);
B98("Barrack::open_0c", 0x004ddc20, &H::barrack_0c);
B98("Workshop::open_0b", 0x004dded0, &H::workshop_0b);
B98("Rts::open_1", 0x004de080, &H::rts_1);
B98("MainMenu::open_04", 0x004de230, &H::mainmenu_04);
B98("MainMenu::open_0a", 0x004de350, &H::mainmenu_0a);
B98("GPopup::open_03", 0x004de500, &H::gpopup_03);
B98("Ending::open_05", 0x004de6f0, &H::ending_05);
B98("Ending::open_0d", 0x004de870, &H::ending_0d);
#undef B98

}  // namespace game_b98_04
}  // namespace

// ---------------------------------------- from modules/game_b97_04.cpp (game/b97_04)
namespace {
namespace game_b97_04 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    const char* menu(img::Addr name, i32 n) { return open_menu(this, str(name), n); }

    // ---- menu wrappers ----
    const char* unitatt_0d() { return menu(d::s_UNITATT, 0xd); }   // 0x004dd3a0 "UNITATT"
    const char* message_2() { return open_text(this, str(d::s_MESSAGE), 2); }      // 0x004dd580 "MESSAGE"
    const char* sms_7(const char* a) { return open_text(this, str(d::s_SMS), 7, a); }  // 0x004dd7a0 "SMS"
    const char* barrack_8() { return menu(d::s_BARRACK, 8); }      // 0x004ddb60 "BARRACK"
    const char* barrack_e() { return open_text(this, str(d::s_BARRACK), 0xe); }    // 0x004ddc90
    const char* workshop_e() { return open_text(this, str(d::s_WORKSHOP), 0xe); }   // 0x004ddf60 "WORKSHOP"
    const char* quit_2() { return open_text(this, str(d::s_QUIT), 2); }         // 0x004de0f0 "QUIT"
    const char* mainmenu_6() { return menu(d::s_MAINMENU, 6); }     // 0x004de290 "MAINMENU"
    const char* armory_1() { return open_text(this, str(d::s_ARMORY), 1); }       // 0x004de3b0 "ARMORY"
    const char* charmove_1(const char* a, u32 b, u32 c, u32 d, u32 e) {  // 0x004de530 "CHARMOVE"
        return open_text(this, str(d::s_CHARMOVE), 1, a, b, c, d, e);
    }
    const char* ending_6() { return menu(d::s_ENDING, 6); }       // 0x004de720 "ENDING"
    const char* ending_e() { return menu(d::s_ENDING, 0xe); }     // 0x004de8a0
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_04", name, addr, fn)
B("Menu::unitatt_0d", 0x004dd3a0, &H::unitatt_0d);
B("Menu::message_2", 0x004dd580, &H::message_2);
B("Menu::sms_7", 0x004dd7a0, &H::sms_7);
B("Menu::barrack_8", 0x004ddb60, &H::barrack_8);
B("Menu::barrack_e", 0x004ddc90, &H::barrack_e);
B("Menu::workshop_e", 0x004ddf60, &H::workshop_e);
B("Menu::quit_2", 0x004de0f0, &H::quit_2);
B("Menu::mainmenu_6", 0x004de290, &H::mainmenu_6);
B("Menu::armory_1", 0x004de3b0, &H::armory_1);
B("Menu::charmove_1", 0x004de530, &H::charmove_1);
B("Menu::ending_6", 0x004de720, &H::ending_6);
B("Menu::ending_e", 0x004de8a0, &H::ending_e);
#undef B

}  // namespace game_b97_04
}  // namespace

// ---------------------------------------- from modules/game_b103_05.cpp (game/b103_05)
namespace {
namespace game_b103_05 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090
template <class... A>
const char* open_menu(void* self, const char* name, i32 n, A... rest) {
    return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n), rest...));  // 0x005a3090
}

struct H {
    // ------------------------------------------------------------------------ menu wrappers
    const char* message_1() { return open_menu(this, str(d::s_MESSAGE), 1); }         // 0x004dd550 "MESSAGE"
    const char* sms_1(u32 a) { return open_menu(this, str(d::s_SMS), 1, a); }     // 0x004dd620 "SMS"
    const char* sms_2(u32 a) { return open_menu(this, str(d::s_SMS), 2, a); }     // 0x004dd660 "SMS"
    const char* ending_8() { return open_menu(this, str(d::s_ENDING), 8); }          // 0x004de780 "ENDING"
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_05", name, addr, fn)
B("Message::open_1", 0x004dd550, &H::message_1);
B("Sms::open_1", 0x004dd620, &H::sms_1);
B("Sms::open_2", 0x004dd660, &H::sms_2);
B("Ending::open_8", 0x004de780, &H::ending_8);
#undef B

}  // namespace game_b103_05
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

using joan::hook::global;

struct H {
    void* text_13() { return joan::orig::arg::as<void*>(joan::orig::strings::get(joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::str(&img::global<const char>(d::s_SMS)), 0xd)); }  // 0x004dd920
    void* text_14() { return joan::orig::arg::as<void*>(joan::orig::strings::get(joan::orig::arg::mptr(static_cast<void*>(this)), joan::orig::arg::str(&img::global<const char>(d::s_SMS)), 0xe)); }  // 0x004dd950
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Strings::text_13", 0x004dd920, &H::text_13);
W1("Strings::text_14", 0x004dd950, &H::text_14);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/game_b83_07.cpp (game/b83_07)
namespace {
namespace game_b83_07 {

struct H {
    // ------------------------------------------------------------------ WORKSHOP texts
    void* workshop(std::int32_t n) { return joan::orig::arg::as<void*>(joan::orig::strings::get(joan::orig::arg::mptr(static_cast<void*>(this)), str(d::s_WORKSHOP), joan::orig::arg::i32(n))); }  // 0x005a3090
    void* workshop_f() { return workshop(0xf); }  // 0x004ddf90
    void* workshop_7() { return workshop(7); }    // 0x004dde10
    void* workshop_6() { return workshop(6); }    // 0x004ddde0
    void* workshop_5() { return workshop(5); }    // 0x004dddb0
    void* workshop_4() { return workshop(4); }    // 0x004ddd80
    void* workshop_3() { return workshop(3); }    // 0x004ddd50
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_07", name, addr, fn)
B("Text::workshop_15", 0x004ddf90, &H::workshop_f);
B("Text::workshop_7", 0x004dde10, &H::workshop_7);
B("Text::workshop_6", 0x004ddde0, &H::workshop_6);
B("Text::workshop_5", 0x004dddb0, &H::workshop_5);
B("Text::workshop_4", 0x004ddd80, &H::workshop_4);
B("Text::workshop_3", 0x004ddd50, &H::workshop_3);
#undef B

}  // namespace game_b83_07
}  // namespace

// ---------------------------------------- from modules/game_b99_05.cpp (game/b99_05)
namespace {
namespace game_b99_05 {

const char* open_menu(void* self, const char* name, i32 n) { return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(self), joan::orig::arg::str(name), joan::orig::arg::i32(n))); }  // 0x005a3090

// The menu text: 0x005a3090 returns the string.
using joan::orig::helpers::open_text;

struct H {
    // ---------------------------------------------------------------- menu wrappers
    const char* menu_workshop_12() { return open_menu(this, str(d::s_WORKSHOP), 0x12); }      // 0x004de020
    const char* menu_msglog_2(u32 a) { return open_text(this, str(d::s_MSGLOG), 2, a); }    // 0x004de160
    const char* menu_mainmenu_7() { return open_menu(this, str(d::s_MAINMENU), 7); }          // 0x004de2c0
    const char* menu_armory_3() { return open_text(this, str(d::s_ARMORY), 3); }            // 0x004de410
    const char* menu_charmove_4() { return open_text(this, str(d::s_CHARMOVE), 4); }          // 0x004de600
    const char* menu_ending_7() { return open_menu(this, str(d::s_ENDING), 7); }            // 0x004de750
    const char* menu_ending_b() { return open_menu(this, str(d::s_ENDING), 0xb); }          // 0x004de810
    const char* menu_credits_1() { return open_text(this, str(d::s_CREDITS), 1); }           // 0x004de960
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_05", name, addr, fn)
B("Menu::workshop_12", 0x004de020, &H::menu_workshop_12);
B("Menu::msglog_2", 0x004de160, &H::menu_msglog_2);
B("Menu::mainmenu_7", 0x004de2c0, &H::menu_mainmenu_7);
B("Menu::armory_3", 0x004de410, &H::menu_armory_3);
B("Menu::charmove_4", 0x004de600, &H::menu_charmove_4);
B("Menu::ending_7", 0x004de750, &H::menu_ending_7);
B("Menu::ending_b", 0x004de810, &H::menu_ending_b);
B("Menu::credits_1", 0x004de960, &H::menu_credits_1);
#undef B

}  // namespace game_b99_05
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {

struct H {
    // 0x004de5c0
    const char* charmove3(const char* a, std::uint32_t b) {
        return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(static_cast<void*>(this)), str(d::s_CHARMOVE), 3, a, b));  // 0x005a3090
    }
    // 0x004de580
    const char* charmove2(const char* a, std::uint32_t b) {
        return joan::orig::arg::as<const char*>(joan::orig::strings::get(joan::orig::arg::mptr(static_cast<void*>(this)), str(d::s_CHARMOVE), 2, a, b));  // 0x005a3090
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("Unit::charmove3", 0x004de5c0, &H::charmove3);
B("Unit::charmove2", 0x004de580, &H::charmove2);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using joan::call::thiscall;

struct H {
    u32 load_interface() {
        joan::orig::as_txt_res(this)->load(str(d::s_RESOURCE_T_INTERF_TXR));  // 0x005a21e0 "RESOURCE\\T_INTERF.TXR"
        return 0;  // 0x005a21e0 returns nothing; the caller (mainwnd.cpp) ignores the result
    }
};

JOAN_REPLACE("game/b108_07", "Obj::load_interface", 0x004de9c0, &H::load_interface);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::TxtRes::buy_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::buy_1>(this)); }  // 0x4da650
auto joan::orig::TxtRes::buy_text_2(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_03::H::buy_2>(this, a0)); }  // 0x4da680
auto joan::orig::Menu::buy_3(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_03::H::buy_3>(this, a0)); }  // 0x4da6c0
auto joan::orig::Menu::buy_4() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::buy_4>(this)); }  // 0x4da700
auto joan::orig::TxtRes::buy_text_5(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_03::H::buy_5>(this, a0)); }  // 0x4da730
auto joan::orig::TxtRes::buy_text_6(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_03::H::buy_6>(this, a0)); }  // 0x4da770
auto joan::orig::TxtRes::buy_10() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::buy_10>(this)); }  // 0x4da840
auto joan::orig::TxtRes::gmsg_text_2(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_03::H::gmsg_2>(this, a0)); }  // 0x4da8a0
auto joan::orig::TxtRes::gmsg_text_3(const char* a0, std::uint32_t a1, std::uint32_t a2) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_03::H::gmsg_3>(this, a0, a1, a2)); }  // 0x4da8e0
auto joan::orig::Menu::gmsg_04() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::gmsg_04>(this)); }  // 0x4da930
auto joan::orig::Menu::gmsg_5(const char* a0, std::uint32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_03::H::gmsg_5>(this, a0, a1)); }  // 0x4da960
auto joan::orig::TxtRes::gmsg_text_6() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_03::H::gmsg_6>(this)); }  // 0x4da9a0
auto joan::orig::TxtRes::gmsg_text_7() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_03::H::gmsg_07>(this)); }  // 0x4da9d0
auto joan::orig::TxtRes::gmsg_8() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::gmsg_8>(this)); }  // 0x4daa00
auto joan::orig::TxtRes::gmsg_9() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::gmsg_9>(this)); }  // 0x4daa30
auto joan::orig::TxtRes::gmsg_text_10() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_03::H::gmsg_10>(this)); }  // 0x4daa60
auto joan::orig::Menu::gmsg_0b() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::gmsg_0b>(this)); }  // 0x4daa90
auto joan::orig::TxtRes::gmsg_text_12() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_03::H::gmsg_12>(this)); }  // 0x4daac0
auto joan::orig::TxtRes::upgrade_text_1(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b82_18::H::upgrade1>(this, a0)); }  // 0x4daaf0
auto joan::orig::TxtRes::upgrade_text_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b82_18::H::upgrade2>(this)); }  // 0x4dab30
auto joan::orig::Menu::attcomb_1(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_03::H::attcomb_1>(this, a0)); }  // 0x4dab60
auto joan::orig::TxtRes::attcomb_text_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_03::H::attcomb_2>(this)); }  // 0x4daba0
auto joan::orig::TxtRes::igamemnu_11() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::igamemnu_11>(this)); }  // 0x4dadf0
auto joan::orig::TxtRes::igamemnu_text_14() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_03::H::igm_14>(this)); }  // 0x4dae90
auto joan::orig::TxtRes::igamemnu_text_15() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_03::H::igm_15>(this)); }  // 0x4daec0
auto joan::orig::TxtRes::sysdet_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::sysdet_1>(this)); }  // 0x4daef0
auto joan::orig::OptMenu::open_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b92_04::H::optmenu_1>(this)); }  // 0x4daf20
auto joan::orig::OptMenu::open_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b92_04::H::optmenu_2>(this)); }  // 0x4daf50
auto joan::orig::Menu::optmenu_06() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::opt_06>(this)); }  // 0x4db010
auto joan::orig::OptMenu::open_07() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_03::H::opt_07>(this)); }  // 0x4db040
auto joan::orig::OptMenu::open_08() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_03::H::opt_08>(this)); }  // 0x4db070
auto joan::orig::TxtRes::optmenu_33() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::optmenu_33>(this)); }  // 0x4db0a0
auto joan::orig::OptMenu::open_2d() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_03::H::opt_2d>(this)); }  // 0x4db790
auto joan::orig::ObjMenu::open_0f() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_03::H::obj_0f>(this)); }  // 0x4dbd30
auto joan::orig::ObjMenu::open_1a() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_03::H::obj_1a>(this)); }  // 0x4dbf40
auto joan::orig::Menu::objmenu_1b() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::obj_1b>(this)); }  // 0x4dbf70
auto joan::orig::ObjMenu::open_1c(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_03::H::obj_1c>(this, a0)); }  // 0x4dbfa0
auto joan::orig::ObjMenu::open_27() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_03::H::obj_27>(this)); }  // 0x4dc1c0
auto joan::orig::Menu::objmenu_28(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::obj_28>(this, a0)); }  // 0x4dc1f0
auto joan::orig::ObjMenu::open_n(std::int32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b93_02::H::objmenu_n>(this, a0)); }  // 0x4dc2f0
auto joan::orig::ObjMenu::open_43() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_03::H::obj_43>(this)); }  // 0x4dc3f0
auto joan::orig::TxtRes::objmenu_44(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::objmenu_44>(this, a0)); }  // 0x4dc420
auto joan::orig::Menu::objmenu_45() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::obj_45>(this)); }  // 0x4dc460
auto joan::orig::ObjMenu::open_46() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_03::H::obj_46>(this)); }  // 0x4dc490
auto joan::orig::Menu::objmenu_47() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_03::H::obj_47>(this)); }  // 0x4dc4c0
auto joan::orig::ObjMenu::open_48() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_03::H::obj_48>(this)); }  // 0x4dc4f0
auto joan::orig::ObjMenu::open_49() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_03::H::obj_49>(this)); }  // 0x4dc520
auto joan::orig::TxtRes::objmenu_4a() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::objmenu_4a>(this)); }  // 0x4dc550
auto joan::orig::Menu::savemenu_06() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::save_06>(this)); }  // 0x4dc700
auto joan::orig::SaveMenu::open_13() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_03::H::save_13>(this)); }  // 0x4dc970
auto joan::orig::Menu::savemenu_14() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_03::H::save_14>(this)); }  // 0x4dc9a0
auto joan::orig::SaveMenu::open_21() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::savemenu_21>(this)); }  // 0x4dc9d0
auto joan::orig::SaveMenu::open_22(std::uint32_t a0, std::uint32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b93_02::H::savemenu_22>(this, a0, a1)); }  // 0x4dca00
auto joan::orig::Menu::savemenu_17() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::save_17>(this)); }  // 0x4dca40
auto joan::orig::TxtRes::savemenu_18() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_06::H::savemenu_18>(this)); }  // 0x4dca70
auto joan::orig::SaveMenu::open_19() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_03::H::save_19>(this)); }  // 0x4dcaa0
auto joan::orig::MainGame::open_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::maingame_1>(this)); }  // 0x4dcad0
auto joan::orig::MainGame::open_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::maingame_2>(this)); }  // 0x4dcb00
auto joan::orig::MainGame::open_3() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b92_04::H::maingame_3>(this)); }  // 0x4dcb30
auto joan::orig::MainGame::state4(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b82_18::H::maingame4>(this, a0)); }  // 0x4dcb60
auto joan::orig::MainGame::open_5(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b93_02::H::maingame_5>(this, a0)); }  // 0x4dcba0
auto joan::orig::MainGame::open_6(std::uint32_t a0, std::uint32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b93_02::H::maingame_6>(this, a0, a1)); }  // 0x4dcbe0
auto joan::orig::MainGame::open_7() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_03::H::maingame_7>(this)); }  // 0x4dcc20
auto joan::orig::TxtRes::qsave_text_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::qsave_1>(this)); }  // 0x4dcce0
auto joan::orig::TxtRes::qsave_text_4() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_03::H::qsave_4>(this)); }  // 0x4dcd70
auto joan::orig::TxtRes::qsave_text_5() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::qsave_5>(this)); }  // 0x4dcda0
auto joan::orig::Menu::qsave_6() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::qsave_6>(this)); }  // 0x4dcdd0
auto joan::orig::TxtRes::invent_text_1(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b93_02::H::invent_1>(this, a0)); }  // 0x4dce00
auto joan::orig::TxtRes::autosave_text_1(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b82_18::H::autosave1>(this, a0)); }  // 0x4dd0e0
auto joan::orig::TxtRes::autosave_text_2(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b82_18::H::autosave2>(this, a0)); }  // 0x4dd120
auto joan::orig::Menu::unitatt_01() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::unitatt_01>(this)); }  // 0x4dd160
auto joan::orig::TxtRes::unitatt_text_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_04::H::unitatt_2>(this)); }  // 0x4dd190
auto joan::orig::TxtRes::unitatt_text_03() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::unitatt_03>(this)); }  // 0x4dd1c0
auto joan::orig::Menu::unitatt_4() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_03::H::unitatt_4>(this)); }  // 0x4dd1f0
auto joan::orig::TxtRes::unitatt_text_5() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::unitatt_5>(this)); }  // 0x4dd220
auto joan::orig::Menu::unitatt_06() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::unitatt_06>(this)); }  // 0x4dd250
auto joan::orig::TxtRes::unitatt_text_7() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_03::H::unitatt_7>(this)); }  // 0x4dd280
auto joan::orig::TxtRes::unitatt_text_08() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_04::H::unitatt_08>(this)); }  // 0x4dd2b0
auto joan::orig::TxtRes::unitatt_text_09() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::unitatt_09>(this)); }  // 0x4dd2e0
auto joan::orig::TxtRes::unitatt_text_10() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_04::H::unitatt_10>(this)); }  // 0x4dd310
auto joan::orig::TxtRes::unitatt_text_0b() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_04::H::unitatt_0b>(this)); }  // 0x4dd340
auto joan::orig::Menu::unitatt_0c() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::unitatt_0c>(this)); }  // 0x4dd370
auto joan::orig::TxtRes::unitatt_text_15() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_04::H::unitatt_15>(this)); }  // 0x4dd520
auto joan::orig::Menu::message_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_04::H::message_2>(this)); }  // 0x4dd580
auto joan::orig::Menu::message_3() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::message_3>(this)); }  // 0x4dd5b0
auto joan::orig::TxtRes::message_text_4(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_04::H::message_4>(this, a0)); }  // 0x4dd5e0
auto joan::orig::Sms::open_3(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::sms_3>(this, a0)); }  // 0x4dd6a0
auto joan::orig::Sms::open_4(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b93_02::H::sms_4>(this, a0)); }  // 0x4dd6e0
auto joan::orig::Sms::open_05(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_04::H::sms_05>(this, a0)); }  // 0x4dd720
auto joan::orig::Sms::open_06(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::sms_06>(this, a0)); }  // 0x4dd760
auto joan::orig::Menu::sms_7(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_04::H::sms_7>(this, a0)); }  // 0x4dd7a0
auto joan::orig::Sms::open_08(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_04::H::sms_08>(this, a0)); }  // 0x4dd7e0
auto joan::orig::Menu::sms_9(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::sms_9>(this, a0)); }  // 0x4dd820
auto joan::orig::Sms::open_0a(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::sms_0a>(this, a0)); }  // 0x4dd860
auto joan::orig::Sms::open_11(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_04::H::sms_11>(this, a0)); }  // 0x4dd8a0
auto joan::orig::Sms::open_0c(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_04::H::sms_0c>(this, a0)); }  // 0x4dd8e0
auto joan::orig::TxtRes::sms_text_13() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::text_13>(this)); }  // 0x4dd920
auto joan::orig::TxtRes::sms_text_14() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_world::H::text_14>(this)); }  // 0x4dd950
auto joan::orig::Sms::open_0f(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::sms_0f>(this, a0)); }  // 0x4dd980
auto joan::orig::TxtRes::victs_text(std::int32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::victs_n>(this, a0)); }  // 0x4dd9c0
auto joan::orig::Barrack::open_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::barrack_1>(this)); }  // 0x4dda00
auto joan::orig::Barrack::open_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::barrack_2>(this)); }  // 0x4dda30
auto joan::orig::Barrack::state3() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b82_18::H::barrack3>(this)); }  // 0x4dda60
auto joan::orig::Barrack::state4() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b82_18::H::barrack4>(this)); }  // 0x4dda90
auto joan::orig::Barrack::state5() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b82_18::H::barrack5>(this)); }  // 0x4ddac0
auto joan::orig::Barrack::state6(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b82_18::H::barrack6>(this, a0)); }  // 0x4ddaf0
auto joan::orig::Barrack::open_0b() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::barrack_0b>(this)); }  // 0x4ddbf0
auto joan::orig::Barrack::open_0c() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_04::H::barrack_0c>(this)); }  // 0x4ddc20
auto joan::orig::Barrack::open_0d(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_04::H::barrack_0d>(this, a0)); }  // 0x4ddc50
auto joan::orig::Menu::barrack_e() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_04::H::barrack_e>(this)); }  // 0x4ddc90
auto joan::orig::Menu::barrack_0f() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::barrack_0f>(this)); }  // 0x4ddcc0
auto joan::orig::TxtRes::workshop_text_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::workshop_1>(this)); }  // 0x4ddcf0
auto joan::orig::TxtRes::workshop_text_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::workshop_2>(this)); }  // 0x4ddd20
auto joan::orig::Text::workshop_3() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::workshop_3>(this)); }  // 0x4ddd50
auto joan::orig::Text::workshop_4() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::workshop_4>(this)); }  // 0x4ddd80
auto joan::orig::Text::workshop_5() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::workshop_5>(this)); }  // 0x4dddb0
auto joan::orig::Text::workshop_6() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::workshop_6>(this)); }  // 0x4ddde0
auto joan::orig::Text::workshop_7() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::workshop_7>(this)); }  // 0x4dde10
auto joan::orig::Menu::workshop_c() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_04::H::workshop_c>(this)); }  // 0x4ddf00
auto joan::orig::TxtRes::workshop_text_0d() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_04::H::workshop_0d>(this)); }  // 0x4ddf30
auto joan::orig::Menu::workshop_e() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_04::H::workshop_e>(this)); }  // 0x4ddf60
auto joan::orig::Text::workshop_15() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b83_07::H::workshop_f>(this)); }  // 0x4ddf90
auto joan::orig::TxtRes::workshop_text_10() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::workshop_10>(this)); }  // 0x4ddfc0
auto joan::orig::TxtRes::workshop_text_17() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_04::H::workshop_17>(this)); }  // 0x4ddff0
auto joan::orig::TxtRes::workshop_text_19() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::workshop_19>(this)); }  // 0x4de050
auto joan::orig::TxtRes::rts_text_1(std::uint32_t a0, const char* a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_04::H::rts_1>(this, a0, a1)); }  // 0x4de080
auto joan::orig::TxtRes::quit_text_01() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_04::H::quit_01>(this)); }  // 0x4de0c0
auto joan::orig::Menu::quit_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_04::H::quit_2>(this)); }  // 0x4de0f0
auto joan::orig::MsgLog::open_1(std::uint32_t a0, std::uint32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::msglog_1>(this, a0, a1)); }  // 0x4de120
auto joan::orig::Menu::msglog_2(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_05::H::menu_msglog_2>(this, a0)); }  // 0x4de160
auto joan::orig::TxtRes::mainmenu_text_0a() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b98_04::H::mainmenu_0a>(this)); }  // 0x4de350
auto joan::orig::Menu::armory_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_04::H::armory_1>(this)); }  // 0x4de3b0
auto joan::orig::TxtRes::armory_text_2() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::armory_2>(this)); }  // 0x4de3e0
auto joan::orig::Menu::armory_3() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_05::H::menu_armory_3>(this)); }  // 0x4de410
auto joan::orig::TxtRes::armory_text_4() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b94_04::H::armory_4>(this)); }  // 0x4de440
auto joan::orig::TxtRes::sceload_text_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_04::H::sceload_1>(this)); }  // 0x4de470
auto joan::orig::Menu::charmove_1(const char* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b97_04::H::charmove_1>(this, a0, a1, a2, a3, a4)); }  // 0x4de530
auto joan::orig::Unit::charmove2(const char* a0, std::uint32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b90_02::H::charmove2>(this, a0, a1)); }  // 0x4de580
auto joan::orig::Unit::charmove3(const char* a0, std::uint32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b90_02::H::charmove3>(this, a0, a1)); }  // 0x4de5c0
auto joan::orig::Menu::charmove_4() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_05::H::menu_charmove_4>(this)); }  // 0x4de600
auto joan::orig::Ending::open_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b95_04::H::ending_1>(this)); }  // 0x4de630
auto joan::orig::Ending::open_09() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b100_07::H::ending_09>(this)); }  // 0x4de7b0
auto joan::orig::Menu::credits_1() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b99_05::H::menu_credits_1>(this)); }  // 0x4de960
auto joan::orig::TxtRes::credits_text_02() -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_04::H::credits_02>(this)); }  // 0x4de990
auto joan::orig::TxtRes::load_interface_txr() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_07::H::load_interface>(this)); }  // 0x4de9c0
