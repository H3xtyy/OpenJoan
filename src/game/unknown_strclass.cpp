// game/unknown_strclass.cpp: functions whose original source file is unknown (formerly game/_unplaced/strclass.cpp;
// re/attribution.csv: between ww/ilib/sound3d.cpp and ww/ilib/txtres.cpp). They move to their original file when
// evidence names it (Phase 3).
// ilib/sound3d.cpp and the string pool; original file unknown, the registrations use the key ilib/str.cpp).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>

#include "../original/gameset.hpp"
#include "../registry.hpp"

// ---------------------------------------- from ilib/gameset.cpp (ilib/str.cpp)
namespace {
namespace ilib_gameset {

using joan::orig::String;

#define STR(name, addr, fn) JOAN_REPLACE("ilib/str.cpp", "String::" name, addr, fn)
STR("String()", 0x005a1170, static_cast<String* (String::*)()>(&String::construct));
STR("String(char*)", 0x005a11a0, static_cast<String* (String::*)(const char*)>(&String::construct));
STR("String(String&)", 0x005a11f0, &String::construct_copy);
STR("substr", 0x005a1230, &String::substr);
STR("upper", 0x005a12e0, &String::upper);
STR("lower", 0x005a1340, &String::lower);
STR("at", 0x005a13a0, &String::at);
STR("operator=(String&)", 0x005a1400, static_cast<String* (String::*)(const String*)>(&String::assign));
STR("operator=(char*)", 0x005a1440, static_cast<String* (String::*)(const char*)>(&String::assign));
STR("operator=(int)", 0x005a1490, static_cast<String* (String::*)(std::int32_t)>(&String::assign));
STR("operator+=(String&)", 0x005a14f0, &String::append_str);
STR("operator+=(char*)", 0x005a1550, static_cast<String* (String::*)(const char*)>(&String::append));
STR("operator+=(int)", 0x005a15d0, static_cast<String* (String::*)(std::int32_t)>(&String::append));
STR("operator*=(int)", 0x005a1640, &String::repeat);
STR("operator+=(char)", 0x005a16d0, static_cast<String* (String::*)(char)>(&String::append));
#undef STR

}  // namespace ilib_gameset
}  // namespace
