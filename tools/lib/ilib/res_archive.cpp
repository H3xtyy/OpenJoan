#include "ilib/res_archive.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <limits>

#include "core/binary_reader.hpp"

namespace joan::ilib {
namespace {

struct Layout {
    std::size_t count_size;
    std::size_t name_len;
    [[nodiscard]] std::size_t record() const { return name_len + 4; }
};

constexpr Layout layout_of(ResFormat f) {
    return f == ResFormat::ShortIndex ? Layout{2, ResArchive::kShortNameLen} : Layout{4, ResArchive::kLongNameLen};
}

bool iequals(std::string_view a, std::string_view b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
               return std::toupper(static_cast<unsigned char>(x)) == std::toupper(static_cast<unsigned char>(y));
           });
}

// Validates the index without throwing: first offset must point right after the
// index, offsets must be monotonic and the sentinel must equal the file size.
bool index_is_consistent(std::span<const std::byte> data, ResFormat f) {
    const Layout l = layout_of(f);
    if (data.size() < l.count_size) return false;
    BinaryReader r(data);
    const std::uint64_t count = l.count_size == 2 ? r.u16() : r.u32();
    const std::uint64_t index_end = l.count_size + (count + 1) * l.record();
    if (index_end > data.size()) return false;
    std::uint32_t prev = 0;
    for (std::uint64_t i = 0; i <= count; ++i) {
        r.skip(l.name_len);
        const std::uint32_t off = r.u32();
        if (i == 0 && off != index_end) return false;
        if (off < prev || off > data.size()) return false;
        prev = off;
    }
    return prev == data.size();
}

}  // namespace

std::optional<ResFormat> ResArchive::detect(std::span<const std::byte> data) {
    for (ResFormat f : {ResFormat::ShortIndex, ResFormat::LongIndex})
        if (index_is_consistent(data, f)) return f;
    return std::nullopt;
}

ResArchive ResArchive::parse(std::vector<std::byte> data, std::optional<ResFormat> format) {
    if (!format) format = detect(data);
    if (!format || !index_is_consistent(data, *format)) throw FormatError("not a valid resource archive");

    ResArchive a;
    a.format_ = *format;
    a.blob_ = std::move(data);

    const Layout l = layout_of(*format);
    BinaryReader r(a.blob_);
    const std::uint32_t count = l.count_size == 2 ? r.u16() : r.u32();
    a.entries_.reserve(count);
    std::vector<std::uint32_t> offsets;
    offsets.reserve(count + 1);
    for (std::uint32_t i = 0; i <= count; ++i) {
        std::string name = r.fixed_string(l.name_len);
        offsets.push_back(r.u32());
        if (i < count) a.entries_.push_back(ResEntry{std::move(name), offsets.back(), 0});
    }
    for (std::uint32_t i = 0; i < count; ++i) a.entries_[i].size = offsets[i + 1] - offsets[i];
    return a;
}

const ResEntry* ResArchive::find(std::string_view name) const {
    for (const auto& e : entries_)
        if (iequals(e.name, name)) return &e;
    return nullptr;
}

std::span<const std::byte> ResArchive::data(const ResEntry& entry) const {
    return std::span<const std::byte>(blob_).subspan(entry.offset, entry.size);
}

std::optional<std::span<const std::byte>> ResArchive::data(std::string_view name) const {
    if (const ResEntry* e = find(name)) return data(*e);
    return std::nullopt;
}

std::vector<std::byte> ResArchive::build(ResFormat format,
                                         std::span<const std::pair<std::string, std::vector<std::byte>>> files) {
    const Layout l = layout_of(format);
    const std::size_t max_count =
        l.count_size == 2 ? std::numeric_limits<std::uint16_t>::max() - 1 : std::numeric_limits<std::uint32_t>::max() - 1;
    if (files.size() > max_count) throw FormatError("too many entries for resource archive");

    std::size_t total = l.count_size + (files.size() + 1) * l.record();
    for (const auto& [name, bytes] : files) {
        if (name.size() >= l.name_len) throw FormatError("resource name too long: " + name);
        total += bytes.size();
    }
    if (total > std::numeric_limits<std::uint32_t>::max()) throw FormatError("archive larger than 4 GiB");

    std::vector<std::byte> out(total);
    std::byte* p = out.data();
    auto put_le = [&](std::uint64_t v, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) *p++ = static_cast<std::byte>((v >> (8 * i)) & 0xff);
    };
    put_le(files.size(), l.count_size);
    std::uint32_t offset = static_cast<std::uint32_t>(l.count_size + (files.size() + 1) * l.record());
    for (std::size_t i = 0; i <= files.size(); ++i) {
        if (i < files.size()) std::memcpy(p, files[i].first.data(), files[i].first.size());
        p += l.name_len;  // remaining bytes are already zero
        put_le(offset, 4);
        if (i < files.size()) offset += static_cast<std::uint32_t>(files[i].second.size());
    }
    for (const auto& [name, bytes] : files) {
        if (!bytes.empty()) std::memcpy(p, bytes.data(), bytes.size());
        p += bytes.size();
    }
    return out;
}

}  // namespace joan::ilib
