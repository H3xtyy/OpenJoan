// openjoan.ini without Windows' profile API (ini.hpp).

#include "ini.hpp"
#include "user_files.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <mutex>
#include <sstream>
#include <string_view>
#include <system_error>
#include <vector>

namespace joan::platform::ini {
namespace {

namespace fs = std::filesystem;

// The settings file to read: the user's copy if the game folder could not be written (user_files.hpp), otherwise `file`.
fs::path own_copy_or(const fs::path& file) {
    std::error_code ec;
    const fs::path copy = user_files::users_folder() / file.filename();
    return fs::exists(copy, ec) ? copy : file;
}


std::string_view trim(std::string_view s) {
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.remove_prefix(1);
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) s.remove_suffix(1);
    return s;
}

bool same(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        const unsigned char x = static_cast<unsigned char>(a[i]), y = static_cast<unsigned char>(b[i]);
        if (std::tolower(x) != std::tolower(y)) return false;
    }
    return true;
}

// One line of the file split for lookups ("[name]", "key=value" or other).
struct Line {
    std::string text;     // as in the file, without its line end
    bool section = false;
    std::string name;     // the section's or the key's name (trimmed)
    std::string value;    // a key's value (trimmed, unquoted)
    bool crlf = true;     // its line end: CR LF, else LF
    bool ended = true;    // it has one (the file's last line may not)
};

struct File {
    bool exists = false;
    std::uintmax_t size = 0;
    fs::file_time_type time{};
    bool bom = false;   // starts with a UTF-8 byte order mark (kept when writing)
    bool crlf = true;   // the line end most lines have: the one a new line gets
    std::vector<Line> lines;
};

Line split(std::string text) {
    Line l;
    l.text = std::move(text);
    const std::string_view t = trim(l.text);
    if (t.size() >= 2 && t.front() == '[') {
        const std::size_t close = t.find(']');
        if (close != std::string_view::npos) {
            l.section = true;
            l.name = std::string(trim(t.substr(1, close - 1)));
            return l;
        }
    }
    const std::size_t eq = t.find('=');
    if (eq == std::string_view::npos || eq == 0) return l;
    l.name = std::string(trim(t.substr(0, eq)));
    std::string_view v = trim(t.substr(eq + 1));
    if (v.size() >= 2 && (v.front() == '"' || v.front() == '\'') && v.back() == v.front()) v = v.substr(1, v.size() - 2);
    l.value = std::string(v);
    return l;
}

File load(const fs::path& path) {
    File f;
    std::error_code ec;
    f.exists = fs::is_regular_file(path, ec);
    if (!f.exists) return f;
    f.size = fs::file_size(path, ec);
    f.time = fs::last_write_time(path, ec);
    std::ifstream in(own_copy_or(path), std::ios::binary);
    std::string all((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (all.size() >= 3 && static_cast<unsigned char>(all[0]) == 0xef && static_cast<unsigned char>(all[1]) == 0xbb &&
        static_cast<unsigned char>(all[2]) == 0xbf) {
        f.bom = true;
        all.erase(0, 3);
    }
    std::size_t at = 0, crlf = 0, lf = 0;
    while (at < all.size()) {
        std::size_t nl = all.find('\n', at);
        const bool ended = nl != std::string::npos;
        if (!ended) nl = all.size();
        std::string text = all.substr(at, nl - at);
        const bool cr = !text.empty() && text.back() == '\r';
        if (cr) text.pop_back();
        Line l = split(std::move(text));
        l.crlf = cr;
        l.ended = ended;
        if (ended) ++(cr ? crlf : lf);
        f.lines.push_back(std::move(l));
        at = nl + 1;
    }
    f.crlf = crlf >= lf;
    return f;
}

std::mutex g_lock;
std::map<fs::path, File> g_files;

// The parsed file, read again when it changed on disk.
const File& cached(const fs::path& path) {
    std::error_code ec;
    File& f = g_files[path];
    const bool exists = fs::is_regular_file(path, ec);
    if (exists != f.exists || (exists && (fs::file_size(path, ec) != f.size || fs::last_write_time(path, ec) != f.time)) ||
        (exists && f.lines.empty() && f.size != 0))
        f = load(path);
    return f;
}

// The value of [section] key, nullptr when missing (the first section and key of those names).
const std::string* find(const File& f, const char* section, const char* key) {
    bool in = false, seen = false;
    for (const Line& l : f.lines) {
        if (l.section) {
            if (seen && in) return nullptr;  // only the first section of that name
            in = same(l.name, section);
            seen = seen || in;
            continue;
        }
        if (in && !l.name.empty() && same(l.name, key)) return &l.value;
    }
    return nullptr;
}

}  // namespace

std::int32_t get_int(const char* section, const char* key, std::int32_t def, const fs::path& file) {
    std::string value;
    {
        std::lock_guard<std::mutex> lock(g_lock);
        const std::string* v = find(cached(file), section, key);
        if (v == nullptr) return def;
        value = *v;
    }
    const char* p = value.c_str();
    while (*p == ' ' || *p == '\t') ++p;
    bool negative = false;
    if (*p == '-' || *p == '+') negative = *p++ == '-';
    std::int64_t n = 0;
    while (*p >= '0' && *p <= '9') {
        n = n * 10 + (*p++ - '0');
        if (n > 0xffffffffll) n = 0xffffffffll;
    }
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(negative ? -n : n));
}

