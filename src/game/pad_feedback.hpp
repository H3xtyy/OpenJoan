// Game pad vibration from what happens to the controlled hero (game/pad_feedback.cpp).
#pragma once

#include <cstdint>

namespace joan::padfx {

/// Once per frame of a mission (Hud::draw): hits taken, knock-downs and hits landed rumble the first pad.
void frame();

/// The joystick input type with the pad's right stick on the camera ([Platform] PadCamera): the left stick moves the
/// hero relative to the camera without swinging it (Ctrl::turn_4352f0). Set by Ctrl::update_432d30 around that call.
inline bool g_stick_keeps_camera = false;

/// The angle the pad's right stick holds the follow camera at (Ctrl::update_432d30, on foot), re-applied by
/// CameraMotion::update 0x004237a0 just before the camera moves, over whatever else retargeted the camera this frame
/// (its wall / ground collision, CameraMotion::collide, still applies).
/// `ms` = g_now when last set: the hold ends when the joystick type stops renewing it.
struct CameraHold {
    void* motion = nullptr;  // CameraFollow motion 0
    float yaw = 0.0f, pitch = 0.0f;
    std::uint32_t ms = 0;
};
inline CameraHold g_camera_hold;

}  // namespace joan::padfx
