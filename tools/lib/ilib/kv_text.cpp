#include "ilib/kv_text.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace joan::ilib {
namespace {

std::string_view trim(std::string_view s) {
    auto ws = [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    while (!s.empty() && ws(s.front())) s.remove_prefix(1);
    while (!s.empty() && ws(s.back())) s.remove_suffix(1);
    return s;
}

bool iequals(std::string_view a, std::string_view b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
               return std::tolower(static_cast<unsigned char>(x)) == std::tolower(static_cast<unsigned char>(y));
           });
}

std::optional<double> to_number(std::optional<std::string_view> v) {
    if (!v) return std::nullopt;
    const std::string s(*v);
    char* end = nullptr;
    const double d = std::strtod(s.c_str(), &end);
    if (end == s.c_str()) return std::nullopt;
    return d;
}

}  // namespace

const KvPair* KvBlock::find(std::string_view key) const {
    for (const auto& p : pairs)
        if (iequals(p.key, key)) return &p;
    return nullptr;
}

std::optional<std::string_view> KvBlock::get(std::string_view key) const {
    if (const KvPair* p = find(key)) return std::string_view(p->value);
    return std::nullopt;
}

std::optional<double> KvBlock::get_number(std::string_view key) const { return to_number(get(key)); }

KvText KvText::parse(std::string_view text) {
    KvText kv;
    KvBlock current;
    std::string pending_heading;
    int line_no = 0;
    auto flush = [&] {
        if (!current.pairs.empty() || !current.heading.empty()) kv.blocks_.push_back(std::move(current));
        current = KvBlock{};
    };

    std::size_t pos = 0;
    while (pos <= text.size()) {
        std::size_t nl = text.find('\n', pos);
        if (nl == std::string_view::npos) nl = text.size();
        std::string_view line = trim(text.substr(pos, nl - pos));
        pos = nl + 1;
        ++line_no;

        if (line.empty()) {
            if (!current.pairs.empty()) flush();
            continue;
        }
        if (line.starts_with("//")) continue;
        if (line.front() == '#') {
            if (!current.pairs.empty()) flush();
            current.heading = std::string(trim(line.substr(1)));
            continue;
        }
        const std::size_t colon = line.find(':');
        if (colon == std::string_view::npos) {
            current.pairs.push_back({std::string(line), {}, line_no});  // bare token
            continue;
        }
        current.pairs.push_back({std::string(trim(line.substr(0, colon))), std::string(trim(line.substr(colon + 1))), line_no});
    }
    flush();
    return kv;
}

std::optional<std::string_view> KvText::get(std::string_view key) const {
    for (const auto& b : blocks_)
        if (auto v = b.get(key)) return v;
    return std::nullopt;
}

std::optional<double> KvText::get_number(std::string_view key) const { return to_number(get(key)); }

}  // namespace joan::ilib
