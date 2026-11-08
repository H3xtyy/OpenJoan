// d:\ww\ilib\reslx.cpp - ResourceIdxL (long-name archives: WAVE.RES,
// animscr.res, texteff.res, S*L1_*.RES, ...), joan.exe 0x0059b920-0x0059c550.
// All 14 functions; the original is a copy of resx.cpp with 34-byte names
// and a 32-bit record count. Implementation shared: res_idx_impl.hpp.
#include "../callconv.hpp"
#include "../original/direct.hpp"
#include "../fn_names.hpp"
#include "res_idx_impl.hpp"
#include <cstdint>
#include <cstring>
#include "../data_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"

template struct joan::orig::BasicResourceIdx<joan::orig::LongIndexTraits>;

namespace {
using joan::orig::LongIndexTraits;
using joan::orig::ResIdxCounters;
using joan::orig::ResourceIdxL;

[[maybe_unused]] auto& g_counters = ResIdxCounters<LongIndexTraits>::get();
}  // namespace

#define RESLX(name, addr, fn) JOAN_REPLACE("ilib/reslx.cpp", "ResourceIdxL::" name, addr, fn)
RESLX("init", 0x0059b920, &ResourceIdxL::init);
RESLX("deinit", 0x0059bb40, &ResourceIdxL::deinit);
RESLX("read_into", 0x0059bc90, &ResourceIdxL::read_into);
RESLX("read(name)", 0x0059bd10, &ResourceIdxL::read);
RESLX("get_data_size(name)", 0x0059bd60, &ResourceIdxL::get_data_size_by_name);
RESLX("get_data_size(rec)", 0x0059bdd0, &ResourceIdxL::get_data_size);
RESLX("get_index", 0x0059be20, &ResourceIdxL::get_index);
RESLX("read(rec)", 0x0059bf70, &ResourceIdxL::read_rec);
RESLX("get_file(name)", 0x0059c200, &ResourceIdxL::get_file);
RESLX("get_file(rec)", 0x0059c2a0, &ResourceIdxL::get_file_rec);
RESLX("set_user_buf", 0x0059c320, &ResourceIdxL::set_user_buf);
RESLX("clear_user_buf", 0x0059c370, &ResourceIdxL::clear_user_buf);
RESLX("get_index_rec", 0x0059c3b0, &ResourceIdxL::get_index_rec);
RESLX("build_sorted_index", 0x0059c440, &ResourceIdxL::build_sorted_index);
#undef RESLX

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
//
//    - cmp_59c410 0x0059c410 / cmp_59d160 0x0059d160 (cdecl, a, b) (dead code): 0x7b16f0(a, b).
namespace {
namespace game_b103_10 {

using i32 = std::int32_t;

using joan::img::str;

// 0x0059c410 / 0x0059d160 (dead code)
static i32 JOAN_CDECL cmp_59c410(const char* a, const char* b) { return std::strcmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("cmp_59c410", 0x0059c410, &cmp_59c410);
#undef B

}  // namespace game_b103_10
}  // namespace
