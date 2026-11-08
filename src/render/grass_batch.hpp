// [Graphics] GrassBatching=1: the grass patches' draws recorded once and replayed (not part of the original game).
//
// DynGrass covers the ground around the camera with thousands of patch objects (game/dyngrass.cpp, holder kind 3),
// each drawn every frame through the whole object path - Actor::draw, Plane::draw, the stream cache's check of its
// arrays, the material set-up (SysEn) and a device draw: ~1.7 us a draw, a third of the frame in a battle. Between
// patches only the geometry and, in the fade range, the fade colour differ; the world (identity) and wind (WORLD1)
// matrices are the same for all.
// So the first time a patch is drawn for a camera, pass and fade state, SceneS::draw_lists lets it go through the
// normal path while the device records each draw it makes (program, raster state, textures, the patch's cached
// geometry - all kept referenced - and the per-draw record); afterwards the device draws those again directly with
// the patch's wind matrix and fade colour, and the camera's normal matrices. A recording is made again when the
// patch's mesh changes or goes (grass_forget), when the sector's lights differ from the recording's (the program),
// or when a pass needs the full path (the ambient occlusion's cut-outs, the shadow map's recording).
// The visibility test and the order of the draws are the game's; nothing the game logic reads changes.
#pragma once

#include <cstdint>

namespace joan::render {

/// Identifies one recording: a patch mesh drawn for a camera in a pass and fade state.
struct GrassKey {
    const void* plane;   ///< the patch's mesh (Plane); identity only, not owned
    const void* ctx;     ///< the camera; identity only, not owned
    std::uint32_t variant;  ///< pass number | 4 when fading
};

/// The patch's recorded draws again: `wind` = its WORLD1 matrix (Direct3D, 16 floats), `colour` its fade colour
/// (Plane::colour_) when `fading`. False = no usable recording (draw it the normal way, recording).
bool grass_replay(const GrassKey& key, const float* wind, std::uint32_t colour, bool fading);
/// The normal draw of the patch that follows is recorded (false: not now - draw it unrecorded).
bool grass_record_begin(const GrassKey& key);
/// Plane::draw, while recording: the material colour (Material::colour) of the run it is about to draw.
void grass_record_colour(std::uint32_t material_colour);
/// Whether a draw of a patch is being recorded (between grass_record_begin and grass_record_end).
bool grass_recording();
/// Ends the recording started by grass_record_begin.
void grass_record_end();
/// The patch's mesh changed or goes: its recordings are dropped.
void grass_forget(const void* plane);
/// Replayed draws go to the GPU queued one after another (one multi-draw while the state stays the same): the caller
/// submits them before it draws anything else itself, and at the end of a run of patches.
void grass_flush();

/// The device's implementations of the grass_* functions above, installed by set_grass_hooks().
struct GrassHooks {
    bool (*replay)(const GrassKey&, const float*, std::uint32_t, bool);  ///< grass_replay
    bool (*record_begin)(const GrassKey&);   ///< grass_record_begin
    void (*record_colour)(std::uint32_t);    ///< grass_record_colour
    void (*record_end)();                    ///< grass_record_end
    void (*forget)(const void*);             ///< grass_forget
    void (*flush)();                         ///< grass_flush
};
void set_grass_hooks(const GrassHooks* hooks);  ///< installed by gld::Device (nullptr: none)

}  // namespace joan::render
