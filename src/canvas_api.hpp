// Canvas / display API for C++ replacements of the functions canvas_hooks.cpp and display.cpp
// change in place (scratchpad canvas115_all.txt: 48 functions).
//
// Today these functions run as original joan.exe code and the display code adapts them from the
// outside: byte patches of instructions inside them (screen-size reads, resolution immediates, one
// call), thunk wrappers around their entries (draw functions laid out for 1024x768, the sprite
// queue, model placement, Camera::set_viewport) and a return-address test (viewport callers in
// 0x006c2120..0x006c27c0). A C++ replacement does not execute the patched bytes and its calls have
// return addresses in the hook DLL, so it calls this API at the exact points where the original
// reads a patched value / makes the patched call. Every function here runs the same code path
// the patches use (canvas_hooks.cpp / display.cpp), so behaviour does not change.
//
// Rule for byte patches (size reads, the drag-item call, resolution immediates): the patch is not
// written into a function that has an installed replacement that really runs in its place
// (original_bypassed: registry installed_replacement + a thunk or entry jump to it); the
// replacement reads the value through this API instead. Its instruction is still checked, so the
// API answers exactly what the patched instruction would have read.
//
// Rule for entry wrappers (screens, sprite queue, model placement, set_viewport): there are no thunk wrappers any
// more (port round P1 removed the naked stubs). Every such function is a C++ replacement declared
// JOAN_CANVAS_SELF(address) in its module that applies the canvas logic itself: a draw function opens
// ScreenScope as the first statement of its body; 0x00632a70 / 0x006349b0 / 0x00613bf0 call queue_sprite_args /
// place_model_args / set_viewport_args on their arguments first thing. Those calls are no-ops unless the function is
// declared JOAN_CANVAS_SELF, its replacement is installed through its thunk slots and the canvas hooks are on. A
// wrapped function without the declaration runs without its canvas logic (install_canvas_hooks logs it).
// Calls from a C++ replacement of these functions must go through their slots (joan::orig facades / fn:: names),
// not to the C++ member directly, so the callee's scope applies exactly where the wrapper applied.
//
// Nothing here is active unless the canvas hooks are installed ([Display] resolution larger than
// 1024x768 and Renderer=opengl; hooks_active()). Inactive, size_read returns the real screen size
// (what the unpatched instruction reads) and the scaling calls do nothing - as the original.
//
// ------------------------------------------------------------------------------------------------
// Per-function table. "size read @a W/H" = the dword read of 0x00a4885c (W) / 0x00a48860 (H) by the
// instruction at a is redirected to display::layout_size() (always {1024, 768}) - or box_size()
// where marked "box" - write `canvas::size_read(0x00a....)` there. All OTHER screen-size reads in
// the same function are not patched and must read the real globals. "wrapped (L)" = thunk wrapper
// with layout L (canvas_hooks.cpp kScreens), now applied by the replacement itself (JOAN_CANVAS_SELF + ScreenScope /
// the *_args calls); calls from a C++ replacement must still go through the original thunk slots.
//
//  function    name (best guess)              what canvas / display does                 replacement must
//  0x0047b830  Game::draw_ending              wrapped (screen)                           SELF + ScreenScope
//  0x0047bce0  Game::draw_loading_screen      wrapped (screen)                           SELF + ScreenScope
//  0x0047bfd0  Game::draw_credits             wrapped (screen)                           SELF + ScreenScope
//  0x004ace90  message slide-in               size read H @0x004acf25 (mov eax), @0x004acfa8 (mov eax),
//                                             @0x004ad081 (mov edx), @0x004ad0a6 (mov ecx)  size_read(...) at those 4
//  0x004ad710  message icon stack             size read H @0x004ad886 (mov ecx), @0x004ad8dd (mov eax)  size_read
//  0x004ad9a0  message icon stack (2)         size read H @0x004adb16 (mov ecx), @0x004adb6d (mov eax)  size_read
//  0x004d8c10  Message::draw                  wrapped (hud); text rect: size read H @0x004d928b (ecx),
//                                             W @0x004d9298 (edx), H @0x004d92a2 (eax), H @0x004d9332 (ecx),
//                                             W @0x004d933f (edx), H @0x004d9349 (eax)   size_read at the 6
//  0x00593af0  Mouse::set_boundary            no code change: display.cpp rewrites the Mouse bounds
//                                             (0x00a516b0+0x636, 4 ints) and cur x/y (0x00a516c4 / c8)
//                                             once per frame (canvas_end_frame)          keep the fields at those addresses
//  0x00593f60  Mouse::reset_boundary          same as 0x00593af0                         same
//  0x005f6c00  D3D start-up (mode search)     imm32 1024 @0x005f6c79 (operand 0x005f6c7c) and 768
//                                             @0x005f6c80 (operand 0x005f6c83) -> resolution
//                                             (display.cpp apply_patches)                resolution_imm(kModeWidthImm / kModeHeightImm)
//  0x00613bf0  Camera::set_viewport           entry wrapper: args x1,y1,x2,y2 scaled about the screen
//                                             centre by UiScale when the return address is in
//                                             0x006c2120..0x006c27c0                     SELF + set_viewport_args(args, JOAN_CANVAS_RETURN_ADDRESS())
//  0x006255d0  Text::draw_line                wrapped (unit)                             SELF + ScreenScope
//  0x00626080  Text::draw_2                   wrapped (unit)                             SELF + ScreenScope
//  0x00626130  Text::draw_3                   wrapped (unit)                             SELF + ScreenScope
//  0x00626320  Text::draw                     wrapped (unit)                             SELF + ScreenScope
//  0x00632a70  sprite queue (16 args, ret 0x40) entry wrapper: args[0..3] (x1,y1,x2,y2) placed for
//                                             the current layout (display::canvas_sprite_from)  SELF + queue_sprite_args(args, JOAN_CANVAS_FRAME())
//  0x006349b0  place 2D-layer model (ret 0x20)  entry wrapper: x, y (args 0, 1) and scale (arg 7)
//                                             (display::canvas_model)                    SELF + place_model_args(args)
//  0x0063b830  Dialogue::draw                 wrapped (hud_left)                         SELF + ScreenScope
//  0x0063c000  dialogue text y                size read H @0x0063c035 (mov edx)          size_read
//  0x0063da60  dialogue text                  size read H @0x0063da87 (edx), W @0x0063dd5e (eax)  size_read
//  0x006b56b0  credits line                   size read W @0x006b579b (mov eax)          size_read
//  0x006b9870  item screen setup              size read W @0x006b9969, @0x006b9c4e, @0x006b9c68, @0x006b9d49,
//                                             @0x006b9dca (all mov eax)                  size_read
//  0x006ba4b0  item screen setup (2)          size read W @0x006ba6ef (mov eax)          size_read
//  0x006bb160  item screen setup (3)          size read W @0x006bb35e (mov eax)          size_read
//  0x006bb420  item screen setup (4)          size read W @0x006bb661 (mov eax)          size_read
//  0x006c2120  shop / blacksmith camera setup  its two Camera::set_viewport calls (0x006c2437 and
//                                             0x006c25f1, thunk 0x0040169f) are scaled by the 0x00613bf0
//                                             wrapper because of their return addresses   shop_viewport_args(args) on the 6 args,
//                                             then call thunk 0x0040169f (set_viewport_args then sees a return address in our code and leaves them)
//  0x006c27c0  (end of the 0x006c2120 range)  nothing: it calls no set_viewport; listed only because it
//                                             bounds the range                           nothing
//  0x006c3690  Shop::draw_portrait            wrapped (ui_center)                        SELF + ScreenScope
//  0x006c4070  Shop::draw                     wrapped (ui_center)                        SELF + ScreenScope
//  0x006d23b0  TalkPrompt::draw               wrapped (hud_top)                          SELF + ScreenScope
//  0x006d3cb0  message box                    size read W box @0x006d3f6d (mov eax), H box @0x006d3f86 (mov ecx)
//                                             (display::box_size(), changes per frame)   size_read
//  0x006d6740  Inventory items                call 0x004067e9 (-> 0x006ddf10, thiscall x, y, item; ret 0xc)
//                                             at 0x006d7766 retargeted to drag_item_stub (ui_cursor_enter /
//                                             leave around it)                           draw_dragged_item(this, x, y, item) for that ONE call
//                                             (the other 0x004067e9 calls stay plain thunk calls)
//  0x006e1130  MessageLog::draw               wrapped (screen)                           SELF + ScreenScope
//  0x006e47b0  QuitPopup::draw                wrapped (screen, overlay)                  SELF + ScreenScope
//  0x006e5140  PickupNotice::draw             wrapped (ui)                               SELF + ScreenScope
//  0x006f3af0  IngameMenu::draw               wrapped (screen)                           SELF + ScreenScope
//  0x006ce450  Ending::draw                   wrapped (screen)                           SELF + ScreenScope
//  0x006f6280  Inventory::draw                wrapped (ui_corner)                               SELF + ScreenScope
//  0x006f9970  Hud::draw_corner_icon          wrapped (ui)                               SELF + ScreenScope
//  0x006f9b60  Hud::draw                      wrapped (ui)                               SELF + ScreenScope
//  0x00702740  combo counter                  imm32 1024 (mov reg, 400h) @0x007028c1, 0x00702a5b, 0x00702bea,
//                                             0x00703001, 0x00703245 (operand at +1) -> screen width; data
//                                             float 864.0f at 0x009de95c (read by fsubr @0x00702d72 only)
//                                             -> 864 + (width - 1024)                    resolution_imm(operand) for the 5; read the
//                                             float through combo_digits_x() (the data patch stays)
//  0x00704810  Hud::draw_objectives           wrapped (ui)                               SELF + ScreenScope
//  0x0070a8b0  MainMenu::draw                 wrapped (screen)                           SELF + ScreenScope
//  0x0071f690  ObjectiveMenu::draw            wrapped (screen)                           SELF + ScreenScope
//  0x0072d380  objective map camera viewport  size read H @0x0072d451 (edx), W @0x0072d45b (eax)  size_read
//  0x0072d6f0  objective map                  size read W @0x0072d9c4 (eax), H @0x0072da0b (eax)  size_read
//  0x0072e480  objective map (draw / clip)    size read W @0x0072e6ec (mov eax), H @0x0072e705 (mov eax),
//                                             W @0x0072ea3d (mov edx), H @0x0072eab4 (mov ecx), H @0x0072eac1 (mov edx),
//                                             W @0x0072eacb (mov eax), W @0x0072ebc2 (mov eax), W @0x0072ebd3 (mov edx),
//                                             W @0x0072ee26 (cmp ecx,[W]), H @0x0072ee35 (cmp edx,[H]),
//                                             W @0x0072f5e7 (cmp ecx,[W]), H @0x0072f5f5 (cmp edx,[H]),
//                                             W @0x0072f9dc (fild [W]), H @0x0072f9fd (fild [H])  size_read (the fild ones:
//                                             the int value, converted as the original's fild does)
//  0x0073d2c0  OptionMenu::draw               wrapped (screen)                           SELF + ScreenScope
//  0x0074e170  SaveMenu::draw                 wrapped (screen)                           SELF + ScreenScope
//
// Wrapped functions not in the list (SELF + ScreenScope as well):
// GoalPanel::draw 0x006df180 (hud_top), ItemPanel::draw 0x006bbac0 / draw_camera 0x006bbc50 (screen, overlay),
// ArmyPanel::draw 0x006bdea0 and WorkshopPanel::draw 0x006ea310 (screen, overlay).
#pragma once

