// GLSL generation for the Direct3D 8 pipeline on OpenGL 4.6:
//
//  * the fixed-function vertex pipeline (transform, vertex blending, lighting,
//    vertex fog, texture coordinate generation / transformation), generated per
//    state combination (VertexKey);
//  * the fixed-function pixel pipeline (texture stages, specular add, fog,
//    alpha test), generated per state combination (FragmentKey);
//  * translation of vs.1.0 / vs.1.1 vertex shader byte code.
//
// Conventions shared by all generated shaders:
//  * vertex attributes: FFP 0 position, 1 blend weights, 2 normal, 3 point
//    size, 4 diffuse, 5 specular, 6-13 texture coordinates; vertex shaders use
//    location = input register vN;
//  * varyings v_diffuse, v_specular, v_tex[8], v_fog;
//  * uniform blocks `Ffp` (binding 0, layout FfpUniforms), `FfpDraw` (binding 3, FfpDrawUniforms) and, for vertex
//    shaders, block `VsConst` (binding 1, 96 vec4 constants);
//  * texture stage N samples `layout(binding = N) uniform sampler2D tN`;
//  * clip space is Direct3D's (z in [0, w], glClipControl(UPPER_LEFT,
//    ZERO_TO_ONE)), with the half-pixel offset applied in the vertex shader.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

