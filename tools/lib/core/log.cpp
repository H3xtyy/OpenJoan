#include "core/log.hpp"

#include <atomic>
#include <cstdio>
#include <mutex>

namespace joan::log {
namespace {
std::atomic<Level> g_min_level{Level::Info};
std::mutex g_mutex;

const char* tag(Level level) {
    switch (level) {
        case Level::Debug: return "debug";
        case Level::Info: return "info ";
        case Level::Warn: return "warn ";
        case Level::Error: return "ERROR";
    }
    return "?";
}
}  // namespace

void set_min_level(Level level) { g_min_level = level; }

void write(Level level, std::string_view message) {
    if (level < g_min_level.load()) return;
    std::lock_guard lock(g_mutex);
    std::FILE* out = level >= Level::Warn ? stderr : stdout;
    std::fprintf(out, "[%s] %.*s\n", tag(level), static_cast<int>(message.size()), message.data());
}

}  // namespace joan::log
