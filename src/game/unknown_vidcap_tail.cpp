// game/unknown_vidcap_tail.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/vidcap_tail.cpp; re/attribution.csv: between ww/uti3d/videocap.cpp and ww/enphysic/collimgr.cpp). They
// move to their original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstddef>
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

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07 {

using i32 = std::int32_t;

using u32 = std::uint32_t;
using u64 = std::uint64_t;

JOAN_COUNTER(c_sound_update, "SoundBuf::update");

using joan::orig::helpers::u;

struct H {
    // ------------------------------------------------------------------ 0x00687060
    std::uintptr_t sound_update(const void* src, i32 ms) {
        c_sound_update.hit();
        jo::VideoCapture* const vc = jo::self<jo::VideoCapture>(this);
        std::uintptr_t eax = u(this);
        if (vc->active == 0 || vc->writer == nullptr) return eax;
        if (src != nullptr) {
            if (src != vc->buffer) {
                const u32 n = static_cast<u32>(vc->bpp) * static_cast<u32>(vc->width) * static_cast<u32>(vc->height);
                eax = u(std::memcpy(joan::orig::arg::mptr(vc->buffer), joan::orig::arg::ptr(src), joan::orig::arg::u32(n)));  // memcpy
            } else {
                eax = u(src);
            }
        }
        if (vc->input == -1) return eax;
        const u64 pos = static_cast<u64>(static_cast<u32>(ms)) * 10000u;  // _allmul
        // IWMWriter::WriteSample (slot 14): (input, QWORD sample time in 100 ns, flags, INSSBuffer*). The QWORD is one
        // 8-byte argument (two stack dwords, low first, on x86; one register on x64).
        using Fn = u32(JOAN_STDCALL*)(jo::IWMWriter*, i32, u64, u32, jo::INSSBuffer*);
        const Fn fn = reinterpret_cast<Fn>(joan::call::vtable_of(vc->writer)[0x38 / 4]);
        return fn(vc->writer, vc->input, pos, 0, vc->sample);
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("SoundBuf::update", 0x00687060, &H::sound_update);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::VideoCapture::write_frame(void* a0, std::int32_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b94_07::H::sound_update>(this, a0, a1)); }  // 0x687060
