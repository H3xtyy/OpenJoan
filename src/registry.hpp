// Registry of reimplemented functions.
//
// Each module registers its replacements with JOAN_REPLACE (JOAN_REPLACE_YIELDING, JOAN_REPLACE_IMPORT). At start-up
// install_all installs every registered replacement, except those switched off in openjoan.ini:
//
//   [Hooks]
//   ; module or function names, comma separated; '*' = everything except the debug/ tools
//   Disable=ResourceIdx::get_data_size
//
// and the module fix/uninit, which follows [Fixes] UninitReads (default 1). [Hooks] Enable= is no longer
// needed: until stage S7d most replacements were "opt-in" and installed only when listed there; now every one is
// on by default (an Enable list is still read, checked for unknown names and otherwise ignored, so old inis keep
// working; the opt-in macros were merged into JOAN_REPLACE in Phase 3).
//
#pragma once

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

#include "build_flavor.hpp"
#include "runtime/patch.hpp"

namespace joan::hook {

struct Replacement {
    const char* name;      // "ResourceIdx::get_data_size"
    const char* module;    // original source file, e.g. "ilib/resx.cpp"
    std::uintptr_t original;
    const void* replacement;
    std::uintptr_t* iat_saved = nullptr;  // non-null: `original` is an IAT slot (patch_import), old pointer saved here
    // Not installed while module `yields_to` is enabled: that module replaces the same address and
    // calls this implementation itself (game/b117_04 Game::process_input inside debug/menu).
    const char* yields_to = nullptr;
};

std::vector<Replacement>& replacements();
/// The replacement installed for `original` by install_all (thunk-redirected), or nullptr.
const void* installed_replacement(std::uintptr_t original);
/// The first registration of a code address (installed or not), or nullptr. IAT slots are not code addresses.
const Replacement* registration(std::uintptr_t original);
/// The registered replacement whose C++ function starts closest at or below `code` (the function a code address of ours
/// most likely lies in, as far as the registry knows), or nullptr. For diagnostics (crash log, stray reports).
const Replacement* nearest_replacement(std::uintptr_t code);
/// True when a replacement of `module` is installed - registered and not switched off ([Hooks] Disable; fix/uninit:
/// [Fixes] UninitReads) - whether install_all puts it at its address or it yields to another module
/// (Replacement::yields_to).
bool module_enabled(const char* module);
/// True when [Hooks] Disable switches `module` off (its name, or '*' unless it is one of the debug/ tools). For
/// modules that are switched on without registered replacements (render/nvlink).
bool module_disabled(const char* module);

struct Registrar {
    Registrar(const char* name, const char* module, std::uintptr_t original, const void* replacement) {
        replacements().push_back({name, module, original, replacement});
    }
    Registrar(const char* name, const char* module, std::uintptr_t slot, const void* replacement,
              std::uintptr_t* iat_saved) {
        replacements().push_back({name, module, slot, replacement, iat_saved});
    }
    Registrar(const char* name, const char* module, std::uintptr_t original, const void* replacement,
              const char* yields_to) {
        replacements().push_back({name, module, original, replacement, nullptr, yields_to});
    }
};

/// Per-function statistics, written to openjoan.log when the game exits: how often a reimplementation ran.
/// (The verify-mode match / mismatch columns went with joan.exe's code, stage S7c; the comparison code was removed
/// from the modules in w129.)
struct Counter {
    explicit Counter(const char* name, const char* cls = nullptr);
    const char* name;
    const char* cls;  // optional class prefix, printed as cls::name
    std::atomic<std::uint32_t> calls{0};

