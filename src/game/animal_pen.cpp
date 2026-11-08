// SPDX-License-Identifier: GPL-3.0-or-later
// [Fixes] AnimalPen (see animal_pen.hpp).
#include "animal_pen.hpp"

#include <unordered_map>

namespace joan::animal_pen {
namespace {

std::unordered_map<const void*, const void*> g_marked;  // body record -> the unit that owned it when it was marked

}  // namespace

void mark_off_mesh(const void* body, const void* owner) { g_marked[body] = owner; }

bool off_mesh(const void* body, const void* owner, bool on_mesh) {
    const auto it = g_marked.find(body);
    if (it == g_marked.end()) return false;
    if (on_mesh || it->second != owner) {  // back on the mesh, or the record now belongs to another unit
        g_marked.erase(it);
        return false;
    }
    return true;
}

}  // namespace joan::animal_pen