std::string get_string(const char* section, const char* key, const char* def, const fs::path& file) {
    std::lock_guard<std::mutex> lock(g_lock);
    const std::string* v = find(cached(file), section, key);
    if (v != nullptr) return *v;
    return std::string(trim(def != nullptr ? std::string_view(def) : std::string_view()));
}

std::uint32_t get_string(const char* section, const char* key, const char* def, char* out, std::uint32_t size,
                         const fs::path& file) {
    if (out == nullptr || size == 0) return 0;
    const std::string v = get_string(section, key, def, file);
    const std::size_t n = std::min<std::size_t>(v.size(), size - 1);
    std::memcpy(out, v.data(), n);
    out[n] = '\0';
    return static_cast<std::uint32_t>(n);
}

bool write_string(const char* section, const char* key, const char* value, const fs::path& file) {
    std::lock_guard<std::mutex> lock(g_lock);
    File f = load(file);
    const std::string line = std::string(key) + "=" + (value != nullptr ? value : "");
    // A new or changed line, with the file's usual line end (a changed line keeps its own).
    auto make = [&f](const std::string& text, const Line* old = nullptr) {
        Line l = split(text);
        l.crlf = old != nullptr ? old->crlf : f.crlf;
        return l;
    };
    // The last line gets a line end when another follows it.
    auto end_last = [&f] {
        if (!f.lines.empty() && !f.lines.back().ended) {
            f.lines.back().ended = true;
            f.lines.back().crlf = f.crlf;
        }
    };
    // The key's line in the first section of that name, else the end of that section (before its trailing blank
    // lines), else a new section at the end.
    bool found = false, done = false;
    std::size_t insert_at = 0;
    for (std::size_t i = 0; i < f.lines.size() && !done; ++i) {
        const Line& l = f.lines[i];
        if (l.section) {
            if (found) break;  // past the section
            if (same(l.name, section)) {
                found = true;
                insert_at = i + 1;
            }
            continue;
        }
        if (!found) continue;
        if (!l.name.empty() && same(l.name, key)) {
            Line changed = make(line, &l);
            changed.ended = l.ended;
            f.lines[i] = std::move(changed);
            done = true;
        } else if (!trim(l.text).empty()) {
            insert_at = i + 1;
        }
    }
    if (!done && found) {
        if (insert_at == f.lines.size()) end_last();
        f.lines.insert(f.lines.begin() + static_cast<std::ptrdiff_t>(insert_at), make(line));
    } else if (!done) {
        end_last();
        if (!f.lines.empty() && !trim(f.lines.back().text).empty()) f.lines.push_back(make(""));
        f.lines.push_back(make("[" + std::string(section) + "]"));
        f.lines.push_back(make(line));
    }
    std::ostringstream out;
    if (f.bom) out << "\xef\xbb\xbf";
    for (const Line& l : f.lines) {
        out << l.text;
        if (l.ended) out << (l.crlf ? "\r\n" : "\n");
    }
    {
        std::ofstream o(file, std::ios::binary | std::ios::trunc);
        if (!o) {  // a game folder that cannot be written: the user's copy, which is read from then on
            o.clear();
            std::error_code ec;
            const fs::path copy = user_files::users_folder() / file.filename();
            fs::create_directories(copy.parent_path(), ec);
            o.open(copy, std::ios::binary | std::ios::trunc);
        }
        if (!o) return false;
        const std::string s = out.str();
        o.write(s.data(), static_cast<std::streamsize>(s.size()));
        if (!o) return false;
    }
    g_files.erase(file);  // read again on the next lookup
    return true;
}

}  // namespace joan::platform::ini
