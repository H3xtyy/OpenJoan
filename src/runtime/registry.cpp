#include "../registry.hpp"
#include "../platform/ini.hpp"
#include "../platform/window.hpp"

#include "../platform/win32_types.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// load_settings: a read of a debug-only setting (registry.hpp JOAN_DEBUG_SETTING; a constant in the release build).
#if JOAN_RELEASE
#define JOAN_DEBUG_READ(...)
#else
#define JOAN_DEBUG_READ(...) __VA_ARGS__
#endif

namespace joan::hook {
namespace {
Settings g_settings;
[[maybe_unused]] std::mutex g_log_mutex;
std::string g_log_path = JOAN_RE_LOG;
std::string g_ini_path;
std::string g_data_dir;

std::string trim(std::string s) {
    const char* ws = " \t\r\n";
    s.erase(0, s.find_first_not_of(ws));
    s.erase(s.find_last_not_of(ws) + 1);
    return s;
}

// [Hooks] Disable lists `name` or `module`, or '*' (which leaves the debug tools, debug/menu, on).
bool disabled_by_name(const char* name, const char* module) {
    // The release build has no debug tools (developer menu, freecam) and no Disable list.
#if JOAN_RELEASE
    (void)name;
    return std::strncmp(module, "debug/", 6) == 0;
#else
    for (const auto& d : g_settings.disabled)
        if ((d == "*" && std::strncmp(module, "debug/", 6) != 0) || (name && d == name) || d == module) return true;
    return false;
#endif
}

bool is_disabled(const Replacement& r) { return disabled_by_name(r.name, r.module); }

// fix/uninit: replacements of functions that read uninitialised memory in the original; they use the value the code
// clearly meant instead, so they are an original-bug fix with its own switch ([Fixes] UninitReads, default 1).
bool is_fix_uninit(const Replacement& r) { return std::strcmp(r.module, "fix/uninit") == 0; }

// Every registered replacement is on by default (since stage S7d also the former opt-in ones, which used to need
// [Hooks] Enable); only fix/uninit has a switch of its own.
bool is_enabled(const Replacement& r) { return !is_fix_uninit(r) || g_settings.fix_uninit_reads; }

}  // namespace

bool module_disabled(const char* module) { return disabled_by_name(nullptr, module); }

bool module_enabled(const char* module) {
    for (const auto& r : replacements())
        if (std::strcmp(r.module, module) == 0 && !is_disabled(r) && is_enabled(r)) return true;
    return false;
}

namespace {
// A replacement that yields to another enabled module (Replacement::yields_to) is left out.
bool yielded(const Replacement& r) { return r.yields_to != nullptr && module_enabled(r.yields_to); }
}  // namespace

std::vector<Replacement>& replacements() {
    static std::vector<Replacement> list;  // function-local: safe during static init
    return list;
}

const Settings& settings() { return g_settings; }
Settings& mutable_settings() { return g_settings; }
const std::string& data_dir() { return g_data_dir; }
const std::string& ini_path() {
    if (g_ini_path.empty()) g_ini_path = joan::platform::settings_path(g_data_dir).string();
    return g_ini_path;
}

std::vector<Counter*>& counters() {
    static std::vector<Counter*> list;
    return list;
}

Counter::Counter(const char* n, const char* c) : name(n), cls(c) { counters().push_back(this); }

void write_counter_report() {
    if constexpr (kReleaseBuild) return;  // no counts and no log (build_flavor.hpp)
    std::uint32_t total = 0, unused = 0;
    log_line("---- call statistics (calls)");
    // Counters of the same name (copies of one counter in several files, see re/split_modules.py) are summed, in
    // the order of their first registration.
    std::vector<std::pair<std::string, std::uint32_t>> rows;
    std::map<std::string, std::size_t> index;
    for (const Counter* c : counters()) {
        char full[128];
        std::snprintf(full, sizeof(full), "%s%s%s", c->cls ? c->cls : "", c->cls ? "::" : "", c->name);
        const auto [it, fresh] = index.emplace(full, rows.size());
        if (fresh) rows.emplace_back(full, 0u);
        rows[it->second].second += c->calls.load();
    }
    for (const auto& [full, calls] : rows) {
        total += calls;
        if (calls == 0) ++unused;
        log_line("%-44s %10u", full.c_str(), static_cast<unsigned>(calls));
    }
    log_line("---- %u calls into reimplemented code, %u functions never called", static_cast<unsigned>(total),
             static_cast<unsigned>(unused));
}

void load_settings(const std::string& ini_path) {
    JOAN_DEBUG_READ(g_settings.log = joan::platform::ini::get_int("Hooks", "Log", 1, ini_path.c_str()) != 0;)
    // Comma-separated lists. The Enable list grows with every batch and passed 4 KB in patch105, so read
    // with a growing buffer: ini::get_string returns size-1 when the value was truncated.
    [[maybe_unused]] auto read_list = [&](const char* key, std::vector<std::string>& out) {
        std::string value;
        for (DWORD size = 4096;; size *= 2) {
            value.assign(size, '\0');
            const DWORD n = joan::platform::ini::get_string("Hooks", key, "", value.data(), size, ini_path.c_str());
            if (n < size - 1 || size >= (1u << 24)) {
                value.resize(n);
                break;
            }
        }
        std::stringstream ss(value);
        std::string item;
        while (std::getline(ss, item, ','))
            if (auto t = trim(item); !t.empty()) out.push_back(t);
    };
    JOAN_DEBUG_READ(read_list("Disable", g_settings.disabled);)
    JOAN_DEBUG_READ(read_list("Enable", g_settings.enabled);)
    JOAN_DEBUG_READ(g_settings.gl_debug = joan::platform::ini::get_int("Graphics", "GLDebug", 0, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.perf_log = static_cast<int>(joan::platform::ini::get_int("Graphics", "PerfLog", 0, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.hitch_log = static_cast<int>(joan::platform::ini::get_int("Debug", "HitchLog", 0, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.anim_census = static_cast<int>(joan::platform::ini::get_int("Debug", "AnimCensus", 0, ini_path.c_str()));)
    g_settings.stream_cache = joan::platform::ini::get_int("Graphics", "StreamCache", 1, ini_path.c_str()) != 0;
    JOAN_DEBUG_READ(g_settings.shadows = static_cast<int>(joan::platform::ini::get_int("Graphics", "Shadows", 1, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.shadow_distance = static_cast<int>(joan::platform::ini::get_int("Graphics", "ShadowDistance", 4000, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.shadow_map_size = static_cast<int>(joan::platform::ini::get_int("Graphics", "ShadowMapSize", 4096, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.shadow_strength = static_cast<int>(joan::platform::ini::get_int("Graphics", "ShadowStrength", 45, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.shadow_softness = static_cast<int>(joan::platform::ini::get_int("Graphics", "ShadowSoftness", 3, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.shadow_foliage = static_cast<int>(joan::platform::ini::get_int("Graphics", "ShadowFoliage", 1, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.baked_shadows = static_cast<int>(joan::platform::ini::get_int("Graphics", "BakedShadows", 25, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.shadow_caster_margin = static_cast<int>(joan::platform::ini::get_int("Graphics", "ShadowCasterMargin", 1500, ini_path.c_str()));)
    g_settings.water = static_cast<int>(joan::platform::ini::get_int("Graphics", "Water", 1, ini_path.c_str()));
    g_settings.pixel_lighting = static_cast<int>(joan::platform::ini::get_int("Graphics", "PixelLighting", 1, ini_path.c_str()));
    g_settings.camera_fade =static_cast<int>(joan::platform::ini::get_int("Graphics", "CameraFade", 0, ini_path.c_str()));
    g_settings.gpu_shadow_volumes = static_cast<int>(joan::platform::ini::get_int("Graphics", "GpuShadowVolumes", 1, ini_path.c_str()));
    g_settings.grass_batching = static_cast<int>(joan::platform::ini::get_int("Graphics", "GrassBatching", 1, ini_path.c_str()));
    g_settings.grass_multi_draw = static_cast<int>(joan::platform::ini::get_int("Graphics", "GrassMultiDraw", 1, ini_path.c_str()));
    g_settings.grass_bindless = static_cast<int>(joan::platform::ini::get_int("Graphics", "GrassBindless", 1, ini_path.c_str()));
    g_settings.water_clarity = static_cast<int>(joan::platform::ini::get_int("Graphics", "WaterClarity", 160, ini_path.c_str()));
    g_settings.water_waves = static_cast<int>(joan::platform::ini::get_int("Graphics", "WaterWaves", 60, ini_path.c_str()));
    g_settings.water_reflection = static_cast<int>(joan::platform::ini::get_int("Graphics", "WaterReflection", 100, ini_path.c_str()));
    g_settings.water_flow = static_cast<int>(joan::platform::ini::get_int("Graphics", "WaterFlow", 60, ini_path.c_str()));
    g_settings.vsync = static_cast<int>(joan::platform::ini::get_int("Graphics", "VSync", 0, ini_path.c_str()));
    g_settings.sounds_in_memory = static_cast<int>(joan::platform::ini::get_int("Audio", "SoundsInMemory", 1, ini_path.c_str()));
    g_settings.files_in_memory = static_cast<int>(joan::platform::ini::get_int("Platform", "FilesInMemory", 1, ini_path.c_str()));
    g_settings.item_preload = static_cast<int>(joan::platform::ini::get_int("Game", "ItemPreload", 1, ini_path.c_str()));
    g_settings.anim_cache = static_cast<int>(joan::platform::ini::get_int("Game", "AnimCache", 1, ini_path.c_str()));
    g_settings.max_fps = static_cast<int>(joan::platform::ini::get_int("Graphics", "MaxFps", static_cast<UINT>(-1), ini_path.c_str()));
    JOAN_DEBUG_READ(g_settings.cull_flip = joan::platform::ini::get_int("Graphics", "CullFlip", 0, ini_path.c_str()) != 0;)
    g_settings.borderless = joan::platform::ini::get_int("Graphics", "Borderless", 1, ini_path.c_str()) != 0;
    JOAN_DEBUG_READ(g_settings.screenshot_key = static_cast<int>(joan::platform::ini::get_int("Graphics", "ScreenshotKey", 0x7A, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.shader_log = joan::platform::ini::get_int("Graphics", "ShaderLog", 0, ini_path.c_str()) != 0;)
    g_settings.anisotropy = static_cast<int>(joan::platform::ini::get_int("Graphics", "Anisotropy", 16, ini_path.c_str()));
    g_settings.msaa = static_cast<int>(joan::platform::ini::get_int("Graphics", "MSAA", 0, ini_path.c_str()));
    g_settings.texture_filter = static_cast<int>(joan::platform::ini::get_int("Graphics", "TextureFilter", 1, ini_path.c_str()));
    g_settings.ao_mode = static_cast<int>(joan::platform::ini::get_int("Graphics", "AmbientOcclusion", 1, ini_path.c_str()));
    auto read_float = [&](const char* key, float fallback) {
        char buf[32] = {};
        joan::platform::ini::get_string("Graphics", key, "", buf, sizeof(buf), ini_path.c_str());
        const std::string v = trim(buf);
        return v.empty() ? fallback : static_cast<float>(std::atof(v.c_str()));
    };
    g_settings.ao_radius = read_float("AmbientOcclusionRadius", 40.0f);
    g_settings.ao_strength = read_float("AmbientOcclusionStrength", 1.8f);
    g_settings.ao_fade = read_float("AmbientOcclusionDistance", 2500.0f);
    JOAN_DEBUG_READ(g_settings.fix_shaders = joan::platform::ini::get_int("Fixes", "Shaders", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_mouse_turn = joan::platform::ini::get_int("Fixes", "MouseTurn", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_cutscene_grass = joan::platform::ini::get_int("Fixes", "CutsceneGrass", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_glow_depth = joan::platform::ini::get_int("Fixes", "GlowDepth", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_sound_slots = joan::platform::ini::get_int("Fixes", "SoundSlots", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_wet_sounds = joan::platform::ini::get_int("Fixes", "WetSounds", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_missing_sounds = joan::platform::ini::get_int("Fixes", "MissingSounds", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_idle_rate = joan::platform::ini::get_int("Fixes", "IdleRate", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_music_start_wait = joan::platform::ini::get_int("Fixes", "MusicStartWait", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_trigger_sort = joan::platform::ini::get_int("Fixes", "TriggerSort", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_unit_stuck = joan::platform::ini::get_int("Fixes", "UnitStuck", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_unit_stuck_watch = g_settings.fix_unit_stuck;)
    JOAN_DEBUG_READ(g_settings.fix_immovable_siege = joan::platform::ini::get_int("Fixes", "ImmovableSiege", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_animal_pen = joan::platform::ini::get_int("Fixes", "AnimalPen", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_mouse_options = joan::platform::ini::get_int("Fixes", "MouseOptions", 1, ini_path.c_str()) != 0;)
    g_settings.cursor_sensitivity = static_cast<int>(joan::platform::ini::get_int("Platform", "CursorSensitivity", 10, ini_path.c_str()));
    JOAN_DEBUG_READ(g_settings.fix_mission_count = joan::platform::ini::get_int("Fixes", "MissionCount", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.fix_uninit_reads = joan::platform::ini::get_int("Fixes", "UninitReads", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.save_audit = static_cast<int>(joan::platform::ini::get_int("Debug", "SaveAudit", 0, ini_path.c_str()));)
    JOAN_DEBUG_READ(g_settings.save_compare = joan::platform::ini::get_int("Debug", "SaveCompare", 0, ini_path.c_str()) != 0;)
    // SaveFieldwise=0 (the raw record writes of the 32-bit game) only on the 32-bit build: on x64 the raw path would write
    // x64 object bytes, a save no build reads (Phase 2 review)
    JOAN_DEBUG_READ(g_settings.save_fieldwise = sizeof(void*) == 8 || joan::platform::ini::get_int("Debug", "SaveFieldwise", 1, ini_path.c_str()) != 0;)
    JOAN_DEBUG_READ(g_settings.profile = static_cast<int>(joan::platform::ini::get_int("Debug", "Profile", 0, ini_path.c_str()));)
    g_settings.unit_index = static_cast<int>(joan::platform::ini::get_int("Game", "UnitIndex", 1, ini_path.c_str()));
    g_settings.all_heroes = static_cast<int>(joan::platform::ini::get_int("Game", "AllHeroes", kReleaseBuild ? 0 : 1, ini_path.c_str()));
    JOAN_DEBUG_READ(g_settings.unit_index_check = static_cast<int>(joan::platform::ini::get_int("Debug", "UnitIndexCheck", 64, ini_path.c_str()));)
    char resolution[64] = {};
    joan::platform::ini::get_string("Display", "Resolution", "desktop", resolution, sizeof(resolution), ini_path.c_str());
    g_settings.resolution = trim(resolution);
    g_settings.dpi_aware = joan::platform::ini::get_int("Display", "DpiAware", 1, ini_path.c_str()) != 0;
    char ui_scale[32] = {};
    joan::platform::ini::get_string("Display", "UiScale", "auto", ui_scale, sizeof(ui_scale), ini_path.c_str());
    const std::string ui = trim(ui_scale);
    g_settings.ui_scale = ui == "auto" ? 0.0f : static_cast<float>(std::atof(ui.c_str()));
    if (ui != "auto" && g_settings.ui_scale <= 0.0f) g_settings.ui_scale = 1.0f;
    const auto slash = ini_path.find_last_of("\\/");
    g_data_dir = slash == std::string::npos ? std::string() : ini_path.substr(0, slash + 1);
    g_ini_path = ini_path;
    g_log_path = g_data_dir + JOAN_RE_LOG;
    if constexpr (kReleaseBuild) return;  // the checks below only write openjoan.log
    // Keys of retired modes and paths (joan.exe's code modes, the Direct3D 8 / old-API paths, their traces and
    // diagnostics): an old ini may still set them; they are ignored and named once in openjoan.log.
    static const char* const kRetired[][2] = {
        {"Loader", "Code"},         {"Loader", "Data"},          {"Loader", "StrayDataGuard"}, {"Hooks", "Verify"},
        {"Hooks", "Census"},        {"Hooks", "Trusted"},        {"Graphics", "Renderer"},    {"Graphics", "NativeCaps"},
        {"Graphics", "Trace"},      {"Graphics", "TraceFrame"},  {"Graphics", "TraceStartup"}, {"Graphics", "Census"},
        {"Graphics", "DrawProfile"}, {"Audio", "OpenAL"},        {"Video", "Decoder"},        {"Platform", "Window"},
        {"Platform", "Input"},      {"Platform", "Files"},       {"Platform", "System"},      {"Debug", "SehSelfTest"},
    };
    for (const auto& r : kRetired) {
        char value[64] = {};
        joan::platform::ini::get_string(r[0], r[1], "", value, sizeof(value), ini_path.c_str());
        if (!trim(value).empty()) log_line("[%s] %s= ignored: a retired key", r[0], r[1]);
    }
}

#if !JOAN_RELEASE
void log_line(const char* fmt, ...) {
    if (!g_settings.log) return;
    std::lock_guard lock(g_log_mutex);
    FILE* f = std::fopen(g_log_path.c_str(), "a");
    if (!f) return;
#ifdef _WIN32
    SYSTEMTIME t;
    GetLocalTime(&t);
    std::fprintf(f, "%02d:%02d:%02d.%03d ", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
#else
    const auto now = std::chrono::system_clock::now();
    const std::time_t secs = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
    std::tm t{};
    localtime_r(&secs, &t);
    std::fprintf(f, "%02d:%02d:%02d.%03d ", t.tm_hour, t.tm_min, t.tm_sec, static_cast<int>(ms));
#endif
    va_list ap;
    va_start(ap, fmt);
    std::vfprintf(f, fmt, ap);
    va_end(ap);
    std::fputc('\n', f);
    std::fclose(f);
}
#endif

const Replacement* registration(std::uintptr_t original) {
    for (const auto& r : replacements())
        if (r.original == original && !r.iat_saved) return &r;
    return nullptr;
}

const Replacement* nearest_replacement(std::uintptr_t code) {
    // Built once: the registry does not change after static initialisation.
    static const std::vector<const Replacement*> sorted = [] {
        std::vector<const Replacement*> v;
        for (const auto& r : replacements())
            if (!r.iat_saved) v.push_back(&r);
        std::sort(v.begin(), v.end(), [](const Replacement* a, const Replacement* b) {
            return reinterpret_cast<std::uintptr_t>(a->replacement) < reinterpret_cast<std::uintptr_t>(b->replacement);
        });
        return v;
    }();
    const auto it = std::upper_bound(sorted.begin(), sorted.end(), code, [](std::uintptr_t c, const Replacement* r) {
        return c < reinterpret_cast<std::uintptr_t>(r->replacement);
    });
    return it == sorted.begin() ? nullptr : *(it - 1);
}

const void* installed_replacement(std::uintptr_t original) {
    for (const auto& r : replacements())
        if (r.original == original && !r.iat_saved && !is_disabled(r) && is_enabled(r) && !yielded(r))
            return r.replacement;
    return nullptr;
}

int install_all() {
    int installed = 0;
    log_line("[Hooks] Enable: %u entries, Disable: %u%s", static_cast<unsigned>(g_settings.enabled.size()),
             static_cast<unsigned>(g_settings.disabled.size()),
             g_settings.enabled.empty() ? "" : " (Enable is no longer needed: every replacement is on by default)");
    for (const auto& e : g_settings.enabled) {  // a typo in an old Enable list (harmless now, but reported)
        // Modules without registered replacements (they ask module_disabled() themselves).
        static const char* const kSelfEnabled[] = {"render/nvlink"};
        const bool known =
            std::any_of(std::begin(kSelfEnabled), std::end(kSelfEnabled), [&](const char* m) { return e == m; }) ||
            std::any_of(replacements().begin(), replacements().end(),
                        [&](const Replacement& r) { return e == r.name || e == r.module; });
        if (!known) log_line("warning: [Hooks] Enable=%s matches no module or function", e.c_str());
    }
    for (const auto& r : replacements()) {
        if (is_disabled(r)) {
            log_line("skip    %-40s 0x%08x (disabled)", r.name, static_cast<unsigned>(r.original));
            continue;
        }
        if (!is_enabled(r)) {
            log_line("skip    %-40s 0x%08x ([Fixes] UninitReads=0)", r.name, static_cast<unsigned>(r.original));
            continue;
        }
        if (yielded(r)) {
            log_line("skip    %-40s 0x%08x (inside %s)", r.name, static_cast<unsigned>(r.original), r.yields_to);
            continue;
        }
        if (r.iat_saved) {
            if (patch_import(r.original, r.replacement, r.iat_saved)) {
                ++installed;
                log_line("replace %-40s 0x%08x (IAT slot) [%s]", r.name, static_cast<unsigned>(r.original), r.module);
            } else {
                log_line("FAILED  %-40s 0x%08x (IAT slot)", r.name, static_cast<unsigned>(r.original));
            }
            continue;
        }
        const int n = redirect_function(r.original, r.replacement);
        if (n == 0 && patch_entry(r.original, r.replacement)) {
            ++installed;
            log_line("replace %-40s 0x%08x (entry slot) [%s]", r.name, static_cast<unsigned>(r.original), r.module);
            continue;
        }
        if (n == 0) {
            log_line("FAILED  %-40s 0x%08x (no code slot: re/gen_code_slots.py)", r.name, static_cast<unsigned>(r.original));
            continue;
        }
        ++installed;
        log_line("replace %-40s 0x%08x (%d thunk slot%s) [%s]", r.name, static_cast<unsigned>(r.original), n,
                 n == 1 ? "" : "s", r.module);
    }
    return installed;
}

}  // namespace joan::hook
