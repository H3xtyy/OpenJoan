// Canvas logic for the screens laid out for 1024x768 (see display.hpp) and the 2D entry points (sprite queue, model
// placement, Camera::set_viewport). Until port round P1 a naked thunk wrapper per function sat on its thunk slot: it
// entered the canvas state (or rewrote the stack arguments), made the function return into canvas_return and jumped
// to the function. Every wrapped function is a C++ replacement now, declared JOAN_CANVAS_SELF in its module: it
// opens a joan::canvas::ScreenScope as the first statement of its body (or calls queue_sprite_args /
// place_model_args / set_viewport_args on its arguments first thing), which does the same at the same points - the
// state is entered before anything of the function runs and left after everything of it (including its locals'
// destructors) ran, on every return path and for nested calls (LIFO like the wrapper's return stack).
//
// C++ replacements of the functions changed here call the same logic through canvas_api.hpp
// (joan::canvas): byte patches are not written into a function whose replacement is installed,
// and every wrapped function is JOAN_CANVAS_SELF (see the table there).
#include "callconv.hpp"
#include <cstdint>
#include <vector>

#include "canvas_api.hpp"
#include "runtime/code_table.hpp"
#include "data_names.hpp"
#include "display.hpp"
#include "fn_names.hpp"
#include "registry.hpp"

