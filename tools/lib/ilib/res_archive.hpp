// Resource archives (.RES / .SET / RESOURCE files).
//
// Reimplements the Enlight engine's ResourceIdx (d:\ww\ilib\resx.cpp,
// joan.exe 0x0059c550..0x0059d160) and its long-name sibling
// (d:\ww\ilib\reslx.cpp, 0x0059b920..0x0059c550).
//
// On-disk layout (little endian, packed):
//
//   Short index (ResourceIdx, used by *.SET, RESOURCE, GAMEPLAY.RES, ...)
//     u16  count
//     struct { char name[9];  u32 offset; } index[count + 1]
//
//   Long index (ResourceIdxL, used by WAVE.RES, animscr.res, texteff.res, ...)
//     u32  count
//     struct { char name[34]; u32 offset; } index[count + 1]
//
// The extra trailing index record is a sentinel whose offset is the file size,
// so the size of entry i is index[i+1].offset - index[i].offset.
// Names are NUL padded and looked up case-insensitively by the original code.
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace joan::ilib {

enum class ResFormat {
    ShortIndex,  // u16 count, 9-byte names  (resx.cpp)
    LongIndex,   // u32 count, 34-byte names (reslx.cpp)
};

struct ResEntry {
    std::string name;
    std::uint32_t offset = 0;
    std::uint32_t size = 0;
};

class ResArchive {
public:
    static constexpr std::size_t kShortNameLen = 9;
    static constexpr std::size_t kLongNameLen = 34;

    /// Parses an archive held in memory. The archive keeps its own copy.
    /// If `format` is empty it is auto-detected. Throws joan::FormatError.
    static ResArchive parse(std::vector<std::byte> data, std::optional<ResFormat> format = std::nullopt);

    /// Returns which index format `data` uses, or nullopt if it is not an archive.
    static std::optional<ResFormat> detect(std::span<const std::byte> data);

    [[nodiscard]] ResFormat format() const { return format_; }
    [[nodiscard]] const std::vector<ResEntry>& entries() const { return entries_; }
    [[nodiscard]] std::size_t size() const { return entries_.size(); }

    /// Case-insensitive lookup by name (as ResourceIdx::get_index does).
    [[nodiscard]] const ResEntry* find(std::string_view name) const;

    /// Raw bytes of an entry. The span stays valid as long as the archive lives.
    [[nodiscard]] std::span<const std::byte> data(const ResEntry& entry) const;
    [[nodiscard]] std::optional<std::span<const std::byte>> data(std::string_view name) const;

    /// Serialises the archive back to disk format (useful for modding tools).
    static std::vector<std::byte> build(ResFormat format,
                                        std::span<const std::pair<std::string, std::vector<std::byte>>> files);

private:
    ResFormat format_ = ResFormat::ShortIndex;
    std::vector<std::byte> blob_;
    std::vector<ResEntry> entries_;
};

}  // namespace joan::ilib
