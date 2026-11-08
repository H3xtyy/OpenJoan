// game/unknown_weffects.cpp: functions whose original source file is unknown (formerly game/_unplaced/weffects.cpp;
// re/attribution.csv: between ww/uti3d/water.cpp and ww/world.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/bind.hpp"

// ---------------------------------------- from modules/game_b100_08.cpp (game/b100_08)
namespace {
namespace game_b100_08 {

// ------------------------------------------------------------------------------- thiscall
struct H {
    // 0x00547030
    void nop_547030() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_08", name, addr, fn)
B("Obj::nop_547030", 0x00547030, &H::nop_547030);
#undef B

}  // namespace game_b100_08
}  // namespace

// ---------------------------------------- from modules/game_b104_07.cpp (game/b104_07)
namespace {
namespace game_b104_07 {

using joan::call::cdecl_call;

using i32 = std::int32_t;

using joan::call::thiscall;

using joan::img::obj;
using joan::img::str;

using joan::hook::global;

JOAN_COUNTER(c_weather_ini, "load_weather_ini");

// ---------------------------------------------------------------- cdecl
// 0x00547140
void JOAN_CDECL load_weather_ini() {
    c_weather_ini.hit();
    void* const ini = obj(d::g_steer);
    auto entry = [ini](img::Addr section, img::Addr key) -> const char* {
        void* const sec = joan::orig::arg::as<void*>(joan::orig::as_ini(ini)->section(joan::orig::arg::str(str(section))));  // 0x0047cbf0
        char* const e = joan::orig::arg::as<char*>(joan::orig::as_ini(sec)->value(joan::orig::arg::str(str(key))));       // 0x0047c180
        return e + 9;
    };
    auto atoi_ = [](const char* s) { return joan::crt::atoi(joan::orig::arg::str(s)); };
    auto atof_ = [](const char* s) { return static_cast<float>(joan::orig::arg::as<double>(joan::orig::libc::atof(joan::orig::arg::str(s)))); };
    const img::Addr weffects = d::s_WEFFECTS, cloudsys = d::s_CLOUDSYS;
    global<i32>(d::g_a3cddc) = atoi_(entry(weffects, d::s_LG_MND));  // LG_MND
    global<i32>(d::g_weather_lg_r) = atoi_(entry(weffects, d::s_LG_R));  // LG_R
    global<i32>(d::g_a3cdd4) = atoi_(entry(weffects, d::s_LG_MNH));  // LG_MNH
    global<i32>(d::g_a3cde0) = atoi_(entry(weffects, d::s_LG_MXH));  // LG_MXH
    global<i32>(d::g_a3cdd8) = atoi_(entry(weffects, d::s_LG_T));  // LG_T
    global<i32>(d::g_a3cdcc) = atoi_(entry(weffects, d::s_LG_LT));  // LG_LT
    global<i32>(d::g_a3cdec) = atoi_(entry(weffects, d::s_LG_ITV));  // LG_ITV
    global<float>(d::g_weather_lg_v) = atof_(entry(weffects, d::s_LG_V));  // LG_V
    global<float>(d::g_a3cdc8) = atof_(entry(weffects, d::s_LG_G));  // LG_G
    global<float>(d::g_a3cdf4) = atof_(entry(cloudsys, d::s_CL_SPD));  // CL_SPD
    global<float>(d::g_a3cdd0) = atof_(entry(cloudsys, d::s_CL_MND));  // CL_MND
    global<float>(d::g_a3cde8) = atof_(entry(cloudsys, d::s_CL_MXD));  // CL_MXD
}

#define B(name, addr, fn) JOAN_REPLACE("game/b104_07", name, addr, fn)
B("load_weather_ini", 0x00547140, &load_weather_ini);
#undef B

}  // namespace game_b104_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::load_weather_ini() -> void { joan::bind::free<&::game_b104_07::load_weather_ini>(); }  // 0x547140
