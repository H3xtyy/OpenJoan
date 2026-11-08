// Between the SDL modules of the platform layer (sdl_window.cpp, sdl_input.cpp).
#pragma once

#include <SDL3/SDL.h>

/// Declarations shared between the SDL modules of the platform layer (window and input).
namespace joan::platform::detail {

// sdl_window.cpp
/// The game window (nullptr before CreateWindowExA / after DestroyWindow).
SDL_Window* window();
/// Does the game window have the keyboard focus?
bool focused();
/// Is this HWND the game window?
bool is_window(void* hwnd);
/// [Platform] Gamepad (game pads through SDL's gamepad API).
bool gamepads_enabled();

// sdl_input.cpp
/// A keyboard / mouse / game-pad event taken from SDL's queue by the window's pump.
void on_input_event(const SDL_Event& e);
/// The game window gained / lost the focus (foreground DirectInput devices are lost with it).
void on_focus(bool focused);

#ifndef _WIN32
/// Windows puts the primary display's top-left at (0, 0) and the game counts on it (its window at 0,0 is on the
/// primary); X11 / Wayland count from the desktop's top-left. The primary display's origin in SDL's coordinates: what
/// is added to the game's screen coordinates for SDL and taken off SDL's for the game.
void screen_origin(int& x, int& y);
#endif

}  // namespace joan::platform::detail
