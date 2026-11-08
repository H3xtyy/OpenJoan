// jview - SDL3 / OpenGL 3.3 viewer for the game's models and animations.
//
//   jview [--game-dir <folder with joan.exe>] [model] [--anim file.AM2] [--action name]
//
// `model` is a path relative to the game folder (default unit/Joan_of_Arc.AM1)
// or an absolute path. The animation is found automatically (anim/<model>.am2,
// else the first AM2 in anim/ with the same bone count).
//
// Controls:
//   left mouse drag  orbit            mouse wheel        zoom
//   right mouse drag pan              Home               reset camera
//   Left / Right     previous / next action
//   Up / Down        play speed x2 / x0.5      Space    pause / resume
//   , / .            step one frame (while paused)
//   PageUp / PageDown previous / next model in model/, unit/, anim/
//   W wireframe   B bones   G ground grid   F11 / Alt+Enter full screen   Esc quit
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "core/file_util.hpp"
#include "core/log.hpp"
#include "core/vfs.hpp"
#include "en3d/animation.hpp"
#include "en3d/asset_paths.hpp"
#include "en3d/model.hpp"
#include "platform/window.hpp"
#include "render/gl.hpp"
#include "render/model_renderer.hpp"

using namespace joan;
using namespace joan::en3d;
namespace fs = std::filesystem;

