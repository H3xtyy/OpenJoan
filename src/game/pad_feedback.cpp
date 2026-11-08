// Game pad vibration (platform/input.hpp rumble, [Platform] PadRumble): once per frame of a mission (Hud::draw
// 0x006f9b60) the controlled hero is compared with the last frame:
//   - hit points lost: a rumble growing with the share of the hero's maximum lost;
//   - knocked down (KNOCKDN / KNOCKDN2 / KNOCKDN3): a heavy one;
//   - a hit landed (the combo counter [0xa5ba62] went up): a short tap, firmer every tenth hit.
// Switching heroes or a new mission starts over (no rumble for the jump in hit points).

#include "pad_feedback.hpp"

#include <cstdint>

#include "../data_names.hpp"
#include "../original/direct.hpp"
#include "../original/layout_access.hpp"
#include "../platform/input.hpp"
#include "../registry.hpp"

namespace jo = joan::orig;

namespace joan::padfx {
namespace {

struct Last {
    std::int32_t unit = 0, hp = 0, combo = 0;
    std::int32_t anim = 0;
};
Last g_last;

bool knocked_down(std::int32_t anim) { return anim == 64 || anim == 65 || anim == 66; }  // KNOCKDN, KNOCKDN2, KNOCKDN3

}  // namespace

void frame() {
    const std::int32_t id = joan::hook::global<std::int32_t>(d::g_player_unit_recno);  // the controlled hero
    void* const units = joan::img::obj(d::g_units);
    if (id == 0 || joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(id)) != 0) {  // 0x00428b70
        g_last = Last{};
        return;
    }
    std::uint8_t* const u = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
    const std::int32_t hp = jo::as_unit(u)->hit_points;
    const std::int32_t anim = static_cast<std::int32_t>(jo::as_unit(u)->anim_kind);
    const std::int32_t combo = static_cast<std::int32_t>(joan::hook::global<std::int16_t>(d::g_kill_combo));
    if (g_last.unit != id) {  // another hero (or the first frame): nothing to compare with
        g_last = Last{id, hp, combo, anim};
        return;
    }
    namespace in = joan::platform::input;
    if (knocked_down(anim) && !knocked_down(g_last.anim)) {
        in::rumble(0.9f, 0.6f, 350);
    } else if (hp < g_last.hp) {
        const std::int32_t max = joan::orig::arg::as<std::int32_t>(joan::orig::as_unit(u)->hit_points_max());  // 0x00523320
        const float share = max > 0 ? static_cast<float>(g_last.hp - hp) / static_cast<float>(max) : 0.1f;
        const float low = share * 4.0f + 0.35f;
        in::rumble(low > 1.0f ? 1.0f : low, 0.2f, static_cast<unsigned>(150.0f + (share > 1.0f ? 1.0f : share) * 600.0f));
    } else if (combo > g_last.combo) {
        if (combo % 10 == 0)
            in::rumble(0.45f, 0.6f, 120);
        else
            in::rumble(0.12f, 0.35f, 60);
    }
    g_last = Last{id, hp, combo, anim};
}

}  // namespace joan::padfx