namespace {

using joan::display::Layout;

struct Screen {
    const char* name;
    std::uintptr_t address;
    Layout layout;
    bool overlay = false;  // a whole 1024x768 screen drawn over the running game (not opaque)
};

// Draw functions laid out for 1024x768 (the same set the community widescreen patch moves).
constexpr Screen kScreens[] = {
    {"MainMenu::draw", 0x0070a8b0, Layout::screen},      {"Game::draw_loading_screen", 0x0047bce0, Layout::screen},
    {"Game::draw_credits", 0x0047bfd0, Layout::screen},  {"Game::draw_ending", 0x0047b830, Layout::screen},
    // The ending's pictures (1024x768, black backdrop, panning art larger than the layout): Game::draw_ending reaches
    // it only after Esc; while the mission's frame draws it, it needs its own scope.
    {"Ending::draw", 0x006ce450, Layout::screen},
    {"IngameMenu::draw", 0x006f3af0, Layout::screen},    {"SaveMenu::draw", 0x0074e170, Layout::screen},
    {"OptionMenu::draw", 0x0073d2c0, Layout::screen},    {"MessageLog::draw", 0x006e1130, Layout::screen},
    {"ObjectiveMenu::draw", 0x0071f690, Layout::screen}, {"QuitPopup::draw", 0x006e47b0, Layout::screen, true},
    // HUD: the mission message box / message icons (bottom-centre) and the dialogue portrait + text (bottom-left).
    // The in-mission HUD (portraits, bars, EXP, combo, level-up, target) and text strings (UiScale).
    {"Hud::draw", 0x006f9b60, Layout::ui},               {"Inventory::draw", 0x006f6280, Layout::ui_corner},
    // The objective list at the top-left (new / completed objectives, LIGHTBAR / DARKBAR rows at
    // y = 0x94 + 0x14 * i); it was wrongly taken for world-space bars and left unscaled.
    {"Hud::draw_objectives", 0x00704810, Layout::ui},
    // Found with the "unscaled 2D" log: the item-pickup notice that slides in from the left and the
    // small icon in the bottom-left corner.
    {"PickupNotice::draw", 0x006e5140, Layout::ui},      {"Hud::draw_corner_icon", 0x006f9970, Layout::ui},
    // Shop / blacksmith panel and its portrait (centred on the screen) and the talk prompt /
    // mercenary panel laid out for 1024x768 at the top.
    {"Shop::draw", 0x006c4070, Layout::ui_center},       {"Shop::draw_portrait", 0x006c3690, Layout::ui_center},
    // Found with the "unscaled 2D" log: the letter (seal, text) and the glory pop-up (zooming frame, glow, text,
    // button), both laid out about the screen centre.
    {"Letter::draw", 0x006e05a0, Layout::ui_center},     {"Glory::draw", 0x006e6dc0, Layout::ui_center},
    {"TalkPrompt::draw", 0x006d23b0, Layout::hud_top},
    // Charles VII's reputation (mission 8; Hud::draw_pause_caption), laid out for 1024x768 at (823, 220): under the
    // minimap, so anchored to the top-right corner as the minimap is.
    {"Hud::draw_pause_caption", 0x006f9a60, Layout::hud_top_right},
    {"Text::draw", 0x00626320, Layout::unit},            {"Text::draw_line", 0x006255d0, Layout::unit},
    {"Text::draw_2", 0x00626080, Layout::unit},          {"Text::draw_3", 0x00626130, Layout::unit},
    {"Message::draw", 0x004d8c10, Layout::hud},          {"Dialogue::draw", 0x0063b830, Layout::hud_left},
    // The escort / goal counter (portraits + "n/m") laid out at the top centre of 1024x768.
    {"GoalPanel::draw", 0x006df180, Layout::hud_top},
    // The "you have acquired" item screen (0xa54c20): a 1024x768 overlay (dark backdrop, title,
    // description, item model in the camera viewport 512,192-991,671, "back to mission") drawn
    // over the running game; its setup code centres pieces on the screen width (size reads below).
    {"ItemPanel::draw", 0x006bbac0, Layout::screen, true}, {"ItemPanel::draw_camera", 0x006bbc50, Layout::screen, true},
    // Found with the "unscaled 2D" log: the barracks (recruit army) screen, a 1024x768 overlay (backdrop, title,
    // unit rows, info frame sliding in from x = 1024) drawn over the running game.
    {"ArmyPanel::draw", 0x006bdea0, Layout::screen, true},
    // Found with the "unscaled 2D" log: the siege workshop screen (build siege engines), the barracks screen's twin -
    // a 1024x768 overlay (backdrop, title, unit rows, check buttons, info frame sliding in from x = 1024).
    {"WorkshopPanel::draw", 0x006ea310, Layout::screen, true},
};

// Screen-size reads (0x00a4885c width, 0x00a48860 height) in code that lays things out for
// 1024x768: they read display::layout_size() instead (or box_size() for the message boxes).
struct SizeRead {
    std::uintptr_t instruction;
    bool height;
    bool box;
};
constexpr SizeRead kSizeReads[] = {
    // Message slide-in end y (H - 0xc2) and message icon stack y (H - 0x112).
    {0x004acf25, true, false}, {0x004acfa8, true, false}, {0x004ad081, true, false}, {0x004ad0a6, true, false},
    {0x004ad886, true, false}, {0x004ad8dd, true, false}, {0x004adb16, true, false}, {0x004adb6d, true, false},
    // Message text rectangle (measure and draw): (0x68, H - 0xcc, W - 0x1c, H - 0x84).
    {0x004d928b, true, false}, {0x004d9298, false, false}, {0x004d92a2, true, false},
    {0x004d9332, true, false}, {0x004d933f, false, false}, {0x004d9349, true, false},
    // Dialogue text y (H - 0x69) and wrap limit (W - 0x64).
    {0x0063c035, true, false}, {0x0063da87, true, false}, {0x0063dd5e, false, false},
    // Credits line centring.
    {0x006b579b, false, false},
    // Objective screen map camera viewport (0, 0, W-1, H-1) -> the 1024x768 layout, placed on the canvas
    // by the renderer like the other mission-brief cameras.
    {0x0072d451, true, false}, {0x0072d45b, false, false},
    // Objective screen map: centring and clipping.
    {0x0072d9c4, false, false}, {0x0072da0b, true, false}, {0x0072e6ec, false, false}, {0x0072e705, true, false},
    {0x0072ea3d, false, false}, {0x0072eab4, true, false}, {0x0072eac1, true, false}, {0x0072eacb, false, false},
    {0x0072ebc2, false, false}, {0x0072ebd3, false, false}, {0x0072ee26, false, false}, {0x0072ee35, true, false},
    {0x0072f5e7, false, false}, {0x0072f5f5, true, false}, {0x0072f9dc, false, false}, {0x0072f9fd, true, false},
    // Item screen setup (0x6b9870, 0x6ba4b0, 0x6bb160, 0x6bb420): title centring, title bars (W - 1),
    // "back to mission" button ((W - 0xfa) / 2, slid out by +0x400).
    {0x006b9969, false, false}, {0x006b9c4e, false, false}, {0x006b9c68, false, false}, {0x006b9d49, false, false},
    {0x006b9dca, false, false}, {0x006ba6ef, false, false}, {0x006bb35e, false, false}, {0x006bb661, false, false},
    // Message box (in-game menu / objective popups) centred at ((W - w) / 2, (H - h) / 2).
    {0x006d3f6d, false, true}, {0x006d3f86, true, true},
};
constexpr img::Addr kScreenWidth = d::g_screen_w, kScreenHeight = d::g_screen_h;

constexpr int kSizeReadCount = static_cast<int>(sizeof kSizeReads / sizeof kSizeReads[0]);

std::int32_t* size_target(const SizeRead& r) {
    std::int32_t* const target = r.box ? joan::display::box_size() : joan::display::layout_size();
    return target + (r.height ? 1 : 0);
}

constexpr int kCount = static_cast<int>(sizeof kScreens / sizeof kScreens[0]);

// Every 2D image goes through this "queue a textured quad" function (thiscall, 16 arguments,
// the first four the quad's screen rectangle); the queue is drawn later, often after the
// screen's draw function has returned, so the rectangle is scaled when it is queued.
constexpr std::uintptr_t kQueueSprite = 0x00632a70;
// Places a model in the 2D layer (objective-screen map icons): thiscall (x, y, id, object,
// rx, ry, rz, scale), ret 0x20, x / y relative to the screen centre at the time of the call.
constexpr std::uintptr_t kPlaceModel = 0x006349b0;
// Camera::set_viewport (thiscall a, b, x1, y1, x2, y2; ret 0x18).
constexpr std::uintptr_t kSetViewport = 0x00613bf0;

// The functions holding the size reads (start, end): their reads are not patched while a C++
// replacement of the function is installed (it calls joan::canvas::size_read instead).
struct Range {
    std::uintptr_t begin, end;
};
constexpr Range kSizeReadFunctions[] = {
    {0x004ace90, 0x004ad6e0}, {0x004ad710, 0x004ad9a0}, {0x004ad9a0, 0x004ae160}, {0x004d8c10, 0x004d96e0},
    {0x0063c000, 0x0063c0a0}, {0x0063da60, 0x0063dfa0}, {0x006b56b0, 0x006b5880}, {0x006b9870, 0x006ba3e0},
    {0x006ba4b0, 0x006ba810}, {0x006bb160, 0x006bb420}, {0x006bb420, 0x006bb750}, {0x006d3cb0, 0x006d5080},
    {0x0072d380, 0x0072d550}, {0x0072d6f0, 0x0072e370}, {0x0072e480, 0x00730150},
};
std::uintptr_t containing_function(std::uintptr_t instruction) {
    for (const Range& r : kSizeReadFunctions)
        if (instruction >= r.begin && instruction < r.end) return r.begin;
    return 0;
}
// Inventory items (0x006d6740) holds the dragged-item call patched below.
constexpr std::uintptr_t kInventoryItems = 0x006d6740;

// --- state for joan::canvas (canvas_api.hpp), set by install_canvas_hooks ---------------------------
bool g_hooks_active = false;          // canvas enabled: the patches below are applied
bool g_size_active[kSizeReadCount];   // size read redirected, or would be (function replaced, bytes as expected)
bool g_screen_self[kCount];           // JOAN_CANVAS_SELF replacement installed: it uses ScreenScope
bool g_sprite_self = false, g_model_self = false, g_viewport_self = false;
bool g_viewport_hooked = false;       // set_viewport replacement installed and self-handled
bool g_drag_active = false;           // dragged-item call patched, or would be (0x006d6740 replaced)

std::vector<std::uintptr_t>& self_declared() {
    static std::vector<std::uintptr_t> list;  // function-local: filled during static initialisation
    return list;
}
bool declared_self(std::uintptr_t original) {
    for (const std::uintptr_t a : self_declared())
        if (a == original) return true;
    return false;
}

// Thunk slots that lead to `target` (src/code_table.hpp).
int count_thunks(std::uintptr_t target) { return joan::code_table::count_thunks_to(target); }

// `original` has an installed replacement that actually runs in its place: a thunk slot leads to the
// replacement, or the original's entry slot was patched with it. (installed_replacement alone only
// says the replacement is enabled; if its installation failed the original's slot still traps.)
bool bypassed(std::uintptr_t original) {
    const void* const replaced = joan::hook::installed_replacement(original);
    if (!replaced) return false;
    const auto target = reinterpret_cast<std::uintptr_t>(replaced);
    return count_thunks(target) > 0 || joan::code_table::entry_patch(original) == target;
}

bool ui_kind(Layout kind) {
    return kind == Layout::ui || kind == Layout::unit || kind == Layout::ui_off || kind == Layout::ui_center || kind == Layout::ui_corner;
}

constexpr std::uintptr_t kLoadingScreen = 0x0047bce0;  // Game::draw_loading_screen

// What a screen's draw function does on entry / return (ScreenScope; formerly the thunk wrapper).
void screen_enter(int screen) {
    const Layout kind = kScreens[screen].layout;
    if (ui_kind(kind)) {
        joan::display::ui_enter(kind);
        return;
    }
    // Every whole screen hides the game behind it except the quit popup (an overlay).
    const bool opaque = kScreens[screen].layout == Layout::screen && !kScreens[screen].overlay;
    joan::display::layout_enter(kScreens[screen].layout, opaque);
    if (kScreens[screen].address == kLoadingScreen) joan::display::loading_screen(true);
}
void screen_leave(int screen) {
    const Layout kind = kScreens[screen].layout;
    if (kScreens[screen].address == kLoadingScreen) joan::display::loading_screen(false);
    if (ui_kind(kind)) joan::display::ui_leave(kind);
    else joan::display::layout_leave();
}

}  // namespace

