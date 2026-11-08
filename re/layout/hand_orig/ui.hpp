// 2D UI objects of the en2d widget library (refactor R7b, docs/RUNTIME.md): the sprite queue's named-sprite entry,
// the widget slide / scale animations, Button::draw and Font::measure. Only the fields the reimplemented members
// read or write are named; everything else is padding. The members are defined in the modules that implemented them
// before (see each declaration) and are still registered there as the replacements of the original addresses.
#pragma once

#include "../call.hpp"
#include "../fn_names.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace joan::orig {

struct BmpRes;

#pragma pack(push, 1)

/// The 2D sprite queue (0x00a46128 and the per-screen canvases). No field is read by the members here.
struct SpriteQueue {
    /// 0x00633500 SpriteQueue::draw_named (src/game/_unplaced/after_en2d_txtbox2d.cpp): looks `name` up in the sprite set `set`,
    /// fills the -1 defaults of the source rectangle / destination corner from the set entry and the texture size and
    /// queues the sprite (0x00632a70). Returns 0 when the sprite is not drawn, else 0x00632a70's eax. The original
    /// reads only the low word of every argument except layer / colour (dwords) and the two pointers.
    std::uint32_t draw_named(std::int16_t x, std::int16_t y, std::uint32_t layer, const char* name, void* set,
                             std::uint32_t colour, std::int16_t a20, std::int16_t a24, std::int16_t a28, std::int16_t a2c,
                             std::int16_t src_x0, std::int16_t src_y0, std::int16_t src_x1, std::int16_t src_y1,
                             std::int16_t rot, std::int16_t x1, std::int16_t y1);

    // The other SpriteQueue functions the game code calls (typed, through their slots).
#include "gen/sprite_queue_methods.inc"
};

/// Base of the en2d widgets: the rectangle +0x9..+0xf, the alpha / scale factor +0x11 and the slide (+0x21..)
/// and scale (+0x5e..) animations.
struct Widget {
    std::uint8_t head_00[9];
    std::int16_t x;              // 0x09
    std::int16_t y;              // 0x0b
    std::int16_t x2;             // 0x0d
    std::int16_t y2;             // 0x0f
    float scale;                 // 0x11 current factor (Button::draw scales the colour alpha by it)
    std::uint32_t move_arg;      // 0x15
    std::uint8_t gap_19[4];
    std::uint32_t scale_arg;     // 0x1d
    std::int16_t target_x;       // 0x21
    std::int16_t target_y;       // 0x23
    std::uint32_t move_25;       // 0x25
    std::uint32_t move_29;       // 0x29
    float move_cos;              // 0x2d
    float move_sin;              // 0x31
    std::uint8_t moving;         // 0x35
    std::uint8_t move_36;        // 0x36
    std::uint8_t gap_37[0x27];
    std::uint8_t scaling;        // 0x5e
    std::uint8_t scale_5f;       // 0x5f
    float scale_target;          // 0x60
    std::uint32_t scale_64;      // 0x64

    /// 0x0061e020 Widget::move_to (src/game/_unplaced/after_en3d_vgalock.cpp): slide to (new_x, new_y); `instant` moves at once. ret 0x18.
    Widget* move_to(std::int16_t new_x, std::int16_t new_y, float speed, float accel, char instant, std::uint32_t arg);
    /// 0x0061f1b0 Widget::scale_to (src/game/_unplaced/after_en3d_vgalock.cpp): animate +0x11 to `target`. ret 0x14.
    Widget* scale_to(std::int16_t target, float speed, float accel, char instant, std::uint32_t arg);

    // The other Widget functions the game code calls (typed, through their slots).
#include "gen/widget_methods.inc"
};

/// One state of a button (normal / pressed / hover / disabled): sprite name, colour and three words for the queue.
struct ButtonState {
    char sprite[9];         // +0x00 (empty: the state is not drawn)
    std::uint32_t colour;   // +0x09
    std::int16_t a20;       // +0x0d
    std::int16_t a24;       // +0x0f
    std::int16_t a28;       // +0x11
};

