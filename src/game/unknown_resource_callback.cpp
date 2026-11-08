// game/unknown_resource_callback.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/misc_005e9010.cpp; re/attribution.csv: between ww/en3d/reorder.cpp and ww/en3d/rgb.cpp). They move to
// their original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../x87.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

}  // namespace

// ---------------------------------------- from modules/en3d_helpers20.cpp (en3d/helpers20)
namespace {
namespace en3d_helpers20 {

struct H {
    H* colour_set(void* a, void* b, std::uint32_t c) {  // 0x005e9010
        jo::self<jo::Resource>(this)->callback = a;
        jo::self<jo::Resource>(this)->context = b;
        jo::self<jo::Resource>(this)->arg = c;
        jo::self<jo::Resource>(this)->owner = nullptr;
        return this;
    }
};

#define HELPER20(name, addr, fn) JOAN_REPLACE("en3d/helpers20", name, addr, fn)
HELPER20("Callback::set", 0x005e9010, &H::colour_set);
#undef HELPER20

}  // namespace en3d_helpers20
}  // namespace

// ---------------------------------------- from modules/en3d_helpers25.cpp (en3d/helpers25)
namespace {
namespace en3d_helpers25 {

struct H {
    H* resource_reset() {                                          // 0x005e9060
        jo::view<void*>(jo::self<jo::Resource>(this)->owner) = nullptr;
        return this;
    }
};

#define HELPER25(name, addr, fn) JOAN_REPLACE("en3d/helpers25", name, addr, fn)
HELPER25("Resource::reset", 0x005e9060, &H::resource_reset);
#undef HELPER25

}  // namespace en3d_helpers25
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::DeviceResource::set_callback(void* a0, void* a1, std::uint32_t a2) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers20::H::colour_set>(this, a0, a1, a2)); }  // 0x5e9010
auto joan::orig::DeviceResource::clear_link() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_helpers25::H::resource_reset>(this)); }  // 0x5e9060
