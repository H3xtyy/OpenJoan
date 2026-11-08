// The key bindings page's control set: Keyboard / Gamepad (interfac/remap_set.cpp).
//
// The original key bindings page (OptMenu page 5, REMAP KEYS on INPUT OPTIONS) shows and edits the bindings of the
// input method INPUT OPTIONS has selected: KEYBOARD (Bindings mode 0, KeyBinding::key), KEYBOARD & MOUSE (mode 3,
// key_mouse / mouse_button) or JOYSTICK (mode 1, joy_button / key_alt). This adds a button right of the category
// selector that switches the page between the keyboard set (the input method's keyboard type; KEYBOARD & MOUSE when
// INPUT OPTIONS is on the joystick) and the gamepad set, whatever the input method. Opening the page starts on the
// set of the input method in use.
#pragma once

#include <cstdint>

namespace joan::optmenu::remap {

/// The Bindings mode (0 / 1 / 3) the key bindings page shows and edits.
std::int32_t mode(void* menu);
/// Opening the key bindings page (from INPUT OPTIONS): back to the input method's set.
void reset(void* menu);

/// OptPanel::init / Screen::release_749e00 / the instant layout on opening / every frame / OptionMenu::draw.
void create(void* menu);
void release(void* menu);
void hide_now(void* menu);
void update(void* menu);
void draw(void* menu);
/// The page's widgets slide in (CreditsScreen::layout 0x00748540) / out (0x00748830).
void slide_in(void* menu, std::uint32_t delay);
void slide_out(void* menu, std::uint32_t delay);
/// Page 5, no slot being bound: the button clicked switches the set. True when it did.
bool input(void* menu);

}  // namespace joan::optmenu::remap
