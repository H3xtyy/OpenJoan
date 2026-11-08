// "Key : value" text files.
//
// Many of the game's definition files share this line format:
//   scenario/*.scr, Resource/*.pro, map/*.sca, map/*.lgt, ...
//
//   code : SCENARIO_01
//   DescriptionFileName\t: S1L1_des
//   # SunRise            <- '#' starts a section/comment line
//   // comment
//
// Records are separated by blank lines in several files (e.g. common.lgt has a
// block per light). We keep the original order and expose both a flat list and
// blank-line-separated blocks; interpretation is up to the owning module.
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace joan::ilib {

struct KvPair {
    std::string key;
    std::string value;
    int line = 0;
};

struct KvBlock {
    std::string heading;  // text of a preceding '#' line, if any
    std::vector<KvPair> pairs;

    [[nodiscard]] const KvPair* find(std::string_view key) const;
    [[nodiscard]] std::optional<std::string_view> get(std::string_view key) const;
    [[nodiscard]] std::optional<double> get_number(std::string_view key) const;
};

class KvText {
public:
    static KvText parse(std::string_view text);

    [[nodiscard]] const std::vector<KvBlock>& blocks() const { return blocks_; }
    /// First value for `key` anywhere in the file (case-insensitive).
    [[nodiscard]] std::optional<std::string_view> get(std::string_view key) const;
    [[nodiscard]] std::optional<double> get_number(std::string_view key) const;

private:
    std::vector<KvBlock> blocks_;
};

}  // namespace joan::ilib
