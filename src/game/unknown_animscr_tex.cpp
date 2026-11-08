// game/unknown_animscr_tex.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/animscr_tex.cpp; re/attribution.csv: between ww/animscr.cpp and ww/enparticle/peffect.cpp). They move
// to their original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_leaves80b.cpp (game/leaves80b)
namespace {
namespace game_leaves80b {

using joan::call::thiscall;
using joan::img::obj;

template <class R = std::uint32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

constexpr img::Addr kSysEn(d::g_sys3d);

JOAN_COUNTER(c_tex_release, "Texture::release");

struct H {
    // 0x004191e0
    void texture_release() {
        c_tex_release.hit();
        if ((joan::orig::arg::as<std::uint8_t>(joan::orig::as_device_resource(static_cast<void*>(&jo::self<jo::Plane>(this)->resource))->is_linked()) & 0xff) == 0) return;  // 0x005d2b10 linked
        if (jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) != nullptr) {
            com_call<std::uint32_t>(jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer), 2);  // Release
            jo::view<void*>(jo::self<jo::Plane>(this)->vertex_buffer) = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) != nullptr) {
            com_call<std::uint32_t>(jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer), 2);  // Release
            jo::view<void*>(jo::self<jo::Plane>(this)->index_buffer) = nullptr;
        }
        joan::orig::as_sys_en(img::obj(kSysEn))->unlink_resource(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Plane>(this)->resource)));  // 0x005ff380 unlink
    }
};

#define L80B(name, addr, fn) JOAN_REPLACE("game/leaves80b", name, addr, fn)
L80B("Texture::release", 0x004191e0, &H::texture_release);
#undef L80B

}  // namespace game_leaves80b
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Texture::release() -> void { joan::bind::member<&::game_leaves80b::H::texture_release>(this); }  // 0x4191e0
