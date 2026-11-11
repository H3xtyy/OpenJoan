// The game's keyboard and mouse, read natively from the platform layer (phase C4, sdl_input.cpp): what the game's
// Input class (src/ilib/mousecr.cpp: Input::init / release / unacquire_all / reacquire / poll) uses instead of
// DirectInput. No Windows or SDL types: the same calls will serve a Linux build.
//
// The model is DirectInput's buffered devices, which the game's code was written for:
// - two devices, the mouse (exclusive: relative motion, the system cursor hidden and kept in the window) and the
//   keyboard; both only while the game window has the focus;
// - acquire() every frame (fails while the window is in the background), read() the buffered items since the last
//   read: mouse X / Y / wheel deltas (offsets 0 / 4 / 8; wheel in 120ths of a notch) and buttons (offset 0xc + n,
//   data 0x80 = down), keyboard keys (offset = DIK code, the PC set-1 scan code with 0x80 for extended keys);
//   time stamps in the game's timeGetTime base, sequence numbers shared by both devices (the game merges the two
//   streams by them).
// Every call is recorded / replayed by the replay layer (Id::IN_*): a replay never reads the real devices.
#pragma once

#include <cstdint>
#include <deque>

/// The game's keyboard and mouse read from the platform layer, modelled on buffered DirectInput devices.
namespace joan::platform::input {

/// The two input devices: the mouse and the keyboard.
enum class Device : std::int32_t { Mouse = 0, Keyboard = 1 };

/// One buffered item; the layout of DIDEVICEOBJECTDATA (0x14 bytes), which the game's poll loop reads.
struct Item {
    std::uint32_t ofs;       ///< Object offset: mouse 0 / 4 / 8 = X / Y / wheel, 0xc + n = button n; keyboard = DIK code.
    std::uint32_t data;      ///< Motion delta (wheel: 120ths of a notch) or key / button state (0x80 = down).
    std::uint32_t time;      ///< Time stamp in the game's timeGetTime base (milliseconds).
    std::uint32_t sequence;  ///< Sequence number shared by both devices; the game merges the streams by it.
    std::uint32_t app_data;  ///< DIDEVICEOBJECTDATA::uAppData; kept for the layout.
};
static_assert(sizeof(Item) == 0x14);

/// Results, DirectInput's HRESULT values (the game's code tests for 0 / 1).
constexpr std::int32_t kOk = 0;
constexpr std::int32_t kNoEffect = 1;  ///< already acquired / not acquired / buffer overflow (data lost)
/// Result: the device is not acquired (the HRESULT 0x8007000c the game's code receives).
constexpr std::int32_t kNotAcquired = static_cast<std::int32_t>(0x8007000cu);
constexpr std::int32_t kOtherAppHasPriority = static_cast<std::int32_t>(0x80070005u);  ///< window in the background

/// Input::init: the devices with their buffer sizes (items kept between two reads).
void open(std::uint32_t mouse_buffer, std::uint32_t keyboard_buffer);
/// Input::release.
void close();
/// Acquires the device (the game does this every frame; fails while the window is in the background: kOtherAppHasPriority); returns a result code of this namespace.
std::int32_t acquire(Device d);
/// Releases the device (Input::unacquire_all); returns a result code of this namespace.
std::int32_t unacquire(Device d);
/// Up to *count items into `items`; *count = items delivered. kOk, kNoEffect (items were lost), kNotAcquired.
std::int32_t read(Device d, Item* items, std::uint32_t* count);

// ---------------------------------------------------------------- co-op (docs/COOP.md)
/// The host of a co-op game: while set (non-null), acquire() succeeds and read() takes its items from these queues
/// (another player's keyboard and mouse, received over the network) instead of this computer's devices. The calls are
/// recorded as usual, so every game that follows the host replays the same items. nullptr, nullptr restores the devices.
void read_from(std::deque<Item>* mouse, std::deque<Item>* keyboard);
/// Receives a keyboard or mouse item of this computer.
using Forward = void (*)(Device device, const Item& item);
/// A co-op player: `to` receives every key, mouse movement, wheel step and button of this computer while the game window
/// has the focus, whether or not the game itself reads the devices (a game that follows the host replays its input).
/// nullptr stops it.
void forward(Forward to);

// ---------------------------------------------------------------- game pads (round P4)
// Up to four pads in the game's joystick slots, laid out as the DirectInput joystick the game was written for:
// axes X / Y = left stick, Z / Rz = right stick (0..65535, centre 32768, +Y = down), the point-of-view hat = the
// D-pad (hundredths of a degree clockwise from up, 0xFFFFFFFF = centred), buttons 0..11 = A, B, X, Y, LB, RB, Back,
// Start, left stick, right stick, LT, RT (0x80 = down). Like a foreground DirectInput device, a pad reads only while
// the game window has the focus. [Platform] Gamepad=0 = no pads.
/// Maximum number of game pads, 4 (the game's joystick slots).
constexpr int kMaxPads = 4;
/// State of one pad in the layout of the DirectInput joystick (see the block comment above).
struct PadState {
    std::int32_t axes[4];     ///< X, Y, Z, Rz: 0..65535, centre 32768, +Y = down.
    std::uint32_t pov;        ///< D-pad hat in hundredths of a degree clockwise from up; 0xFFFFFFFF = centred.
    std::uint8_t buttons[12];  ///< Buttons 0..11 (A, B, X, Y, LB, RB, Back, Start, LS, RS, LT, RT); 0x80 = down.
};
static_assert(sizeof(PadState) == 32);
/// Bit i set = a pad is connected in slot i (a pad keeps its slot while connected).
std::uint32_t pads();
/// The state of the pad in `slot`: kOk, kNotAcquired (no pad there, or the window is in the background).
std::int32_t read_pad(int slot, PadState* out);

// ---------------------------------------------------------------- the pad in the menus
// While a menu screen is up (display::canvas_armed: the main menu, the in-game menu, options ...) the first pad
// drives the menu cursor ([Platform] PadMenu): the left stick moves it, the D-pad hops between the menu's buttons,
// A clicks, B is Esc, the right stick scrolls. The game sees ordinary mouse / keyboard items.
/// Button::draw 0x00622cc0: a visible, enabled button at (x0, y0)-(x1, y1) this frame (the D-pad's targets).
void menu_widget(int x0, int y0, int x1, int y1);
/// A screen reading the pad's buttons itself (the key bindings page binding a pad button): the pad leaves the menu
/// alone for the next 0.15 s - call it every frame while it should. Buttons held then do not click afterwards.
void pad_menu_hold();

// ---------------------------------------------------------------- vibration
/// Rumble the first pad: `low` / `high` = the low- / high-frequency motors 0..1, `ms` long ([Platform] PadRumble).
void rumble(float low, float high, unsigned ms);

/// The first pad is connected and its right stick turns the camera in the game ([Platform] PadCamera > 0): `x` / `y`
/// = the right stick (-1..1 after the deadzone and curve, times PadCamera, y reversed by PadCameraInvert; +y = down).
bool pad_look(float& x, float& y);

/// The name of the game's joystick button `n` (1..12, KEYDESC "JOYPADn") on the first pad: Xbox, PlayStation or
/// Nintendo labels by its type; nullptr when no pad is connected (the game's own text then).
const char* pad_button_name(int n);

}  // namespace joan::platform::input
