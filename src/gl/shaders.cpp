#include "shaders.hpp"

#include <cstdio>
#include <cstring>
#include <sstream>

namespace joan::gld {
namespace {

// Must match FfpUniforms / FfpDrawUniforms member for member.
const char* const kFfpBlock = R"(layout(std140, binding = 0) uniform Ffp {
    mat4 u_view;
    mat4 u_proj;
    mat4 u_texture_matrix[8];
    vec4 u_global_ambient;
    vec4 u_fog_params;
    vec4 u_fog_color;
    vec4 u_viewport;
    vec4 u_point_params;
    vec4 u_light_diffuse[8];
    vec4 u_light_specular[8];
    vec4 u_light_ambient[8];
    vec4 u_light_position[8];
    vec4 u_light_direction[8];
    vec4 u_light_attenuation[8];
    vec4 u_light_spot[8];
    vec4 u_bump_matrix[8];
    vec4 u_bump_luminance[8];
    vec4 u_canvas;
};
)";

// Block FfpDraw: every draw's record in one storage buffer (the uniform ring, bound whole at binding 3); a draw's
// base instance is the index of its record. The members keep their uniform names (u_world ...) through macros:
// FFP_DRAW is the index - gl_BaseInstance in a vertex shader, which hands it to the pixel shader as v_draw.
const char* const kFfpDrawBlock = R"(struct FfpDrawRecord {
    mat4 world[4];
    mat4 normal_matrix[4];
    vec4 material_diffuse, material_ambient, material_specular, material_emissive;
    vec4 material_power;
    vec4 texture_factor;
    vec4 alpha_ref;
};
layout(std430, binding = 3) readonly buffer FfpDraw { FfpDrawRecord ffp_draws[]; };
#define u_world ffp_draws[FFP_DRAW].world
#define u_normal_matrix ffp_draws[FFP_DRAW].normal_matrix
#define u_material_diffuse ffp_draws[FFP_DRAW].material_diffuse
#define u_material_ambient ffp_draws[FFP_DRAW].material_ambient
#define u_material_specular ffp_draws[FFP_DRAW].material_specular
#define u_material_emissive ffp_draws[FFP_DRAW].material_emissive
#define u_material_power ffp_draws[FFP_DRAW].material_power
#define u_texture_factor ffp_draws[FFP_DRAW].texture_factor
#define u_alpha_ref ffp_draws[FFP_DRAW].alpha_ref
)";
// Vertex shaders: #version 460 (gl_BaseInstance); the first line of main() is kDrawIndexOut.
const char* const kVsHead = "#version 460 core\nflat out int v_draw;\n#define FFP_DRAW gl_BaseInstance\n";
const char* const kDrawIndexOut = "    v_draw = gl_BaseInstance;\n";
const char* const kFsHead = "#version 450 core\nflat in int v_draw;\n#define FFP_DRAW v_draw\n";

const char* const kVaryingsOut = "out vec4 v_diffuse;\nout vec4 v_specular;\nout vec4 v_tex[8];\nout float v_fog;\n";
const char* const kVaryingsIn = "in vec4 v_diffuse;\nin vec4 v_specular;\nin vec4 v_tex[8];\nin float v_fog;\n";

// [Graphics] BakedShadows: the level map's placement and the lift (render/baked_shadows.cpp writes it).
const char* const kLiftBlock = R"(layout(std140, binding = 2) uniform Lift {
    vec4 u_lift;         // xy: world x, y of the level map's corner; zw: 1 / its size (world units)
    vec4 u_lift_params;  // x: lift (1 - BakedShadows), y: 1 = the map is valid
};
)";

// Half-pixel offset: Direct3D samples pixel centres at integer coordinates.
const char* const kHalfPixel =
    "    gl_Position.x += gl_Position.w / u_viewport.z;\n"
    "    gl_Position.y -= gl_Position.w / u_viewport.w;\n";
// 2D canvas: screens laid out for 1024x768 are scaled into a centred rectangle (identity otherwise).
const char* const kCanvas = "    gl_Position.xy = gl_Position.xy * u_canvas.xy + u_canvas.zw * gl_Position.w;\n";

// make_shadow_vertex_shader: replaces kHalfPixel + kCanvas.
const char* const kShadowTail =
    "    {\n"
    "        vec4 c = gl_Position;\n"
    "        vec3 v = vec3((c.x - u_sh_proj.z * c.w) / u_sh_proj.x, (c.y - u_sh_proj.w * c.w) / u_sh_proj.y, c.w);\n"
    "        gl_Position = u_sh_light * vec4(v, 1.0);\n"
    "    }\n";

