// Recording file format - see stream.hpp.
#include "stream.hpp"

#include <algorithm>
#include <cstring>

namespace joan::replay {

const char* event_name(Ev e) {
    switch (e) {
        case Ev::Call: return "call";
        case Ev::WndProc: return "window message";
        case Ev::Apc: return "I/O completion";
        case Ev::Frame: return "frame (Present)";
        case Ev::Snapshot: return "snapshot";
        case Ev::Env: return "environment";
        case Ev::End: return "end of game";
    }
    return "?";
}

namespace {

bool parse_number(const std::string& s, std::uint32_t& v) {
    if (s.empty()) return false;
    std::size_t i = 0;
    unsigned base = 10;
    if (s.size() > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        base = 16;
        i = 2;
    }
    std::uint64_t r = 0;
    for (; i < s.size(); ++i) {
        const char c = s[i];
        unsigned d;
        if (c >= '0' && c <= '9')
            d = static_cast<unsigned>(c - '0');
        else if (base == 16 && c >= 'a' && c <= 'f')
            d = static_cast<unsigned>(c - 'a' + 10);
        else if (base == 16 && c >= 'A' && c <= 'F')
            d = static_cast<unsigned>(c - 'A' + 10);
        else
            return false;
        r = r * base + d;
        if (r > 0xffffffffull) return false;
    }
    v = static_cast<std::uint32_t>(r);
    return true;
}

std::string trim(const std::string& s) {
    std::size_t a = 0, b = s.size();
    while (a < b && (s[a] == ' ' || s[a] == '\t')) ++a;
    while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t')) --b;
    return s.substr(a, b - a);
}

}  // namespace

bool parse_ranges(const std::string& text, std::vector<Range>& out, std::string* bad) {
    std::size_t start = 0;
    while (start <= text.size()) {
        std::size_t comma = text.find(',', start);
        if (comma == std::string::npos) comma = text.size();
        const std::string tok = trim(text.substr(start, comma - start));
        start = comma + 1;
        if (tok.empty()) continue;
        const std::size_t plus = tok.find('+');
        std::uint32_t begin = 0, len = 4;
        const bool ok = parse_number(trim(tok.substr(0, plus)), begin) &&
                        (plus == std::string::npos || parse_number(trim(tok.substr(plus + 1)), len));
        if (!ok || len == 0) {
            if (bad) *bad = tok;
            return false;
        }
        out.push_back({begin, begin + len});
    }
    return true;
}

bool Cursor::get_raw(void* out, std::size_t n) {
    if (static_cast<std::size_t>(end_ - p_) < n) {
        ok_ = false;
        if (n) std::memset(out, 0, n);
        p_ = end_;
        return false;
    }
    if (n) std::memcpy(out, p_, n);
    p_ += n;
    return true;
}

std::uint32_t Cursor::get_blob(void* out, std::uint32_t cap) {
    const std::uint32_t n = get<std::uint32_t>();
    if (left() < n) {
        ok_ = false;
        p_ = end_;
        return 0;
    }
    if (out && cap) std::memcpy(out, p_, n < cap ? n : cap);
    p_ += n;
    return n;
}

// ---------------------------------------------------------------- Writer

bool Writer::open(const std::string& path, const Header& h) {
    close();
    f_ = std::fopen(path.c_str(), "wb");
    if (!f_) return false;
    std::setvbuf(f_, nullptr, _IOFBF, 1 << 20);
    std::uint8_t head[kHeaderSize] = {};
    std::memcpy(head, kMagic, sizeof(kMagic));
    const std::uint32_t fields[] = {h.version,  h.flags,     h.data_va,        h.data_size,
                                    h.page_size, h.exe_stamp, h.snapshot_every, h.reserved};
    std::memcpy(head + sizeof(kMagic), fields, sizeof(fields));
    put(head, sizeof(head));
    return true;
}

void Writer::put(const void* p, std::size_t n) {
    if (!f_ || n == 0) return;
    std::fwrite(p, 1, n, f_);
    if (tee_) tee_(p, n);
    bytes_ += n;
}

void Writer::call(std::uint16_t id, std::uint32_t check, const void* payload, std::uint32_t n) {
    put(static_cast<std::uint8_t>(Ev::Call));
    put(id);
    put(check);
    put(n);
    put(payload, n);
}

void Writer::wndproc(std::uint32_t msg, std::uint32_t wparam, std::uint32_t lparam) {
    put(static_cast<std::uint8_t>(Ev::WndProc));
    put(msg);
    put(wparam);
    put(lparam);
}

void Writer::apc(std::uint32_t seq, std::uint32_t error, std::uint32_t bytes) {
    put(static_cast<std::uint8_t>(Ev::Apc));
    put(seq);
    put(error);
    put(bytes);
}

void Writer::frame(const FrameHash& f) {
    put(static_cast<std::uint8_t>(Ev::Frame));
    put(f.frame);
    put(f.image);
    put(f.heap);
    put(f.back_buffer);
    put(static_cast<std::uint32_t>(f.pages.size()));
    put(f.pages.data(), f.pages.size() * sizeof(std::uint32_t));
}

void Writer::snapshot(std::uint32_t frame, const std::vector<std::uint32_t>& words) {
    put(static_cast<std::uint8_t>(Ev::Snapshot));
    put(frame);
    put(static_cast<std::uint32_t>(words.size()));
    put(words.data(), words.size() * sizeof(std::uint32_t));
}

void Writer::env(const std::string& text) {
    put(static_cast<std::uint8_t>(Ev::Env));
    put(static_cast<std::uint32_t>(text.size()));
    put(text.data(), text.size());
}

void Writer::end(std::uint32_t frames) {
    put(static_cast<std::uint8_t>(Ev::End));
    put(frames);
}

void Writer::flush() {
    if (f_) std::fflush(f_);
    if (tee_flush_) tee_flush_();
}

void Writer::close() {
    if (f_) std::fclose(f_);
    f_ = nullptr;
}

// ---------------------------------------------------------------- Reader

const char* Reader::load(const std::string& path) {
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return "cannot open the recording";
    std::fseek(f, 0, SEEK_END);
    const long n = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (n < static_cast<long>(kHeaderSize)) {
        std::fclose(f);
        return "the recording is too short (no header)";
    }
    data_.resize(static_cast<std::size_t>(n));
    const std::size_t got = std::fread(data_.data(), 1, data_.size(), f);
    std::fclose(f);
    if (got != data_.size()) return "cannot read the recording";
    return read_header();
}

const char* Reader::read_header() {
    if (std::memcmp(data_.data(), kMagic, sizeof(kMagic)) != 0) return "not an OpenJoan recording (magic)";
    std::uint32_t fields[8];
    std::memcpy(fields, data_.data() + sizeof(kMagic), sizeof(fields));
    header_.version = fields[0];
    header_.flags = fields[1];
    header_.data_va = fields[2];
    header_.data_size = fields[3];
    header_.page_size = fields[4];
    header_.exe_stamp = fields[5];
    header_.snapshot_every = fields[6];
    header_.reserved = fields[7];
    if (header_.version < kFormatVersionUntyped || header_.version > kFormatVersion)
        return "unsupported recording format version (recorded by a newer build?)";
    pos_ = next_pos_ = kHeaderSize;
    return nullptr;
}

const char* Reader::follow(Source source) {
    source_ = std::move(source);
    data_.clear();
    base_ = 0;
    while (data_.size() < kHeaderSize)
        if (!source_(data_)) return "the stream ended before its header";
    return read_header();
}

std::vector<std::uint8_t> Reader::used_since(std::size_t& from) const {
    std::vector<std::uint8_t> out;
    const std::size_t to = base_ + pos_;
    if (from < base_) from = base_;
    if (from < to)
        out.assign(data_.begin() + static_cast<std::ptrdiff_t>(from - base_), data_.begin() + static_cast<std::ptrdiff_t>(pos_));
    from = to;
    return out;
}

bool Reader::peek(Event& e) {
    for (;;) {
        short_ = false;
        error_ = nullptr;
        if (decode(e)) return true;
        if (!source_ || !short_) return false;
        // A followed recording: wait for the rest of the event, after dropping what was used (event payloads point into
        // the buffer and are used only until the next peek).
        if (pos_ > (64u << 20)) {
            data_.erase(data_.begin(), data_.begin() + static_cast<std::ptrdiff_t>(pos_));
            base_ += pos_;
            next_pos_ -= std::min(next_pos_, pos_);
            pos_ = 0;
        }
        if (!source_(data_)) {
            error_ = nullptr;  // the host ended the stream: the end of the recording
            return false;
        }
    }
}

bool Reader::decode(Event& e) {
    if (pos_ >= data_.size()) {
        short_ = true;
        return false;
    }
    Cursor c(data_.data() + pos_, data_.size() - pos_);
    e = Event{};
    e.offset = base_ + pos_;
    e.type = static_cast<Ev>(c.get<std::uint8_t>());
    switch (e.type) {
        case Ev::Call:
            e.id = c.get<std::uint16_t>();
            e.check = c.get<std::uint32_t>();
            e.size = c.get<std::uint32_t>();
            if (c.left() < e.size) {
                error_ = "truncated call event";
                short_ = true;
                return false;
            }
            e.payload = data_.data() + (data_.size() - c.left());
            next_pos_ = data_.size() - c.left() + e.size;
            return true;
        case Ev::WndProc:
        case Ev::Apc:
            e.a = c.get<std::uint32_t>();
            e.b = c.get<std::uint32_t>();
            e.c = c.get<std::uint32_t>();
            break;
        case Ev::Frame: {
            e.frame.frame = c.get<std::uint32_t>();
            e.frame.image = c.get<std::uint64_t>();
            e.frame.heap = c.get<std::uint64_t>();
            e.frame.back_buffer = c.get<std::uint64_t>();
            const std::uint32_t n = c.get<std::uint32_t>();
            if (c.left() / 4 < n) {
                error_ = "truncated frame event";
                short_ = true;
                return false;
            }
            e.frame.pages.resize(n);
            c.get_raw(e.frame.pages.data(), n * sizeof(std::uint32_t));
            break;
        }
        case Ev::Snapshot: {
            e.a = c.get<std::uint32_t>();
            const std::uint32_t n = c.get<std::uint32_t>();
            if (c.left() / 4 < n) {
                error_ = "truncated snapshot event";
                short_ = true;
                return false;
            }
            e.words.resize(n);
            c.get_raw(e.words.data(), n * sizeof(std::uint32_t));
            break;
        }
        case Ev::Env: {
            e.size = c.get<std::uint32_t>();
            if (c.left() < e.size) {
                error_ = "truncated environment event";
                short_ = true;
                return false;
            }
            e.payload = data_.data() + (data_.size() - c.left());
            next_pos_ = data_.size() - c.left() + e.size;
            return true;
        }
        case Ev::End:
            e.a = c.get<std::uint32_t>();
            break;
        default:
            error_ = "unknown event type (damaged recording)";
            return false;
    }
    if (!c.ok()) {
        error_ = "truncated event at the end of the recording";
        short_ = true;
        return false;
    }
    next_pos_ = data_.size() - c.left();
    return true;
}

}  // namespace joan::replay
