// jmodel - inspect and convert the game's 3D assets.
//
//   jmodel <file.AM1|file.SMO|file.AM2|file.AM3|file.dds>       print a summary
//   jmodel <model> --obj out.obj [--anim file.AM2] [--action name] [--time ms]
//                                                                 export a (posed) mesh as Wavefront OBJ
//
// Skinned AM1 meshes have no bind pose of their own: without --anim they are
// exported with every bone at identity, which collapses the mesh; pass the
// unit's AM2 (anim/<name>.am2) to get the actual pose.
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include "core/file_util.hpp"
#include "en3d/animation.hpp"
#include "en3d/asset_paths.hpp"
#include "en3d/dds.hpp"
#include "en3d/model.hpp"

using namespace joan;
using namespace joan::en3d;

namespace {

void print_model(const Model& m) {
    std::printf("version %u, %s, %u bones, %zu meshes, %zu textures, bounding sphere (%.2f %.2f %.2f) r=%.2f\n",
                m.version, m.skinned() ? "skinned" : "static", m.bone_count, m.meshes.size(), m.textures.size(),
                m.sphere_center.x, m.sphere_center.y, m.sphere_center.z, m.sphere_radius);
    for (std::size_t i = 0; i < m.meshes.size(); ++i) {
        const Mesh& me = m.meshes[i];
        std::size_t max_inf = 0;
        for (std::size_t v = 0; v + 1 < me.influence_begin.size(); ++v)
            max_inf = std::max<std::size_t>(max_inf, me.influence_begin[v + 1] - me.influence_begin[v]);
        std::printf("  mesh %zu: %zu vertices, %zu triangles, texture %d, diffuse (%.2f %.2f %.2f %.2f)", i,
                    me.vertex_count(), me.indices.size() / 3, me.texture, me.material.diffuse.r, me.material.diffuse.g,
                    me.material.diffuse.b, me.material.diffuse.a);
        if (me.skinned()) std::printf(", up to %zu bones per vertex", max_inf);
        std::printf("\n");
    }
    for (std::size_t i = 0; i < m.textures.size(); ++i)
        std::printf("  texture %zu: %s%s\n", i, m.textures[i].name.c_str(),
                    m.textures[i].embedded.empty() ? "" : " (embedded)");
    for (const auto& s : m.sections) std::printf("  section %u: %zu bytes\n", s.id, s.data.size());
}

void print_anim(const Animation& a) {
    std::size_t keys = 0;
    for (const auto& t : a.tracks) keys += t.size();
    std::printf("%u bones, %zu actions, %zu keys, timeline %.1f ms (tail %.1f..%.1f)%s\n", a.bone_count,
                a.actions.size(), keys, a.tail_end_ms, a.tail_start_ms, a.tail_end_ms,
                a.trailer.empty() ? "" : ", has trailer");
    for (const auto& act : a.actions)
        std::printf("  %-28s %10.1f .. %10.1f ms\n", act.name.c_str(), act.start_ms, act.end_ms);
}

void print_skeleton(const Skeleton& s) {
    std::printf("%zu bones\n", s.bones.size());
    for (std::size_t i = 0; i < s.bones.size(); ++i) {
        int depth = 0;
        for (auto p = s.bones[i].parent; p >= 0; p = s.bones[static_cast<std::size_t>(p)].parent) ++depth;
        std::printf("  %2zu %*s%s\n", i, depth * 2, "", s.bones[i].name.c_str());
    }
}

void export_obj(const Model& m, const std::vector<Mat34>& bones, const std::filesystem::path& out) {
    std::ofstream f(out);
    if (!f) throw std::runtime_error("cannot write " + out.string());
    f << "# exported by jmodel (joan-re)\n";
    std::size_t base = 1;
    std::vector<Vec3> pos, nrm;
    for (std::size_t i = 0; i < m.meshes.size(); ++i) {
        const Mesh& me = m.meshes[i];
        skin_mesh(me, bones, pos, nrm);
        f << "o mesh" << i << "\n";
        for (const auto& p : pos) f << "v " << p.x << ' ' << p.y << ' ' << p.z << "\n";
        for (const auto& t : me.uv) f << "vt " << t.x << ' ' << 1.0f - t.y << "\n";
        for (const auto& n : nrm) f << "vn " << n.x << ' ' << n.y << ' ' << n.z << "\n";
        for (std::size_t t = 0; t + 2 < me.indices.size(); t += 3) {
            f << 'f';
            for (int k = 0; k < 3; ++k) {
                const std::size_t v = base + me.indices[t + static_cast<std::size_t>(k)];
                f << ' ' << v << '/' << v << '/' << v;
            }
            f << "\n";
        }
        base += pos.size();
    }
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr,
                     "usage: jmodel <file.AM1|.SMO|.AM2|.AM3|.dds>\n"
                     "       jmodel <model> --obj out.obj [--anim file.AM2] [--action name] [--time ms]\n");
        return 2;
    }
    try {
        const std::filesystem::path in = argv[1];
        std::optional<std::filesystem::path> obj, anim_path;
        std::string action;
        float time_ms = 0;
        for (int i = 2; i + 1 < argc; i += 2) {
            if (!std::strcmp(argv[i], "--obj")) obj = argv[i + 1];
            else if (!std::strcmp(argv[i], "--anim")) anim_path = argv[i + 1];
            else if (!std::strcmp(argv[i], "--action")) action = argv[i + 1];
            else if (!std::strcmp(argv[i], "--time")) time_ms = std::stof(argv[i + 1]);
        }
        const auto data = read_file(in);
        const std::string ext = in.extension().string();
        std::string low;
        for (char c : ext) low += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        if (low == ".am2") {
            print_anim(load_am2(data));
        } else if (low == ".am3") {
            print_skeleton(load_am3(data));
        } else if (low == ".dds") {
            const Image img = load_dds(data);
            std::printf("%ux%u %s, %u mip levels\n", img.width, img.height, img.format.c_str(), img.mip_count);
        } else {
            const Model m = load_model(data, in.filename().string());
            print_model(m);
            if (obj) {
                std::vector<Mat34> bones(m.bone_count);
                if (anim_path) {
                    const Animation a = load_am2(read_file(*anim_path));
                    if (a.bone_count != m.bone_count)
                        std::fprintf(stderr, "warning: animation has %u bones, model %u\n", a.bone_count, m.bone_count);
                    if (!action.empty()) {
                        const auto idx = a.find_action(action);
                        if (!idx) throw std::runtime_error("no action named " + action);
                        time_ms += a.actions[*idx].start_ms;
                    }
                    a.sample_all(time_ms, bones);
                    bones.resize(m.bone_count);
                }
                export_obj(m, bones, *obj);
                std::printf("wrote %s\n", obj->string().c_str());
            }
        }
        return 0;
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "error: %s\n", ex.what());
        return 1;
    }
}