#include <cstdint>

#include "registry.hpp"  // JOAN_HOOK_CAT

#if defined(_MSC_VER) && !defined(__clang__)
#include "compiler.hpp"  // JOAN_RETURN_ADDRESS / JOAN_RETURN_ADDRESS_SLOT (JOAN_CANVAS_RETURN_ADDRESS / JOAN_CANVAS_FRAME)
#endif

// Usage in a module (e.g. a replacement of MainMenu::draw that handles its own scope):
//     #include "../canvas_api.hpp"
//     JOAN_CANVAS_SELF(0x0070a8b0);
//     void H::draw() { joan::canvas::ScreenScope scope(0x0070a8b0); ... }
// and inside 0x004ace90's replacement, for `mov eax, ds:0xa48860` at 0x004acf25:
//     const std::int32_t h = joan::canvas::size_read(0x004acf25);

namespace joan::canvas {

/// The canvas hooks are installed (screen larger than 1024x768, Renderer=opengl).
bool hooks_active();
/// `original` has an installed replacement that really runs instead of it (a thunk jumps to the
/// replacement, or the original's entry jumps to it). The byte-patch skip rule uses this, so a
/// replacement that is enabled but failed to install leaves the original with its patches.
bool original_bypassed(std::uintptr_t original);

// --- Self-handled replacements -------------------------------------------------------------------
/// Declares that the replacement of `original` (a wrapped function: a kScreens draw function,
/// 0x00632a70, 0x006349b0 or 0x00613bf0) applies the canvas logic itself (there is no thunk wrapper any
/// more: every wrapped function needs it). Use through JOAN_CANVAS_SELF.
struct SelfHandled {
    explicit SelfHandled(std::uintptr_t original);
};
/// `original` is declared JOAN_CANVAS_SELF and its replacement is installed (valid after
/// install_canvas_hooks).
bool self_handled(std::uintptr_t original);

// --- 1024x768 draw functions (kScreens) ------------------------------------------------------------
/// RAII scope = what the thunk wrapper did around the call (layout_enter / ui_enter on entry,
/// layout_leave / ui_leave on return). Active only when `draw_fn` is declared JOAN_CANVAS_SELF, its
/// replacement is installed and the canvas hooks are on; otherwise a no-op.
/// Put it first in the replacement's body (before any other local, so it is destroyed last); it must live until the
/// function returns.
class ScreenScope {
public:
    explicit ScreenScope(std::uintptr_t draw_fn);
    ~ScreenScope();
    ScreenScope(const ScreenScope&) = delete;
    ScreenScope& operator=(const ScreenScope&) = delete;

private:
    int screen_ = -1;
};

// --- Screen-size reads (kSizeReads) ----------------------------------------------------------------
/// The dword the instruction at `instruction` reads (it reads 0x00a4885c or 0x00a48860 in the
/// original): the 1024x768 layout size (or the message-box size) while the canvas hooks are on and
/// the instruction is a redirected size read, the real screen-size global otherwise. An address that
/// is not in the table (a programming error: use only the listed addresses) is logged and answered
/// with the global its original instruction reads (width when that cannot be told).
std::int32_t size_read(std::uintptr_t instruction);

// --- Entry wrappers, for JOAN_CANVAS_SELF replacements only (no-ops otherwise) ---------------------
/// 0x00632a70 (sprite queue): `args` = its 16 arguments as 32-bit words (x1, y1, x2, y2 first; only those four are
/// read or rewritten), rewritten in place; call first thing, before the arguments are used. `caller_frame` = a frame
/// for the "unscaled 2D" diagnostic's frame-pointer walk ([f + 4] = return address): JOAN_CANVAS_FRAME() (0 = no log).
void queue_sprite_args(std::int32_t* args, std::uintptr_t caller_frame = 0);
/// 0x006349b0 (2D-layer model): `args` = its 8 arguments as 32-bit words (x, y and the scale bits [7] are read and
/// rewritten), rewritten in place.
void place_model_args(std::int32_t* args);
/// 0x00613bf0 (Camera::set_viewport): `args` = its 6 arguments as 32-bit words (x1..y2 = [2..5] are read and
/// rewritten), `caller` = the return address (JOAN_CANVAS_RETURN_ADDRESS()).
void set_viewport_args(std::int32_t* args, std::uintptr_t caller);

// --- Caller-side helpers ------------------------------------------------------------------------
/// For a replacement of 0x006c2120: the scaling the set_viewport wrapper gives calls from inside
/// 0x006c2120..0x006c27c0 (args = a, b, x1, y1, x2, y2; x1..y2 rewritten). Call it on the argument
/// values right before each of the two `Camera::set_viewport` thunk calls (0x0040169f).
void shop_viewport_args(std::int32_t* args);
/// For a replacement of 0x006d6740: the call at 0x006d7766 (`thiscall 0x004067e9(x, y, item)` ->
/// 0x006ddf10, ret 0xc) as patched: ui_cursor_enter, the thunk call, ui_cursor_leave. Returns eax. `name` is the
/// item's name (a pointer, as the facade Inventory::draw_item_6ddf10 types it: full width on x64).
std::uint32_t draw_dragged_item(void* self, std::int32_t x, std::int32_t y, const char* name);

// --- Resolution immediates / data (display.cpp apply_patches) ------------------------------------
inline constexpr std::uintptr_t kModeWidthImm = 0x005f6c7c;   // mov [ebp-0xc], 400h  @0x005f6c79
inline constexpr std::uintptr_t kModeHeightImm = 0x005f6c83;  // mov [ebp-0x10], 300h @0x005f6c80
inline constexpr std::uintptr_t kComboImm[5] = {0x007028c2, 0x00702a5c, 0x00702beb, 0x00703002, 0x00703246};
/// The 32-bit immediate at `operand` as the patched code has it: the resolution value when
/// apply_patches applied the resolution (also when it skipped writing it into a replaced function),
/// otherwise the bytes there (1024 / 768, or whatever a community d3d8.dll wrote).
std::uint32_t resolution_imm(std::uintptr_t operand);
/// The float the combo digits are placed from (0x009de95c, data patched by apply_patches).
float combo_digits_x();

}  // namespace joan::canvas

#define JOAN_CANVAS_SELF(address) \
    static ::joan::canvas::SelfHandled JOAN_HOOK_CAT(joan_canvas_self_, __LINE__)(address)

// Used inside the replacement of 0x00613bf0 / 0x00632a70 (what the old entry wrappers read at the function's entry):
// the return address into the caller, and a frame f of the replacement with [f + sizeof(void*)] = that return
// address (the frame-pointer walk of the "unscaled 2D" log starts there).
#if defined(_MSC_VER) && !defined(__clang__)
#define JOAN_CANVAS_RETURN_ADDRESS() reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS())
#define JOAN_CANVAS_FRAME() (reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS_SLOT()) - sizeof(void*))
#else
#define JOAN_CANVAS_RETURN_ADDRESS() reinterpret_cast<std::uintptr_t>(__builtin_return_address(0))
#define JOAN_CANVAS_FRAME() reinterpret_cast<std::uintptr_t>(__builtin_frame_address(0))
#endif
