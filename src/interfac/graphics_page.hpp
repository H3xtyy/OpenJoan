// The options menu's GRAPHICS OPTIONS page and the openjoan.ini rows of VIDEO OPTIONS (interfac/graphics_page.cpp).
//
// The original OPTIONS screen (OptMenu, interfac/optmenui.cpp) has four pages behind four buttons: 1 VIDEO, 2 AUDIO,
// 3 INPUT, 4 GAME (5 = key bindings, 6 = closing). This adds:
//   - a fifth button, GRAPHICS OPTIONS, under VIDEO OPTIONS, for page kPage (7);
//   - on GRAPHICS OPTIONS: Anisotropy, MSAA, Texture Filter, Ambient Occlusion and Water (openjoan.ini), then the
//     original Draw Distance, Dynamic Grass, its draw distance and density, and Shadow (moved from VIDEO OPTIONS);
//   - on VIDEO OPTIONS: Resolution and Borderless (openjoan.ini) above the original Brightness, Contrast and Gamma,
//     then VSync, Max FPS and UI Scale (openjoan.ini).
// The openjoan.ini rows are widgets of the game's own classes (a label button and a < value > list, as the original
// rows), owned here. Their values apply at once where they can (VSync, Max FPS, UI Scale, Anisotropy, Texture
// Filter, Ambient Occlusion, Water); Resolution, Borderless and MSAA set up the window and back buffer and say
// "(restart)". Leaving either page writes the changed ones to openjoan.ini.
#pragma once

#include <cstdint>

namespace joan::optmenu::graphics {

inline constexpr std::int32_t kPage = 7;  // OptMenu::page of GRAPHICS OPTIONS

// The layout of the two pages (y of a row's label, before the in-game offset; its value widget sits 7 higher).
inline constexpr std::int32_t kVideoTop = 0xf3, kVideoStep = 0x34;      // VIDEO OPTIONS: 8 rows, the original spacing
inline constexpr std::int32_t kGraphicsTop = 0xdd, kGraphicsStep = 0x30; // GRAPHICS OPTIONS: 10 rows
inline constexpr std::int32_t video_row(std::int32_t i) { return kVideoTop + i * kVideoStep; }
inline constexpr std::int32_t graphics_row(std::int32_t i) { return kGraphicsTop + i * kGraphicsStep; }

// The dark panel behind the value column is the FRAME image (OptMenu::title, 0x21e tall): each page slides it so its
// bottom edge sits 0xc above the bottom of its last value widget (0x3e tall, 7 above the label). INPUT OPTIONS (5
// rows) puts it at -0x30, the original VIDEO OPTIONS (8 rows) at 0x6c.
inline constexpr std::int32_t frame_y(std::int32_t last_label) { return last_label - 7 + 0x3e - 0xc - 0x21e; }
inline constexpr std::int32_t kVideoFrameY = frame_y(video_row(7));        // 0x6c, as the original 8 rows
inline constexpr std::int32_t kGraphicsFrameY = frame_y(graphics_row(9));  // 0x9a
static_assert(frame_y(0xf3 + 7 * 0x34) == 0x6c, "the original VIDEO OPTIONS frame");

// The OPTIONS list (page 0): the five buttons' y, top to bottom (VIDEO, GRAPHICS, AUDIO, INPUT, GAME).
inline constexpr std::int32_t kTabY[5] = {0x130, 0x178, 0x1c0, 0x208, 0x250};

/// OptPanel::init: makes the widgets (hidden).
void create(void* menu);
/// Screen::release_749e00: frees them.
void release(void* menu);
/// The menu's instant layout on opening (0x0074b6b0): every widget of ours hidden at once.
void hide_now(void* menu);
/// Every frame while the menu is shown: the widgets' slide animations (0x0061f3b0).
void update(void* menu);
/// OptionMenu::draw: draws the GRAPHICS OPTIONS button and the rows.
void draw(void* menu);

/// The GRAPHICS OPTIONS button on the OPTIONS list: slides in / out (with the original four, `delay` ms).
void tab_in(void* menu, std::uint32_t delay);
void tab_out(void* menu, std::uint32_t delay);
/// True when the GRAPHICS OPTIONS button was clicked (page 0).
bool tab_clicked(void* menu);

/// Our rows of VIDEO OPTIONS / the whole GRAPHICS OPTIONS page slide in / out. graphics_in also brings the moved
/// original rows (draw distance, grass, shadow), the title and the DEFAULT / BACK buttons; graphics_out slides out our
/// rows only (the caller runs the original slide-out of the video widgets, 0x0074c2e0).
void video_in(void* menu);
void video_out(void* menu);
void graphics_in(void* menu);
void graphics_out(void* menu);

/// Input on page 1 / kPage: our rows (hover, < > clicks). True when a value changed.
bool input(void* menu, std::int32_t page);
/// DEFAULT pressed on page 1 / kPage: our rows of that page back to their default values.
void reset_defaults(void* menu, std::int32_t page);
/// Leaving page 1 / kPage: the changed values to openjoan.ini.
void save();

/// The page title of GRAPHICS OPTIONS (OptionMenu::draw).
const char* title();

}  // namespace joan::optmenu::graphics
