// Field-wise save serialisation - see io.hpp. Portable: no Win32, no game code (the game's parts are hooks()).
#include "io.hpp"

#include <cstdint>
#include <cstring>

namespace joan::save {
namespace {

bool no() { return false; }
bool yes() { return true; }
std::uint32_t low32(std::uintptr_t v) { return static_cast<std::uint32_t>(v); }
std::uintptr_t widen(std::uint32_t v) { return v; }
// The old ImageWords path, from the image_out / image_in hooks (the game installs img::save_words / load_words).
void words_out(void* dst, const void* src, std::uint32_t n) {
    std::memcpy(dst, src, n);
    auto* d = static_cast<std::uint8_t*>(dst);
    for (std::uint32_t i = 0; i + 4 <= n; i += 4) {
        std::uint32_t v;
        std::memcpy(&v, d + i, 4);
        v = hooks().image_out(v);
        std::memcpy(d + i, &v, 4);
    }
}
void words_in(void* p, std::uint32_t n) {
    auto* d = static_cast<std::uint8_t*>(p);
    for (std::uint32_t i = 0; i + 4 <= n; i += 4) {
        std::uint32_t v;
        std::memcpy(&v, d + i, 4);
        v = static_cast<std::uint32_t>(hooks().image_in(v));
        std::memcpy(d + i, &v, 4);
    }
}
void ignore(const Event&) {}

const char* file_name(const Port& p) {
    const char* n = p.name != nullptr ? p.name(p.obj) : nullptr;
    return n != nullptr ? n : "";
}

Event event(Event::Kind kind, Dir dir, const Record* rec, const char* name, const Port& p) {
    Event e{};
    e.kind = kind;
    e.dir = dir;
    e.rec = rec;
    e.name = name;
    e.file = file_name(p);
    return e;
}

void misuse(Dir dir, const Record* rec, const char* name, const Port& p, std::uint32_t size, std::uint32_t got) {
    Event e = event(Event::Misuse, dir, rec, name, p);
    e.size = size;
    e.off = got;
    hooks().event(e);
}

}  // namespace

// SaveCompare: one Mismatch event per differing field (bytes outside the table: one per differing byte run), then one
// Compared event.
void compare_chunk(Dir dir, const Record* rec, const char* name, const Port& p, const std::uint8_t* now,
                   const std::uint8_t* legacy, std::uint32_t size) {
    Event e = event(Event::Mismatch, dir, rec, name, p);
    e.now = now;
    e.legacy = legacy;
    e.size = size;
    std::uint32_t bad = 0;
    for (std::uint32_t off = 0; off < size && std::memcmp(now + off, legacy + off, size - off) != 0;) {
        if (now[off] == legacy[off]) {
            ++off;
            continue;
        }
        e.off = off;
        e.field = rec != nullptr ? field_at(*rec, off) : nullptr;
        hooks().event(e);
        ++bad;
        std::uint32_t next = e.field != nullptr ? e.field->off + e.field->size : off + 1;
        if (e.field == nullptr)
            while (next < size && now[next] != legacy[next]) ++next;
        off = next;
    }
    e.kind = Event::Compared;
    e.off = bad;
    e.field = nullptr;
    hooks().event(e);
}

Hooks& hooks() noexcept {
    static Hooks h{&yes, &no, &no, &low32, &widen, &words_out, &words_in, &ignore};
    return h;
}

const Field* field_at(const Record& r, std::uint32_t off) noexcept {
    for (std::uint32_t i = 0; i < r.count; ++i)
        if (off - r.fields[i].off < r.fields[i].size) return &r.fields[i];
    return nullptr;
}

// ---------------------------------------------------------------------------------------------------------- Out
Out::Out(const Port& p, const Record& r, const void* legacy) noexcept
    : port_(p), rec_(&r), name_(r.name), legacy_(legacy), size_(r.size) {
    overflow_ = size_ > kMaxChunk;
}

Out::Out(const Port& p, std::uint32_t size, const char* name, const void* legacy) noexcept
    : port_(p), rec_(nullptr), name_(name), legacy_(legacy), size_(size) {
    overflow_ = size_ > kMaxChunk;
}

Out::~Out() {
    if (!ended_) misuse(Dir::Write, rec_, name_, port_, size_, pos_);  // a writer returned without end()
}

void Out::put(const void* p, std::uint32_t n) noexcept {
    if (overflow_ || n > size_ - pos_) {
        overflow_ = true;
        return;
    }
    std::memcpy(buf_ + pos_, p, n);
    pos_ += n;
}

void Out::put_le(std::uint32_t v, std::uint32_t n) noexcept {
    std::uint8_t b[4] = {static_cast<std::uint8_t>(v), static_cast<std::uint8_t>(v >> 8),
                         static_cast<std::uint8_t>(v >> 16), static_cast<std::uint8_t>(v >> 24)};
    put(b, n);
}

void Out::word(std::uint32_t v, Ptr k) noexcept { u32(k == Ptr::Image ? hooks().image_out(v) : v); }

void Out::ptr(const void* p, Ptr k) noexcept {
    const auto v = reinterpret_cast<std::uintptr_t>(p);
    if (k == Ptr::Image) {
        u32(hooks().image_out(v));
        return;
    }
    if constexpr (kX86Layout)
        u32(static_cast<std::uint32_t>(v));  // the raw value, as the old raw write
    else
        u32(v == 0 ? 0u : static_cast<std::uint32_t>(v) | 1u);  // 0 iff null
}

std::int32_t Out::end() noexcept {
    if (ended_) return 0;
    ended_ = true;
    if (overflow_ || pos_ != size_) {
        misuse(Dir::Write, rec_, name_, port_, size_, pos_);
        return 0;
    }
    if constexpr (kX86Layout) {
        if (legacy_ != nullptr && hooks().compare()) {
            alignas(4) std::uint8_t old[kMaxChunk];
            if (rec_ != nullptr && rec_->legacy == Legacy::ImageWords)
                hooks().legacy_out(old, legacy_, size_);
            else
                std::memcpy(old, legacy_, size_);
            compare_chunk(Dir::Write, rec_, name_, port_, buf_, old, size_);
        }
    }
    if (hooks().audit()) {
        Event e = event(Event::Audit, Dir::Write, rec_, name_, port_);
        e.now = buf_;
        e.size = size_;
        hooks().event(e);
    }
    return port_.write(port_.obj, buf_, size_);
}

// ----------------------------------------------------------------------------------------------------------- In
In::In(const Port& p, const Record& r, void* legacy) noexcept
    : port_(p), rec_(&r), name_(r.name), legacy_(legacy), size_(r.size) {
    read_chunk();
}

In::In(const Port& p, std::uint32_t size, const char* name, void* legacy) noexcept
    : port_(p), rec_(nullptr), name_(name), legacy_(legacy), size_(size) {
    read_chunk();
}

In::~In() {
    if (!ended_) misuse(Dir::Read, rec_, name_, port_, size_, pos_);
}

void In::read_chunk() noexcept {
    if (size_ > kMaxChunk) {
        overflow_ = true;
        return;
    }
    // What the raw read would have found in the bytes a failing ReadFile does not write: the object's own (x86).
    std::memset(buf_, 0, size_);
    if constexpr (kX86Layout)
        if (legacy_ != nullptr) std::memcpy(buf_, legacy_, size_);
    result_ = port_.read(port_.obj, buf_, size_);
    if constexpr (kX86Layout)
        if (legacy_ != nullptr && hooks().compare()) std::memcpy(raw_, buf_, size_);
}

std::uint32_t In::get_le(std::uint32_t n) noexcept {
    if (overflow_ || n > size_ - pos_) {
        overflow_ = true;
        return 0;
    }
    std::uint32_t v = 0;
    for (std::uint32_t i = 0; i < n; ++i) v |= static_cast<std::uint32_t>(buf_[pos_ + i]) << (8 * i);
    pos_ += n;
    return v;
}

void In::bytes(void* p, std::uint32_t n) noexcept {
    if (overflow_ || n > size_ - pos_) {
        overflow_ = true;
        return;
    }
    std::memcpy(p, buf_ + pos_, n);
    pos_ += n;
}

std::uint32_t In::word(Ptr k) noexcept {
    const std::uint32_t v = get_le(4);
    if (k != Ptr::Image) return v;
    // A 32-bit field cannot hold an address above 4 GB (x64): it keeps the original address, which Out::word writes
    // back unchanged - nobody uses such a field as a pointer there.
    const std::uintptr_t m = hooks().image_in(v);
    return m <= 0xffffffffu ? static_cast<std::uint32_t>(m) : v;
}

const void* In::ptr_value(const void* current, Ptr k) noexcept {
    const std::uint32_t v = get_le(4);
    if (k == Ptr::Image) return reinterpret_cast<const void*>(hooks().image_in(v));
    if constexpr (!kX86Layout) {
        if (k == Ptr::Kept) return current;
    } else {
        (void)current;  // x86: the stored value, as the raw read
    }
    return reinterpret_cast<const void*>(static_cast<std::uintptr_t>(v));  // x64: a token, null iff 0
}

std::int32_t In::end() noexcept {
    if (ended_) return 0;
    ended_ = true;
    if (overflow_ || pos_ != size_) {
        misuse(Dir::Read, rec_, name_, port_, size_, pos_);
        return 0;
    }
    if constexpr (kX86Layout) {
        if (legacy_ != nullptr && hooks().compare()) {
            // The raw read left raw_ in the object (ImageWords: then load_words over it).
            if (rec_ != nullptr && rec_->legacy == Legacy::ImageWords) hooks().legacy_in(raw_, size_);
            compare_chunk(Dir::Read, rec_, name_, port_, static_cast<const std::uint8_t*>(legacy_), raw_, size_);
        }
    }
    return result_;
}

}  // namespace joan::save
