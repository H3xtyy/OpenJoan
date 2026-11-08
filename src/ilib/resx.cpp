// d:\ww\ilib\resx.cpp - ResourceIdx (short-index archives: *.SET, RESOURCE,
// GAMEPLAY.RES, ...), joan.exe 0x0059c550-0x0059d180. All 14 functions.
// Implementation shared with reslx.cpp: res_idx_impl.hpp.
#include "../callconv.hpp"
#include "../original/direct.hpp"
#include "../fn_names.hpp"
#include "res_idx_impl.hpp"
#include <cstdint>
#include <cstring>
#include "../data_names.hpp"
#include "../registry.hpp"
#include "../original/dynarray.hpp"
#include "../x87.hpp"

template struct joan::orig::BasicResourceIdx<joan::orig::ShortIndexTraits>;

namespace {
using joan::orig::ResIdxCounters;
using joan::orig::ResourceIdx;
using joan::orig::ShortIndexTraits;

// Create the counters at load time so unused functions show up with 0 calls.
[[maybe_unused]] auto& g_counters = ResIdxCounters<ShortIndexTraits>::get();
}  // namespace

#define RESX(name, addr, fn) JOAN_REPLACE("ilib/resx.cpp", "ResourceIdx::" name, addr, fn)
RESX("init", 0x0059c550, &ResourceIdx::init);
RESX("deinit", 0x0059c780, &ResourceIdx::deinit);
RESX("read_into", 0x0059c8d0, &ResourceIdx::read_into);
RESX("read(name)", 0x0059c950, &ResourceIdx::read);
RESX("get_data_size(name)", 0x0059c9a0, &ResourceIdx::get_data_size_by_name);
RESX("get_data_size(rec)", 0x0059ca10, &ResourceIdx::get_data_size);
RESX("get_index", 0x0059ca60, &ResourceIdx::get_index);
RESX("read(rec)", 0x0059cbb0, &ResourceIdx::read_rec);
RESX("get_file(name)", 0x0059ce40, &ResourceIdx::get_file);
RESX("get_file(rec)", 0x0059cee0, &ResourceIdx::get_file_rec);
RESX("set_user_buf", 0x0059cf60, &ResourceIdx::set_user_buf);
RESX("clear_user_buf", 0x0059cfb0, &ResourceIdx::clear_user_buf);
RESX("get_index_rec", 0x0059cff0, &ResourceIdx::get_index_rec);
RESX("build_sorted_index", 0x0059d050, &ResourceIdx::build_sorted_index);
#undef RESX

// ---------------------------------------- from modules/game_b103_10.cpp (game/b103_10)
namespace {
namespace game_b103_10 {

using i32 = std::int32_t;

using joan::img::str;

static i32 JOAN_CDECL cmp_59d160(const char* a, const char* b) { return std::strcmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_10", name, addr, fn)
B("cmp_59d160", 0x0059d160, &cmp_59d160);
#undef B

}  // namespace game_b103_10
}  // namespace
