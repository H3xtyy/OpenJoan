// See item_preload.hpp.
#include "item_preload.hpp"

#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <string>

#include "../compiler.hpp"
#include "../data_names.hpp"
#include "../original/direct.hpp"
#include "../original/layout_access.hpp"
#include "../platform/files.hpp"
#include "../registry.hpp"

namespace jo = joan::orig;

namespace joan::item_preload {
namespace {

// The item classes (ITEMCLAS) of what breaking a box, a chest or a unit can leave on the ground.
constexpr const char* kDropClasses[] = {"POTION", "GOLD", "ENHANCER", "CROSS"};

bool drop_class(const char* name) {
    for (const char* c : kDropClasses)
        if (_stricmp(name, c) == 0) return true;
    return false;
}

// Whether the model file of model-table entry `id` (ITEM\<file>.AM1 or .SMO, as ActorDef::load 0x005afdd0 / load2
// 0x005af3d0 build it) exists. The original builds an item's model only when the item lies in the world, so a model
// file an edition does not ship never mattered there: the Chinese release has the potions in ITEM.RES but none of
// their 24 models, and their load here would end in the loader's "Error loading 3D Model" fatal error.
bool model_file_exists(const jo::ModelRes* models, std::int16_t id) {
    if (id <= 0 || id > models->count) return true;  // not ours to judge: the acquire decides
    const jo::ModelEntry& e = models->entries[id - 1];
    if (!e.dir || !e.file) return true;
    const std::string base = std::string(e.dir) + e.file;
    std::error_code ec;
    for (const char* ext : {".AM1", ".SMO"})
        if (std::filesystem::is_regular_file(platform::files::native_path((base + ext).c_str()), ec)) return true;
    return false;
}

}  // namespace

void once() {
    static bool done = false;
    if (done || hook::settings().item_preload == 0) return;
    done = true;
    jo::ItemRes* const res = jo::as_item_res(img::obj(d::g_item_res));
    if (res->loaded == 0 || res->items == nullptr || res->classes == nullptr) return;
    const auto t0 = std::chrono::steady_clock::now();
    const jo::ModelRes* const models = jo::as_model_res(img::obj(d::g_model_res_mgr));  // the model table ItemDef::acquire_anim uses
    int count = 0, missing = 0;
    for (std::int32_t i = 0; i < res->item_count; ++i) {
        jo::ItemDef* const def = &res->items[i];
        if (def->pickable == 0) continue;
        const std::int32_t cls = def->class_id;
        if (cls < 1 || cls > res->class_count) continue;
        char name[sizeof res->classes[0].name + 1] = {};
        std::memcpy(name, res->classes[cls - 1].name, sizeof res->classes[0].name);
        if (!drop_class(name)) continue;
        if (!model_file_exists(models, def->icon_sprite) || (def->model_sprite != 0 && !model_file_exists(models, def->model_sprite))) {
            ++missing;
            continue;
        }
        jo::as_item_def(def)->acquire_anim(0, 0);  // 0x004a39f0, as a dropped item; never released
        ++count;
    }
    hook::log_line("[Game] ItemPreload: %d drop item models built in %.0f ms (kept for the session); %d skipped, their model file missing", count,
                   std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count(), missing);
}

}  // namespace joan::item_preload