    // A plain load + store, not a locked add (~20 cycles on x86, in thousands of functions, some called millions
    // of times a second): the counts are statistics for the log only, and the game calls nearly all of them from
    // one thread (a rare concurrent hit may be lost).
    // The release build (build_flavor.hpp) writes no report: no count at all.
    void hit() {
        if constexpr (!kReleaseBuild) calls.store(calls.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed);
    }
};
std::vector<Counter*>& counters();
void write_counter_report();

// A setting of the debug build only: read from openjoan.ini there (default `debug`); in the release build
// (build_flavor.hpp) a constant `release`, so the code it switches folds away (settings().x is a constant expression).
#if JOAN_RELEASE
#define JOAN_DEBUG_SETTING(type, name, debug, release) static constexpr type name = release
#else
#define JOAN_DEBUG_SETTING(type, name, debug, release) type name = debug
#endif

struct Settings {
    JOAN_DEBUG_SETTING(bool, log, true, false);
    std::vector<std::string> disabled;
    std::vector<std::string> enabled;  // [Hooks] Enable: only checked for unknown names (every replacement is on)
    // [Graphics] (the renderer is always our OpenGL 4.6 device; retired keys: registry.cpp load_settings)
    JOAN_DEBUG_SETTING(bool, gl_debug, false, false);          // opengl: debug context, GL errors/warnings go to openjoan.log
    JOAN_DEBUG_SETTING(int, perf_log, 0, 0);               // PerfLog: every N seconds a frame-time / GL traffic line in openjoan.log (0 = off)
    JOAN_DEBUG_SETTING(int, hitch_log, 0, 0);              // [Debug] HitchLog: frames longer than N ms logged with what happened in them (hitch.hpp), 0 = off
    JOAN_DEBUG_SETTING(int, anim_census, 0, 0);            // [Debug] AnimCensus: every N seconds which animations play how often and whether they move anything (en3d/animdef.cpp), 0 = off
    bool stream_cache = true;       // StreamCache: unchanged model vertices / indices drawn from cached buffers (game/unknown_d3dsys.cpp)
    JOAN_DEBUG_SETTING(int, shadows, 1, 1);                // Shadows: 0 off, 1 the original stencil shadows, 2 shadow maps (render/shadow_map.*)
    JOAN_DEBUG_SETTING(int, shadow_distance, 4000, 4000);     // ShadowDistance: world units from the camera covered by the shadow map
    JOAN_DEBUG_SETTING(int, shadow_map_size, 4096, 4096);     // ShadowMapSize: texels per side
    JOAN_DEBUG_SETTING(int, shadow_strength, 45, 45);       // ShadowStrength: % darkening in full shadow
    JOAN_DEBUG_SETTING(int, shadow_softness, 3, 3);        // ShadowSoftness: penumbra width in shadow map texels
    JOAN_DEBUG_SETTING(int, shadow_foliage, 1, 1);         // ShadowFoliage: the transparent pass's cut-outs (trees, bushes) cast shadows
    JOAN_DEBUG_SETTING(int, baked_shadows, 25, 100);         // BakedShadows: % of the level lightmaps' baked shadows kept with Shadows=2
    JOAN_DEBUG_SETTING(int, shadow_caster_margin, 1500, 1500);  // ShadowCasterMargin: world units outside the picture still drawn to cast
    int water = 1;                  // Water: 0 the original water, 1 modern water (render/water.*)
    int pixel_lighting = 1;         // PixelLighting: lit draws lit per pixel (render/pixel_lighting.hpp)
    int ui_animations = 1;          // [Display] UiAnimations: 1 menu widgets slide and zoom (original), 0 they are at once where they go
    int camera_fade = 0;            // CameraFade: 1 objects between the camera and the player turn see-through (original), 0 off
    int cursor_sensitivity = 10;    // [Platform] CursorSensitivity: position of the options page's cursor slider, 0..20 (10 = the original's speed)
    int gpu_shadow_volumes = 1;     // GpuShadowVolumes: 1 the original shadows' volumes made by the GPU (render/shadow_volumes.hpp), 0 by the CPU
    int grass_batching = 1;         // GrassBatching: 1 the grass patches' draws recorded once and replayed (render/grass_batch.hpp), 0 drawn as the original
    int grass_multi_draw = 1;       // GrassMultiDraw: 1 consecutive replayed grass draws sent as one glMultiDrawElementsIndirect, 0 one call each
    int grass_bindless = 1;         // GrassBindless: 1 replayed grass samples its textures through bindless handles (GL_ARB_bindless_texture) so draws of different textures share a multi-draw
    int water_clarity = 160;        // WaterClarity: world units of water before it is mostly the water's colour
    int water_waves = 60;           // WaterWaves: wave strength, %
    int water_reflection = 100;     // WaterReflection: reflection strength, %
    int water_flow = 60;            // WaterFlow: rivers' flow speed, % of the original
    int vsync = 0;                  // opengl: swap interval (0 = off, 1 = on, -1 = adaptive)
    int sounds_in_memory = 1;       // [Audio] SoundsInMemory: 1 the sound archives kept in memory (ilib/sound_archives.hpp), 0 read from disk as the original
    int files_in_memory = 1;        // [Platform] FilesInMemory: 1 the item / particle / item-script files served from memory (platform/files.cpp), 0 from disk
    int item_preload = 1;           // [Game] ItemPreload: 1 the drop items' models built at the first mission load and kept (game/item_preload.hpp), 0 on first drop as the original
    int anim_cache = 1;             // [Game] AnimCache: 1 converted animation keys kept in ANIM_TMP (en3d/anim_cache.hpp), 0 converted on every load as the original
    int max_fps = -1;               // opengl: frame limiter (-1 = monitor refresh rate, 0 = off)
    JOAN_DEBUG_SETTING(bool, cull_flip, false, false);         // opengl: invert the cull winding (if geometry looks inside-out)
    bool borderless = true;         // opengl: full-screen requests become a borderless window on the monitor
    JOAN_DEBUG_SETTING(int, screenshot_key, 0x7A, 0x7A);      // opengl: virtual-key code that saves openjoan_shotNNN.bmp (F11; 0 = off)
    JOAN_DEBUG_SETTING(bool, shader_log, false, false);        // log each vertex shader nvlinker.dll links: fragments, declaration, tokens,
                                    // constant registers (needs en3d/helpers22; render/nvlink_log.cpp)
    // opengl, quality options (0 = the original look; gl/device.cpp, render/renderer.cpp):
    int anisotropy = 16;            // Anisotropy: anisotropic filtering of mipmapped 3D textures (2..16)
    int msaa = 0;                   // MSAA: multisample anti-aliasing of the back buffer (2, 4, 8)
    int texture_filter = 1;        // TextureFilter: 1 = trilinear (mip LINEAR) where the game asks mip POINT
    int ao_mode = 1;                // AmbientOcclusion: 0 off, 1 on, 2 show the occlusion only (render/ambient_occlusion.*)
    float ao_radius = 40.0f;        // AmbientOcclusionRadius: world units (Joan is ~79 tall)
    float ao_strength = 1.8f;       // AmbientOcclusionStrength
    float ao_fade = 2500.0f;        // AmbientOcclusionDistance: full effect up to this distance, gone at twice it
    // [Display]
    std::string resolution = "desktop";  // desktop | original (1024x768) | WIDTHxHEIGHT
    bool dpi_aware = true;               // real pixels on scaled (125%/150%) desktops
    float ui_scale = 0.0f;               // in-mission HUD size (0 = auto from the resolution)
    // [Fixes] (corrections of original bugs; 0 = exactly as the original game)
    JOAN_DEBUG_SETTING(bool, fix_shaders, true, true);  // Shaders: render/nvlink links corrected vertex shaders (NVLINK.md 13)
    JOAN_DEBUG_SETTING(bool, fix_mouse_turn, true, true);  // MouseTurn: mouse turning keeps sub-degree motion between frames (0x00431790)
    JOAN_DEBUG_SETTING(bool, fix_cutscene_grass, true, true);  // CutsceneGrass: grass stays on in cutscene camera modes (0x00429860)
    JOAN_DEBUG_SETTING(bool, fix_glow_depth, true, true);  // GlowDepth: light glows (lanterns, 0x00486d10) are hidden behind objects
    JOAN_DEBUG_SETTING(bool, fix_sound_slots, true, true);  // SoundSlots: 32 sound slots instead of the original 12 (game/sound_slots.hpp)
    JOAN_DEBUG_SETTING(bool, fix_wet_sounds, true, true);  // WetSounds: in rain a sound without a "_WET" version plays its ground version (game/unknown_unit_core.cpp)
    JOAN_DEBUG_SETTING(bool, fix_missing_sounds, true, true);  // MissingSounds: sounds the game data forgot to name (idle voices of the Bastard of Orleans and Richemont, attack swings of heroes 5..7; game/animgrp.cpp)
    JOAN_DEBUG_SETTING(bool, fix_idle_rate, true, true);  // IdleRate: idle variety (IDLE2 / IDLE3) as often at any frame rate as at 60 fps (game/unknown_unit_addon.cpp)
    JOAN_DEBUG_SETTING(bool, fix_music_start_wait, true, true);  // MusicStartWait: music / ambience start without the original's ~9 ms wait on the game thread (uti3d/video.cpp Video::open)
    JOAN_DEBUG_SETTING(bool, fix_trigger_sort, true, true);  // TriggerSort: the trigger list is sorted by the records' own key byte (+0x12); the original's
                                   // comparator 0x0045bdf0 reads that byte relative to the pointer SLOT, i.e. from a neighbouring pointer's
                                   // address bits, an order no other build can reproduce (game/unknown_chatbox.cpp; a replay applies the
                                   // recording's value, stream.hpp kFlagTriggerSortFix)
    JOAN_DEBUG_SETTING(bool, fix_unit_stuck, true, true);  // UnitStuck: an escorting soldier whose formation spot is off the walkable area
                                   // goes to the nearest walkable point toward its leader, and one that cannot get closer stops
                                   // instead of running in place (game/unknown_ai_behav.cpp Escort::update_4be770; a replay
                                   // applies the recording's value, stream.hpp kFlagUnitStuckFix)
    JOAN_DEBUG_SETTING(bool, fix_unit_stuck_watch, true, true);  // UnitStuck part 2 (not an ini key: = UnitStuck): a unit that does not move
                                   // along its route for 1 s stops (game/unit_stuck.hpp). Separate so that a replay of a recording made
                                   // before part 2 existed applies part 1 only (stream.hpp kFlagUnitStuckWatch)
    JOAN_DEBUG_SETTING(bool, fix_immovable_siege, true, true);  // ImmovableSiege: ordering a siege weapon that has no crew position for
                                   // moving (the trebuchet, the arbalest) to move answers "unable to comply" (RTS_LTINF_011) instead of
                                   // the "need more men" line RTS_SIEGE_005 (game/unknown_rts2.cpp RtsGroup::move_formation_44cfb0;
                                   // a replay applies the recording's value, stream.hpp kFlagImmovableSiege)
    JOAN_DEBUG_SETTING(bool, fix_animal_pen, true, true);  // AnimalPen: an animal whose steps the physics refuses for leaving the
                                   // navigation mesh (the pigs in their pens) gets no wander route and keeps its idle animations
                                   // instead of walking on the spot (game/animal_pen.hpp; a replay applies the recording's
                                   // value, stream.hpp kFlagAnimalPen)
    JOAN_DEBUG_SETTING(bool, fix_mission_count, true, true);  // MissionCount: the campaign ends after the last mission the data holds
                                   // (SCENARIO\scenario_NN.scr), not after mission 8 (game/mission_count.hpp; a replay applies the
                                   // recording's value, stream.hpp kFlagMissionCount)
    JOAN_DEBUG_SETTING(bool, fix_ending_layout, true, true);  // not an ini key: the ending's pictures are placed like every 1024x768 screen (Ending::draw 0x006ce450); off only while replaying a recording made before this fix
    JOAN_DEBUG_SETTING(bool, fix_wheel_zoom, true, true);  // not an ini key: a wheel notch zooms the same at any frame rate (Unit::zoom_step 0x00447960, the hero camera's step_height 0x00438720); off only while replaying a recording made before this fix
    JOAN_DEBUG_SETTING(bool, fix_popup_buttons, true, true);  // not an ini key: a message box's buttons are centred on the real screen (MessageBox::draw 0x006d3cb0); off only while replaying a recording made before this fix
    JOAN_DEBUG_SETTING(bool, fix_mouse_options, true, true);  // MouseOptions: ten lower steps on the camera slider, a cursor sensitivity slider (interfac/mouse_options.hpp; a replay applies the recording's value)
    JOAN_DEBUG_SETTING(bool, fix_uninit_reads, true, true);  // UninitReads: module fix/uninit (functions whose original reads uninitialised memory;
                                   // default 1: without joan.exe's code the uninitialised-read originals are traps)
    // [Debug]
    JOAN_DEBUG_SETTING(int, save_audit, 0, 0);  // SaveAudit: 1 = log joan.exe image addresses found in written save data (save_audit.cpp),
                         // 2 = also heap values, and field-wise chunks mapped to their record's fields (save/io.hpp)
    JOAN_DEBUG_SETTING(bool, save_compare, false, false);  // SaveCompare: field-wise chunks compared with the old raw bytes (x86; save/io.hpp)
    JOAN_DEBUG_SETTING(bool, save_fieldwise, true, true);  // SaveFieldwise: converted records written / read field by field (0 = old raw path)
    JOAN_DEBUG_SETTING(int, profile, 0, 0);             // Profile: every N seconds a sampling profile of the main thread in openjoan.log (profiler.hpp; 0 = off)
    int unit_index = 1;          // UnitIndex: the sectors' unit actors kept in walk order for the "units near" queries (en3d/scene_lists.hpp)
    int all_heroes = kReleaseBuild ? 0 : 1;  // AllHeroes (off by default in the release build): the Bastard of Orleans, Richemont and Charles VII playable with their keys in action mode too
    JOAN_DEBUG_SETTING(int, unit_index_check, 64, 0);   // UnitIndexCheck: every Nth lookup compared with the list walk (0 = never, 1 = every one)
};

const Settings& settings();
/// The settings for code that changes them while the game runs (the options menu, interfac/graphics_page.cpp).
Settings& mutable_settings();
/// Folder of openjoan.ini / openjoan.log (the game folder), with a trailing separator.
const std::string& data_dir();
/// The settings file load_settings read (openjoan.ini, or joan_re.ini as the fallback: platform::settings_path);
/// before load_settings, the name settings_path chooses in data_dir().
const std::string& ini_path();
void load_settings(const std::string& ini_path);
#if JOAN_RELEASE
// The release build writes no openjoan.log (build_flavor.hpp): logging compiles away.
inline void log_line(const char*, ...) {}
#else
void log_line(const char* fmt, ...);
#endif

/// Installs every enabled replacement. Returns the number installed.
int install_all();

}  // namespace joan::hook

