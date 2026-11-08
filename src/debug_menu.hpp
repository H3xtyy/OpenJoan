// In-game developer menu (module "debug/menu", on by default; [Hooks] Disable=debug/menu turns it off).
//
// Opened / closed with [Debug] MenuKey (default the ` key, VK_OEM_3 = 0xC0). Pages:
//   Missions   - jump to any of the 8 campaign missions (the game's own developer
//                "next mission" path: 0x0041cc80 + game state 12)
//   Hero       - the controlled hero's strength / dexterity / defense / leadership
//                (+ / - with Left / Right, Shift = x10), max HP, EP, level (+1, Shift +5)
//   Spawn      - unit types with HORSE in the name, spawned in front of the hero
//   Cheats     - the game's hidden console commands (invincible, fog of war, gold)
// Keys: Up / Down select, Enter / Right open or apply, Esc / Backspace back.
// While the menu is open the game's per-frame input handler (0x0052f930) is not run,
// so the game ignores keys and clicks (the world keeps running).
//
// The menu is drawn by the OpenGL device (Renderer=opengl) over the presented frame.
#pragma once

#include <cstdint>

namespace joan::debugmenu {

struct Overlay {
    bool visible = false;
    int width = 0, height = 0;          // pixels
    const std::uint32_t* pixels = nullptr;  // BGRA, top row first, premultiplied-free (straight alpha)
    unsigned version = 0;               // changes whenever the pixels change
};

/// The current menu image (game thread only).
const Overlay& overlay();

}  // namespace joan::debugmenu