int texture_coordinate_size(std::uint32_t fvf, int set) {
    switch ((fvf >> (16 + set * 2)) & 3) {
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
        default: return 1;
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// keys
// ---------------------------------------------------------------------------
std::string FragmentKey::serialize() const {
    std::string s;
    append_to(s);
    return s;
}

void FragmentKey::append_to(std::string& s) const {
    s.append(reinterpret_cast<const char*>(stages), sizeof(stages));
    s += static_cast<char>(specular_enable);
    s += static_cast<char>(fog_enable);
    s += static_cast<char>(alpha_test);
    s += static_cast<char>(alpha_func);
    s += static_cast<char>(pixel);
    if (pixel & kPixelLighting) {
        s += static_cast<char>(light_count);
        s.append(reinterpret_cast<const char*>(light_types), sizeof(light_types));
        s += static_cast<char>(light_specular);
        s += static_cast<char>(local_viewer);
    }
    if (pixel & kPixelLift) {
        s += static_cast<char>(lift_stage);
    }
}

std::string VertexKey::serialize() const {
    std::string s;
    append_to(s);
    return s;
}

void VertexKey::append_to(std::string& s) const {
    s.append(reinterpret_cast<const char*>(&fvf), sizeof(fvf));
    const std::uint8_t bytes[] = {vertex_blend,  lighting,       light_count,     specular_enable, local_viewer,
                                  normalize_normals, color_vertex, diffuse_source, specular_source, ambient_source,
                                  emissive_source, fog_mode, range_fog};
    s.append(reinterpret_cast<const char*>(bytes), sizeof(bytes));
    s.append(reinterpret_cast<const char*>(light_types), sizeof(light_types));
    s.append(reinterpret_cast<const char*>(tex), sizeof(tex));
    s += static_cast<char>(pixel);
}

// ---------------------------------------------------------------------------
// fixed-function vertex pipeline
// ---------------------------------------------------------------------------
std::string generate_ffp_vertex_shader(const VertexKey& k) {
    const std::uint32_t pos_type = k.fvf & 0x00e;
    const bool rhw = pos_type == 0x004;
    const int fvf_weights = pos_type >= 0x006 ? (static_cast<int>(pos_type) - 0x006) / 2 + 1 : 0;
    const bool has_normal = (k.fvf & 0x010) != 0;
    const bool has_psize = (k.fvf & 0x020) != 0;
    const bool has_diffuse = (k.fvf & 0x040) != 0;
    const bool has_specular = (k.fvf & 0x080) != 0;
    const int tex_sets = static_cast<int>((k.fvf >> 8) & 0xf);
    const int blend = rhw ? 0 : (k.vertex_blend <= fvf_weights ? k.vertex_blend : fvf_weights);

    std::ostringstream o;
    o << kVsHead << kFfpBlock << kFfpDrawBlock << kVaryingsOut;
    o << "layout(location = 0) in vec4 a_position;\n";
    if (fvf_weights) o << "layout(location = 1) in vec4 a_blend;\n";
    if (has_normal) o << "layout(location = 2) in vec3 a_normal;\n";
    if (has_psize) o << "layout(location = 3) in float a_psize;\n";
    if (has_diffuse) o << "layout(location = 4) in vec4 a_diffuse;\n";
    if (has_specular) o << "layout(location = 5) in vec4 a_specular;\n";
    for (int i = 0; i < tex_sets && i < 8; ++i) o << "layout(location = " << 6 + i << ") in vec4 a_tex" << i << ";\n";
    // [Graphics] PixelLighting: the surface for the pixel shader; with per-pixel lighting the
    // vertex keeps only the light-independent part (emissive, ambient) and passes the material colours on.
    const bool pix = !rhw && (k.pixel & kPixelVaryings) != 0;
    const bool pix_light = pix && k.lighting && (k.pixel & kPixelLighting) != 0;
    if (pix) o << "out vec3 v_normal;\nout vec3 v_view_pos;\n";
    if (pix_light) o << "out vec4 v_mdiffuse;\nout vec4 v_mspecular;\n";
    const bool lift = !rhw && (k.pixel & kPixelLift) != 0;
    if (lift) o << kLiftBlock << "out vec2 v_lift;\n";
    o << "\n// D3D fills missing texture coordinate components: the first with 1, the rest with 0.\n"
         "vec4 pad_coord(vec4 c, int n) {\n"
         "    if (n == 1) return vec4(c.x, 1.0, 0.0, 0.0);\n"
         "    if (n == 2) return vec4(c.xy, 1.0, 0.0);\n"
         "    if (n == 3) return vec4(c.xyz, 1.0);\n"
         "    return c;\n"
         "}\n\n";
    o << "void main() {\n" << kDrawIndexOut;
    o << "    vec4 diffuse = " << (has_diffuse ? "a_diffuse" : "vec4(1.0)") << ";\n";
    o << "    vec4 specular = " << (has_specular ? "a_specular" : "vec4(0.0)") << ";\n";
    o << "    vec3 view_pos = vec3(0.0);\n    vec3 normal = vec3(0.0);\n";

    if (rhw) {
        // Pre-transformed vertices: x, y in pixels, z depth, w = 1 / w.
        o << "    float w = a_position.w != 0.0 ? 1.0 / a_position.w : 1.0;\n"
             "    vec2 ndc = vec2((a_position.x + 0.5 - u_viewport.x) / u_viewport.z * 2.0 - 1.0,\n"
             "                    1.0 - (a_position.y + 0.5 - u_viewport.y) / u_viewport.w * 2.0);\n"
             "    gl_Position = vec4(ndc * w, a_position.z * w, w);\n";
        o << "    v_diffuse = diffuse;\n    v_specular = specular;\n";
        o << "    v_fog = " << (has_specular ? "specular.a" : "1.0") << ";\n";
    } else {
        o << "    vec4 pos = vec4(a_position.xyz, 1.0);\n";
        if (blend > 0) {
            o << "    float weights[4];\n    float last = 1.0;\n";
            for (int i = 0; i < blend; ++i) o << "    weights[" << i << "] = a_blend[" << i << "]; last -= a_blend[" << i << "];\n";
            o << "    weights[" << blend << "] = last;\n";
            o << "    vec4 world_pos = vec4(0.0);\n    vec3 n = vec3(0.0);\n";
            for (int i = 0; i <= blend; ++i) {
                o << "    world_pos += (u_world[" << i << "] * pos) * weights[" << i << "];\n";
                if (has_normal) o << "    n += (mat3(u_normal_matrix[" << i << "]) * a_normal) * weights[" << i << "];\n";
            }
            o << "    vec4 eye = u_view * world_pos;\n";
            if (has_normal) o << "    normal = n;\n";
        } else {
            o << "    vec4 world_pos = u_world[0] * pos;\n    vec4 eye = u_view * world_pos;\n";
            if (has_normal) o << "    normal = mat3(u_normal_matrix[0]) * a_normal;\n";
        }
        if (has_normal && k.normalize_normals) o << "    normal = normalize(normal);\n";
        o << "    view_pos = eye.xyz;\n";
        o << "    gl_Position = u_proj * eye;\n";
        if (lift) o << "    v_lift = (world_pos.xy - u_lift.xy) * u_lift.zw;\n";

        // Lighting (camera space).
        if (k.lighting) {
            auto source = [&](int src, const char* material) -> std::string {
                if (k.color_vertex && src == 1 && has_diffuse) return "a_diffuse";
                if (k.color_vertex && src == 2 && has_specular) return "a_specular";
                return material;
            };
            o << "    vec4 m_diffuse = " << source(k.diffuse_source, "u_material_diffuse") << ";\n";
            o << "    vec4 m_ambient = " << source(k.ambient_source, "u_material_ambient") << ";\n";
            o << "    vec4 m_specular = " << source(k.specular_source, "u_material_specular") << ";\n";
            o << "    vec4 m_emissive = " << source(k.emissive_source, "u_material_emissive") << ";\n";
            o << "    vec3 sum_ambient = vec3(0.0), sum_diffuse = vec3(0.0), sum_specular = vec3(0.0);\n";
            o << "    vec3 to_eye = " << (k.local_viewer ? "-normalize(view_pos)" : "vec3(0.0, 0.0, -1.0)") << ";\n";
            for (int i = 0; i < k.light_count && i < kMaxLights; ++i) {
                const int type = k.light_types[i];
                o << "    {\n";
                if (type == 3) {
                    o << "        vec3 L = -u_light_direction[" << i << "].xyz;\n        float atten = 1.0;\n";
                } else {
                    o << "        vec3 d = u_light_position[" << i << "].xyz - view_pos;\n"
                         "        float dist = length(d);\n"
                         "        vec3 L = dist > 0.0 ? d / dist : vec3(0.0, 0.0, 1.0);\n"
                         "        vec4 at = u_light_attenuation[" << i << "];\n"
                         "        float atten = dist > at.w ? 0.0 : 1.0 / max(at.x + at.y * dist + at.z * dist * dist, 1e-6);\n";
                    if (type == 2) {
                        o << "        float rho = dot(-L, u_light_direction[" << i << "].xyz);\n"
                             "        vec4 sp = u_light_spot[" << i << "];\n"
                             "        float spot = rho > sp.x ? 1.0 : (rho <= sp.y ? 0.0 : pow(max((rho - sp.y) / max(sp.x - sp.y, 1e-6), 0.0), sp.z));\n"
                             "        atten *= spot;\n";
                    }
                }
                if (pix_light) {  // diffuse / specular: per pixel
                    o << "        sum_ambient += atten * u_light_ambient[" << i << "].rgb;\n    }\n";
                    continue;
                }
                o << "        float ndotl = dot(normal, L);\n"
                     "        sum_ambient += atten * u_light_ambient[" << i << "].rgb;\n"
                     "        if (ndotl > 0.0) {\n"
                     "            sum_diffuse += atten * ndotl * u_light_diffuse[" << i << "].rgb;\n";
                if (k.specular_enable)
                    o << "            vec3 H = normalize(L + to_eye);\n"
                         "            float ndoth = max(dot(normal, H), 0.0);\n"
                         "            if (u_material_power.x > 0.0)\n"
                         "                sum_specular += atten * pow(ndoth, u_material_power.x) * u_light_specular[" << i << "].rgb;\n";
                o << "        }\n    }\n";
            }
            o << "    diffuse = vec4(m_emissive.rgb + m_ambient.rgb * (u_global_ambient.rgb + sum_ambient)\n"
                 "                   + m_diffuse.rgb * sum_diffuse, m_diffuse.a);\n";
            o << "    specular = vec4(m_specular.rgb * sum_specular, m_specular.a);\n";
            if (pix_light) o << "    v_mdiffuse = m_diffuse;\n    v_mspecular = m_specular;\n";
        }
        o << "    v_diffuse = clamp(diffuse, 0.0, 1.0);\n    v_specular = clamp(specular, 0.0, 1.0);\n";

        // Vertex fog.
        if (k.fog_mode) {
            o << "    float fog_dist = " << (k.range_fog ? "length(view_pos)" : "abs(view_pos.z)") << ";\n";
            if (k.fog_mode == 3) o << "    v_fog = clamp((u_fog_params.y - fog_dist) * u_fog_params.w, 0.0, 1.0);\n";
            else if (k.fog_mode == 1) o << "    v_fog = clamp(exp(-u_fog_params.z * fog_dist), 0.0, 1.0);\n";
            else o << "    v_fog = clamp(exp(-pow(u_fog_params.z * fog_dist, 2.0)), 0.0, 1.0);\n";
        } else {
            o << "    v_fog = 1.0;\n";
        }
    }

    // Texture coordinates, one per stage.
    for (int s = 0; s < kMaxStages; ++s) {
        const TexCoordConfig& t = k.tex[s];
        std::string src;
        int dims = 2;
        if (rhw || t.generate == 0) {
            if (t.index < tex_sets) {
                dims = texture_coordinate_size(k.fvf, t.index);
                src = "a_tex" + std::to_string(t.index);
            } else {
                src = "vec4(0.0, 0.0, 0.0, 1.0)";
                dims = 4;
            }
        } else if (t.generate == 1) {
            src = "vec4(normal, 1.0)";
            dims = 4;
        } else if (t.generate == 2) {
            src = "vec4(view_pos, 1.0)";
            dims = 4;
        } else {  // camera space reflection vector
            src = "vec4(reflect(normalize(view_pos), normalize(normal)), 1.0)";
            dims = 4;
        }
        if (t.transform && !rhw)
            o << "    v_tex[" << s << "] = u_texture_matrix[" << s << "] * pad_coord(" << src << ", " << dims << ");\n";
        else
            o << "    v_tex[" << s << "] = " << src << ";\n";
    }

    if (pix) o << "    v_normal = normal;\n    v_view_pos = view_pos;\n";
    o << "    gl_PointSize = " << (has_psize ? "a_psize" : "u_point_params.x") << ";\n";
    if (!rhw) o << kHalfPixel;
    o << kCanvas;
    o << "}\n";
    return o.str();
}

// ---------------------------------------------------------------------------
// fixed-function pixel pipeline
// ---------------------------------------------------------------------------
namespace {

// D3DTA_* argument (with COMPLEMENT / ALPHAREPLICATE modifiers) as a vec4 expression.
std::string stage_arg(std::uint8_t arg, int stage) {
    std::string base;
    switch (arg & 0x0f) {
        case 0: base = "diffuse_in"; break;
        case 1: base = "current"; break;
        case 2: base = "tex" + std::to_string(stage); break;
        case 3: base = "u_texture_factor"; break;
        case 4: base = "specular_in"; break;
        case 5: base = "temp"; break;
        default: base = "vec4(1.0)"; break;
    }
    if (arg & 0x20) base = "vec4(" + base + ".a)";
    if (arg & 0x10) base = "(vec4(1.0) - " + base + ")";
    return base;
}

// D3DTEXTUREOP applied to a1, a2, a0 (component selector `c` = ".rgb" or ".a").
std::string stage_op(std::uint8_t op, const std::string& a1, const std::string& a2, const std::string& a0,
                     const std::string& c, int stage) {
    const std::string x1 = a1 + c, x2 = a2 + c, x0 = a0 + c;
    const std::string tex_alpha = "tex" + std::to_string(stage) + ".a";
    switch (op) {
        case 2: return x1;
        case 3: return x2;
        case 4: return x1 + " * " + x2;
        case 5: return x1 + " * " + x2 + " * 2.0";
        case 6: return x1 + " * " + x2 + " * 4.0";
        case 7: return x1 + " + " + x2;
        case 8: return x1 + " + " + x2 + " - 0.5";
        case 9: return "(" + x1 + " + " + x2 + " - 0.5) * 2.0";
        case 10: return x1 + " - " + x2;
        case 11: return x1 + " + " + x2 + " * (1.0 - " + x1 + ")";
        case 12: return "mix(" + x2 + ", " + x1 + ", diffuse_in.a)";
        case 13: return "mix(" + x2 + ", " + x1 + ", " + tex_alpha + ")";
        case 14: return "mix(" + x2 + ", " + x1 + ", u_texture_factor.a)";
        case 15: return x1 + " + " + x2 + " * (1.0 - " + tex_alpha + ")";
        case 16: return "mix(" + x2 + ", " + x1 + ", current.a)";
        case 17: return x1;  // PREMODULATE: not emulated
        case 18: return c == ".a" ? x1 : (a1 + ".rgb + " + a1 + ".a * " + a2 + ".rgb");
        case 19: return c == ".a" ? x1 + " * " + x2 : (a1 + ".rgb * " + a2 + ".rgb + " + a1 + ".a");
        case 20: return c == ".a" ? x1 : (a1 + ".rgb + (1.0 - " + a1 + ".a) * " + a2 + ".rgb");
        case 21: return c == ".a" ? x1 : ("(1.0 - " + a1 + ".rgb) * " + a2 + ".rgb + " + a1 + ".a");
        case 24: {
            const std::string d = "dot(" + a1 + ".rgb - 0.5, " + a2 + ".rgb - 0.5) * 4.0";
            return c == ".a" ? d : "vec3(" + d + ")";
        }
        case 25: return x0 + " + " + x1 + " * " + x2;
        case 26: return "mix(" + x2 + ", " + x1 + ", " + x0 + ")";
        default: return c == ".a" ? "current.a" : "current.rgb";
    }
}

const char* compare(std::uint8_t func) {
    switch (func) {
        case 1: return "false";
        case 2: return "a < ref";
        case 3: return "a == ref";
        case 4: return "a <= ref";
        case 5: return "a > ref";
        case 6: return "a != ref";
        case 7: return "a >= ref";
        default: return "true";
    }
}

}  // namespace

std::string generate_fragment_shader(const FragmentKey& k, std::uint8_t bindless) {
    std::ostringstream o;
    if (bindless != 0) {
        // [Graphics] GrassMultiDraw: stages 0..2 sampled through bindless handles kept in the draw's record (the
        // components the program does not read: material_power.yzw, alpha_ref.yzw), so draws of different textures
        // share one multi-draw.
        o << "#version 450 core\n#extension GL_ARB_bindless_texture : require\nflat in int v_draw;\n#define FFP_DRAW v_draw\n";
    } else {
        o << kFsHead;
    }
    o << kFfpBlock << kFfpDrawBlock << kVaryingsIn;
    static const char* const kHandle[3] = {
        "floatBitsToUint(ffp_draws[FFP_DRAW].material_power.yz)",
        "uvec2(floatBitsToUint(ffp_draws[FFP_DRAW].material_power.w), floatBitsToUint(ffp_draws[FFP_DRAW].alpha_ref.y))",
        "floatBitsToUint(ffp_draws[FFP_DRAW].alpha_ref.zw)",
    };
    for (int s = 0; s < kMaxStages; ++s) {
        if (!k.stages[s].has_texture) continue;
        if (s < 3 && (bindless >> s & 1) != 0) o << "#define t" << s << " sampler2D(" << kHandle[s] << ")\n";
        else o << "layout(binding = " << s << ") uniform sampler2D t" << s << ";\n";
    }
    // Location 1: the fragment's depth, for [Graphics] AmbientOcclusion's cut-out target (render/ao_pass.hpp). It is
    // a draw buffer only while the occlusion collects the transparent pass's cut-outs; everywhere else it is dropped.
    // [Graphics] PixelLighting (render/pixel_lighting.hpp).
    const bool pix_light = (k.pixel & kPixelVaryings) != 0 && (k.pixel & kPixelLighting) != 0;
    if (pix_light) o << "in vec3 v_normal;\nin vec3 v_view_pos;\nin vec4 v_mdiffuse;\nin vec4 v_mspecular;\n";
    // [Graphics] BakedShadows: the lightmap stage's texel lifted towards the level map's lit level (render/
    // baked_shadows.hpp), fading out over the map's outer tenth and where the map has no ground.
    const bool lift = (k.pixel & kPixelLift) != 0 && k.lift_stage > 0 && k.lift_stage < kLiftUnit;
    if (lift) o << kLiftBlock << "in vec2 v_lift;\nlayout(binding = " << kLiftUnit << ") uniform sampler2D t_lift;\n";
    o << "layout(location = 0) out vec4 frag_color;\nlayout(location = 1) out float frag_cut_depth;\n\nvoid main() {\n";
    o << "    vec4 diffuse_in = v_diffuse;\n    vec4 specular_in = v_specular;\n";
    {
        if (pix_light) {
            // The Direct3D light model of the vertex shader, with this pixel's normal and position.
            o << "    vec3 P = v_view_pos;\n"
                 "    vec3 N = dot(v_normal, v_normal) > 1e-12 ? normalize(v_normal) : vec3(0.0, 0.0, -1.0);\n";
            o << "    {\n        vec3 sum_diffuse = vec3(0.0), sum_specular = vec3(0.0);\n"
              << "        vec3 to_eye = " << (k.local_viewer ? "-normalize(P)" : "vec3(0.0, 0.0, -1.0)") << ";\n";
            for (int i = 0; i < k.light_count && i < 8; ++i) {
                const int type = k.light_types[i];
                o << "        {\n";
                if (type == 3) {
                    o << "            vec3 L = -u_light_direction[" << i << "].xyz;\n            float atten = 1.0;\n";
                } else {
                    o << "            vec3 d = u_light_position[" << i << "].xyz - P;\n"
                         "            float dist = length(d);\n"
                         "            vec3 L = dist > 0.0 ? d / dist : vec3(0.0, 0.0, 1.0);\n"
                         "            vec4 at = u_light_attenuation[" << i << "];\n"
                         "            float atten = dist > at.w ? 0.0 : 1.0 / max(at.x + at.y * dist + at.z * dist * dist, 1e-6);\n";
                    if (type == 2)
                        o << "            float rho = dot(-L, u_light_direction[" << i << "].xyz);\n"
                             "            vec4 sp = u_light_spot[" << i << "];\n"
                             "            atten *= rho > sp.x ? 1.0 : (rho <= sp.y ? 0.0 : pow(max((rho - sp.y) / max(sp.x - sp.y, 1e-6), 0.0), sp.z));\n";
                }
                o << "            float ndotl = dot(N, L);\n"
                     "            if (ndotl > 0.0) {\n"
                     "                sum_diffuse += atten * ndotl * u_light_diffuse[" << i << "].rgb;\n";
                if (k.light_specular)
                    o << "                vec3 H = normalize(L + to_eye);\n"
                         "                if (u_material_power.x > 0.0)\n"
                         "                    sum_specular += atten * pow(max(dot(N, H), 0.0), u_material_power.x) * u_light_specular[" << i << "].rgb;\n";
                o << "            }\n        }\n";
            }
            o << "        diffuse_in = vec4(clamp(v_diffuse.rgb + v_mdiffuse.rgb * sum_diffuse, 0.0, 1.0), v_diffuse.a);\n"
                 "        specular_in = vec4(clamp(v_mspecular.rgb * sum_specular, 0.0, 1.0), v_specular.a);\n"
                 "    }\n";
        }
    }
    o << "    vec4 current = diffuse_in;\n    vec4 temp = vec4(0.0);\n";
    o << "    vec2 bump = vec2(0.0);\n    float bump_lum = 1.0;\n";
    for (int s = 0; s < kMaxStages; ++s) {
        const StageConfig& st = k.stages[s];
        if (st.color_op == 1) break;  // DISABLE ends the cascade
        const std::string coords = "v_tex[" + std::to_string(st.coord_source) + "]";
        // Texture lookup (with the previous stage's bump offset applied).
        if (st.has_texture) {
            std::string uv = coords + ".xy";
            if (st.projected) {
                const char* comp = st.projected_count == 4 ? "w" : st.projected_count == 3 ? "z" : "y";
                uv = "(" + coords + ".xy / " + coords + "." + comp + ")";
            }
            if (s > 0 && (k.stages[s - 1].color_op == 22 || k.stages[s - 1].color_op == 23)) uv = "(" + uv + " + bump)";
            o << "    vec4 tex" << s << " = texture(t" << s << ", " << uv << ");\n";
            if (lift && s == k.lift_stage)
                o << "    if (u_lift_params.y > 0.0) {\n"
                     "        vec4 lw = texture(t_lift, v_lift);\n"
                     "        vec2 edge = min(v_lift, 1.0 - v_lift);\n"
                     "        float w = smoothstep(0.0, 0.1, min(edge.x, edge.y)) * smoothstep(0.5, 1.0, lw.a);\n"
                     "        tex" << s << ".rgb = mix(tex" << s << ".rgb, max(tex" << s << ".rgb, lw.rgb), u_lift_params.x * w);\n"
                     "    }\n";
        } else {
            o << "    vec4 tex" << s << " = vec4(1.0);\n";
        }
        if (st.color_op == 22 || st.color_op == 23) {  // BUMPENVMAP(LUMINANCE): offsets the next stage's lookup
            o << "    {\n        vec4 m = u_bump_matrix[" << s << "];\n"
              << "        bump = vec2(m.x * tex" << s << ".r + m.z * tex" << s << ".g, m.y * tex" << s << ".r + m.w * tex" << s
              << ".g);\n";
            if (st.color_op == 23)
                o << "        bump_lum = clamp(tex" << s << ".b * u_bump_luminance[" << s << "].x + u_bump_luminance[" << s
                  << "].y, 0.0, 1.0);\n";
            o << "    }\n";
            continue;  // the colour passes through
        }
        const std::string c1 = stage_arg(st.color_arg1, s), c2 = stage_arg(st.color_arg2, s), c0 = stage_arg(st.color_arg0, s);
        const std::string a1 = stage_arg(st.alpha_arg1, s), a2 = stage_arg(st.alpha_arg2, s), a0 = stage_arg(st.alpha_arg0, s);
        o << "    {\n        vec3 rgb = clamp(" << stage_op(st.color_op, c1, c2, c0, ".rgb", s) << ", 0.0, 1.0);\n";
        if (st.color_op == 24)  // DOTPRODUCT3 replicates its result into alpha, whatever the alpha op
            o << "        float alpha = rgb.r;\n";
        else if (st.alpha_op == 1)
            o << "        float alpha = current.a;\n";
        else
            o << "        float alpha = clamp(" << stage_op(st.alpha_op, a1, a2, a0, ".a", s) << ", 0.0, 1.0);\n";
        if (s > 0 && k.stages[s - 1].color_op == 23) o << "        rgb *= bump_lum;\n";
        o << "        " << (st.result_temp ? "temp" : "current") << " = vec4(rgb, alpha);\n    }\n";
    }
    if (k.specular_enable) o << "    current.rgb = clamp(current.rgb + specular_in.rgb, 0.0, 1.0);\n";
    if (k.fog_enable) o << "    current.rgb = mix(u_fog_color.rgb, current.rgb, clamp(v_fog, 0.0, 1.0));\n";
    if (k.alpha_test) {
        o << "    float a = floor(current.a * 255.0 + 0.5);\n    float ref = u_alpha_ref.x;\n";
        o << "    if (!(" << compare(k.alpha_func) << ")) discard;\n";
    }
    o << "    frag_color = current;\n    frag_cut_depth = gl_FragCoord.z;\n}\n";
    return o.str();
}

// ---------------------------------------------------------------------------
// vs.1.x translation
// ---------------------------------------------------------------------------
namespace {

struct Operand {
    std::uint32_t token = 0;
    std::uint32_t type() const { return (token >> 28) & 7; }
    std::uint32_t number() const { return token & 0x7ff; }
};

std::string register_name(std::uint32_t type, std::uint32_t number, bool relative, std::string& error) {
    switch (type) {
        case 0: return "r" + std::to_string(number);
        case 1: return "v" + std::to_string(number);
        case 2:
            if (relative) return "c[clamp(a0 + " + std::to_string(number) + ", 0, 95)]";
            return "c[" + std::to_string(number) + "]";
        case 3: return "vec4(a0)";
        case 4:
            if (number == 0) return "o_pos";
            if (number == 1) return "o_fog";
            return "o_pts";
        case 5: return "o_d" + std::to_string(number);
        case 6: return "o_t" + std::to_string(number);
        default: error = "unknown register type " + std::to_string(type); return "vec4(0.0)";
    }
}

std::string source(std::uint32_t t, std::uint32_t number_offset, std::string& error) {
    std::string s = register_name((t >> 28) & 7, (t & 0x7ff) + number_offset, (t & 0x2000) != 0, error);
    const std::uint32_t sw = (t >> 16) & 0xff;
    if (sw != 0xe4) {
        s += '.';
        for (int i = 0; i < 4; ++i) s += "xyzw"[(sw >> (2 * i)) & 3];
    }
    const std::uint32_t mod = (t >> 24) & 0xf;
    if (mod == 1) s = "(-" + s + ")";
    return s;
}

// Scalar operand of rcp / rsq / exp / log: the replicated component (w for an identity swizzle).
std::string scalar(std::uint32_t t, std::string& error) {
    std::string base = register_name((t >> 28) & 7, t & 0x7ff, (t & 0x2000) != 0, error);
    const std::uint32_t sw = (t >> 16) & 0xff;
    const char comp = sw == 0xe4 ? 'w' : "xyzw"[sw & 3];
    std::string s = base + "." + comp;
    if (((t >> 24) & 0xf) == 1) s = "(-" + s + ")";
    return s;
}

std::string write_mask(std::uint32_t t) {
    const std::uint32_t m = (t >> 16) & 0xf;
    if (m == 0xf) return "";
    std::string s = ".";
    for (int i = 0; i < 4; ++i)
        if (m >> i & 1) s += "xyzw"[i];
    return s;
}

}  // namespace

std::string make_shadow_vertex_shader(const std::string& vs) {
    const std::string tail = std::string(kHalfPixel) + kCanvas;
    const std::size_t at = vs.find(tail);
    const std::size_t line = vs.find('\n');
    if (at == std::string::npos || line == std::string::npos || line > at) return {};
    std::string out = vs.substr(0, line + 1);  // #version
    out += "uniform vec4 u_sh_proj;\nuniform mat4 u_sh_light;\n";
    out += vs.substr(line + 1, at - line - 1);
    out += kShadowTail;
    out += vs.substr(at + tail.size());
    return out;
}

std::string translate_vertex_shader(const std::vector<std::uint32_t>& tokens, std::uint32_t input_mask,
                                    std::string& error) {
    if (tokens.empty() || (tokens[0] & 0xffff0000u) != 0xfffe0000u || ((tokens[0] >> 8) & 0xff) != 1) {
        error = "not a vs.1.x shader";
        return {};
    }
    std::ostringstream o;
    o << kVsHead << kFfpBlock << kFfpDrawBlock;
    o << "layout(std140, binding = 1) uniform VsConst { vec4 c[96]; };\n" << kVaryingsOut;
    for (int i = 0; i < 16; ++i)
        if (input_mask >> i & 1) o << "layout(location = " << i << ") in vec4 v" << i << ";\n";
    o << "\nvoid main() {\n" << kDrawIndexOut;
    for (int i = 0; i < 16; ++i)
        if (!(input_mask >> i & 1)) o << "    const vec4 v" << i << " = vec4(0.0, 0.0, 0.0, 1.0);\n";
    o << "    vec4 r0 = vec4(0.0), r1 = vec4(0.0), r2 = vec4(0.0), r3 = vec4(0.0), r4 = vec4(0.0), r5 = vec4(0.0),\n"
         "         r6 = vec4(0.0), r7 = vec4(0.0), r8 = vec4(0.0), r9 = vec4(0.0), r10 = vec4(0.0), r11 = vec4(0.0);\n"
         "    int a0 = 0;\n"
         "    vec4 o_pos = vec4(0.0, 0.0, 0.0, 1.0), o_fog = vec4(1.0), o_pts = vec4(1.0);\n"
         "    vec4 o_d0 = vec4(1.0), o_d1 = vec4(0.0);\n"
         "    vec4 o_t0 = vec4(0.0, 0.0, 0.0, 1.0), o_t1 = o_t0, o_t2 = o_t0, o_t3 = o_t0, o_t4 = o_t0, o_t5 = o_t0,\n"
         "         o_t6 = o_t0, o_t7 = o_t0;\n";

    std::size_t i = 1;
    while (i < tokens.size()) {
        const std::uint32_t t = tokens[i];
        if (t == 0x0000ffff) break;
        const std::uint32_t op = t & 0xffff;
        if (op == 0xfffe) {  // comment
            i += 1 + ((t >> 16) & 0x7fff);
            continue;
        }
        auto need = [&](std::size_t n) {
            if (i + n >= tokens.size()) {
                error = "truncated instruction";
                return false;
            }
            return true;
        };
        if (op == 81) {  // def c#, f, f, f, f
            if (!need(5)) return {};
            float v[4];
            std::memcpy(v, &tokens[i + 2], sizeof v);
            char b[160];
            std::snprintf(b, sizeof b, "    // def c%u = (%g, %g, %g, %g) (handled at creation)\n", tokens[i + 1] & 0x7ff,
                          static_cast<double>(v[0]), static_cast<double>(v[1]), static_cast<double>(v[2]),
                          static_cast<double>(v[3]));
            o << b;
            i += 6;
            continue;
        }
        int operands = 0;
        switch (op) {
            case 0: operands = 0; break;
            case 1: case 6: case 7: case 14: case 15: case 16: case 19: case 78: case 79: operands = 2; break;
            case 2: case 3: case 5: case 8: case 9: case 10: case 11: case 12: case 13: case 17:
            case 20: case 21: case 22: case 23: case 24: operands = 3; break;
            case 4: case 18: operands = 4; break;
            default: error = "unsupported vs opcode " + std::to_string(op); return {};
        }
        if (!need(static_cast<std::size_t>(operands))) return {};
        if (operands == 0) {
            ++i;
            continue;
        }
        const std::uint32_t d = tokens[i + 1];
        const std::uint32_t s0 = operands > 1 ? tokens[i + 2] : 0, s1 = operands > 2 ? tokens[i + 3] : 0,
                            s2 = operands > 3 ? tokens[i + 4] : 0;
        const std::uint32_t dtype = (d >> 28) & 7;
        const std::string dst = register_name(dtype, d & 0x7ff, false, error);
        const std::string mask = write_mask(d);
        std::string e;  // vec4 expression
        switch (op) {
            case 1:
                if (dtype == 3) {  // mov a0.x, src: floor
                    o << "    a0 = int(floor(" << source(s0, 0, error) << ".x));\n";
                    i += 1 + operands;
                    continue;
                }
                e = source(s0, 0, error);
                break;
            case 2: e = source(s0, 0, error) + " + " + source(s1, 0, error); break;
            case 3: e = source(s0, 0, error) + " - " + source(s1, 0, error); break;
            case 4: e = source(s0, 0, error) + " * " + source(s1, 0, error) + " + " + source(s2, 0, error); break;
            case 5: e = source(s0, 0, error) + " * " + source(s1, 0, error); break;
            case 6: e = "vec4(1.0 / " + scalar(s0, error) + ")"; break;
            case 7: e = "vec4(inversesqrt(abs(" + scalar(s0, error) + ")))"; break;
            case 8: e = "vec4(dot(" + source(s0, 0, error) + ".xyz, " + source(s1, 0, error) + ".xyz))"; break;
            case 9: e = "vec4(dot(" + source(s0, 0, error) + ", " + source(s1, 0, error) + "))"; break;
            case 10: e = "min(" + source(s0, 0, error) + ", " + source(s1, 0, error) + ")"; break;
            case 11: e = "max(" + source(s0, 0, error) + ", " + source(s1, 0, error) + ")"; break;
            case 12: e = "vec4(lessThan(" + source(s0, 0, error) + ", " + source(s1, 0, error) + "))"; break;
            case 13: e = "vec4(greaterThanEqual(" + source(s0, 0, error) + ", " + source(s1, 0, error) + "))"; break;
            case 14: e = "vec4(exp2(" + scalar(s0, error) + "))"; break;
            case 15: e = "vec4(log2(abs(" + scalar(s0, error) + ")))"; break;
            case 16: {
                const std::string s = source(s0, 0, error);
                e = "vec4(1.0, max(" + s + ".x, 0.0), (" + s + ".x > 0.0 && " + s + ".y > 0.0) ? pow(" + s +
                    ".y, clamp(" + s + ".w, -128.0, 128.0)) : 0.0, 1.0)";
                break;
            }
            case 17: {
                const std::string a = source(s0, 0, error), b = source(s1, 0, error);
                e = "vec4(1.0, " + a + ".y * " + b + ".y, " + a + ".z, " + b + ".w)";
                break;
            }
            case 18: e = "mix(" + source(s2, 0, error) + ", " + source(s1, 0, error) + ", " + source(s0, 0, error) + ")"; break;
            case 19: e = "fract(" + source(s0, 0, error) + ")"; break;
            case 20: case 21: case 22: case 23: case 24: {
                const int rows = op == 20 ? 4 : op == 21 ? 3 : op == 22 ? 4 : op == 23 ? 3 : 2;
                const bool three = op >= 22;
                const std::string a = source(s0, 0, error);
                e = "vec4(";
                for (int r = 0; r < 4; ++r) {
                    if (r) e += ", ";
                    if (r < rows) {
                        const std::string row = source(s1, static_cast<std::uint32_t>(r), error);
                        e += three ? "dot(" + a + ".xyz, " + row + ".xyz)" : "dot(" + a + ", " + row + ")";
                    } else {
                        e += dst + "." + "xyzw"[r];
                    }
                }
                e += ")";
                break;
            }
            case 78: {  // expp (partial precision)
                const std::string x = scalar(s0, error);
                e = "vec4(exp2(floor(" + x + ")), fract(" + x + "), exp2(" + x + "), 1.0)";
                break;
            }
            case 79: e = "vec4(log2(abs(" + scalar(s0, error) + ")))"; break;
        }
        if (!error.empty()) return {};
        o << "    " << dst << mask << " = (" << e << ")" << mask << ";\n";
        i += 1 + operands;
    }
    o << "    gl_Position = o_pos;\n" << kHalfPixel << kCanvas;
    o << "    v_diffuse = clamp(o_d0, 0.0, 1.0);\n    v_specular = clamp(o_d1, 0.0, 1.0);\n";
    o << "    v_tex[0] = o_t0; v_tex[1] = o_t1; v_tex[2] = o_t2; v_tex[3] = o_t3;\n"
         "    v_tex[4] = o_t4; v_tex[5] = o_t5; v_tex[6] = o_t6; v_tex[7] = o_t7;\n";
    o << "    v_fog = o_fog.x;\n    gl_PointSize = o_pts.x;\n}\n";
    return o.str();
}

std::string make_volume_vertex_shader() {
    std::ostringstream o;
    o << kVsHead << kFfpBlock << kFfpDrawBlock;
    o << "// The mesh's vertices as the game has them this frame (positions: the first 3 floats of u_stride).\n"
         "layout(std430, binding = 0) readonly buffer Positions { float p[]; };\n"
         "layout(location = 0) in vec4 a_ref;   // this vertex; its triangle's three (welded vertex indices)\n"
         "layout(location = 1) in float a_kind; // 0 edge, 1 open edge (always moved), 2 cap\n"
         "uniform vec4 u_volume;                // xyz: the light direction (model space), w: the extrusion length\n"
         "uniform int u_stride;                 // floats per vertex\n"
         "vec3 position(float i) {\n"
         "    int o = int(i) * u_stride;\n"
         "    return vec3(p[o], p[o + 1], p[o + 2]);\n"
         "}\n"
         "void main() {\n"
         "    // The original's test (ShadowPlane::build): n = (b - a) x (c - b) faces the light when n . dir < 1e-7.\n"
         "    vec3 a = position(a_ref.y), b = position(a_ref.z), c = position(a_ref.w);\n"
         "    vec3 e1 = b - a, e2 = c - b;\n"
         "    vec3 n = vec3(e1.y * e2.z - e1.z * e2.y, e1.z * e2.x - e1.x * e2.z, e1.x * e2.y - e1.y * e2.x);\n"
         "    bool faces = (n.x * u_volume.x + n.y * u_volume.y) + n.z * u_volume.z < 1e-7;\n"
         "    if (a_kind > 1.5 && !faces) {  // the cap: the lit triangles, not moved; the others collapse\n"
         "        gl_Position = vec4(0.0);\n"
         "        return;\n"
         "    }\n"
         "    bool moved = a_kind > 1.5 ? false : (a_kind > 0.5 || !faces);\n"
         "    vec3 v = position(a_ref.x);\n"
         "    if (moved) v += u_volume.xyz * u_volume.w;\n"
         "    gl_Position = u_proj * (u_view * (u_world[0] * vec4(v, 1.0)));\n";
    o << kHalfPixel << kCanvas << "}\n";
    return o.str();
}

std::string make_volume_fragment_shader() {
    return "#version 450 core\nlayout(location = 0) out vec4 frag_color;\nlayout(location = 1) out float frag_cut_depth;\n"
           "void main() {\n    frag_color = vec4(0.0, 0.0, 0.0, 1.0);\n    frag_cut_depth = gl_FragCoord.z;\n}\n";
}

}  // namespace joan::gld
