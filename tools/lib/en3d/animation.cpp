#include "en3d/animation.hpp"

#include <algorithm>
#include <cctype>

#include "core/binary_reader.hpp"

namespace joan::en3d {

namespace {

bool iequals(std::string_view a, std::string_view b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
               return std::tolower(static_cast<unsigned char>(x)) == std::tolower(static_cast<unsigned char>(y));
           });
}

// Action names are char[54]; some exporters pad with 0xCC (MSVC debug fill) after the NUL.
std::string read_name(BinaryReader& r, std::size_t len) {
    std::string s = r.fixed_string(len);
    while (!s.empty() && static_cast<unsigned char>(s.back()) == 0xCC) s.pop_back();
    return s;
}

}  // namespace

std::optional<std::size_t> Animation::find_action(std::string_view name) const {
    for (std::size_t i = 0; i < actions.size(); ++i)
        if (iequals(actions[i].name, name)) return i;
    return std::nullopt;
}

Mat34 Animation::sample(std::size_t bone, float time_ms) const {
    if (bone >= tracks.size() || tracks[bone].empty()) return {};
    const auto& keys = tracks[bone];
    if (time_ms <= keys.front().time_ms) return keys.front().transform;
    if (time_ms >= keys.back().time_ms) return keys.back().transform;
    const auto it = std::upper_bound(keys.begin(), keys.end(), time_ms,
                                     [](float t, const Key& k) { return t < k.time_ms; });
    const Key& b = *it;
    const Key& a = *(it - 1);
    const float span = b.time_ms - a.time_ms;
    return interpolate(a.transform, b.transform, span > 0 ? (time_ms - a.time_ms) / span : 0.0f);
}

void Animation::sample_all(float time_ms, std::vector<Mat34>& out) const {
    out.resize(bone_count);
    for (std::size_t b = 0; b < bone_count; ++b) out[b] = sample(b, time_ms);
}

Animation load_am2(std::span<const std::byte> data) {
    BinaryReader r(data);
    Animation a;
    a.bone_count = r.u32();
    const std::uint32_t action_count = r.u32();
    if (a.bone_count > 4096 || action_count > 65536) throw FormatError("implausible AM2 header");

    a.actions.resize(action_count);
    for (auto& act : a.actions) {
        act.start_ms = r.f32();
        act.end_ms = r.f32();
        act.name = read_name(r, 54);
    }
    a.tail_start_ms = r.f32();
    a.tail_end_ms = r.f32();

    a.tracks.resize(a.bone_count);
    for (auto& track : a.tracks) {
        const std::uint32_t n = r.u32();
        if (static_cast<std::uint64_t>(n) * 52 > r.remaining()) throw FormatError("AM2 track runs past end of file");
        track.resize(n);
        for (auto& k : track) {
            k.time_ms = r.f32();
            for (auto& v : k.transform.m) v = r.f32();
        }
        if (!std::is_sorted(track.begin(), track.end(),
                            [](const Key& x, const Key& y) { return x.time_ms < y.time_ms; }))
            throw FormatError("AM2 keys out of order");
    }
    auto rest = r.bytes(r.remaining());
    a.trailer.assign(rest.begin(), rest.end());
    return a;
}

}  // namespace joan::en3d
