#include "ilib/database.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>

#include "core/binary_reader.hpp"

namespace joan::ilib {
namespace {

bool iequals(std::string_view a, std::string_view b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
               return std::toupper(static_cast<unsigned char>(x)) == std::toupper(static_cast<unsigned char>(y));
           });
}

std::string_view trim(std::string_view s) {
    while (!s.empty() && (s.front() == ' ' || s.front() == '\0')) s.remove_prefix(1);
    while (!s.empty() && (s.back() == ' ' || s.back() == '\0')) s.remove_suffix(1);
    return s;
}

}  // namespace

Database Database::parse(std::span<const std::byte> data) {
    Database db;
    db.data_.assign(data.begin(), data.end());
    BinaryReader r(db.data_);

    const std::uint8_t version = r.u8();
    if ((version & 0x07) != 0x03) throw FormatError("not a dBase III table");
    r.skip(3);  // last update date
    db.record_count_ = r.u32();
    db.header_size_ = r.u16();
    db.record_size_ = r.u16();
    r.seek(32);

    std::uint16_t offset = 1;  // byte 0 of each record is the delete flag
    while (true) {
        if (r.eof()) throw FormatError("unterminated dBase field list");
        if (std::to_integer<std::uint8_t>(db.data_[r.pos()]) == 0x0D) break;
        DbField f;
        f.name = r.fixed_string(11);
        f.type = static_cast<char>(r.u8());
        r.skip(4);  // in-memory address, unused
        f.length = r.u8();
        f.decimals = r.u8();
        r.skip(14);
        f.offset = offset;
        offset = static_cast<std::uint16_t>(offset + f.length);
        db.fields_.push_back(std::move(f));
    }

    if (offset != db.record_size_) throw FormatError("dBase record size does not match its fields");
    // Some tools omit the trailing 0x1A EOF marker, so only require the records themselves.
    if (db.header_size_ + db.record_count_ * db.record_size_ > db.data_.size())
        throw FormatError("dBase table is truncated");
    return db;
}

std::optional<std::size_t> Database::field_index(std::string_view name) const {
    for (std::size_t i = 0; i < fields_.size(); ++i)
        if (iequals(fields_[i].name, name)) return i;
    return std::nullopt;
}

DbRecord Database::record(std::size_t index) const {
    if (index >= record_count_) throw FormatError("dBase record index out of range");
    return DbRecord(*this, std::span<const std::byte>(data_).subspan(header_size_ + index * record_size_, record_size_));
}

std::string Database::to_csv() const {
    auto quote = [](std::string_view s) {
        std::string out = "\"";
        for (char c : s) {
            if (c == '"') out += '"';
            out += c;
        }
        return out + "\"";
    };
    std::string csv;
    for (std::size_t i = 0; i < fields_.size(); ++i) csv += (i ? "," : "") + quote(fields_[i].name);
    csv += '\n';
    for_each([&](const DbRecord& rec) {
        for (std::size_t i = 0; i < fields_.size(); ++i) csv += (i ? "," : "") + quote(trim(rec.raw(i)));
        csv += '\n';
    });
    return csv;
}

std::string_view DbRecord::raw(std::size_t field_index) const {
    const DbField& f = db_->fields().at(field_index);
    return as_string_view(bytes_.subspan(f.offset, f.length));
}

std::optional<std::string_view> DbRecord::raw(std::string_view field_name) const {
    if (auto i = db_->field_index(field_name)) return raw(*i);
    return std::nullopt;
}

std::string DbRecord::str(std::string_view field_name) const {
    auto v = raw(field_name);
    if (!v) return {};
    std::string_view s = *v;
    while (!s.empty() && (s.back() == ' ' || s.back() == '\0')) s.remove_suffix(1);
    return std::string(s);
}

long long DbRecord::num(std::string_view field_name, long long fallback) const {
    auto v = raw(field_name);
    if (!v) return fallback;
    std::string_view s = trim(*v);
    if (!s.empty() && s.front() == '+') s.remove_prefix(1);
    long long value = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    return (ec == std::errc{} && ptr != s.data()) ? value : fallback;
}

double DbRecord::real(std::string_view field_name, double fallback) const {
    auto v = raw(field_name);
    if (!v) return fallback;
    const std::string s(trim(*v));
    if (s.empty()) return fallback;
    char* end = nullptr;
    const double value = std::strtod(s.c_str(), &end);
    return end != s.c_str() ? value : fallback;
}

}  // namespace joan::ilib
