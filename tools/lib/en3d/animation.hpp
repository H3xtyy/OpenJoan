// .AM2 skeletal animation (en3d/animdef.cpp, joan.exe 0x5b4170-0x5b9710).
//
// One file holds every action of a unit on a single timeline: the action table
// names time ranges ("f_run" 0-3333 ms, "f_walk" 4333-5133 ms, ...) and each bone
// has its own track of keys. A key stores the bone's complete model-space
// transform (not relative to the parent), so a pose is just "sample every track".
// Keys are sparse: a bone that does not move gets fewer keys.
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "en3d/math.hpp"

namespace joan::en3d {

struct Action {
    std::string name;  // "default" (whole timeline), "f_run", "f_atkA", "undefine action", ...
    float start_ms = 0;
    float end_ms = 0;
};

struct Key {
    float time_ms = 0;
    Mat34 transform;  // model space
};

struct Animation {
    std::uint32_t bone_count = 0;
    std::vector<Action> actions;
    float tail_start_ms = 0;  // header before the tracks; tail_end_ms is the timeline length
    float tail_end_ms = 0;
    std::vector<std::vector<Key>> tracks;  // one per bone, sorted by time
    std::vector<std::byte> trailer;        // optional trailing data (not yet understood)

    [[nodiscard]] std::optional<std::size_t> find_action(std::string_view name) const;

    /// Model-space transform of one bone at an absolute timeline time
    /// (clamped to the track; interpolated between keys).
    [[nodiscard]] Mat34 sample(std::size_t bone, float time_ms) const;

    /// Samples every bone.
    void sample_all(float time_ms, std::vector<Mat34>& out) const;
};

Animation load_am2(std::span<const std::byte> data);

}  // namespace joan::en3d