// The dragged inventory item: 0x006d6740 (Inventory items, inside Inventory::draw) draws it with
// 0x006ddf10(mouse x - w*20, mouse y - h*20, name) through the call at 0x006d7766 (thunk 0x004067e9).
// 0x006ddf10 also draws the items lying in the grid, so only that one call is wrapped: its sprites
// follow the real cursor under UiScale (otherwise, outside the HUD, they were scaled about the
// nearest screen edge and drifted away from the cursor). The replacement of 0x006d6740 calls
// joan::canvas::draw_dragged_item there (joan.exe's call instruction no longer exists to patch).

// ---- joan::canvas API (canvas_api.hpp) ----------------------------------------------------------
namespace joan::canvas {

bool hooks_active() { return g_hooks_active; }

bool original_bypassed(std::uintptr_t original) { return bypassed(original); }

SelfHandled::SelfHandled(std::uintptr_t original) { self_declared().push_back(original); }

bool self_handled(std::uintptr_t original) {
    for (int i = 0; i < kCount; ++i)
        if (kScreens[i].address == original) return g_screen_self[i];
    if (original == kQueueSprite) return g_sprite_self;
    if (original == kPlaceModel) return g_model_self;
    if (original == kSetViewport) return g_viewport_self;
    return false;
}

ScreenScope::ScreenScope(std::uintptr_t draw_fn) {
    for (int i = 0; i < kCount; ++i) {
        if (kScreens[i].address != draw_fn) continue;
        if (g_screen_self[i]) {
            screen_ = i;
            screen_enter(i);
        }
        return;
    }
}
ScreenScope::~ScreenScope() {
    if (screen_ >= 0) screen_leave(screen_);
}

std::int32_t size_read(std::uintptr_t instruction) {
    for (int i = 0; i < kSizeReadCount; ++i) {
        const SizeRead& r = kSizeReads[i];
        if (r.instruction != instruction) continue;
        if (g_size_active[i]) return *size_target(r);
        return hook::global<std::int32_t>(r.height ? kScreenHeight : kScreenWidth);
    }
    // Not a listed size read: joan.exe's instruction no longer exists to tell which value it read - the width.
    const img::Addr operand = kScreenWidth;
    hook::log_line("[Display] canvas::size_read(0x%08x): not a redirected size read", static_cast<unsigned>(instruction));
    return hook::global<std::int32_t>(operand);
}

void queue_sprite_args(std::int32_t* args, std::uintptr_t caller_frame) {
    if (g_sprite_self) display::canvas_sprite_from(args, caller_frame);
}
void place_model_args(std::int32_t* args) {
    if (g_model_self) display::canvas_model(args);
}
void set_viewport_args(std::int32_t* args, std::uintptr_t caller) {
    if (g_viewport_self) display::camera_viewport(args, caller);
}

void shop_viewport_args(std::int32_t* args) {
    // The wrapper's test is on the caller's return address; any address inside the range will do.
    if (g_viewport_hooked) display::camera_viewport(args, 0x006c2120);
}

std::uint32_t draw_dragged_item(void* self, std::int32_t x, std::int32_t y, const char* name) {
    using Fn = std::uint32_t(JOAN_THISCALL*)(void*, std::int32_t, std::int32_t, const char*);
    const auto fn = reinterpret_cast<Fn>(fn::code_value(fn::Inventory_draw_item_6ddf10));  // thunk 0x004067e9 -> 0x006ddf10
    if (!g_drag_active) return fn(self, x, y, name);
    display::ui_cursor_enter();
    const std::uint32_t result = fn(self, x, y, name);
    display::ui_cursor_leave();
    return result;
}

}  // namespace joan::canvas

