// Tests for the 3D asset loaders (src/en3d).
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "core/file_util.hpp"
#include "core/vfs.hpp"
#include "en3d/animation.hpp"
#include "en3d/asset_paths.hpp"
#include "en3d/dds.hpp"
#include "en3d/model.hpp"
#include "test.hpp"

using namespace joan;
using namespace joan::en3d;

namespace {

struct Writer {
    std::vector<std::byte> d;
    void u32(std::uint32_t v) { for (int i = 0; i < 4; ++i) d.push_back(std::byte((v >> (8 * i)) & 0xff)); }
    void u16(std::uint16_t v) { for (int i = 0; i < 2; ++i) d.push_back(std::byte((v >> (8 * i)) & 0xff)); }
    void f32(float f) {
        std::uint32_t v;
        std::memcpy(&v, &f, 4);
        u32(v);
    }
    void str(const std::string& s, std::size_t len) {
        for (std::size_t i = 0; i < len; ++i) d.push_back(std::byte(i < s.size() ? s[i] : 0));
    }
};

bool near(float a, float b, float eps = 1e-4f) { return std::fabs(a - b) <= eps; }

std::optional<std::filesystem::path> game_dir() {
    if (const char* p = std::getenv("JOAN_GAME_DIR"); p && looks_like_game_dir(p)) return std::filesystem::path(p);
    return std::nullopt;
}

}  // namespace

TEST("en3d: strip to list keeps winding and drops degenerates") {
    const std::vector<std::uint16_t> strip{0, 1, 2, 3, 3, 4};
    const auto list = strip_to_list(strip);
    // (0,1,2) (2,1,3)  then (2,3,3) and (3,3,4) are degenerate
    CHECK(list.size() == 6);
    CHECK(list[0] == 0 && list[1] == 1 && list[2] == 2);
    CHECK(list[3] == 2 && list[4] == 1 && list[5] == 3);
}

TEST("en3d: synthetic SMO round trip") {
    Writer w;
    w.u32(500); w.u32(1); w.u32(1); w.u32(0);
    for (float f : {0.f, 0.f, 0.f, 5.f}) w.f32(f);
    w.u32(1); w.u32(3);                                     // strip_n 1 -> 3 indices, 3 vertices
    for (int c = 0; c < 4; ++c) { w.f32(1); w.f32(0.5f); w.f32(0.25f); w.f32(0.125f); }  // A,R,G,B
    for (float f : {1.f, 2.f, 3.f, 4.f}) w.f32(f);          // sphere
    w.u32(0);                                               // texture 0
    w.u16(0); w.u16(1); w.u16(2);
    for (int v = 0; v < 3; ++v) {
        w.f32(0); w.f32(0); w.f32(1);                       // normal
        w.f32(float(v)); w.f32(0); w.f32(0);                // position
        w.f32(0.5f); w.f32(0.5f);                           // uv
    }
    w.str("tex.bmp", 20); w.u32(0);
    w.u32(3); w.u32(0); w.u32(0);                           // section 3 (8 bytes), ends the file
    const Model m = load_smo(w.d);
    CHECK(m.meshes.size() == 1 && m.textures.size() == 1 && !m.skinned());
    CHECK(m.meshes[0].indices.size() == 3);
    CHECK(near(m.meshes[0].material.diffuse.r, 0.5f) && near(m.meshes[0].material.diffuse.a, 1.0f));
    CHECK(near(m.meshes[0].positions[2].x, 2.0f));
    CHECK(m.textures[0].name == "tex.bmp");
    CHECK(m.sections.size() == 1 && m.sections[0].id == 3);
    w.d.resize(w.d.size() - 48);
    CHECK_THROWS(load_smo(w.d));
}

TEST("en3d: AM2 sampling interpolates between sparse keys") {
    Writer w;
    w.u32(1); w.u32(1);
    w.f32(0); w.f32(100); w.str("f_test", 54);
    w.f32(0); w.f32(100);
    w.u32(2);
    const float a[12] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
    // 90 degrees about Z, translated by (10, 0, 0)
    const float b[12] = {0, -1, 0, 10, 1, 0, 0, 0, 0, 0, 1, 0};
    w.f32(0);
    for (float f : a) w.f32(f);
    w.f32(100);
    for (float f : b) w.f32(f);
    const Animation an = load_am2(w.d);
    CHECK(an.find_action("F_TEST") == 0u);
    const Mat34 mid = an.sample(0, 50);
    CHECK(near(mid.translation().x, 5.0f));
    const float c = std::sqrt(0.5f);  // 45 degrees
    CHECK(near(mid(0, 0), c) && near(mid(1, 0), c) && near(mid(0, 1), -c));
    CHECK(near(an.sample(0, 1000).translation().x, 10.0f));  // clamped
}

