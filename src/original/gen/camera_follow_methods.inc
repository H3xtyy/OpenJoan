// Generated (re/facades/gen2.py): typed members for the CameraFollow functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x00563a20 CameraFollow::start
std::int32_t start(std::int32_t a0);
/// 0x00563c80 CameraFollow::motion
void* motion(std::int32_t a0) { return ::joan::call::thiscall<void*>(::joan::fn::CameraFollow_motion, this, a0); }
/// 0x00563dc0 CameraFollow::target
std::uint32_t target() { return ::joan::call::thiscall<std::uint32_t>(::joan::fn::CameraFollow_target, this); }