namespace {

constexpr float kPi = 3.14159265358979f;

std::string lower(std::string s) {
    for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

std::optional<fs::path> find_game_dir(const std::optional<fs::path>& arg) {
    if (arg) return arg;
    if (const char* env = std::getenv("JOAN_GAME_DIR")) return fs::path(env);
    for (fs::path p : {fs::path(platform::executable_dir()), fs::current_path()}) {
        // Also accept running from a build folder inside the game folder (joan-re/build/...).
        for (int up = 0; up < 10 && !p.empty(); ++up, p = p.parent_path()) {
            if (looks_like_game_dir(p)) return p;
            if (p == p.parent_path()) break;
        }
    }
    return std::nullopt;
}

std::vector<fs::path> list_models(const fs::path& game_dir) {
    std::vector<fs::path> out;
    std::error_code ec;
    for (const auto& dir : fs::directory_iterator(game_dir, ec)) {
        const std::string name = lower(dir.path().filename().string());
        if (!dir.is_directory(ec) || (name != "model" && name != "unit" && name != "anim")) continue;
        for (const auto& e : fs::directory_iterator(dir.path(), ec)) {
            const std::string ext = lower(e.path().extension().string());
            if (ext == ".am1" || ext == ".smo") out.push_back(e.path());
        }
    }
    std::sort(out.begin(), out.end(), [](const fs::path& a, const fs::path& b) {
        return lower(a.parent_path().filename().string() + "/" + a.filename().string()) <
               lower(b.parent_path().filename().string() + "/" + b.filename().string());
    });
    return out;
}

std::uint32_t am2_bone_count(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    unsigned char b[4] = {};
    f.read(reinterpret_cast<char*>(b), 4);
    return f ? static_cast<std::uint32_t>(b[0] | b[1] << 8 | b[2] << 16 | b[3] << 24) : 0;
}

std::optional<fs::path> guess_animation(const fs::path& game_dir, const fs::path& model, std::uint32_t bones) {
    if (auto p = find_animation(game_dir, model)) return p;
    std::error_code ec;
    for (const auto& dir : fs::directory_iterator(game_dir, ec)) {
        if (!dir.is_directory(ec) || lower(dir.path().filename().string()) != "anim") continue;
        std::vector<fs::path> candidates;
        for (const auto& e : fs::directory_iterator(dir.path(), ec))
            if (lower(e.path().extension().string()) == ".am2" && am2_bone_count(e.path()) == bones)
                candidates.push_back(e.path());
        std::sort(candidates.begin(), candidates.end());
        if (!candidates.empty()) return candidates.front();
    }
    return std::nullopt;
}

struct Scene {
    fs::path model_path;
    Model model;
    std::optional<Skeleton> skeleton;
    std::optional<Animation> anim;
    fs::path anim_path;
    std::unique_ptr<render::ModelRenderer> renderer;
    std::size_t action = 0;
    float action_time = 0;  // ms since the start of the action
};

struct Camera {
    Vec3 target;
    float distance = 150;
    float yaw = 0.6f;    // radians around +Z, 0 = looking at the model's front (-Y side)
    float pitch = 0.25f;
    float base_distance = 150;

    [[nodiscard]] Vec3 eye() const {
        const float cp = std::cos(pitch);
        return target + Vec3{std::sin(yaw) * cp, -std::cos(yaw) * cp, std::sin(pitch)} * distance;
    }
};

std::unique_ptr<Scene> load_scene(const fs::path& game_dir, const fs::path& model_path,
                                  const std::optional<fs::path>& anim_override, const TextureIndex& textures) {
    auto s = std::make_unique<Scene>();
    s->model_path = model_path;
    s->model = load_model(read_file(model_path), model_path.filename().string());
    if (s->model.skinned()) {
        if (auto am3 = find_sibling(model_path, ".am3")) s->skeleton = load_am3(read_file(*am3));
        if (auto am2 = anim_override ? anim_override : guess_animation(game_dir, model_path, s->model.bone_count)) {
            s->anim = load_am2(read_file(*am2));
            s->anim_path = *am2;
            if (s->anim->bone_count != s->model.bone_count)
                log::warn("{} has {} bones but the model has {}", am2->filename().string(), s->anim->bone_count,
                          s->model.bone_count);
        } else {
            log::warn("no animation found for {}; showing the identity pose", model_path.filename().string());
        }
    }
    s->renderer = std::make_unique<render::ModelRenderer>(s->model, textures);
    if (s->anim) {
        // Start on something that moves: the first action other than the whole-timeline "default".
        for (std::size_t i = 0; i < s->anim->actions.size(); ++i)
            if (lower(s->anim->actions[i].name) != "default") {
                s->action = i;
                break;
            }
    }
    log::info("{}: {} meshes, {} bones{}", model_path.filename().string(), s->model.meshes.size(), s->model.bone_count,
              s->anim ? std::format(", animation {} ({} actions)", s->anim_path.filename().string(),
                                    s->anim->actions.size())
                      : std::string());
    return s;
}

void pose(const Scene& s, std::vector<Mat34>& bones) {
    bones.assign(s.model.bone_count, Mat34{});
    if (!s.anim) return;
    const Action& a = s.anim->actions[s.action];
    std::vector<Mat34> sampled;
    s.anim->sample_all(a.start_ms + s.action_time, sampled);
    for (std::size_t i = 0; i < std::min(bones.size(), sampled.size()); ++i) bones[i] = sampled[i];
}

void frame_camera(Camera& cam, const Scene& s, const std::vector<Mat34>& bones) {
    s.renderer->update_pose(bones);
    const Vec3 lo = s.renderer->bounds_min(), hi = s.renderer->bounds_max();
    cam.target = (lo + hi) * 0.5f;
    const float radius = std::max(1.0f, length(hi - lo) * 0.5f);
    cam.base_distance = cam.distance = radius * 2.6f;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        std::optional<fs::path> game_arg, model_arg, anim_arg;
        std::string action_arg;
        for (int i = 1; i < argc; ++i) {
            const std::string a = argv[i];
            if (a == "--game-dir" && i + 1 < argc) game_arg = argv[++i];
            else if (a == "--anim" && i + 1 < argc) anim_arg = argv[++i];
            else if (a == "--action" && i + 1 < argc) action_arg = argv[++i];
            else if (a == "--help" || a == "-h") {
                std::printf("usage: jview [--game-dir DIR] [model] [--anim file.AM2] [--action name]\n");
                return 0;
            } else model_arg = a;
        }
        const auto game_dir = find_game_dir(game_arg);
        if (!game_dir || !looks_like_game_dir(*game_dir)) {
            log::error("game data not found: pass --game-dir <folder containing joan.exe> or set JOAN_GAME_DIR");
            return 1;
        }
        Vfs vfs(*game_dir);
        fs::path model_path;
        const std::string wanted = model_arg ? model_arg->string() : "unit/Joan_of_Arc.AM1";
        if (fs::path(wanted).is_absolute() && fs::exists(wanted)) model_path = wanted;
        else if (auto p = vfs.resolve(wanted)) model_path = *p;
        else if (fs::exists(wanted)) model_path = wanted;
        else throw std::runtime_error("model not found: " + wanted);
        if (anim_arg && !fs::exists(*anim_arg))
            if (auto p = vfs.resolve(anim_arg->string())) anim_arg = *p;

        const TextureIndex textures(*game_dir);
        log::info("game data: {} ({} textures indexed)", game_dir->string(), textures.size());

        platform::WindowConfig wc;
        wc.title = "jview";
        platform::Window window(wc);
        const char* missing = nullptr;
        if (!gl::load([](const char* n) { return reinterpret_cast<void*>(SDL_GL_GetProcAddress(n)); }, &missing))
            throw std::runtime_error(std::string("OpenGL function missing: ") + (missing ? missing : "?"));
        log::info("OpenGL {} on {}", reinterpret_cast<const char*>(gl::GetString(gl::VERSION)),
                  reinterpret_cast<const char*>(gl::GetString(gl::RENDERER)));

        const std::vector<fs::path> all_models = list_models(*game_dir);
        std::size_t model_index = 0;
        for (std::size_t i = 0; i < all_models.size(); ++i) {
            std::error_code ec;
            if (fs::equivalent(all_models[i], model_path, ec)) model_index = i;
        }

        auto scene = load_scene(*game_dir, model_path, anim_arg, textures);
        if (scene->anim && !action_arg.empty()) {
            if (auto idx = scene->anim->find_action(action_arg)) scene->action = *idx;
            else log::warn("no action named {}", action_arg);
        }
        std::vector<Mat34> bones;
        pose(*scene, bones);
        Camera cam;
        frame_camera(cam, *scene, bones);

        render::LineRenderer lines;
        bool paused = false, wireframe = false, show_bones = false, show_grid = true;
        float speed = 1.0f;
        bool orbiting = false, panning = false;
        std::uint64_t last = SDL_GetTicksNS();
        std::string last_title;
        std::uint64_t last_title_ns = 0;

        auto switch_model = [&](int delta) {
            if (all_models.empty()) return;
            const std::size_t n = all_models.size();
            model_index = delta > 0 ? (model_index + 1) % n : (model_index + n - 1) % n;
            try {
                scene = load_scene(*game_dir, all_models[model_index], std::nullopt, textures);
                pose(*scene, bones);
                frame_camera(cam, *scene, bones);
            } catch (const std::exception& e) {
                log::error("{}: {}", all_models[model_index].string(), e.what());
            }
        };
        auto switch_action = [&](int delta) {
            if (!scene->anim || scene->anim->actions.empty()) return;
            const auto n = static_cast<int>(scene->anim->actions.size());
            scene->action = static_cast<std::size_t>((static_cast<int>(scene->action) + delta + n) % n);
            scene->action_time = 0;
        };

        const auto on_event = [&](const SDL_Event& ev) {
            switch (ev.type) {
                case SDL_EVENT_KEY_DOWN:
                    switch (ev.key.key) {
                        case SDLK_ESCAPE: {
                            SDL_Event quit{};
                            quit.type = SDL_EVENT_QUIT;
                            SDL_PushEvent(&quit);
                            break;
                        }
                        case SDLK_SPACE: paused = !paused; break;
                        case SDLK_RIGHT: switch_action(+1); break;
                        case SDLK_LEFT: switch_action(-1); break;
                        case SDLK_UP: speed = std::min(speed * 2, 16.0f); break;
                        case SDLK_DOWN: speed = std::max(speed / 2, 1.0f / 16); break;
                        case SDLK_PERIOD: scene->action_time += 1000.0f / 30; break;
                        case SDLK_COMMA: scene->action_time = std::max(0.0f, scene->action_time - 1000.0f / 30); break;
                        case SDLK_PAGEDOWN: switch_model(+1); break;
                        case SDLK_PAGEUP: switch_model(-1); break;
                        case SDLK_W: wireframe = !wireframe; break;
                        case SDLK_B: show_bones = !show_bones; break;
                        case SDLK_G: show_grid = !show_grid; break;
                        case SDLK_HOME:
                            cam.yaw = 0.6f, cam.pitch = 0.25f;
                            frame_camera(cam, *scene, bones);
                            break;
                        default: break;
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    const bool down = ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
                    if (ev.button.button == SDL_BUTTON_LEFT) orbiting = down;
                    if (ev.button.button == SDL_BUTTON_RIGHT || ev.button.button == SDL_BUTTON_MIDDLE) panning = down;
                    break;
                }
                case SDL_EVENT_MOUSE_MOTION:
                    if (orbiting) {
                        cam.yaw -= ev.motion.xrel * 0.01f;
                        cam.pitch = std::clamp(cam.pitch + ev.motion.yrel * 0.01f, -1.5f, 1.5f);
                    } else if (panning) {
                        const Vec3 fwd = normalize(cam.target - cam.eye());
                        const Vec3 right = normalize(cross(fwd, {0, 0, 1}));
                        const Vec3 up = cross(right, fwd);
                        const float k = cam.distance * 0.0015f;
                        cam.target += right * (-ev.motion.xrel * k) + up * (ev.motion.yrel * k);
                    }
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    cam.distance = std::clamp(cam.distance * std::pow(0.9f, ev.wheel.y), cam.base_distance * 0.05f,
                                              cam.base_distance * 20);
                    break;
                default: break;
            }
        };

        std::printf(
            "jview controls: drag = orbit, right drag = pan, wheel = zoom, Left/Right = action, Up/Down = speed,\n"
            "  Space = pause, ,/. = step, PageUp/PageDown = model, W = wireframe, B = bones, G = grid, Home = reset\n");

        while (window.poll_events(on_event)) {
            const std::uint64_t now = SDL_GetTicksNS();
            const float dt_ms = static_cast<float>(now - last) / 1e6f;
            last = now;

            std::string action_name = "-";
            float action_len = 0;
            if (scene->anim && !scene->anim->actions.empty()) {
                const Action& a = scene->anim->actions[scene->action];
                action_name = a.name;
                action_len = std::max(1.0f, a.end_ms - a.start_ms);
                if (!paused) scene->action_time += dt_ms * speed;
                scene->action_time = std::fmod(scene->action_time, action_len);
            }
            pose(*scene, bones);

            int w = 0, h = 0;
            window.drawable_size(w, h);
            gl::Viewport(0, 0, w, h);
            gl::ClearColor(0.16f, 0.17f, 0.19f, 1.0f);
            gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

            const float aspect = h > 0 ? static_cast<float>(w) / static_cast<float>(h) : 1.0f;
            const Mat4 proj = Mat4::perspective(40 * kPi / 180, aspect, cam.distance * 0.01f, cam.distance * 50);
            const Mat4 view = Mat4::look_at(cam.eye(), cam.target, {0, 0, 1});
            const Mat4 vp = proj * view;

            if (show_grid) {
                lines.clear();
                const float step = std::pow(10.0f, std::floor(std::log10(std::max(1.0f, cam.base_distance / 4))));
                const int n = 10;
                for (int i = -n; i <= n; ++i) {
                    const float t = static_cast<float>(i) * step, e = static_cast<float>(n) * step;
                    const Color c = i == 0 ? Color{0.45f, 0.45f, 0.5f, 1} : Color{0.28f, 0.29f, 0.32f, 1};
                    lines.add({t, -e, 0}, {t, e, 0}, c);
                    lines.add({-e, t, 0}, {e, t, 0}, c);
                }
                lines.draw(vp, false);
            }

            render::DrawParams dp;
            dp.view_projection = vp;
            dp.wireframe = wireframe;
            dp.light_dir = cam.eye() - cam.target + Vec3{0, 0, cam.distance};  // over the viewer's shoulder
            scene->renderer->draw(bones, dp);

            if (show_bones && scene->skeleton) {
                lines.clear();
                for (std::size_t i = 0; i < scene->skeleton->bones.size() && i < bones.size(); ++i) {
                    const auto parent = scene->skeleton->bones[i].parent;
                    const Vec3 p = bones[i].translation();
                    if (parent >= 0 && static_cast<std::size_t>(parent) < bones.size())
                        lines.add(bones[static_cast<std::size_t>(parent)].translation(), p, {1, 0.8f, 0.1f, 1});
                    const float k = cam.distance * 0.01f;  // small axis tripod per bone
                    lines.add(p, p + bones[i].transform_vector({k, 0, 0}), {1, 0.2f, 0.2f, 1});
                    lines.add(p, p + bones[i].transform_vector({0, k, 0}), {0.2f, 1, 0.2f, 1});
                }
                lines.draw(vp, true);
            }

            const std::string title = std::format(
                "jview - {}/{}{}{} - action [{}/{}] {} {:.0f}/{:.0f} ms x{}{}", scene->model_path.parent_path().filename().string(),
                scene->model_path.filename().string(), scene->anim ? " + " : "",
                scene->anim ? scene->anim_path.filename().string() : "", scene->anim ? scene->action + 1 : 0,
                scene->anim ? scene->anim->actions.size() : 0, action_name, scene->action_time, action_len, speed,
                paused ? " (paused)" : "");
            if (title != last_title && now - last_title_ns > 100'000'000) {  // at most 10 updates/s
                window.set_title(title);
                last_title = title;
                last_title_ns = now;
            }
            window.swap();
        }
        scene.reset();
        render::release_shaders();
        return 0;
    } catch (const std::exception& e) {
        log::error("fatal: {}", e.what());
        return 1;
    }
}
