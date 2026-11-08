// [Debug] HitchLog=N: a frame that took longer than N ms is reported in openjoan.log with what happened during it -
// the files opened, shader programs compiled, textures made, sounds loaded - so a short stutter (an explosion's
// first use of a shader, a model read from disk) can be pinned down. Debug build only (the release build writes no
// log; enabled() is a constant false there and the notes compile away).
#pragma once

#include "../registry.hpp"

/// [Debug] HitchLog: per-frame notes of what happened in a slow frame (files opened, shaders compiled, textures made).
namespace joan::hitch {

/// Whether [Debug] HitchLog is set (> 0); a constant false in the release build.
inline bool enabled() { return hook::settings().hitch_log > 0; }

/// Something that happened in this frame (printf format; kept up to 32 per frame). Any thread.
void note(const char* fmt, ...);

/// Time spent in file opens / reads (platform/files.cpp) this frame; reported as the frame's file total. Any thread.
void add_file_time(double ms);

/// The frame ends (Device::Present): logs the frame's notes when it took longer than HitchLog ms, then clears them.
void frame_end();

/// The time of a scope, added to the frame's total for `label` (a string literal) and listed in a slow frame's
/// report ("time in: ..."). Inclusive (a span inside another counts in both); a recursive call is counted once.
/// Game thread only. Nothing at all while the log is off.
class Span {
public:
    /// Starts timing `label` (a string literal that must outlive the frame).
    explicit Span(const char* label);
    ~Span();
    Span(const Span&) = delete;
    Span& operator=(const Span&) = delete;

private:
    int slot_ = -1;  // -1: not timed (log off, nested, table full)
    long long t0_ = 0;
};

}  // namespace joan::hitch
