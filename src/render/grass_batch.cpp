// [Graphics] GrassBatching - see grass_batch.hpp.
#include "grass_batch.hpp"

namespace joan::render {

namespace {
const GrassHooks* g_hooks = nullptr;
bool g_recording = false;
}  // namespace

void set_grass_hooks(const GrassHooks* hooks) { g_hooks = hooks; }

bool grass_replay(const GrassKey& key, const float* wind, std::uint32_t colour, bool fading) {
    return g_hooks && g_hooks->replay(key, wind, colour, fading);
}
bool grass_record_begin(const GrassKey& key) {
    g_recording = g_hooks && g_hooks->record_begin(key);
    return g_recording;
}
void grass_record_colour(std::uint32_t material_colour) {
    if (g_recording) g_hooks->record_colour(material_colour);
}
bool grass_recording() { return g_recording; }
void grass_record_end() {
    if (!g_recording) return;
    g_recording = false;
    g_hooks->record_end();
}
void grass_forget(const void* plane) {
    if (g_hooks) g_hooks->forget(plane);
}
void grass_flush() {
    if (g_hooks) g_hooks->flush();
}

}  // namespace joan::render