namespace {
// `original` (a wrapped function) is self-handled: declared JOAN_CANVAS_SELF and its installed replacement is what
// its thunk slots lead to (install_all ran first). Returns the number of those thunk slots (0: not self-handled -
// the function then runs without its canvas logic, as when the old wrapper found no thunk).
int self_entry(std::uintptr_t original, bool& self) {
    self = false;
    const void* const replaced = joan::hook::installed_replacement(original);
    if (!replaced || !declared_self(original)) return 0;
    const int n = count_thunks(reinterpret_cast<std::uintptr_t>(replaced));
    self = n > 0;
    return n;
}
}  // namespace

namespace joan::display {
void install_canvas_hooks() {
    if (!canvas().enabled) return;
    g_hooks_active = true;
    int redirected = 0, kept = 0;
    for (int i = 0; i < kSizeReadCount; ++i) {
        const SizeRead& r = kSizeReads[i];
        // Inside a replaced function the original instruction never runs: leave its bytes alone, the
        // replacement reads joan::canvas::size_read (same value while the instruction is as expected).
        const std::uintptr_t fn = containing_function(r.instruction);
        // joan.exe's code no longer exists: no instruction bytes to check or patch. A replaced function reads the
        // value through size_read (the instruction is known from the table); a function that is not replaced only
        // traps, so its read stays inactive.
        g_size_active[i] = fn != 0 && bypassed(fn);
        if (g_size_active[i]) ++kept;
        if (!g_size_active[i])
            hook::log_line("[Display] screen-size read at 0x%08x not as expected - left alone", static_cast<unsigned>(r.instruction));
    }
    int installed = 0;
    for (int i = 0; i < kCount; ++i) {
        const int n = self_entry(kScreens[i].address, g_screen_self[i]);
        if (n == 0) hook::log_line("[Display] %s: no thunk found - not scaled", kScreens[i].name);
        else ++installed;
    }
    // The dragged item: the replacement of 0x006d6740 calls draw_dragged_item (an original 0x006d6740 would only trap).
    g_drag_active = bypassed(kInventoryItems);
    if (!g_drag_active) hook::log_line("[Display] 0x006d6740 not replaced - dragged item not wrapped");
    const int n = self_entry(kQueueSprite, g_sprite_self);
    g_viewport_hooked = self_entry(kSetViewport, g_viewport_self) != 0;
    if (!g_viewport_hooked) hook::log_line("[Display] Camera::set_viewport: no thunk found - shop views not scaled");
    if (self_entry(kPlaceModel, g_model_self) == 0) hook::log_line("[Display] map model placement: no thunk found - not scaled");
    hook::log_line("[Display] %d of %d 1024x768 layouts placed (sprite queue: %d thunk%s, %d of %d size reads, %d more read by "
                   "replacements)",
                   installed, kCount, n, n == 1 ? "" : "s", redirected, kSizeReadCount, kept);
}
}  // namespace joan::display
