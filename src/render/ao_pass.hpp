// GL side of [Graphics] AmbientOcclusion (see ambient_occlusion.hpp). Owned by gl::Device;
// needs the device's context to be current.
#pragma once

#include "../gl/glapi.hpp"

namespace joan::render {

/// The GL passes of [Graphics] AmbientOcclusion: depth copy, half-resolution view distance, occlusion,
/// blur and the composite onto the scene framebuffer, in two phases (opaque world; cut-out foliage).
/// Owned by gl::Device; every call needs its GL context current. Failures disable the pass (run returns false).
class AoPass {
   public:
    /// What a pass needs to know about the frame being drawn.
    struct Input {
        gl::GLuint scene_fbo;       ///< the framebuffer the world was drawn into (colour + depth)
        int width;  ///< width in pixels
        int height;  ///< height in pixels
        int vx;  ///< viewport left edge, pixels
        int vy;  ///< viewport top edge, pixels (top row = 0, as in Direct3D)
        int vw;  ///< viewport width, pixels
        int vh;  ///< viewport height, pixels
        float depth_min;  ///< viewport depth range minimum, 0..1
        float depth_max;  ///< viewport depth range maximum, 0..1
        const float* projection;    ///< Direct3D projection matrix (16 floats, row-major); not owned
    };
    /// Phase 1 (opaque world drawn): darkens the scene framebuffer with the occlusion of its
    /// depth and keeps that depth for phase 2. Leaves the GL framebuffer binding, viewport,
    /// scissor, raster state, program and texture units 0..2 changed (the caller restores what
    /// it needs). False = nothing was done (no phase 2 for this camera).
    bool run(const Input& in);

    /// Phase 2 support, after a successful run(): attaches an R32F target (multisampled with
    /// `samples` > 0, like the scene's) to the scene framebuffer as colour attachment 1, cleared
    /// to 1.0 (far), and makes it a draw buffer. Every generated fragment shader writes its depth
    /// at location 1 (gl/shaders.cpp); cutout_draw_state() lets only the transparent pass's
    /// alpha-tested, depth-writing draws (foliage the game puts into pass 2) reach it, with MIN
    /// blending, so it ends up holding the nearest cut-out depth in front of the scene's depth.
    /// (Earlier versions repeated each such draw into a separate depth framebuffer: two
    /// framebuffer switches and a second draw per grass patch.) Leaves the scissor test off and
    /// buffer 1's colour mask set (the next draw re-applies its raster state). False = unavailable.
    bool begin_cutout(gl::GLuint scene_fbo, int samples);
    /// Before each draw into the scene framebuffer between begin_cutout() and end_cutout(), after
    /// the draw's raster state is applied: buffer 1 is written (MIN blend) only for a cut-out draw.
    static void cutout_draw_state(bool cutout);
    /// Ends phase 2's collection: the scene framebuffer gets back its single draw buffer and
    /// loses attachment 1. Must come before run_cutout() and before the frame is presented.
    void end_cutout();
    /// Phase 2 (transparent world drawn): occlusion of the cut-out geometry (and the extra
    /// occlusion it casts on the opaque world) from the cut-out depth, multiplied onto pixels
    /// where that geometry is still the nearest depth-writing surface. Same `in` as run().
    /// Leaves the same state changed as run(), plus texture units 3..4.
    void run_cutout(const Input& in);

    /// Frees the GL objects (context current).
    void shutdown();

   private:
    bool ensure_programs();
    bool ensure_targets(int width, int height);
    void release_targets();
    void blur_and_composite(const Input& in, gl::GLuint z_tex, gl::GLuint composite_program);

    bool failed_ = false;
    gl::GLuint vao_ = 0;
    gl::GLuint prog_linear_ = 0, prog_ao_ = 0, prog_blur_ = 0, prog_composite_ = 0;
    gl::GLuint prog_linear_cut_ = 0, prog_ao_cut_ = 0, prog_composite_cut_ = 0;
    gl::GLuint prog_cut_resolve_[2] = {};  // cut-out target -> cut_tex_: [0] single-sampled, [1] multisampled
    int w_ = 0, h_ = 0, hw_ = 0, hh_ = 0;
    gl::GLuint depth_tex_ = 0, depth_fbo_ = 0;  // full resolution depth copy (phase 1: opaque world)
    gl::GLuint z_tex_ = 0, z_fbo_ = 0;          // half resolution view distance (R32F)
    gl::GLuint ao_tex_[2] = {}, ao_fbo_[2] = {};  // half resolution occlusion (R8), ping-pong
    // Phase 2: the cut-out target attached to the scene framebuffer (R32F window depth, 1.0 =
    // none; multisampled like the scene), the opaque + cut-out depth resolved from it (R32F), the
    // scene's depth after the transparent pass, and the half resolution (view distance, "cut-out
    // pixel") pair.
    gl::GLuint cutz_tex_ = 0;
    int cutz_samples_ = -1;
    gl::GLuint cut_attached_fbo_ = 0;  // the scene framebuffer cutz_tex_ is attached to (0 = none)
    gl::GLuint cut_tex_ = 0, cut_fbo_ = 0;
    gl::GLuint final_tex_ = 0, final_fbo_ = 0;
    gl::GLuint zc_tex_ = 0, zc_fbo_ = 0;  // RG32F
    bool cut_ready_ = false;              // cutz_tex_ was cleared for the last run()
    bool cut_failed_ = false;             // the scene framebuffer rejected attachment 1: no phase 2
    unsigned frames_ = 0, cutout_frames_ = 0;
    float logged_near_ = 0.0f, logged_far_ = 0.0f;
    int log_count_ = 0;
};

}  // namespace joan::render