// Base name of the log files (openjoan.log / openjoan_loader.log). A game folder holds one build of the executable -
// 32-bit or 64-bit - so every build uses the same names.
#define JOAN_RE_LOG_BASE "openjoan"
#define JOAN_RE_LOG JOAN_RE_LOG_BASE ".log"
#define JOAN_RE_LOADER_LOG JOAN_RE_LOG_BASE "_loader.log"

#define JOAN_HOOK_CAT2(a, b) a##b
#define JOAN_HOOK_CAT(a, b) JOAN_HOOK_CAT2(a, b)
/// Defines a namespace-scope Counter: JOAN_COUNTER(c_read, "File::file_read");
#define JOAN_COUNTER(id, name) static ::joan::hook::Counter id(name)

#define JOAN_REPLACE(module, name, address, fn) \
    static ::joan::hook::Registrar JOAN_HOOK_CAT(joan_replace_, __LINE__)(name, module, address, ::joan::hook::code_address(fn))
/// A replacement that is left out while module `other` (which replaces the same address and calls this one itself)
/// is enabled.
#define JOAN_REPLACE_YIELDING(module, name, address, fn, other)                                                 \
    static ::joan::hook::Registrar JOAN_HOOK_CAT(joan_replace_, __LINE__)(name, module, address,             \
                                                                           ::joan::hook::code_address(fn),       \
                                                                           static_cast<const char*>(other))
/// A replacement of an imported function: the IAT slot `slot` is pointed at fn and the DLL's pointer is saved in
/// `saved` (std::uintptr_t).
#define JOAN_REPLACE_IMPORT(module, name, slot, fn, saved)                                                      \
    static ::joan::hook::Registrar JOAN_HOOK_CAT(joan_replace_, __LINE__)(name, module, slot,                \
                                                                           ::joan::hook::code_address(fn), &(saved))
