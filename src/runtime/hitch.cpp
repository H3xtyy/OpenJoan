// See hitch.hpp.
#include "hitch.hpp"

#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <string>
#include <vector>

namespace joan::hitch {
namespace {

constexpr std::size_t kMaxNotes = 32;

std::mutex g_lock;
std::vector<std::string> g_notes;
std::size_t g_dropped = 0;
double g_file_ms = 0;  // this frame's file opens / reads
std::chrono::steady_clock::time_point g_last{};
double g_average = 0;  // ms, running average of the frames that were not hitches
unsigned g_reported = 0;

// Span totals of the current frame, by label pointer.
struct SpanTotal {
    const char* label = nullptr;
    double ms = 0;
    unsigned calls = 0;
    int depth = 0;
};
constexpr int kMaxSpans = 32;
SpanTotal g_spans[kMaxSpans];

long long now_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

}  // namespace

Span::Span(const char* label) {
    if (!enabled()) return;
    for (int i = 0; i < kMaxSpans; ++i) {
        if (g_spans[i].label != nullptr && g_spans[i].label != label) continue;
        g_spans[i].label = label;
        slot_ = i;
        if (g_spans[i].depth++ == 0) t0_ = now_ns();  // only the outermost call of a recursion is timed
        return;
    }
}

Span::~Span() {
    if (slot_ < 0) return;
    SpanTotal& s = g_spans[slot_];
    if (--s.depth != 0) return;
    s.ms += static_cast<double>(now_ns() - t0_) / 1e6;
    ++s.calls;
}

void note(const char* fmt, ...) {
    if (!enabled()) return;
    char text[256];
    va_list args;
    va_start(args, fmt);
    std::vsnprintf(text, sizeof text, fmt, args);
    va_end(args);
    std::lock_guard<std::mutex> lock(g_lock);
    if (g_notes.size() < kMaxNotes)
        g_notes.emplace_back(text);
    else
        ++g_dropped;
}

void add_file_time(double ms) {
    if (!enabled()) return;
    std::lock_guard<std::mutex> lock(g_lock);
    g_file_ms += ms;
}

void frame_end() {
    if (!enabled()) return;
    const auto now = std::chrono::steady_clock::now();
    const bool first = g_last == std::chrono::steady_clock::time_point{};
    const double ms = first ? 0 : std::chrono::duration<double, std::milli>(now - g_last).count();
    g_last = now;
    std::lock_guard<std::mutex> lock(g_lock);
    if (!first && ms > hook::settings().hitch_log && g_reported < 2000) {
        ++g_reported;
        std::string text;
        for (const std::string& n : g_notes) {
            text += "\n    ";
            text += n;
        }
        if (g_dropped) text += "\n    (" + std::to_string(g_dropped) + " more)";
        // The spans' totals, longest first.
        int order[kMaxSpans];
        int n = 0;
        for (int i = 0; i < kMaxSpans; ++i)
            if (g_spans[i].calls != 0) order[n++] = i;
        std::sort(order, order + n, [](int a, int b) { return g_spans[a].ms > g_spans[b].ms; });
        if (n != 0) text += "\n    time in:";
        for (int k = 0; k < n; ++k) {
            char line[128];
            std::snprintf(line, sizeof line, " %s %.1f ms (%u)%s", g_spans[order[k]].label, g_spans[order[k]].ms,
                          g_spans[order[k]].calls, k + 1 < n ? "," : "");
            text += line;
        }
        hook::log_line("[Debug] HitchLog: frame took %.1f ms (usually %.1f), %.1f ms of it opening / reading files%s", ms,
                       g_average, g_file_ms,
                       text.empty() ? " - nothing noted: the game's own work, or the GPU / driver" : text.c_str());
    } else if (!first && ms < 250) {
        g_average = g_average == 0 ? ms : g_average * 0.98 + ms * 0.02;
    }
    g_notes.clear();
    g_dropped = 0;
    g_file_ms = 0;
    for (SpanTotal& s : g_spans) {  // (a span still open keeps its label and depth)
        s.ms = 0;
        s.calls = 0;
    }
}

}  // namespace joan::hitch