struct Button {
    std::uint8_t head_00[4];
    std::uint8_t child;              // 0x04 sub-object passed to Child::visible 0x00623360
    std::uint8_t gap_05;
    std::int8_t fit;                 // 0x06 fit the rectangle to the screen (0x00633e60 / 0x006340e0)
    std::int8_t enabled;             // 0x07
    std::uint8_t gap_08;
    std::int16_t x;                  // 0x09
    std::int16_t y;                  // 0x0b
    std::int16_t x2;                 // 0x0d
    std::int16_t y2;                 // 0x0f
    float scale;                     // 0x11
    std::uint8_t gap_15[0x5f];
    std::int8_t shown;               // 0x74
    std::int16_t tri[6];             // 0x75 hit triangle (shape 1)
    std::uint8_t gap_81[9];
    std::int8_t pressed;             // 0x8a
    std::uint8_t gap_8b;
    std::uint8_t drawn;              // 0x8c
    std::int8_t shape;               // 0x8d hit test: 0 rectangle, 1 triangle
    std::int8_t i8_8e;               // 0x8e (round P4: raw reads in button2d / scrlbox; spec: shape[1])
    std::uint8_t hover;              // 0x8f
    std::uint8_t gap_90[3];
    BmpRes* sprite_set;              // 0x93
    ButtonState states[4];           // 0x97
    std::uint8_t gap_e3[0x144];
    char caption[0x101];             // 0x227
    void* caption_box;               // 0x328 TextRenderer (0x00626e00)
    std::uint32_t caption_colour;    // 0x32c
    std::int16_t caption_330;        // 0x330
    std::int16_t caption_332;        // 0x332
    std::int16_t caption_334;        // 0x334
    std::int16_t press_dx;           // 0x336
    std::int16_t press_dy;           // 0x338
    std::uintptr_t callback;         // 0x33a cdecl (button, arg, flags) -> nonzero when drawn

    /// 0x00622cc0 Button::draw (src/game/_unplaced/after_en2d_bmpres.cpp): the callback or the state sprite and the caption.
    /// The original reads only the low byte of `flags`. Returns 0 / 1. ret 8.
    std::int32_t draw(std::uint32_t arg, std::uint8_t flags);

    // The other Button functions the game code calls (typed, through their slots).
#include "gen/button_methods.inc"
};

struct Font {
    std::uint8_t loaded;             // 0x00
    std::uint8_t gap_01[0x35];
    std::int16_t space_width;        // 0x36
    std::int16_t spacing;            // 0x38

    /// 0x00624f00 Font::measure (src/game/_unplaced/after_en2d_font2d.cpp): width of `text` (at most `max_chars`, < 0 = all),
    /// wrapped at `wrap` (0 = no wrap), `mode` 1 = upper case; counts the lines into 0x00a69ece. ret 0x14.
    std::int16_t measure(const char* text, std::int16_t max_chars, std::int16_t wrap, std::int16_t mode, float scale);

    // The other Font functions the game code calls (typed, through their slots).
#include "gen/font_methods.inc"
};

#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ButtonState) == 0x13);
static_assert(offsetof(Widget, x) == 0x09 && offsetof(Widget, y2) == 0x0f && offsetof(Widget, scale) == 0x11);
static_assert(offsetof(Widget, move_arg) == 0x15 && offsetof(Widget, scale_arg) == 0x1d);
static_assert(offsetof(Widget, target_x) == 0x21 && offsetof(Widget, target_y) == 0x23);
static_assert(offsetof(Widget, move_25) == 0x25 && offsetof(Widget, move_29) == 0x29);
static_assert(offsetof(Widget, move_cos) == 0x2d && offsetof(Widget, move_sin) == 0x31);
static_assert(offsetof(Widget, moving) == 0x35 && offsetof(Widget, move_36) == 0x36);
static_assert(offsetof(Widget, scaling) == 0x5e && offsetof(Widget, scale_5f) == 0x5f);
static_assert(offsetof(Widget, scale_target) == 0x60 && offsetof(Widget, scale_64) == 0x64);
static_assert(offsetof(Button, child) == 0x04 && offsetof(Button, fit) == 0x06 && offsetof(Button, enabled) == 0x07);
static_assert(offsetof(Button, x) == 0x09 && offsetof(Button, y2) == 0x0f && offsetof(Button, scale) == 0x11);
static_assert(offsetof(Button, shown) == 0x74 && offsetof(Button, tri) == 0x75);
static_assert(offsetof(Button, pressed) == 0x8a && offsetof(Button, drawn) == 0x8c && offsetof(Button, shape) == 0x8d);
static_assert(offsetof(Button, hover) == 0x8f && offsetof(Button, sprite_set) == 0x93 && offsetof(Button, states) == 0x97);
static_assert(offsetof(Button, caption) == 0x227 && offsetof(Button, caption_box) == 0x328);
static_assert(offsetof(Button, caption_colour) == 0x32c && offsetof(Button, caption_330) == 0x330);
static_assert(offsetof(Button, press_dx) == 0x336 && offsetof(Button, press_dy) == 0x338 && offsetof(Button, callback) == 0x33a);
static_assert(offsetof(Font, space_width) == 0x36 && offsetof(Font, spacing) == 0x38);
#endif

}  // namespace joan::orig