/// The Direct3D 8 device on OpenGL 4.6: GLSL generation, surface formats, adapter queries and the device itself.
namespace joan::gld {

/// Number of Direct3D 8 texture stages the generated shaders handle.
inline constexpr int kMaxStages = 8;
/// Number of lights the generated vertex shaders handle.
inline constexpr int kMaxLights = 8;
/// Number of vertex shader constant registers (c0..c95).
inline constexpr int kVsConstants = 96;

// --- uniform blocks (std140; every member is a vec4 or mat4) -----------------------
// Block Ffp (binding 0): the state many draws share (camera, lights, fog, texture matrices), uploaded only
// when it changes.
/// Contents of uniform block `Ffp` (binding 0, std140): camera, lights, fog and texture matrices, uploaded by
/// gld::Device when one of them changes. Matrices are 16 floats in the same layout as FfpDrawUniforms::world.
struct FfpUniforms {
    float view[16];  ///< View matrix.
    float proj[16];  ///< Projection matrix.
    float texture_matrix[kMaxStages][16];  ///< Texture transform matrix of each stage.
    float global_ambient[4];  ///< Global ambient colour, RGBA.
    float fog_params[4];       ///< start, end, density, 1 / (end - start)
    float fog_color[4];  ///< Fog colour, RGBA.
    float viewport[4];         ///< x, y, width, height (pixels)
    float point_params[4];     ///< size, min, max, -
    float light_diffuse[kMaxLights][4];  ///< Per light: diffuse colour, RGBA.
    float light_specular[kMaxLights][4];  ///< Per light: specular colour, RGBA.
    float light_ambient[kMaxLights][4];  ///< Per light: ambient colour, RGBA.
    float light_position[kMaxLights][4];   ///< view space
    float light_direction[kMaxLights][4];  ///< view space, normalised, direction the light travels
    float light_attenuation[kMaxLights][4];  ///< a0, a1, a2, range
    float light_spot[kMaxLights][4];         ///< cos(theta / 2), cos(phi / 2), falloff, -
    float bump_matrix[kMaxStages][4];        ///< m00, m01, m10, m11
    float bump_luminance[kMaxStages][4];     ///< scale, offset
    float canvas[4];                         ///< clip xy scale, clip xy offset (* w)
};
// Block FfpDraw (binding 3): what changes from draw to draw (the object's matrices and material).
/// Contents of uniform block `FfpDraw` (binding 3, std140): what changes between draws (matrices, material, texture
/// factor, alpha reference).
struct FfpDrawUniforms {
    float world[4][16];       ///< WORLD..WORLD3 (vertex blending), D3D row-major = GLSL column-major transposed
    float normal_matrix[4][16];  ///< inverse transpose of (world_i * view), upper 3x3 used
    float material_diffuse[4] /**< material diffuse colour, RGBA */, material_ambient[4] /**< material ambient colour */,
        material_specular[4] /**< material specular colour */, material_emissive[4] /**< material emissive colour */;
    float material_power[4];   ///< specular power in x; y, z, w carry bindless texture handles of stages 0..1 when generate_fragment_shader() is asked for `bindless`
    float texture_factor[4];  ///< D3DRS_TEXTUREFACTOR colour, RGBA.
    float alpha_ref[4];        ///< x = alpha test reference (0..255); y, z, w carry bindless texture handles when `bindless` is on
};

// [Graphics] PixelLighting (render/pixel_lighting.hpp): VertexKey::pixel / FragmentKey::pixel bits.
inline constexpr std::uint8_t kPixelVaryings = 1;  ///< v_normal / v_view_pos passed to the pixel shader
inline constexpr std::uint8_t kPixelLighting = 2;  ///< the lights evaluated per pixel (v_mdiffuse / v_mspecular)
// [Graphics] BakedShadows (render/baked_shadows.hpp): the ground lightmap stage FragmentKey::lift_stage lifted
// towards the lit level of the level map (block Lift, unit kLiftUnit; v_lift = the map position).
/// Bit of VertexKey::pixel / FragmentKey::pixel: see the comment above ([Graphics] BakedShadows lift of the ground lightmap stage).
inline constexpr std::uint8_t kPixelLift = 4;
/// Unit of the lift amount (block Lift): 7.
inline constexpr int kLiftUnit = 7;

// --- keys --------------------------------------------------------------------------------
/// One texture stage of FragmentKey, in Direct3D 8 texture-stage-state terms (D3DTSS_*). Defaults: colour op 1
/// (D3DTOP_DISABLE).
struct StageConfig {
    std::uint8_t color_op = 1 /**< D3DTSS_COLOROP */, color_arg1 = 2 /**< D3DTSS_COLORARG1 (D3DTA_*; 2 = texture) */,
                 color_arg2 = 1 /**< D3DTSS_COLORARG2 (1 = current) */, color_arg0 = 1 /**< D3DTSS_COLORARG0 (three-argument ops) */;
    std::uint8_t alpha_op = 1 /**< D3DTSS_ALPHAOP */, alpha_arg1 = 2 /**< D3DTSS_ALPHAARG1 */,
                 alpha_arg2 = 1 /**< D3DTSS_ALPHAARG2 */, alpha_arg0 = 1 /**< D3DTSS_ALPHAARG0 */;
    std::uint8_t result_temp = 0;   ///< RESULTARG == TEMP
    std::uint8_t has_texture = 0;  ///< Non-zero when a texture is bound to the stage.
    std::uint8_t coord_source = 0;  ///< which v_tex[] the stage reads
    std::uint8_t projected = 0;     ///< divide by the last coordinate
    std::uint8_t projected_count = 0;  ///< Number of coordinates divided by when `projected` (D3DTTFF_COUNTn). TODO(docs): confirm against shaders.cpp.
};

/// State combination the fixed-function pixel shader is generated for; its serialised form is the program cache key.
struct FragmentKey {
    StageConfig stages[kMaxStages];  ///< The texture stages in order.
    std::uint8_t specular_enable = 0;  ///< D3DRS_SPECULARENABLE: add the specular colour.
    std::uint8_t fog_enable = 0;  ///< D3DRS_FOGENABLE: apply fog.
    std::uint8_t alpha_test = 0;  ///< D3DRS_ALPHATESTENABLE.
    std::uint8_t alpha_func = 8;  ///< D3DCMPFUNC
    std::uint8_t pixel = 0;       ///< kPixel* bits (the vertex shader's VertexKey::pixel)
    std::uint8_t light_count = 0;  ///< kPixelLighting: the vertex key's lights
    std::uint8_t light_types[8] = {};  ///< Light type of each of the first `light_count` lights (D3DLIGHTTYPE), for per-pixel lighting.
    std::uint8_t light_specular = 0 /**< per-pixel lighting evaluates specular */, local_viewer = 1 /**< D3DRS_LOCALVIEWER */;
    std::uint8_t lift_stage = 0;  ///< kPixelLift: the lightmap stage (1..6)
    /// The key's fields as a byte string.
    std::string serialize() const;
    void append_to(std::string& s) const;  ///< serialize() appended to `s` (no allocation once `s` has grown)
};

/// Texture coordinate source and transform of one stage (D3DTSS_TEXCOORDINDEX and D3DTSS_TEXTURETRANSFORMFLAGS), part
/// of VertexKey.
struct TexCoordConfig {
    std::uint8_t index = 0;        ///< TEXCOORDINDEX & 0xffff
    std::uint8_t generate = 0;     ///< TEXCOORDINDEX >> 16: 0 passthrough, 1 normal, 2 position, 3 reflection
    std::uint8_t transform = 0;    ///< TEXTURETRANSFORMFLAGS & 0xff (0 = off, 1..4 = COUNTn)
};

/// State combination the fixed-function vertex shader is generated for; its serialised form is the program cache key.
struct VertexKey {
    std::uint32_t fvf = 0;  ///< Vertex format (D3DFVF_*) of the bound stream.
    std::uint8_t vertex_blend = 0;   ///< number of weights used (D3DRS_VERTEXBLEND 0..3)
    std::uint8_t lighting = 0;  ///< D3DRS_LIGHTING.
    std::uint8_t light_count = 0;  ///< Number of enabled lights used.
    std::uint8_t light_types[kMaxLights] = {};  ///< Light type (D3DLIGHTTYPE) of each of the first `light_count` lights.
    std::uint8_t specular_enable = 0;  ///< D3DRS_SPECULARENABLE.
    std::uint8_t local_viewer = 1;  ///< D3DRS_LOCALVIEWER.
    std::uint8_t normalize_normals = 0;  ///< D3DRS_NORMALIZENORMALS.
    std::uint8_t color_vertex = 1;  ///< D3DRS_COLORVERTEX: material sources may come from the vertex colours.
    std::uint8_t diffuse_source = 1 /**< D3DRS_DIFFUSEMATERIALSOURCE */, specular_source = 2 /**< D3DRS_SPECULARMATERIALSOURCE */,
                 ambient_source = 0 /**< D3DRS_AMBIENTMATERIALSOURCE */, emissive_source = 0 /**< D3DRS_EMISSIVEMATERIALSOURCE */;
    std::uint8_t fog_mode = 0;       ///< vertex fog: 0 none, 1 exp, 2 exp2, 3 linear
    std::uint8_t range_fog = 0;  ///< D3DRS_RANGEFOGENABLE.
    TexCoordConfig tex[kMaxStages];  ///< Per-stage texture coordinate configuration.
    std::uint8_t pixel = 0;          ///< kPixelVaryings / kPixelLighting
    /// The key's fields as a byte string.
    std::string serialize() const;
    void append_to(std::string& s) const;  ///< serialize() appended to `s` (no allocation once `s` has grown)
};

/// GLSL vertex shader of the fixed-function pipeline for the state combination `key`.
std::string generate_ffp_vertex_shader(const VertexKey& key);
/// `bindless`: stages (bits 0..2) sampled through handles in the draw record (material_power.yzw, alpha_ref.yzw: stage 0
/// in .yz of material_power, 1 in its .w + alpha_ref.y, 2 in alpha_ref.zw), for GL_ARB_bindless_texture.
std::string generate_fragment_shader(const FragmentKey& key, std::uint8_t bindless = 0);

/// Translates vs.1.0 / vs.1.1 byte code. `inputs` lists the input registers
/// declared by the shader's vertex declaration. Returns an empty string and
/// sets `error` if the byte code contains something unsupported.
std::string translate_vertex_shader(const std::vector<std::uint32_t>& tokens, std::uint32_t input_mask,
                                    std::string& error);

/// [Graphics] Shadows=2 (render/shadow_map.hpp): the shadow-map version of a vertex shader made by
/// generate_ffp_vertex_shader / translate_vertex_shader for a 3D draw. Instead of the half-pixel offset
/// and canvas transform, the camera's clip position is turned back into its view-space position (from
/// x, y and w and the camera projection `u_sh_proj` = P11, P22, P31, P32 - the depth row is not needed,
/// which keeps it exact with distant far planes) and taken into the light's clip space by `u_sh_light`.
/// Empty for a shader without that tail (pretransformed 2D vertices).
std::string make_shadow_vertex_shader(const std::string& vs);
/// [Graphics] GpuShadowVolumes (render/shadow_volumes.hpp): the stencil shadow volume of a mesh prepared by
/// render::ShadowVolumes, extruded per vertex along `u_volume` (attributes: vertex indices + kind; positions from
/// shader storage buffer 0).
std::string make_volume_vertex_shader();
/// GLSL fragment shader for the stencil shadow volumes (see make_volume_vertex_shader).
std::string make_volume_fragment_shader();

}  // namespace joan::gld