TEST("en3d: DXT1 block decode") {
    Writer w;
    w.u32(0x20534444); w.u32(124); w.u32(0); w.u32(4); w.u32(4); w.u32(8); w.u32(0); w.u32(1);
    for (int i = 0; i < 11; ++i) w.u32(0);
    w.u32(32); w.u32(4); w.u32(0x31545844); for (int i = 0; i < 5; ++i) w.u32(0);
    for (int i = 0; i < 5; ++i) w.u32(0);  // caps
    w.u16(0xF800); w.u16(0x001F);          // red, blue
    w.u32(0x000000E4);                     // first row: 0,1,2,3
    const Image img = load_dds(w.d);
    CHECK(img.width == 4 && img.format == "DXT1");
    CHECK(img.rgba[0] == 255 && img.rgba[2] == 0);       // red
    CHECK(img.rgba[4] == 0 && img.rgba[6] == 255);       // blue
    CHECK(img.rgba[8] > img.rgba[12]);                   // 2/3 red vs 1/3 red
}

// ---------------------------------------------------------------------------
// Real game data
// ---------------------------------------------------------------------------

TEST("game: Joan of Arc mesh + animation pose correctly") {
    auto dir = game_dir();
    if (!dir) SKIP("JOAN_GAME_DIR not set");
    Vfs vfs(*dir);
    const Model m = load_am1(vfs.read("unit/Joan_of_Arc.AM1"));
    CHECK(m.bone_count == 24 && m.meshes.size() == 6 && m.textures.size() == 2);
    for (const Mesh& me : m.meshes) {
        // Materials are plain 0..1 colours; the 4th AM1 block is a bounding sphere, not a colour.
        CHECK(me.material.emissive.r == 0 && me.material.emissive.g == 0 && me.material.emissive.b == 0);
        CHECK(me.material.diffuse.r >= 0 && me.material.diffuse.r <= 1);
        CHECK(me.sphere_radius > 1);
    }
    const Skeleton s = load_am3(vfs.read("unit/Joan_of_Arc.AM3"));
    CHECK(s.bones.size() == 24 && s.bones[0].name == "Bip01" && s.bones[1].parent == 0);
    const Animation a = load_am2(vfs.read("anim/joan_of_arc.am2"));
    CHECK(a.bone_count == 24 && a.actions.size() == 86);
    CHECK(a.find_action("f_run").has_value());

    std::vector<Mat34> bones;
    a.sample_all(0, bones);
    float zmin = 1e9f, zmax = -1e9f, worst = 0;
    std::vector<Vec3> pos, nrm;
    for (const Mesh& me : m.meshes) {
        skin_mesh(me, bones, pos, nrm);
        for (const Vec3& p : pos) zmin = std::min(zmin, p.z), zmax = std::max(zmax, p.z);
        // Every influence of a vertex must land on the same spot in the rest pose.
        for (std::size_t v = 0; v < me.vertex_count(); ++v) {
            const auto b = me.influence_begin[v], e = me.influence_begin[v + 1];
            const Vec3 p0 = bones[me.influences[b].bone].transform_point(me.influences[b].position);
            for (auto i = b + 1; i < e; ++i)
                worst = std::max(worst, length(bones[me.influences[i].bone].transform_point(me.influences[i].position) - p0));
        }
    }
    CHECK(zmin > -1.0f && zmin < 1.0f);  // feet on the ground
    CHECK(zmax > 70.0f && zmax < 90.0f); // ~79 units tall
    CHECK(worst < 0.1f);

    TextureIndex tex(*dir);
    const auto joan_tex = tex.find(m.textures[0].name);
    CHECK(joan_tex.has_value());
    const Image img = load_dds(read_file(*joan_tex));
    CHECK(img.width == 512 && img.height == 512 && img.format == "DXT3");
}

TEST("game: every model, skeleton and animation parses") {
    auto dir = game_dir();
    if (!dir) SKIP("JOAN_GAME_DIR not set");
    int count = 0;
    for (const char* sub : {"model", "unit", "anim"}) {
        const auto d = Vfs(*dir).resolve(sub);
        if (!d) continue;
        for (const auto& e : std::filesystem::directory_iterator(*d)) {
            std::string ext = e.path().extension().string();
            for (char& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            const auto data = read_file(e.path());
            try {
                if (ext == ".am1" || ext == ".smo") (void)load_model(data, e.path().filename().string());
                else if (ext == ".am2") (void)load_am2(data);
                else if (ext == ".am3") (void)load_am3(data);
                else continue;
            } catch (const std::exception& ex) {
                throw jt::Failure{e.path().string() + ": " + ex.what()};
            }
            ++count;
        }
    }
    CHECK(count > 0);
}
