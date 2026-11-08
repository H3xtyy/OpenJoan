// [Debug] FreecamKey: a free-flying camera for looking around a level (finding the heavy spots).
//
// Not part of the original game. The key (default F9) toggles it; while it is on the game gets no input
// (as with the debug menu open) and the main camera is drawn from the free camera's pose instead of its own:
// Camera::render (game/unknown_camera3d.cpp) swaps the camera's parameters (position, orbit angles) for
// the frame and puts the game's back afterwards, so culling, levels of detail and every pass follow the
// free camera while the game's camera logic is left alone. W / A / S / D move, Q / E down / up, Shift
// faster, Ctrl slower; holding the right mouse button (or the arrow keys) turns. It starts where the game
// camera is.
#pragma once

namespace joan::freecam {

/// Called once per game frame (the debug menu's input hook): the toggle key and this frame's motion.
/// `focused` = the game window has the focus; `menu_open` = the debug menu takes the keys.
void frame(bool focused, bool menu_open);
/// The free camera is on (the game's input is dropped).
bool active();
/// The game gets no input this frame (the free camera is on, or its key was just pressed either way).
bool takes_input();

/// The free camera's pose, in the game camera's terms (CamParams: position, orbit yaw / pitch).
struct Pose {
    float position[3];
    float yaw, pitch;
};
/// Camera::render: true = draw this camera from the free pose. `start` = the game camera's eye and angles,
/// used when the free camera was just switched on. Advances the pose by the frame's input along the
/// camera's axes (`forward`, `right`: its z / x axes at the current angles, given by the caller's function).
using Axes = void (*)(float yaw, float pitch, float* forward, float* right);
bool pose(const Pose& start, Axes axes, Pose& out);

}  // namespace joan::freecam
