// Display resolution ([Display] in openjoan.ini).
//
// joan.exe is written for any screen size: its 2D layout reads the current
// screen description at 0x00a48858 (width 0x00a4885c, height 0x00a48860, the
// screen rectangles after them; ~700 references), which is copied from the
// table at 0x00a016a0 when the display starts. Only two places pin it to
// 1024x768: that table's initial values and the display mode the Direct3D
// start-up code searches for (immediates at 0x005f6c79 / 0x005f6c80). Patching
// both makes the game run at any resolution; the 3D projection follows the
// screen size by itself. Screens drawn for a fixed 1024x768 layout (menus,
// message box, dialogue, map) are dealt with separately.
#pragma once

#include <cstdint>

namespace joan::display {

struct Size {
    int width = 1024;
    int height = 768;
};

/// Applies the resolution patches (call once at start-up, before the game's
/// WinMain). Does nothing for Resolution=original or when the community
/// widescreen d3d8.dll is installed (it patches the same places).
void apply_patches();

/// The screen size the game was set up for (1024x768 unless patched).
Size game_size();

// --- 2D canvas -------------------------------------------------------------------------
// Some screens (main menu, loading screen, credits, in-game / save / option menus, message
// log, objectives, quit popup) are drawn for a fixed 1024x768 layout at the top-left corner
// of the screen. While one of their draw functions runs, the renderer maps game-screen
// pixels p to p * scale + offset, i.e. the 1024x768 layout onto a centred rectangle; while
// such a screen is shown the mouse works in layout coordinates.
struct Canvas {
    bool enabled = false;  // screen larger than 1024x768
    float scale = 1.0f;
    float x = 0.0f, y = 0.0f;  // offset in screen pixels
};
const Canvas& canvas();
/// Inside one of the 1024x768 screens' draw functions.
bool canvas_drawing();
/// A 1024x768 screen is being shown (the game's mouse position is in layout coordinates).
bool canvas_armed();
/// How a draw function's 1024x768 layout is placed on the screen.
enum class Layout {
    screen,  // whole screens: scaled to fit, centred (the canvas)
    hud,     // HUD pieces (messages): unscaled, centred at the bottom
    hud_left,  // HUD pieces (dialogue): unscaled, bottom-left corner
    ui,      // the in-mission HUD (UiScale): each piece scaled towards its nearest edge / corner
    unit,    // a text string: all its glyphs share the placement of its first one
    ui_off,  // inside the HUD but tied to the 3D world (health bars over heads): not scaled
    ui_center,  // a panel centred on the screen (shop / blacksmith): scaled about the screen centre
    ui_corner,  // a panel in the top-left corner (the inventory): scaled as one block about that corner
    hud_top,    // a 1024x768-layout piece at the top (the "Press R to talk" prompt): centred, UiScale
    hud_top_right,  // a 1024x768-layout piece in the top-right corner (under the minimap): UiScale, from that corner
};
/// Draw-function hooks: entering / leaving a draw function laid out for 1024x768.
void layout_enter(Layout layout, bool opaque = false);
/// A full 1024x768 screen (not an overlay over the game like the quit popup) was drawn in the
/// frame just presented (valid after canvas_end_frame): the bars beside it are cleared to black.
bool opaque_screen_shown();
/// The frame presented last showed a full 1024x768 screen (used while drawing the next one).
bool opaque_screen_active();
/// The loading screen (Game::draw_loading_screen): entering / leaving its draw function; after it was
/// shown, true until loading_done (the renderer keeps the level's slow first frames black meanwhile).
void loading_screen(bool drawing);
bool loading_just_ended();
/// Inside the loading screen's draw function (its 3D view keeps the modern effects).
bool loading_drawing();
void loading_done();
void layout_leave();
/// Inside a layout scope: the pieces queued until it is undone are placed for `layout` instead of the scope's (one
/// piece of a screen that belongs elsewhere, e.g. the talk prompt's hire price under the minimap). Returns the
/// layout to put back; outside a layout scope nothing changes.
Layout swap_layout(Layout layout);
/// swap_layout for the lifetime of the object.
class LayoutSwap {
   public:
    explicit LayoutSwap(Layout layout) : previous_(swap_layout(layout)) {}
    ~LayoutSwap() { swap_layout(previous_); }
    LayoutSwap(const LayoutSwap&) = delete;
    LayoutSwap& operator=(const LayoutSwap&) = delete;

   private:
    Layout previous_;
};
/// Kinds ui / unit (not layout scopes): entering / leaving the HUD or a text string.
void ui_enter(Layout kind);
void ui_leave(Layout kind);
/// Around a draw that follows the game's mouse (the dragged inventory item): its pieces are kept
/// at the real cursor under UiScale.
/// Around a draw that scales its own 2D pieces for the screen (lens flares, the archer icon): the "unscaled
/// 2D" diagnostic leaves them out.
void self_scaled_enter();
void self_scaled_leave();
struct SelfScaled {
    SelfScaled() { self_scaled_enter(); }
    ~SelfScaled() { self_scaled_leave(); }
    SelfScaled(const SelfScaled&) = delete;
    SelfScaled& operator=(const SelfScaled&) = delete;
};
void ui_cursor_enter();
void ui_cursor_leave();
/// [Display] UiScale in effect (1 = the original pixel size).
float ui_scale();
/// UiScale changed while the game runs (the options menu): `setting` as [Display] UiScale (0 = auto).
void set_ui_scale(float setting);
/// Called after every change of the UI scale: `listener` re-places what was placed once with the old scale (the shop's
/// portrait cameras). Only one listener is kept.
void set_ui_scale_listener(void (*listener)());
/// Sprite hook with the queueing function's frame pointer (diagnostics of unscaled 2D pieces).
void canvas_sprite_from(std::int32_t* args, std::uintptr_t frame);
/// Where the cursor really is (back-buffer pixels) while HUD clicks are remapped for UiScale;
/// false when the game's own mouse position is the cursor position.
bool ui_cursor(int& x, int& y);
/// Camera::set_viewport hook: args[2..5] = x1, y1, x2, y2; caller = return address.
void camera_viewport(std::int32_t* args, std::uintptr_t caller);
/// Sprite hook: places a sprite's screen rectangle (x1, y1, x2, y2 in args[0..3]) for the
/// outermost layout being drawn.
void canvas_sprite(std::int32_t* args);
/// Map-object hook (0x006349b0): places a 2D-layer model at args[0..1] (x, y shorts) with the
/// scale in args[7] (float) for the canvas.
void canvas_model(std::int32_t* args);
/// Renderer: lowest edge of this frame's 3D viewports (0 = no 3D drawn). The dialogue is
/// centred in the cinematic bar below it.
void set_scene_bottom(int bottom);
/// {1024, 768}: code laid out for 1024x768 reads this instead of the screen size.
std::int32_t* layout_size();
/// Screen size the message boxes centre in: {1024, 768} while a 1024x768 screen is shown or
/// drawn, the real screen size otherwise.
std::int32_t* box_size();
/// Called by the renderer once per presented frame.
void canvas_end_frame();
/// Retargets the screens' draw functions (call after apply_patches and hook::install_all).
/// C++ replacements of the patched / wrapped functions use canvas_api.hpp instead.
void install_canvas_hooks();

}  // namespace joan::display
